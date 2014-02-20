CMAKE_MINIMUM_REQUIRED (VERSION 2.8.7)

PROJECT (versos_test)

#------------------------------------------------------------------------------
# Check for dependencies, they should have been SET in parent scope
#------------------------------------------------------------------------------

IF (ENABLE_MPI_COORDINATOR)
  IF (!MPI_C_FOUND)
    MESSAGE (FATAL_ERROR "MPI required")
  ENDIF ()
ENDIF()

IF (NOT TARGET libversos)
  MESSAGE (FATAL_ERROR "libversos target not found")
ENDIF ()

# TODO: check for the rest of dependencies: OpenSSL and librados (boost is OK since those are just headers, i.e. we don't use boost::threads or alike)

#------------------------------------------------------------------------------
# Add gmock/gtest libraries
#------------------------------------------------------------------------------

ADD_SUBDIRECTORY (gmock)

INCLUDE_DIRECTORIES ("gmock/include")
INCLUDE_DIRECTORIES ("gmock/gtest/include")

SET (DEP_LIBS gtest gtest_main gmock gmock_main ${OPENSSL_LIBRARIES})

IF (ENABLE_MPI_COORDINATOR)
  SET(DEP_LIBS ${DEP_LIBS} ${MPI_LIBRARIES})
ENDIF()

#------------------------------------------------------------------------------
# Set up test macros
#------------------------------------------------------------------------------

MACRO (BUILD_TEST test_name file)
  ADD_EXECUTABLE (${test_name} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
  TARGET_LINK_LIBRARIES (${test_name} libversos ${DEP_LIBS})
ENDMACRO ()

MACRO (BUILD_AND_ADD_TEST test_name file)
  BUILD_TEST(${test_name} ${file})
  ADD_TEST(${test_name} ${test_name})
ENDMACRO()

IF (ENABLE_MPI_COORDINATOR)
  MACRO (ADD_MPI_TEST FILE)
    FOREACH(F ${ARGV})
      IF ((NOT (MPIEXEC_MAX_NUMPROCS LESS F)) AND (NOT (${F} STREQUAL ${FILE})))
        ADD_TEST (NAME ${FILE}_${F} COMMAND ${MPIEXEC} ${MPIEXEC_PREFLAGS}
          ${MPIEXEC_NUMPROC_FLAG} ${F} ${MPIEXEC_POSTFLAGS} $<TARGET_FILE:${FILE}>)
      ENDIF ()
    ENDFOREACH ()
  ENDMACRO ()
  MACRO (BUILD_AND_ADD_MPI_TEST name file)
    BUILD_TEST(name file)
    ADD_MPI_TEST(name COMMAND name)
ENDMACRO()

ENDIF()

#------------------------------------------------------------------------------
# Build and define tests
#------------------------------------------------------------------------------

BUILD_AND_ADD_TEST (singleclientcoordinator_test "coordination/singleclientcoordinator_test.cpp")

IF (ENABLE_MPI_COORDINATOR)
  BUILD_AND_ADD_MPI_TEST(somename "some/path/to/the/test.cpp" 2 4 8 16)
ENDIF()

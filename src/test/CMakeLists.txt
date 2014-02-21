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

IF (!OPENSSL_FOUND)
  MESSAGE (FATAL_ERROR "OpenSSL required")
ENDIF ()

IF (NOT TARGET versos)
  MESSAGE (FATAL_ERROR "versos target not found")
ENDIF ()

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

MACRO (BUILD_TEST bin_name src_name)
  ADD_EXECUTABLE (${bin_name} ${CMAKE_CURRENT_SOURCE_DIR}/${src_name})
  TARGET_LINK_LIBRARIES (${bin_name} versos ${DEP_LIBS})
ENDMACRO ()

MACRO (BUILD_AND_ADD_TEST bin_name src_name)
  BUILD_TEST(${bin_name} ${src_name})
  ADD_TEST(${bin_name} ${bin_name})
ENDMACRO()

IF (ENABLE_MPI_COORDINATOR)

  MACRO (ADD_MPI_TEST test_name np)
    ADD_TEST (NAME ${test_name}_${np} COMMAND ${MPIEXEC} ${MPIEXEC_PREFLAGS}
          ${MPIEXEC_NUMPROC_FLAG} ${np} ${MPIEXEC_POSTFLAGS} $<TARGET_FILE:${test_name}>)
  ENDMACRO ()

  MACRO (BUILD_AND_ADD_MPI_TEST bin_name src_name np_vararg)
    BUILD_TEST(${bin_name} ${src_name})
    FOREACH(F ${ARGV})
      IF (NOT(${F} STREQUAL ${bin_name}) AND
          NOT(${F} STREQUAL ${src_name}) AND
          NOT(MPIEXEC_MAX_NUMPROCS LESS F))
        ADD_MPI_TEST(${bin_name} ${F})
      ENDIF()
    ENDFOREACH ()
  ENDMACRO()

ENDIF()

#------------------------------------------------------------------------------
# Build and define tests
#------------------------------------------------------------------------------

BUILD_AND_ADD_TEST (singleclientcoordinator_test "coordination/singleclientcoordinator_test.cpp")

IF (ENABLE_MPI_COORDINATOR)
  BUILD_AND_ADD_MPI_TEST(mpicoordinator_test "coordination/mpicoordinator_test.cpp" 2 4 8 16)
ENDIF()

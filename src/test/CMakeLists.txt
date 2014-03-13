CMAKE_MINIMUM_REQUIRED (VERSION 2.8.7)

PROJECT (versos_test)

#------------------------------------------------------------------------------
# Check for dependencies, they should have been SET in parent scope
#------------------------------------------------------------------------------

IF (ENABLE_MPI_COORDINATOR)
  IF (!MPI_C_FOUND)
    MESSAGE (FATAL_ERROR "MPI required")
  ENDIF ()
  IF (!Boost_MPI_FOUND)
    MESSAGE (FATAL_ERROR "Boost_MPI required")
  ENDIF ()
ENDIF()

IF (!OPENSSL_FOUND)
  MESSAGE (FATAL_ERROR "OpenSSL required")
ENDIF ()

IF (!Boost_SERIALIZATION_FOUND)
  MESSAGE (FATAL_ERROR "Boost_SERIALIZATION required")
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

SET (GTEST_FLAGS "--gtest_break_on_failure")

SET (DEP_LIBRARIES gtest gtest_main gmock gmock_main ${OPENSSL_LIBRARIES} ${Boost_SERIALIZATION_LIBRARY})

IF (ENABLE_MPI_COORDINATOR)
  SET(DEP_LIBRARIES ${DEP_LIBRARIES} ${MPI_LIBRARIES} ${Boost_MPI_LIBRARY})
ENDIF()

#------------------------------------------------------------------------------
# Set up test macros
#------------------------------------------------------------------------------

MACRO (BUILD_TEST bin_name src_name)
  ADD_EXECUTABLE (${bin_name} ${CMAKE_CURRENT_SOURCE_DIR}/${src_name})
  TARGET_LINK_LIBRARIES (${bin_name} versos ${DEP_LIBRARIES})
ENDMACRO ()

MACRO (BUILD_AND_ADD_TEST bin_name src_name)
  BUILD_TEST(${bin_name} ${src_name})
  ADD_TEST(${bin_name} ${bin_name} ${GTEST_FLAGS})
ENDMACRO()

IF (ENABLE_MPI_COORDINATOR)

  MACRO (ADD_MPI_TEST test_name np)
    ADD_TEST (NAME ${test_name}_${np} COMMAND ${MPIEXEC} ${MPIEXEC_PREFLAGS}
          ${MPIEXEC_NUMPROC_FLAG} ${np} ${MPIEXEC_POSTFLAGS} $<TARGET_FILE:${test_name}> ${GTEST_FLAGS})
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

BUILD_AND_ADD_TEST (repository_test "repository_test.cpp")
BUILD_AND_ADD_TEST (memrefdb_test "refdb/memrefdb_test.cpp")
BUILD_AND_ADD_TEST (singleclientcoordinator_test "coordination/singleclientcoordinator_test.cpp")
BUILD_AND_ADD_TEST (backendcoordinator_test "coordination/backendcoordinator_test.cpp")

IF (ENABLE_MPI_COORDINATOR)
  BUILD_AND_ADD_MPI_TEST(mpicoordinator_test "coordination/mpicoordinator_test.cpp" 2 4 8 16)
ENDIF()

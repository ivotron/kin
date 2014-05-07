CMAKE_MINIMUM_REQUIRED (VERSION 2.8.7)

PROJECT (versos_test)

#------------------------------------------------------------------------------
# Check for dependencies
#------------------------------------------------------------------------------

IF (NOT TARGET versos)
  MESSAGE (FATAL_ERROR "versos target not found")
ENDIF ()

# dependencies for libversos should have been added to VERSOS_DEP_LIBRARIES
SET (TEST_DEP_LIBRARIES ${VERSOS_DEP_LIBRARIES} gtest gtest_main gmock gmock_main)

#------------------------------------------------------------------------------
# Add gmock/gtest libraries
#------------------------------------------------------------------------------

ADD_SUBDIRECTORY (gmock)

INCLUDE_DIRECTORIES ("gmock/include")
INCLUDE_DIRECTORIES ("gmock/gtest/include")

SET (GTEST_FLAGS "--gtest_break_on_failure")

#------------------------------------------------------------------------------
# Set up test macros
#------------------------------------------------------------------------------

MACRO (BUILD_TEST bin_name src_name)
  ADD_EXECUTABLE (${bin_name} ${CMAKE_CURRENT_SOURCE_DIR}/${src_name})
  TARGET_LINK_LIBRARIES (${bin_name} versos ${TEST_DEP_LIBRARIES})
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

IF (ENABLE_REDIS_BACKEND)
  BUILD_AND_ADD_TEST (redisrefdb_test "refdb/redisrefdb_test.cpp")
ENDIF()

IF (ENABLE_MPI_COORDINATOR)
  BUILD_AND_ADD_MPI_TEST(mpicoordinator_test "coordination/mpicoordinator_test.cpp" 2 4 8 16)
ENDIF()

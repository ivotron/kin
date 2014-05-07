CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7)

PROJECT (versos)

###############
# Dependencies
###############

# Core

## Boost
FIND_PACKAGE (Boost 1.46 COMPONENTS serialization REQUIRED)
INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})

## OpenSSL
FIND_PACKAGE (OpenSSL REQUIRED)
INCLUDE_DIRECTORIES (${OPENSSL_INCLUDE_DIR})

SET (VERSOS_DEP_LIBRARIES ${OPENSSL_LIBRARIES} ${Boost_SERIALIZATION_LIBRARY})

# Optional

SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

## MPI
IF (ENABLE_MPI_COORDINATOR)
  FIND_PACKAGE (MPI REQUIRED)
  INCLUDE_DIRECTORIES (${MPI_C_INCLUDE_PATH})
  FIND_PACKAGE (Boost 1.46 COMPONENTS mpi REQUIRED)
  INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})
  ADD_DEFINITIONS(-DENABLE_MPI_COORDINATOR)
  SET(VERSOS_DEP_LIBRARIES ${VERSOS_DEP_LIBRARIES} ${MPI_LIBRARIES} ${Boost_MPI_LIBRARY})
ENDIF()

## Rados
IF (ENABLE_RADOS_BACKEND)
  FIND_PACKAGE (RADOS REQUIRED)
  INCLUDE_DIRECTORIES (${RADOS_INCLUDE_DIRS})
  ADD_DEFINITIONS(-DENABLE_RADOS_BACKEND)
ENDIF()

## Redis
IF (ENABLE_REDIS_BACKEND)
  FIND_PACKAGE (HiRedis REQUIRED)
  INCLUDE_DIRECTORIES (${LIBHIREDIS_INCLUDE_DIRS})
  ADD_DEFINITIONS(-DENABLE_REDIS_BACKEND)
  SET(VERSOS_DEP_LIBRARIES ${VERSOS_DEP_LIBRARIES} ${LIBHIREDIS_LIBRARIES})
ENDIF()

###############
# Source
###############

# Core

SET(VERSOS_SRC
  "src/versos/version.cpp"
  "src/versos/repository.cpp"
  "src/versos/obj/object.cpp"
  "src/versos/obj/kvobject.cpp"
  "src/versos/objdb/memobjdb.cpp"
  "src/versos/refdb/refdb.cpp"
  "src/versos/refdb/memrefdb.cpp"
  "src/versos/coordination/singleclientcoordinator.cpp"
  "src/versos/coordination/backendcoordinator.cpp")

# Optional

IF (ENABLE_MPI_COORDINATOR)
  SET(VERSOS_SRC ${VERSOS_SRC}
    "src/versos/coordination/mpicoordinator.cpp")
ENDIF()

IF (ENABLE_RADOS_BACKEND)
  SET(VERSOS_SRC ${VERSOS_SRC}
    "src/versos/objdb/radosobjdb.cpp")
ENDIF()

IF (ENABLE_REDIS_BACKEND)
  SET(VERSOS_SRC ${VERSOS_SRC}
    "src/versos/util/rediswrapper.cpp"
    "src/versos/objdb/redisobjdb.cpp"
    "src/versos/refdb/redisrefdb.cpp")
ENDIF()

INCLUDE_DIRECTORIES ("src/")
ADD_LIBRARY (versos ${VERSOS_SRC})

#######################
# Tests
#######################

OPTION (BUILD_TOOLS "Build tools." OFF)

IF (BUILD_TOOLS)
  ADD_SUBDIRECTORY (src/tools)
ENDIF()

#######################
# Tests
#######################

OPTION (ENABLE_TESTS "Build all tests." OFF)

IF (ENABLE_TESTS)
  ENABLE_TESTING()
  ADD_SUBDIRECTORY (src/test)
ENDIF()

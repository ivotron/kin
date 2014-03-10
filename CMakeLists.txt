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

# Optional

SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

## MPI
IF (ENABLE_MPI_COORDINATOR)
  FIND_PACKAGE (MPI REQUIRED)
  INCLUDE_DIRECTORIES (${MPI_C_INCLUDE_PATH})
  FIND_PACKAGE (Boost 1.46 COMPONENTS mpi REQUIRED)
  INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})
  ADD_DEFINITIONS(-DENABLE_MPI_COORDINATOR)
ENDIF()

## Ceph
IF (ENABLE_RADOS_OBJECT)
  FIND_PACKAGE (RADOS REQUIRED)
  INCLUDE_DIRECTORIES (${RADOS_INCLUDE_DIRS})
  ADD_DEFINITIONS(-DENABLE_RADOS_OBJECT)
ENDIF()

## HiRedis
IF (ENABLE_REDIS_METADB)
  FIND_PACKAGE (HiRedis REQUIRED)
  INCLUDE_DIRECTORIES (${LIBHIREDIS_INCLUDE_DIRS})
  ADD_DEFINITIONS(-DENABLE_REDIS_METADB)
ENDIF()

###############
# Source
###############

# Core

SET(VERSOS_SRC
  "src/versos/version.cpp"
  "src/versos/repository.cpp"
  "src/versos/refdb/refdb.cpp"
  "src/versos/refdb/memrefdb.cpp"
  "src/versos/coordination/singleclientcoordinator.cpp"
  "src/versos/coordination/backendcoordinator.cpp"
  "src/versos/objectversioning/versionedobject.cpp"
  "src/versos/objectversioning/memversionedobject.cpp")

# Optional

IF (ENABLE_RADOS_OBJECT)
  SET(VERSOS_SRC ${VERSOS_SRC}
    "src/versos/objectversioning/radosversionedobject.cpp")
ENDIF()

IF (ENABLE_MPI_COORDINATOR)
  SET(VERSOS_SRC ${VERSOS_SRC}
    "src/versos/coordination/mpicoordinator.cpp")
ENDIF()

IF (ENABLE_REDIS_METADB)
  SET(VERSOS_SRC ${VERSOS_SRC}
    "src/versos/refdb/rediswrapper.cpp"
    "src/versos/refdb/redisrefdb.cpp")
ENDIF()

INCLUDE_DIRECTORIES ("src/")
ADD_LIBRARY (versos ${VERSOS_SRC})

#######################
# Tests
#######################

OPTION (ENABLE_TESTS "Build all tests." OFF)

IF (ENABLE_TESTS)
  ENABLE_TESTING()
  ADD_SUBDIRECTORY (src/test)
ENDIF()

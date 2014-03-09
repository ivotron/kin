CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7)

PROJECT (versos)

###############
# Dependencies
###############

# Boost
FIND_PACKAGE (Boost 1.46 COMPONENTS serialization REQUIRED)
INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})

# MPI
IF (ENABLE_MPI_COORDINATOR)
  FIND_PACKAGE (MPI REQUIRED)
  INCLUDE_DIRECTORIES (${MPI_C_INCLUDE_PATH})
  FIND_PACKAGE (Boost 1.46 COMPONENTS mpi REQUIRED)
  INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})
  ADD_DEFINITIONS(-DENABLE_MPI_COORDINATOR)
ENDIF()

# OpenSSL
FIND_PACKAGE (OpenSSL REQUIRED)
INCLUDE_DIRECTORIES (${OPENSSL_INCLUDE_DIR})

# Ceph
SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
FIND_PACKAGE (RADOS REQUIRED)
INCLUDE_DIRECTORIES (${RADOS_INCLUDE_DIRS})

###############
# Libs
###############

INCLUDE_DIRECTORIES ("src/")

SET(VERSOS_SRC
  "src/versos/version.cpp"
  "src/versos/repository.cpp"
  "src/versos/refdb/refdb.cpp"
  "src/versos/refdb/memrefdb.cpp"
  "src/versos/refdb/rediswrapper.cpp"
  "src/versos/refdb/redisrefdb.cpp"
  "src/versos/coordination/singleclientcoordinator.cpp"
  "src/versos/objectversioning/versionedobject.cpp"
  "src/versos/objectversioning/memversionedobject.cpp"
  "src/versos/objectversioning/radosversionedobject.cpp"
)

IF (ENABLE_MPI_COORDINATOR)
  SET(VERSOS_SRC ${VERSOS_SRC} "src/versos/coordination/mpicoordinator.cpp")
ENDIF()

ADD_LIBRARY (versos ${VERSOS_SRC})

OPTION (ENABLE_TESTS "Build all tests." OFF)

IF (ENABLE_TESTS)
  ENABLE_TESTING()
  ADD_SUBDIRECTORY (src/test)
ENDIF()

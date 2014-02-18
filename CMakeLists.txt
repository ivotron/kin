CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7)

PROJECT (versos)

###############
# Dependencies
###############

# MPI
FIND_PACKAGE (MPI REQUIRED)
INCLUDE_DIRECTORIES (${MPI_C_INCLUDE_PATH})

# Boost
FIND_PACKAGE (Boost 1.46 REQUIRED)
INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})

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
  "src/versos/refdb/memrefdb.cpp"
  "src/versos/coordination/singleclientcoordinator.cpp"
  #"src/versos/coordination/mpicoordinator.cpp"
  "src/versos/objectversioning/radosversionedobject.cpp"
)

ADD_LIBRARY (libversos ${VERSOS_SRC})

OPTION (ENABLE_TESTS "Build all tests." OFF)

IF (ENABLE_TESTS)
  ENABLE_TESTING()
  ADD_SUBDIRECTORY (src/test)
ENDIF()

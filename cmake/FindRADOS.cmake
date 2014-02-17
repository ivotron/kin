# Find RADOS
#
# Find the native or dev RADOS includes and library (gives priority to native)
#
# RADOS_INCLUDE_DIRS - where to find librados.h, etc.
# RADOS_LIBRARIES - List of libraries when using RADOS.
# RADOS_FOUND - True if RADOS found.


IF (RADOS_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(RADOS_FIND_QUIETLY TRUE)
ENDIF ()

FIND_PATH(
  RADOS_INCLUDE_DIRS
  rados
  /usr/local/include
  $ENV{HOME}/ceph/src/include/
  $ENV{HOME}/projects/ceph/src/include/ )
MARK_AS_ADVANCED(RADOS_INCLUDE_DIRS)

FIND_LIBRARY(
  RADOS_LIBRARIES
  rados
  /usr/local/lib
  $ENV{HOME}/ceph/src/.libs
  $ENV{HOME}/projects/ceph/src/.libs )
MARK_AS_ADVANCED(RADOS_LIB)

IF (RADOS_INCLUDE_DIRS AND RADOS_LIBRARIES)
  SET(RADOS_FOUND TRUE)
ELSE ()
   SET(RADOS_FOUND FALSE)
ENDIF ()

IF (RADOS_FOUND)
   MESSAGE(STATUS "Found RADOS: ${RADOS_LIBRARIES}")
ELSE ()
   MESSAGE(STATUS "Did not find RADOS libraries")
   IF (RADOS_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could NOT find RADOS libraries (librados)")
   ENDIF ()
ENDIF ()

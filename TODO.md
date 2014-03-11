# general

  - use exceptions instead of error codes

# repository

  - move refdb opening/initialization to the coordinator, since the 
    coordinator is the one that owns the responsibility of // talking 
    to the DB

# version

  - create unit test

# syncModes

  - remove "EACH" from names
  - breake AT_ADD_OR_REMOVE into AT_ADD, AT_REMOVE, separately

# coordinators

## all

  - every client is not fault-tolerant (what if a client dies?)
  - for AT_EACH_ADD_OR_REMOVE, instead of sending the whole list of 
    objects that each client is working on, we can optimize this by 
    "flattening" of a version's internal state.

## singleclientcoordinator

  - cover all syncModes in unit test

## backendCoordinator

  - cover all syncModes in unit test
  - check if a

## mpiCoordinator

  - support commented-out tests in unit test
  - throw an exception if two or more ranks added/removed the same 
    object

# refdb

## redisrefdb

  - the counter and the object lists are a potential bottleneck. How 
    can we avoid this?

## mpirefdb

  - create an MPI-based backend

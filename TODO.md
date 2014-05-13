# repository

# version

  - create unit test

# syncModes

  - remove "EACH" from names

# coordinators

## all

  - coordinators aren't fault-tolerant (what if a client dies?)
  - for AT_EACH_ADD_OR_REMOVE, instead of sending the whole list of 
    objects that each client is working on, we can optimize this by 
    "flattening" a version's internal state.
  - add unit tests for conflicting scenarios (more than one client 
    working on the same object)

## singleclientcoordinator

  - cover all syncModes in unit test

## backendCoordinator

  - cover all syncModes in unit test.
  - unit tests execute `FLUSHALL` which is overkill for a production 
    environment
  - when a version is committed and `sync_mode` is `AT_ADD_OR_REMOVE` 
    or `AT_COMMIT`, only the last client that observes `lockCount == 
    0` should call `o.commit()` for all the objects. Also, running in 
    `AT_CREATE`, the client placing the first lock is in charge of 
    calling `o.commit()`.
  - when a checkout is done and the retrieved version is not 
    committed, we HAVE to consult the RefDB, since the version might 
    have been committed by other client
  - when a create is done, we should return `Version::ERROR` if the 
    lock count of the given parent version is not zero.

## mpiCoordinator

  - support commented-out tests in unit test
  - throw an exception if two or more ranks added/removed the same 
    object

# refdb

  - refactor ID assignment. This should be implementation-specific:
      - SHA1
      - timestamp
      - sequence number
  - make use of the lockKey

## memrefdb

## redisrefdb

 1. the counter and the object lists are a potential bottleneck. How 
    can we avoid this?
 2. the parent of a version is registered when the version is 
    committed. Ideally, we would like to register the parent of a 
    version when we create the version.
 3. we might want to impose the restriction over the lock counter 
    that once a counter has started to decrease, i.e. clients have 
    started to commit, we disallow the lock increase. In other words, 
    once a set of clients "registered" by increasing the counter, no 
    other new client should "come late to the party".
 4. With 3, we can solve 2 by registering the parent id of a version 
    when a version is created i.e. `cnt == 0`. This is safe to do 
    since by 3, we would have the certainty that `cnt` won't decrease 
    and then increase again, once the counter has started to decrease, 
    it will keep doing so.


# obj

  - add unit tests for KVObject

# objdb

  - add unit tests for exec()

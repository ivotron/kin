# immediate:

  - finish backendcoordinator:
      - every sync point has to synchronize the object list

  - implement mpirefdb. This would dup the communicator used by the 
    mpicoordinator and synchronize the memrefdb in the background

  - measure backendcoordinator with vs. without mpirefdb to see if 
    there's any interference between communicators in terms of 
    performance:
      - if there isn't, then we can use that to show results
      - if it is, then we need to implement a redisrefdb

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

  - make use of the lockKey

## memrefdb

  - when inquiring about the status

## redisrefdb

  - the counter and the object lists are a potential bottleneck. How 
    can we avoid this?

## mpirefdb

  - create an MPI-based backend

----------------

# Options affecting overall behavior

## coordination modes

### publishing (when are my adds/removes visible on the meta db?)

  - never
  - at_create
  - at_commit
  - at_add_or_remove

### conflict detection (what should I know about what others' adds/removes?)

  - nothing (only what I did is what I know)
  - everything (everybody knows what everybody did)
  - neighborhood (only a subset of clients and I share our lists)

### conflict resolution (what do I do if other's adds/removes conflict with mine)

  - nothing (ultra optimistic)
  - fail_on_conflicting_add
  - fail_on_conflicting_remove
  - fail_on_any_conflict
  - concurrency_coordination (when more than two clients writing to 
    the same object, how do we coordinate our writes):
      - lock
      - share_offsets (we share our offsets and only coordinate on 
        conflicting ranges)
      - optimistic
      - etc...

### publishing intervals (when I create a new version over a parent 
who has already objects in it, how do I announce my intentions on what 
I'll do to them)

  - none (assume complete independence and hope for the best, i.e. 
    hope that working sets won't overlap)
  - from_parent (whatever my parent version has, that's what my 
    working set will have; this will be terrible if combined with 
    locks)
  - per_txn_explicit (every new version "resets" the working set what 
    I'm writing to, so I have to explicitly add objects again, even 
    though they might have been in my parent's version)
  - per_txn_implicit_on_write (when a write occurs, we implicitly add 
    that object to the clients working set)

## backend coordinator:

  - change `Version` object status to committed when a client commits, 
    regardless of whether the lock count got to 0
  - the opposite of the above

## refdb

  - when a client commits a version, no more locks can be placed on 
    the same version
  - the opposite

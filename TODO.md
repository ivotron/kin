
We first describe all the possible coordination scenarios

# Options affecting overall behavior

## coordination modes

### 1. publishing (when are my adds/removes visible on the meta db?)

  1. never
  2. at_create
  3. at_commit
  4. at_add_or_remove

### 2. conflict detection (what should I know about what others' adds/removes?)

  1. nothing (only what I did is what I know)
  2. everything (everybody knows what everybody did)
  3. neighborhood (only a subset of clients and I share our lists)

### 3. conflict resolution (what do I do if other's adds/removes conflict with mine)

  1. nothing (ultra optimistic)
  2. fail_on_conflicting_add
  3. fail_on_conflicting_remove
  4. fail_on_any_conflict
  5. concurrency_coordination (when more than two clients writing to the same object, how do we coordinate our writes):
      1. locking
      2. optimistic
      3. share_offsets (we share our offsets and only coordinate on conflicting ranges)
      4. etc...

### 4. publishing intervals (when I create a new version over a parent who has already objects in it, how do I announce my intentions on what I'll do to them) 

  1. none (assume complete independence and hope for the best, i.e. hope that working sets won't overlap)
  2. from_parent (whatever my parent version has, that's what my working set will have; this will be terrible if combined with locks)
  3. per_txn_explicit (every new version "resets" the working set what I'm writing to, so I have to explicitly add objects again, even though they might have been in my parent's version)
  4. per_txn_implicit_on_write (when a write occurs, we implicitly add that object to the clients working set)

## backend coordinator:

  - change `Version` object status to committed when a client commits, 
    regardless of whether the lock count got to 0
  - the opposite of the above

## refdb

  - when a client commits a version, no more locks can be placed on 
    the same version
  - the opposite

----------

**TO-DO**

# immediate:

  - add support for all the scenarios described above. We don't need 
    to handle them, we might as well throw exceptions for most of 
    them, but we need to add the structural paths that get to each of 
    them for each coordinator



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

  - when inquiring about the status

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

## mpirefdb

  - create an MPI-based backend

----------------


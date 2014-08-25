# testing
  * using mocks, write tests for factory and singleclient

# general

  * add `stage` command (see below)
  * add `get` command to fetch BLOB objects

# git

## stage
  * It will be easier to leverage the `git` command to handle all 
  branching, checkout. Since implementing the "detached HEAD" mode is 
  involved, we can instead create a specific `stage` command that is 
  distinct from `checkout` so that the flow is: `checkout` first to 
  see read-only stuff and explicitly run `stage` to make changes. In 
  this way, all we have to do is lock objects after invoking `git 
  checkout` so that they are read-only. `kin stage` would unlock them 
  in order to make them writable

## checkout

  * Change the status of a commit after the staging has been done. 
  Right know, the status is always committed which disallows adding or 
  removing.

# posix

## unlock

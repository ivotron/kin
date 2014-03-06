#include "versos/repository.h"
#include "versos/objectversioning/memversionedobject.h"
#include "versos/utils.h"

#include <mpi.h>
#include <gtest/gtest.h>

MPI_Comm comm;
int myRank;
int commSize;

TEST(mpicoordinator_test, factory)
{
  versos::Options o;

  o.coordinator_type = versos::Options::Coordinator::MPI;
  o.sync_mode = versos::Options::ClientSync::NONE;

  // TODO: handle this case with a MPI mock since right now this causes MPI_Abort() instead of a throw
  //
  // ASSERT_ANY_THROW(versos::Repository repo("mydataset", o));

  o.metadb_initialize_if_empty = true;

  // TODO: same as above
  //
  // ASSERT_ANY_THROW(versos::Repository repo("mydataset", o));

  o.mpi_comm = comm;

  ASSERT_NO_THROW(versos::Repository repo("mydataset", o));

  // TODO: same as above
  //
  // o.mpi_leader_rank = -1;
  //
  //ASSERT_ANY_THROW(versos::Repository repo("mydataset", o));

  // TODO: same as above
  //
  // int size;
  //
  // MPI_Comm_size(comm, &size);
  //
  // o.mpi_leader_rank = size;
  //
  //ASSERT_ANY_THROW(versos::Repository repo("mydataset", o));

  o.mpi_leader_rank = 0;

  ASSERT_NO_THROW(versos::Repository repo("mydataset", o));

  // TODO: check factory for other types of sync_modes
}

TEST(mpicoordinator_test, NONE_object_containment)
{
  versos::Options o;

  o.metadb_type = versos::Options::MetaDB::MEM;
  o.metadb_initialize_if_empty = true;
  o.coordinator_type = versos::Options::Coordinator::MPI;
  o.mpi_leader_rank = 0;
  o.mpi_comm = comm;
  o.sync_mode = versos::Options::ClientSync::NONE;
  o.hash_seed = "theseedforthetest";

  versos::Repository repo("mydataset", o);

  const versos::Version& head = repo.checkoutHEAD();

  ASSERT_TRUE(head.isCommitted()) << "at rank " << myRank;
  ASSERT_EQ(0u, head.size());
  ASSERT_EQ(versos::Version::PARENT_FOR_ROOT.getId(), head.getId());

  ASSERT_TRUE(head.isOK());

  versos::Version& v1 = repo.create(head);

  ASSERT_TRUE(v1.isOK());
  ASSERT_FALSE(v1.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v1.getStatus());
  ASSERT_EQ(0u, v1.size());
  ASSERT_NE(versos::Version::PARENT_FOR_ROOT.getId(), v1.getId());

  versos::MemVersionedObject o1(repo, "o" + Utils::to_str(myRank));

  ASSERT_NE(0, repo.add(v1, o1));

  ASSERT_FALSE(v1.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v1.getStatus());
  ASSERT_EQ(0u, v1.size());
  ASSERT_FALSE(v1.contains(o1));

  ASSERT_EQ(0, repo.commit(v1));

  ASSERT_EQ(v1, repo.checkoutHEAD());
  ASSERT_TRUE(v1.isCommitted());
  ASSERT_EQ(0u, v1.size());
  MPI_Barrier(comm);

  versos::Version& v2 = repo.create(v1);

  ASSERT_NE(v1, v2);
  ASSERT_TRUE(v2.isOK());
  ASSERT_FALSE(v2.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v2.getStatus());
  ASSERT_EQ(0u, v2.size());
  ASSERT_FALSE(v2.contains(o1));

  ASSERT_NE(0, repo.remove(v2, o1));

  ASSERT_FALSE(v2.isCommitted());
  ASSERT_EQ(0u, v2.size());
  ASSERT_FALSE(v2.contains(o1));

  ASSERT_EQ(0, repo.commit(v2));

  ASSERT_EQ(v2, repo.checkoutHEAD());
  ASSERT_TRUE(v2.isCommitted());
  ASSERT_EQ(0u, v2.size());
  ASSERT_FALSE(v2.contains(o1));

  // exec. summary :)
  ASSERT_FALSE(head.contains(o1));
  ASSERT_FALSE(v1.contains(o1));
  ASSERT_FALSE(v2.contains(o1));
}

TEST(mpicoordinator_test, AT_EACH_COMMIT_object_containment)
{
  versos::Options o;

  o.metadb_type = versos::Options::MetaDB::MEM;
  o.metadb_initialize_if_empty = true;
  o.coordinator_type = versos::Options::Coordinator::MPI;
  o.mpi_leader_rank = 0;
  o.mpi_comm = comm;
  o.sync_mode = versos::Options::ClientSync::AT_EACH_COMMIT;
  o.hash_seed = "theseedforthetest";

  versos::Repository repo("mydataset", o);

  const versos::Version& head = repo.checkoutHEAD();

  ASSERT_TRUE(head.isCommitted());
  ASSERT_EQ(0u, head.size());
  ASSERT_EQ(versos::Version::PARENT_FOR_ROOT.getId(), head.getId());

  ASSERT_TRUE(head.isOK());

  versos::Version& v1 = repo.create(head);

  ASSERT_NE(head, v1);
  ASSERT_TRUE(v1.isOK());
  ASSERT_FALSE(v1.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v1.getStatus());
  ASSERT_EQ(0u, v1.size());
  ASSERT_NE(versos::Version::PARENT_FOR_ROOT.getId(), v1.getId());

  versos::MemVersionedObject o1(repo, "o1-" + Utils::to_str(myRank));

  ASSERT_EQ(0, repo.add(v1, o1));

  ASSERT_FALSE(v1.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v1.getStatus());

  // we synchronize after each commit so we should have 1 in total
  ASSERT_EQ(1u, v1.size());
  ASSERT_TRUE(v1.contains(o1));
  for (int i = 0; i < commSize; ++i)
  {
    versos::MemVersionedObject o(repo, "o1-" + Utils::to_str(i));

    if (i == myRank)
      ASSERT_TRUE(v1.contains(o));
    else
      ASSERT_FALSE(v1.contains(o));
  }

  ASSERT_EQ(0, repo.commit(v1));

  ASSERT_EQ(v1, repo.checkoutHEAD());
  ASSERT_TRUE(v1.isCommitted());

  // we should have commSize now
  ASSERT_EQ((unsigned int) commSize, v1.size());
  for (int i = 0; i < commSize; ++i)
  {
    versos::MemVersionedObject o(repo, "o1-" + Utils::to_str(i));
    ASSERT_TRUE(v1.contains(o));
  }

  versos::Version& v2 = repo.create(v1);

  ASSERT_NE(v1, v2);
  ASSERT_TRUE(v2.isOK());
  ASSERT_FALSE(v2.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v2.getStatus());

  // we should still have commSize
  ASSERT_EQ((unsigned int) commSize, v2.size());
  for (int i = 0; i < commSize; ++i)
  {
    versos::MemVersionedObject o(repo, "o1-" + Utils::to_str(i));
    ASSERT_TRUE(v1.contains(o));
  }

  // let's add one more object from each rank
  versos::MemVersionedObject o2(repo, "o2-" + Utils::to_str(myRank));

  ASSERT_EQ(0, repo.add(v2, o2));
  ASSERT_FALSE(v2.isCommitted());

  // we added locally, so we should have only the local object
  ASSERT_EQ(commSize + 1u, v2.size());
  for (int i = 0; i < commSize; ++i)
  {
    versos::MemVersionedObject o(repo, "o2-" + Utils::to_str(i));

    if (i == myRank)
      ASSERT_TRUE(v2.contains(o));
    else
      ASSERT_FALSE(v2.contains(o));
  }

  // let's add a third one on odd-numbered ranks
  versos::MemVersionedObject o3(repo, "o3-" + Utils::to_str(myRank));

  if (myRank % 2 == 1)
    ASSERT_EQ(0, repo.add(v2, o3));

  ASSERT_FALSE(v2.isCommitted());

  // we added locally on odd-numbered ranks, so we should have only the local object
  if (myRank % 2 == 1)
    ASSERT_EQ(commSize + 2u, v2.size());
  else
    ASSERT_EQ(commSize + 1u, v2.size());

  for (int i = 0; i < commSize; ++i)
  {
    versos::MemVersionedObject o(repo, "o3-" + Utils::to_str(i));

    if (i == myRank && myRank % 2 == 1)
      ASSERT_TRUE(v2.contains(o));
    else
      ASSERT_FALSE(v2.contains(o));
  }

  // TODO: adding unevenly from distinct ranks is not supported, instead of returning, the code below aborts, 
  // so we should either test using MPI mocks or by returning instead of aborting
  //
  // ASSERT_NE(0, repo.commit(v2));

  // ok, so let's add the object to the even-numbered ranks and commit again
  if (myRank % 2 == 0)
    ASSERT_EQ(0, repo.add(v2, o3));

  ASSERT_EQ(0, repo.commit(v2));

  ASSERT_EQ(v2, repo.checkoutHEAD());
  ASSERT_TRUE(v2.isCommitted());

  ASSERT_EQ(commSize * 3u, v2.size());

  for (int i = 0; i < commSize; ++i)
  {
    versos::MemVersionedObject oTwo(repo, "o2-" + Utils::to_str(i));
    versos::MemVersionedObject oThree(repo, "o3-" + Utils::to_str(i));

    ASSERT_TRUE(v2.contains(oTwo));
    ASSERT_TRUE(v2.contains(oThree));
  }

  versos::Version& v3 = repo.create(v2);

  ASSERT_NE(v2, v3);
  ASSERT_TRUE(v3.isOK());
  ASSERT_FALSE(v3.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v3.getStatus());

  // now let's remove the one added in v1
  ASSERT_EQ(0, repo.remove(v3, o1));
  ASSERT_FALSE(v3.isCommitted());

  // we removed locally, so we should have only one less
  ASSERT_EQ(v2.size() - 1u, v3.size());
  for (int i = 0; i < commSize; ++i)
  {
    versos::MemVersionedObject o(repo, "o1-" + Utils::to_str(i));

    if (i == myRank)
      ASSERT_FALSE(v3.contains(o));
    else
      ASSERT_TRUE(v3.contains(o));
  }

  ASSERT_EQ(0, repo.commit(v3));

  ASSERT_EQ(v3, repo.checkoutHEAD());
  ASSERT_TRUE(v3.isCommitted());

  // we synchronized now, so we should have commSize less
  ASSERT_EQ(v2.size() - commSize, v3.size());
  for (int i = 0; i < commSize; ++i)
  {
    versos::MemVersionedObject o(repo, "o1-" + Utils::to_str(i));
    ASSERT_FALSE(v3.contains(o));
  }
  // exec. summary :)
  ASSERT_FALSE(head.contains(o1));
  ASSERT_FALSE(head.contains(o2));
  ASSERT_FALSE(head.contains(o3));
  ASSERT_TRUE(v1.contains(o1));
  ASSERT_FALSE(v1.contains(o2));
  ASSERT_FALSE(v1.contains(o3));
  ASSERT_TRUE(v2.contains(o1));
  ASSERT_TRUE(v2.contains(o2));
  ASSERT_TRUE(v2.contains(o3));
  ASSERT_FALSE(v3.contains(o1));
  ASSERT_TRUE(v3.contains(o2));
  ASSERT_TRUE(v2.contains(o3));
  ASSERT_TRUE(v3.contains(o3));
}

TEST(mpicoordinator_test, AT_EACH_ADD_OR_REMOVE_object_containment)
{
  versos::Options o;

  o.metadb_type = versos::Options::MetaDB::MEM;
  o.metadb_initialize_if_empty = true;
  o.coordinator_type = versos::Options::Coordinator::MPI;
  o.mpi_leader_rank = 0;
  o.mpi_comm = comm;
  o.sync_mode = versos::Options::ClientSync::AT_EACH_ADD_OR_REMOVE;
  o.hash_seed = "theseedforthetest";

  versos::Repository repo("mydataset", o);

  const versos::Version& head = repo.checkoutHEAD();

  ASSERT_TRUE(head.isCommitted());
  ASSERT_EQ(0u, head.size());
  ASSERT_EQ(versos::Version::PARENT_FOR_ROOT.getId(), head.getId());

  ASSERT_TRUE(head.isOK());

  versos::Version& v1 = repo.create(head);

  ASSERT_NE(head, v1);
  ASSERT_TRUE(v1.isOK());
  ASSERT_FALSE(v1.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v1.getStatus());
  ASSERT_EQ(0u, v1.size());
  ASSERT_NE(versos::Version::PARENT_FOR_ROOT.getId(), v1.getId());

  versos::MemVersionedObject o1(repo, "o1-" + Utils::to_str(myRank));

  ASSERT_EQ(0, repo.add(v1, o1));

  ASSERT_FALSE(v1.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v1.getStatus());

  // we synchronize after each add/remove so we should have commSize in total
  ASSERT_EQ((unsigned int) commSize, v1.size());
  ASSERT_TRUE(v1.contains(o1));
  for (int i = 0; i < commSize; ++i)
  {
    versos::MemVersionedObject o(repo, "o1-" + Utils::to_str(i));
    ASSERT_TRUE(v1.contains(o));
  }

  ASSERT_EQ(0, repo.commit(v1));

  ASSERT_EQ(v1, repo.checkoutHEAD());
  ASSERT_TRUE(v1.isCommitted());

  // we should still have commSize
  ASSERT_EQ((unsigned int) commSize, v1.size());
  for (int i = 0; i < commSize; ++i)
  {
    versos::MemVersionedObject o(repo, "o1-" + Utils::to_str(i));
    ASSERT_TRUE(v1.contains(o));
  }

  versos::Version& v2 = repo.create(v1);

  ASSERT_NE(v1, v2);
  ASSERT_TRUE(v2.isOK());
  ASSERT_FALSE(v2.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v2.getStatus());
  ASSERT_EQ((unsigned int) commSize, v2.size());
  for (int i = 0; i < commSize; ++i)
  {
    versos::MemVersionedObject o(repo, "o1-" + Utils::to_str(i));
    ASSERT_TRUE(v1.contains(o));
  }

  versos::MemVersionedObject o2(repo, "o2-" + Utils::to_str(myRank));
  versos::MemVersionedObject o3(repo, "o3-" + Utils::to_str(myRank));

  // let's add one more object
  ASSERT_EQ(0, repo.add(v2, o2));
  ASSERT_FALSE(v2.isCommitted());

  // we added at every rank, so we should have one more object
  ASSERT_EQ(commSize * 2u, v2.size());
  for (int i = 0; i < commSize; ++i)
  {
    versos::MemVersionedObject o(repo, "o2-" + Utils::to_str(i));
    ASSERT_TRUE(v2.contains(o));
  }

  // let's add a third one
  ASSERT_EQ(0, repo.add(v2, o3));
  ASSERT_FALSE(v2.isCommitted());

  MPI_Barrier(comm);
  // we added at every rank, so we should have one more object
  ASSERT_EQ(commSize * 3u, v2.size());
  for (int i = 0; i < commSize; ++i)
  {
    versos::MemVersionedObject o(repo, "o3-" + Utils::to_str(i));
    ASSERT_TRUE(v2.contains(o));
  }
  MPI_Barrier(comm);

  ASSERT_EQ(0, repo.commit(v2));

  ASSERT_EQ(v2, repo.checkoutHEAD());
  ASSERT_TRUE(v2.isCommitted());

  ASSERT_EQ(commSize * 3u, v2.size());
  ASSERT_TRUE(v2.contains(o1));

  versos::Version& v3 = repo.create(v2);

  ASSERT_NE(v2, v3);
  ASSERT_TRUE(v3.isOK());
  ASSERT_FALSE(v3.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v3.getStatus());

  // now let's remove the one added in v1
  ASSERT_EQ(0, repo.remove(v3, o1));
  ASSERT_FALSE(v3.isCommitted());

  // we removed globally, so we should have commSize num of objects less
  ASSERT_EQ(commSize * 2u, v3.size());
  for (int i = 0; i < commSize; ++i)
  {
    versos::MemVersionedObject o(repo, "o1-" + Utils::to_str(i));
    ASSERT_FALSE(v3.contains(o));
  }

  ASSERT_EQ(0, repo.commit(v3));

  ASSERT_EQ(v3, repo.checkoutHEAD());
  ASSERT_TRUE(v3.isCommitted());
  ASSERT_EQ(commSize * 2u, v3.size());
  for (int i = 0; i < commSize; ++i)
  {
    versos::MemVersionedObject o(repo, "o1-" + Utils::to_str(i));
    ASSERT_FALSE(v3.contains(o));
  }

  // exec. summary :)
  ASSERT_FALSE(head.contains(o1));
  ASSERT_FALSE(head.contains(o2));
  ASSERT_FALSE(head.contains(o3));
  ASSERT_TRUE(v1.contains(o1));
  ASSERT_FALSE(v1.contains(o2));
  ASSERT_FALSE(v1.contains(o3));
  ASSERT_TRUE(v2.contains(o1));
  ASSERT_TRUE(v2.contains(o2));
  ASSERT_TRUE(v2.contains(o3));
  ASSERT_FALSE(v3.contains(o1));
  ASSERT_TRUE(v3.contains(o2));
  ASSERT_TRUE(v3.contains(o3));
}

TEST(mpicoordinator_test, wrong_mode_for_object_containment_operations)
{
  versos::Options o;

  o.metadb_type = versos::Options::MetaDB::MEM;
  o.metadb_initialize_if_empty = true;
  o.coordinator_type = versos::Options::Coordinator::MPI;
  o.sync_mode = versos::Options::ClientSync::NONE;
  o.mpi_leader_rank = 0;
  o.mpi_comm = comm;
  o.hash_seed = "theseedforthetest";

  versos::Repository repo("mydataset", o);

  const versos::Version& head = repo.checkoutHEAD();
  versos::Version& v1 = repo.create(head);

  versos::MemVersionedObject ob(repo, "o1-" + Utils::to_str(myRank));

  // since we're in NONE mode, we get errors
  ASSERT_NE(0, repo.add(v1, ob));

  // we have to create repo with default AT_EACH_COMMIT (default) or AT_EACH_ADD_OR_REMOVE in order to be able 
  // to add to it, see next test:
}

TEST(mpicoordinator_test, values_between_tests)
{
  versos::Options o;

  o.metadb_type = versos::Options::MetaDB::MEM;
  o.metadb_initialize_if_empty = true;
  o.coordinator_type = versos::Options::Coordinator::MPI;
  o.mpi_leader_rank = 0;
  o.mpi_comm = comm;
  o.hash_seed = "theseedforthetest";

  versos::Repository repo("mydataset", o);

  const versos::Version& head = repo.checkoutHEAD();
  versos::Version& v1 = repo.create(head);

  versos::MemVersionedObject o1(repo, "o1-" + Utils::to_str(myRank));

  ASSERT_EQ(0, repo.add(v1, o1));

  ASSERT_EQ(0, o1.write(v1, "first"));

  ASSERT_EQ(0, repo.commit(v1));

  versos::Version& v2 = repo.create(v1);

  ASSERT_TRUE(v2.isOK());
  ASSERT_NE(v1, v2);

  ASSERT_EQ(0, o1.write(v2, "second"));

  std::string o1v1;
  std::string o1v2;

  ASSERT_EQ(0, o1.read(v1, o1v1));
  ASSERT_EQ(0, o1.read(v2, o1v2));

  ASSERT_EQ("first", o1v1);
  ASSERT_EQ("second", o1v2);

  versos::MemVersionedObject o2(repo, "o2-" + Utils::to_str(myRank));
  // TODO: the following aborts:
  //
  // ASSERT_NE(0, repo.add(v1, o2));
  ASSERT_EQ(0, repo.add(v2, o2));
  ASSERT_EQ(0, repo.commit(v2));
}

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);

  comm = MPI_COMM_WORLD;
  MPI_Comm_rank(comm, &myRank);
  MPI_Comm_size(comm, &commSize);

  ::testing::InitGoogleTest(&argc, argv);

  int out = RUN_ALL_TESTS();

  MPI_Finalize();

  return out;
}

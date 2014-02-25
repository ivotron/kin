#include "versos/repository.h"
#include "versos/objectversioning/memversionedobject.h"
#include "versos/utils.h"

#include <mpi.h>
#include <gtest/gtest.h>

MPI_Comm comm;
int myrank;

TEST(mpicoordinator_test, factory)
{
  versos::Options o;

  o.coordinator_type = versos::Options::Coordinator::MPI;

  ASSERT_ANY_THROW(versos::Repository repo("mydataset", o));

  o.mpi_comm = comm;

  ASSERT_NO_THROW(versos::Repository repo("mydataset", o));

  o.mpi_leader_rank = -1;

  ASSERT_ANY_THROW(versos::Repository repo("mydataset", o));

  int size;

  MPI_Comm_size(comm, &size);

  o.mpi_leader_rank = size;

  ASSERT_ANY_THROW(versos::Repository repo("mydataset", o));

  o.mpi_leader_rank = 0;

  ASSERT_NO_THROW(versos::Repository repo("mydataset", o));
}

TEST(mpicoordinator_test, basic_commit_of_root)
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

  ASSERT_TRUE(head.isCommitted());
  ASSERT_EQ(versos::Version::COMMITTED, head.getStatus());
  ASSERT_EQ(0u, head.getObjects().size());
  ASSERT_EQ(versos::Version::PARENT_FOR_ROOT.getId(), head.getId());

  ASSERT_TRUE(head.isOK());

  versos::Version& v1 = repo.create(head);

  ASSERT_TRUE(v1.isOK());
  ASSERT_TRUE(!v1.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v1.getStatus());
  ASSERT_EQ(0u, v1.getObjects().size());
  ASSERT_NE(versos::Version::PARENT_FOR_ROOT.getId(), v1.getId());

  versos::MemVersionedObject o1(repo, "o" + Utils::to_str(myrank));

  ASSERT_EQ(0, repo.add(v1, o1));

  ASSERT_TRUE(!v1.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v1.getStatus());
  ASSERT_EQ(1u, v1.getObjects().size());
  ASSERT_TRUE(v1.contains(o1));

  ASSERT_EQ(0, repo.commit(v1));

  ASSERT_EQ(v1, repo.checkoutHEAD());
  ASSERT_TRUE(v1.isCommitted());
  ASSERT_EQ(versos::Version::COMMITTED, v1.getStatus());
  ASSERT_EQ(1u, v1.getObjects().size());

  versos::Version& v2 = repo.create(v1);

  ASSERT_NE(v1, v2);
  ASSERT_TRUE(v2.isOK());
  ASSERT_TRUE(!v2.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v2.getStatus());
  ASSERT_EQ(1u, v2.getObjects().size());
  ASSERT_TRUE(v2.contains(o1));

  ASSERT_EQ(0, repo.remove(v2, o1));

  ASSERT_TRUE(!v2.isCommitted());
  ASSERT_EQ(0u, v2.getObjects().size());
  ASSERT_TRUE(!v2.contains(o1));

  ASSERT_EQ(0, repo.commit(v2));

  ASSERT_EQ(v2, repo.checkoutHEAD());
  ASSERT_TRUE(v2.isCommitted());
  ASSERT_EQ(versos::Version::COMMITTED, v2.getStatus());
  ASSERT_EQ(0u, v2.getObjects().size());
  ASSERT_TRUE(!v2.contains(o1));

  ASSERT_TRUE(!head.contains(o1));
  ASSERT_TRUE(v1.contains(o1));
}

TEST(mpicoordinator_test, values_between_versions)
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

  versos::MemVersionedObject o1(repo, "o" + Utils::to_str(myrank));

  ASSERT_EQ(0, repo.add(v1, o1));

  ASSERT_EQ(0, o1.write(v1, "first"));

  ASSERT_EQ(0, repo.commit(v1));

  versos::Version& v2 = repo.create(v1);

  ASSERT_NE(v1, v2);

  ASSERT_EQ(0, o1.write(v2, "second"));

  std::string o1v1;
  std::string o1v2;

  ASSERT_EQ(0, o1.read(v1, o1v1));
  ASSERT_EQ(0, o1.read(v2, o1v2));

  ASSERT_EQ("first", o1v1);
  ASSERT_EQ("second", o1v2);
}

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);

  comm = MPI_COMM_WORLD;
  myrank = MPI_Comm_rank(comm, &myrank);

  ::testing::InitGoogleTest(&argc, argv);

  int out = RUN_ALL_TESTS();

  MPI_Finalize();

  return out;
}

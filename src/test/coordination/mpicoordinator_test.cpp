#include "versos/repository.h"
#include "versos/refdb/memrefdb.h"
#include "versos/coordination/mpicoordinator.h"
#include "versos/objectversioning/memversionedobject.h"

#include <mpi.h>
#include <boost/lexical_cast.hpp>
#include <gtest/gtest.h>

MPI_Comm comm;
int myrank;

TEST(singlecoordinator, basic_commit_of_root)
{
  versos::MemRefDB refdb;
  versos::MpiCoordinator coord(comm, 0, refdb, "theseedforthetest");

  versos::Repository repo("mydataset", coord);

  if (repo.isEmpty())
    repo.init();

  const versos::Version& head = repo.checkoutHEAD();

  ASSERT_TRUE(head.isCommitted());
  ASSERT_EQ(head.getStatus(), versos::Version::COMMITTED);
  ASSERT_EQ(head.getObjects().size(), 0u);
  ASSERT_EQ(head.getId(), versos::Version::PARENT_FOR_ROOT.getId());

  ASSERT_NE(head, versos::Version::ERROR);
  ASSERT_NE(head, versos::Version::NOT_FOUND);

  versos::Version& newVersion = repo.create(head);

  ASSERT_NE(newVersion, versos::Version::ERROR);
  ASSERT_NE(newVersion, versos::Version::NOT_FOUND);
  ASSERT_TRUE(!newVersion.isCommitted());
  ASSERT_EQ(newVersion.getStatus(), versos::Version::STAGED);
  ASSERT_EQ(newVersion.getObjects().size(), 0u);
  ASSERT_NE(newVersion.getId(), versos::Version::PARENT_FOR_ROOT.getId());

  versos::MemVersionedObject o1(repo.getName(), "o1");

  ASSERT_EQ(0, newVersion.add(o1));

  ASSERT_TRUE(!newVersion.isCommitted());
  ASSERT_EQ(newVersion.getStatus(), versos::Version::STAGED);
  ASSERT_EQ(newVersion.getObjects().size(), 1u);
  ASSERT_TRUE(newVersion.contains(o1));

  ASSERT_EQ(0, newVersion.commit());

  ASSERT_TRUE(newVersion.isCommitted());
  ASSERT_EQ(newVersion.getStatus(), versos::Version::COMMITTED);
  ASSERT_EQ(newVersion.getObjects().size(), 1u);
}

TEST(mpicoordinator, values_between_versions)
{
  versos::MemRefDB refdb;
  versos::MpiCoordinator coord(comm, 0, refdb, "theseedforthetest");

  versos::Repository repo("mydataset", coord);

  if (repo.isEmpty())
    repo.init();

  const versos::Version& head = repo.checkoutHEAD();
  versos::Version& v1 = repo.create(head);

  versos::MemVersionedObject o1(repo.getName(), "o" + boost::lexical_cast<std::string>(myrank));

  ASSERT_EQ(0, v1.add(o1));

  ASSERT_EQ(0, o1.write(v1, "first"));

  ASSERT_EQ(0, v1.commit());

  versos::Version& v2 = repo.create(v1);

  ASSERT_NE(v2, versos::Version::ERROR);
  ASSERT_NE(v2, versos::Version::NOT_FOUND);

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

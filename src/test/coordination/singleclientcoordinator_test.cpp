#include "versos/repository.h"
#include "versos/refdb/memrefdb.h"
#include "versos/coordination/singleclientcoordinator.h"
#include "versos/objectversioning/memversionedobject.h"

#include <gtest/gtest.h>

TEST(singlecoordinator, basic_commit_of_root)
{
  versos::MemRefDB refdb;
  versos::SingleClientCoordinator coord(refdb);

  versos::Repository repo("mydataset", coord);

  if (repo.isEmpty())
    repo.init();

  const versos::Version& head = repo.checkoutHEAD();

  ASSERT_TRUE(head.isCommitted());
  ASSERT_EQ(head.getStatus(), versos::Version::COMMITTED);
  ASSERT_EQ(head.getObjects().size(), 0u);
  ASSERT_EQ(head.getId(), versos::Version::PARENT_FOR_ROOT.getId());

  ASSERT_TRUE(head.isOK());

  versos::Version& v1 = repo.create(head);

  ASSERT_TRUE(v1.isOK());
  ASSERT_TRUE(!v1.isCommitted());
  ASSERT_EQ(v1.getStatus(), versos::Version::STAGED);
  ASSERT_EQ(v1.getObjects().size(), 0u);
  ASSERT_NE(v1.getId(), versos::Version::PARENT_FOR_ROOT.getId());

  versos::MemVersionedObject o1(repo, "o1");

  ASSERT_EQ(0, v1.add(o1));

  ASSERT_TRUE(!v1.isCommitted());
  ASSERT_EQ(v1.getStatus(), versos::Version::STAGED);
  ASSERT_EQ(v1.getObjects().size(), 1u);
  ASSERT_TRUE(v1.contains(o1));

  ASSERT_EQ(0, v1.commit());

  ASSERT_TRUE(v1.isCommitted());
  ASSERT_EQ(v1.getStatus(), versos::Version::COMMITTED);
  ASSERT_EQ(v1.getObjects().size(), 1u);

  versos::Version& v2 = repo.create(v1);

  ASSERT_NE(v1, v2);
  ASSERT_TRUE(v2.isOK());
  ASSERT_TRUE(!v2.isCommitted());
  ASSERT_EQ(v2.getStatus(), versos::Version::STAGED);
  ASSERT_EQ(v2.getObjects().size(), 1u);
  ASSERT_TRUE(v2.contains(o1));

  ASSERT_EQ(v2.remove(o1), 0);

  ASSERT_TRUE(!v2.isCommitted());
  ASSERT_EQ(v2.getObjects().size(), 0u);
  ASSERT_TRUE(!v2.contains(o1));

  ASSERT_EQ(v2.commit(), 0);

  ASSERT_TRUE(v2.isCommitted());
  ASSERT_EQ(v2.getStatus(), versos::Version::COMMITTED);
  ASSERT_EQ(v2.getObjects().size(), 0u);
  ASSERT_TRUE(!v2.contains(o1));

  ASSERT_TRUE(!head.contains(o1));
  ASSERT_TRUE(v1.contains(o1));
}

TEST(singlecoordinator, values_between_versions)
{
  versos::MemRefDB refdb;
  versos::SingleClientCoordinator coord(refdb);

  versos::Repository repo("mydataset", coord);

  if (repo.isEmpty())
    repo.init();

  const versos::Version& head = repo.checkoutHEAD();
  versos::Version& v1 = repo.create(head);

  versos::MemVersionedObject o1(repo, "o1");

  ASSERT_EQ(0, v1.add(o1));

  ASSERT_EQ(0, o1.write(v1, "first"));

  ASSERT_EQ(0, v1.commit());

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
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

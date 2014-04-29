#include "versos/repository.h"
#include "versos/refdb/memrefdb.h"

#include <gtest/gtest.h>

class TestableMemRefDB : public versos::MemRefDB {
public:
  TestableMemRefDB(const std::string& repoName) : versos::MemRefDB(repoName){};
  using MemRefDB::insert; // changes access rights
};

TEST(memrefdb_test, fail_if_not_initialized)
{
  versos::Options o;

  o.metadb_type = versos::Options::Backend::MEM;
  o.coordinator_type = versos::Options::Coordinator::SINGLE_CLIENT;

  ASSERT_NO_THROW(versos::Repository repo("mydataset", o));
  ASSERT_EQ(versos::Version::NOT_FOUND, versos::Repository("mydataset", o).checkoutHEAD());

  o.metadb_initialize_if_empty = true;

  ASSERT_NO_THROW(versos::Repository repo("mydataset", o));
  ASSERT_NE(versos::Version::NOT_FOUND, versos::Repository("mydataset", o).checkoutHEAD());
}

TEST(memrefdb_test, locking)
{
  TestableMemRefDB refdb("myrepo");
  versos::Version v1("id1", versos::Version::PARENT_FOR_ROOT);

  ASSERT_NO_THROW(refdb.insert(v1, versos::RefDB::EXCLUSIVE_LOCK, "thelock"));
  ASSERT_ANY_THROW(refdb.insert(v1, versos::RefDB::EXCLUSIVE_LOCK, "thelock"));

  versos::Version v2("id2", versos::Version::PARENT_FOR_ROOT);

  ASSERT_NO_THROW(refdb.insert(v2, versos::RefDB::SHARED_LOCK, "thelock"));
  ASSERT_NO_THROW(refdb.insert(v2, versos::RefDB::SHARED_LOCK, "thelock"));
  ASSERT_NO_THROW(refdb.insert(v2, versos::RefDB::SHARED_LOCK, "thelock"));
  ASSERT_NO_THROW(refdb.insert(v2, versos::RefDB::SHARED_LOCK, "thelock"));

  ASSERT_EQ(3, refdb.commit(v2));
  ASSERT_EQ(2, refdb.commit(v2));
  ASSERT_EQ(1, refdb.commit(v2));
  ASSERT_EQ(0, refdb.commit(v2));
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

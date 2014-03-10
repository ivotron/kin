#include "versos/repository.h"

#include <gtest/gtest.h>

TEST(memrefdb_test, fail_if_not_initialized)
{
  versos::Options o;

  o.metadb_type = versos::Options::MetaDB::REDIS;
  o.metadb_server_address = "127.0.0.1";

  versos::Repository repo("mydataset", o);
  const versos::Version& head = repo.checkoutHEAD();

  ASSERT_TRUE(!head.isOK());

  o.metadb_initialize_if_empty = true;

  versos::Repository repo2("mydataset", o);

  const versos::Version& head2 = repo2.checkoutHEAD();

  ASSERT_TRUE(head2.isOK());
}

// TODO:
//  - init()
//  - own()
//  - commit()

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


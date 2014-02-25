#include "versos/repository.h"

#include <gtest/gtest.h>

TEST(repository_test, test_default_options)
{
  versos::Options o;

  ASSERT_NO_THROW(versos::Repository repo("mydataset", o));

  o.metadb_type = versos::Options::MetaDB::MEM;
  o.coordinator_type = versos::Options::Coordinator::SINGLE_CLIENT;

  ASSERT_NO_THROW(versos::Repository repo("mydataset", o));
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

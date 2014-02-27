#include "versos/repository.h"

#include <gtest/gtest.h>

TEST(repository_test, test_default_options)
{
  versos::Options o;

  ASSERT_EQ(versos::Options::Coordinator::SINGLE_CLIENT, o.coordinator_type);
  ASSERT_EQ(versos::Options::MetaDB::MEM, o.metadb_type);
  ASSERT_EQ("", o.hash_seed);
  ASSERT_EQ(false, o.metadb_initialize_if_empty);
  ASSERT_EQ(false, o.metadb_initialize_if_empty);
  ASSERT_EQ(versos::Options::ClientSync::AT_EACH_COMMIT, o.sync_mode);
  ASSERT_EQ(0, o.mpi_leader_rank);
  ASSERT_EQ(1, o.mpi_comm);

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

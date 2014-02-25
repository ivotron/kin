#include "versos/repository.h"

#include <gtest/gtest.h>

#include <mpi.h>

TEST(repository_test, factory)
{
  versos::Options o;

  o.metadb = "mem";
  o.coordinator = "single";

  ASSERT_NO_THROW(versos::Repository repo("mydataset", o));

  o.metadb = "foo";
  o.coordinator = "single";

  ASSERT_ANY_THROW(versos::Repository repo("mydataset", o));

  o.metadb = "mem";
  o.coordinator = "foo";

  ASSERT_ANY_THROW(versos::Repository repo("mydataset", o));
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#include "versos/repository.h"
#include "versos/objectversioning/memversionedobject.h"
#include "versos/refdb/rediswrapper.h"

#include <gtest/gtest.h>

TEST(redisrefdb_test, factory)
{
  redis::DB redisdb;
  redisdb.connect("127.0.0.1");

  versos::Options o;

  o.metadb_type = versos::Options::MetaDB::REDIS;

  ASSERT_NO_THROW(versos::Repository repo("mydataset", o));
  redisdb.flushall();
  ASSERT_ANY_THROW(versos::Repository("mydataset", o).checkoutHEAD());
  redisdb.flushall();

  o.metadb_initialize_if_empty = true;

  ASSERT_NO_THROW(versos::Repository repo("mydataset", o));
  redisdb.flushall();
  ASSERT_NO_THROW(versos::Repository("mydataset", o).checkoutHEAD());
  redisdb.flushall();
  ASSERT_NE(versos::Version::NOT_FOUND, versos::Repository("mydataset", o).checkoutHEAD());
  redisdb.flushall();
}

TEST(redisrefdb_test, multiple_clients_no_conflict)
{
  // simulate multiple clients
  versos::Options o;

  o.metadb_type = versos::Options::MetaDB::REDIS;
  o.coordinator_type = versos::Options::Coordinator::BACKEND;
  o.metadb_initialize_if_empty = true;

  versos::Repository repo1("mydataset", o);
  versos::Repository repo2("mydataset", o);

  const versos::Version& head1 = repo1.checkoutHEAD();
  const versos::Version& head2 = repo2.checkoutHEAD();

  ASSERT_TRUE(head1.isOK());
  ASSERT_TRUE(head2.isOK());

  versos::Version& v1c1 = repo1.create(head1);
  versos::Version& v1c2 = repo2.create(head2);

  ASSERT_TRUE(v1c1.isOK());
  ASSERT_TRUE(v1c2.isOK());

  ASSERT_EQ(v1c1, v1c2);

  versos::MemVersionedObject o1c1(repo1, "o1");
  versos::MemVersionedObject o2c2(repo2, "o2");

  ASSERT_NO_THROW(repo1.add(v1c1, o1c1));
  ASSERT_NO_THROW(repo2.add(v1c2, o2c2));

  ASSERT_NO_THROW(o1c1.put(v1c1, "c1first"));
  ASSERT_NO_THROW(o2c2.put(v1c2, "c2first"));

  ASSERT_EQ(1, repo1.commit(v1c1));

  // TODO:
  // versos::Version& v2 = repo.create(v1c1);
  // ASSERT_FALSE(v2.isOK());

  ASSERT_EQ(0, repo2.commit(v1c2));

  versos::Version& v2c1 = repo1.create(v1c1);

  ASSERT_TRUE(v2c1.isOK());
  ASSERT_NE(v1c1, v2c1);

  redis::DB redisdb;
  redisdb.connect("127.0.0.1");
  redisdb.flushall();
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


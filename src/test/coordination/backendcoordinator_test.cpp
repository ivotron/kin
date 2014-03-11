#include "versos/repository.h"
#include "versos/objectversioning/memversionedobject.h"
#include "versos/utils.h"

#include <gtest/gtest.h>

TEST(backendcoordinator_test, factory)
{
  versos::Options o;

  o.coordinator_type = versos::Options::Coordinator::BACKEND;

  ASSERT_NO_THROW(versos::Repository repo("mydataset", o));
}

TEST(backendcoordinator_test, NONE_object_containment)
{
  versos::Options o;

  o.metadb_type = versos::Options::MetaDB::MEM;
  o.metadb_initialize_if_empty = true;
  o.coordinator_type = versos::Options::Coordinator::BACKEND;
  o.sync_mode = versos::Options::ClientSync::NONE;
  o.hash_seed = "theseedforthetest";

  versos::Repository repo("mydataset", o);

  const versos::Version& head = repo.checkoutHEAD();

  ASSERT_TRUE(head.isCommitted());
  ASSERT_EQ(0u, head.size());
  ASSERT_EQ(versos::Version::PARENT_FOR_ROOT.getId(), head.getId());

  ASSERT_TRUE(head.isOK());

  std::cout << "v1 " << std::endl << std::flush;
  versos::Version& v1 = repo.create(head);
  std::cout << "v1 after " << std::endl << std::flush;

  ASSERT_TRUE(v1.isOK());
  ASSERT_FALSE(v1.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v1.getStatus());
  ASSERT_EQ(0u, v1.size());
  ASSERT_NE(versos::Version::PARENT_FOR_ROOT.getId(), v1.getId());

  versos::MemVersionedObject o1(repo, "o1");

  ASSERT_NE(0, repo.add(v1, o1));

  ASSERT_FALSE(v1.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v1.getStatus());
  ASSERT_EQ(0u, v1.size());
  ASSERT_FALSE(v1.contains(o1));

  ASSERT_EQ(0, repo.commit(v1));

  ASSERT_EQ(v1, repo.checkoutHEAD());
  ASSERT_TRUE(v1.isCommitted());
  ASSERT_EQ(0u, v1.size());

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

TEST(backendcoordinator_test, wrong_mode_for_object_containment_operations)
{
  versos::Options o;

  o.metadb_type = versos::Options::MetaDB::MEM;
  o.metadb_initialize_if_empty = true;
  o.coordinator_type = versos::Options::Coordinator::BACKEND;
  o.sync_mode = versos::Options::ClientSync::NONE;
  o.hash_seed = "theseedforthetest";

  versos::Repository repo("mydataset", o);

  const versos::Version& head = repo.checkoutHEAD();
  versos::Version& v1 = repo.create(head);

  versos::MemVersionedObject ob(repo, "o1");

  // since we're in NONE mode, we get errors
  ASSERT_NE(0, repo.add(v1, ob));

  // we have to create repo with default AT_EACH_COMMIT (default) or AT_EACH_ADD_OR_REMOVE in order to be able 
  // to add to it, see next test:
}

TEST(backendcoordinator_test, values_between_tests)
{
  versos::Options o;

  o.metadb_type = versos::Options::MetaDB::MEM;
  o.metadb_initialize_if_empty = true;
  o.coordinator_type = versos::Options::Coordinator::BACKEND;
  o.hash_seed = "theseedforthetest";

  versos::Repository repo("mydataset", o);

  const versos::Version& head = repo.checkoutHEAD();
  versos::Version& v1 = repo.create(head);

  versos::MemVersionedObject o1(repo, "o1");

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

  versos::MemVersionedObject o2(repo, "o2");
  // TODO: the following aborts:
  //
  // ASSERT_NE(0, repo.add(v1, o2));
  ASSERT_EQ(0, repo.add(v2, o2));
  ASSERT_EQ(0, repo.commit(v2));
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}

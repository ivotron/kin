#include "versos/repository.h"
#include "versos/obj/kvobject.h"
#include "versos/util/stringutils.h"

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

  o.metadb_type = versos::Options::Backend::MEM;
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

  versos::Version& v1 = repo.create(head);

  ASSERT_TRUE(v1.isOK());
  ASSERT_FALSE(v1.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v1.getStatus());
  ASSERT_EQ(0u, v1.size());
  ASSERT_NE(versos::Version::PARENT_FOR_ROOT.getId(), v1.getId());

  ASSERT_ANY_THROW(repo.add(v1, "o1"));

  ASSERT_FALSE(v1.isCommitted());
  ASSERT_EQ(versos::Version::STAGED, v1.getStatus());
  ASSERT_EQ(0u, v1.size());
  ASSERT_FALSE(v1.contains("o1"));

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
  ASSERT_FALSE(v2.contains("o1"));

  ASSERT_ANY_THROW(repo.remove(v2, "o1"));

  ASSERT_FALSE(v2.isCommitted());
  ASSERT_EQ(0u, v2.size());
  ASSERT_FALSE(v2.contains("o1"));

  ASSERT_EQ(0, repo.commit(v2));

  ASSERT_EQ(v2, repo.checkoutHEAD());
  ASSERT_TRUE(v2.isCommitted());
  ASSERT_EQ(0u, v2.size());
  ASSERT_FALSE(v2.contains("o1"));

  // exec. summary :)
  ASSERT_FALSE(head.contains("o1"));
  ASSERT_FALSE(v1.contains("o1"));
  ASSERT_FALSE(v2.contains("o1"));
}

TEST(backendcoordinator_test, wrong_mode_for_object_containment_operations)
{
  versos::Options o;

  o.metadb_type = versos::Options::Backend::MEM;
  o.metadb_initialize_if_empty = true;
  o.coordinator_type = versos::Options::Coordinator::BACKEND;
  o.sync_mode = versos::Options::ClientSync::NONE;
  o.hash_seed = "theseedforthetest";

  versos::Repository repo("mydataset", o);

  const versos::Version& head = repo.checkoutHEAD();
  versos::Version& v1 = repo.create(head);

  versos::KVObject ob("ob", "valueob");

  // since we're in NONE mode, we get errors
  ASSERT_ANY_THROW(repo.add(v1, ob));

  // we have to create repo with default AT_EACH_COMMIT (default) or AT_EACH_ADD_OR_REMOVE in order to be able 
  // to add to it, see next test:
}

TEST(backendcoordinator_test, values_between_tests)
{
  versos::Options o;

  o.metadb_type = versos::Options::Backend::MEM;
  o.metadb_initialize_if_empty = true;
  o.coordinator_type = versos::Options::Coordinator::BACKEND;
  o.hash_seed = "theseedforthetest";

  versos::Repository repo("mydataset", o);

  const versos::Version& head = repo.checkoutHEAD();
  versos::Version& v1 = repo.create(head);

  ASSERT_NO_THROW(repo.add(v1, "o1"));
  ASSERT_NO_THROW(repo.set<std::string>(v1, "o1", "first"));

  ASSERT_NO_THROW(repo.commit(v1));

  versos::Version& v2 = repo.create(v1);

  ASSERT_TRUE(v2.isOK());
  ASSERT_NE(v1, v2);

  ASSERT_NO_THROW(repo.set<std::string>(v2, "o1", "second"));
  ASSERT_EQ(0, repo.commit(v2));

  ASSERT_EQ("first", repo.get<std::string>(v1, "o1"));
  ASSERT_EQ("second", repo.get<std::string>(v2, "o1"));
}

TEST(backendcoordinator_test, multiple_clients_no_conflict)
{
  // simulate multiple clients from a single node
  versos::Options o;

  o.coordinator_type = versos::Options::Coordinator::BACKEND;
  o.metadb_initialize_if_empty = true;

  versos::Repository repo("mydataset", o);

  const versos::Version& head = repo.checkoutHEAD();

  versos::Version& v1c1 = repo.create(head);
  versos::Version& v1c2 = repo.create(head);

  ASSERT_TRUE(v1c1.isOK());
  ASSERT_TRUE(v1c2.isOK());

  ASSERT_EQ(v1c1, v1c2);

  ASSERT_NO_THROW(repo.add(v1c1, "o1"));
  ASSERT_NO_THROW(repo.add(v1c2, "o2"));

  ASSERT_NO_THROW(repo.set<std::string>(v1c1, "o1", "from_client_c1"));
  ASSERT_NO_THROW(repo.set<std::string>(v1c2, "o2", "from_client_c2"));

  ASSERT_EQ(1, repo.commit(v1c1));

  // TODO: ASSERT_ANY_THROW(repo.create(v1c1));

  ASSERT_EQ(0, repo.commit(v1c2));

  versos::Version& v2c1 = repo.create(v1c1);

  ASSERT_TRUE(v2c1.isOK());
  ASSERT_NE(v1c1, v2c1);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}

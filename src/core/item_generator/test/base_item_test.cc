#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>

#include "base_item.h"
#include "item_rarity.h"
#include "item_status.h"
#include "item_type.h"
#include "uuid.h"

using json = nlohmann::json;

namespace
{

/// Helper: read the full contents of a file as a string.
std::string readFile(const std::string& path)
{
  std::ifstream in(path);
  EXPECT_TRUE(in.is_open()) << "Failed to open: " << path;
  std::ostringstream oss;
  oss << in.rdbuf();
  return oss.str();
}

/// Helper: construct a path into the test data directory.
std::string dataPath(const std::string& filename)
{
  return std::string(TEST_DATA_DIR) + "/" + filename;
}

/// Helper: replace every occurrence of a substring in the source string.
void replaceAll(std::string& s, const std::string& from, const std::string& to)
{
  size_t pos = 0;
  while ((pos = s.find(from, pos)) != std::string::npos)
  {
    s.replace(pos, from.length(), to);
    pos += to.length();
  }
}

// =============================================================================
//  Fixture — a BaseItem with all fields populated
// =============================================================================
class BaseItemTest : public ::testing::Test
{
  protected:
  void SetUp() override
  {
    item_.setName("Iron Sword");
    item_.setDescription("A sturdy iron sword.");
    item_.setWeight(3.5f);
    item_.setRarity(item::Rarity::Common);
    item_.setType(item::Type::OneHandedPrimary);
    item_.setStatus(item::Status::OWNED);
  }

  BaseItem item_;
};

// =============================================================================
//  Construction
// =============================================================================
TEST_F(BaseItemTest, DefaultConstruction)
{
  BaseItem fresh{};
  EXPECT_FALSE(fresh.getUUID().toString().empty());
  EXPECT_TRUE(fresh.isValid());
  EXPECT_TRUE(fresh.getName().empty());
  EXPECT_TRUE(fresh.getDescription().empty());
  EXPECT_FALSE(fresh.getOwner().has_value());
}

// =============================================================================
//  Name
// =============================================================================
TEST_F(BaseItemTest, SetAndGetName)
{
  item_.setName("Excalibur");
  EXPECT_EQ(item_.getName(), "Excalibur");

  item_.setName("");
  EXPECT_TRUE(item_.getName().empty());
}

// =============================================================================
//  Description
// =============================================================================
TEST_F(BaseItemTest, SetAndGetDescription)
{
  item_.setDescription("The legendary sword.");
  EXPECT_EQ(item_.getDescription(), "The legendary sword.");

  item_.setDescription("");
  EXPECT_TRUE(item_.getDescription().empty());
}

// =============================================================================
//  Weight
// =============================================================================
TEST_F(BaseItemTest, SetAndGetWeight)
{
  item_.setWeight(12.75f);
  EXPECT_FLOAT_EQ(item_.getWeight(), 12.75f);

  item_.setWeight(0.0f);
  EXPECT_FLOAT_EQ(item_.getWeight(), 0.0f);

  item_.setWeight(-5.0f);
  EXPECT_FLOAT_EQ(item_.getWeight(), -5.0f);
}

// =============================================================================
//  Rarity
// =============================================================================
TEST_F(BaseItemTest, SetAndGetRarity)
{
  item_.setRarity(item::Rarity::Common);
  EXPECT_EQ(item_.getRarity(), item::Rarity::Common);

  item_.setRarity(item::Rarity::Uncommon);
  EXPECT_EQ(item_.getRarity(), item::Rarity::Uncommon);

  item_.setRarity(item::Rarity::Rare);
  EXPECT_EQ(item_.getRarity(), item::Rarity::Rare);

  item_.setRarity(item::Rarity::Epic);
  EXPECT_EQ(item_.getRarity(), item::Rarity::Epic);

  item_.setRarity(item::Rarity::Legendary);
  EXPECT_EQ(item_.getRarity(), item::Rarity::Legendary);

  item_.setRarity(item::Rarity::Artifact);
  EXPECT_EQ(item_.getRarity(), item::Rarity::Artifact);
}

// =============================================================================
//  Type
// =============================================================================
TEST_F(BaseItemTest, SetAndGetType)
{
  item_.setType(item::Type::Ring);
  EXPECT_EQ(item_.getType(), item::Type::Ring);

  item_.setType(item::Type::TwoHandedPrimary);
  EXPECT_EQ(item_.getType(), item::Type::TwoHandedPrimary);

  item_.setType(item::Type::ChestGear);
  EXPECT_EQ(item_.getType(), item::Type::ChestGear);

  item_.setType(item::Type::Consumable);
  EXPECT_EQ(item_.getType(), item::Type::Consumable);

  item_.setType(item::Type::Currency);
  EXPECT_EQ(item_.getType(), item::Type::Currency);
}

// =============================================================================
//  Status
// =============================================================================
TEST_F(BaseItemTest, SetAndGetStatus)
{
  item_.setStatus(item::Status::NOT_OWNDED);
  EXPECT_EQ(item_.getStatus(), item::Status::NOT_OWNDED);

  item_.setStatus(item::Status::OWNED);
  EXPECT_EQ(item_.getStatus(), item::Status::OWNED);

  item_.setStatus(item::Status::EQUIPPED);
  EXPECT_EQ(item_.getStatus(), item::Status::EQUIPPED);
}

// =============================================================================
//  Owner
// =============================================================================
TEST_F(BaseItemTest, SetAndGetOwner)
{
  UUID owner;
  const auto ownerStr = owner.toString();
  item_.setOwner(std::move(owner));

  ASSERT_TRUE(item_.getOwner().has_value());
  EXPECT_EQ(item_.getOwner()->toString(), ownerStr);

  item_.setOwner(std::nullopt);
  EXPECT_FALSE(item_.getOwner().has_value());
}

// =============================================================================
//  isEquivalent — name, description, weight, rarity, and type must all match.
//  Status and owner are intentionally excluded from the comparison.
// =============================================================================
TEST_F(BaseItemTest, IsEquivalent)
{
  BaseItem other{};

  // Matching properties → true (status/owner ignored)
  other.setName(item_.getName());
  other.setDescription(item_.getDescription());
  other.setWeight(item_.getWeight());
  other.setRarity(item_.getRarity());
  other.setType(item_.getType());
  other.setStatus(
    item::Status::EQUIPPED); // different status → still equivalent
  EXPECT_TRUE(item_.isEquivalent(other));

  // Name differs → false
  other.setName("Steel Sword");
  EXPECT_FALSE(item_.isEquivalent(other));

  // Description differs → false
  other.setName(item_.getName());
  other.setDescription("A completely different description.");
  EXPECT_FALSE(item_.isEquivalent(other));

  // Weight differs → false
  other.setDescription(item_.getDescription());
  other.setWeight(99.0f);
  EXPECT_FALSE(item_.isEquivalent(other));

  // Rarity differs → false
  other.setWeight(item_.getWeight());
  other.setRarity(item::Rarity::Artifact);
  EXPECT_FALSE(item_.isEquivalent(other));

  // Type differs → false
  other.setRarity(item_.getRarity());
  other.setType(item::Type::Amulet);
  EXPECT_FALSE(item_.isEquivalent(other));

  // Owner differs but core fields match → true (owner is ignored)
  other.setType(item_.getType());
  UUID someOwner;
  other.setOwner(std::move(someOwner));
  EXPECT_TRUE(item_.isEquivalent(other));
}

// =============================================================================
//  isEqual — equivalence plus matching UUID
// =============================================================================
TEST_F(BaseItemTest, IsEqual)
{
  // Self-comparison: equivalent + same UUID → true
  EXPECT_TRUE(item_.isEqual(item_));

  // Different properties, different UUID → false
  BaseItem other{};
  other.setName("Other Sword");
  other.setDescription("Different.");
  other.setWeight(10.0f);
  other.setRarity(item::Rarity::Epic);
  other.setType(item::Type::TwoHandedPrimary);
  other.setStatus(item::Status::EQUIPPED);
  EXPECT_FALSE(item_.isEqual(other));

  // Same core properties but different UUID → false
  BaseItem clone{};
  clone.setName(item_.getName());
  clone.setDescription(item_.getDescription());
  clone.setWeight(item_.getWeight());
  clone.setRarity(item_.getRarity());
  clone.setType(item_.getType());
  clone.setStatus(item_.getStatus());
  EXPECT_FALSE(item_.isEqual(clone));
}

// =============================================================================
//  toJson — inline checks
// =============================================================================
TEST_F(BaseItemTest, ToJson)
{
  // Without owner
  {
    const auto j = item_.toJson();
    EXPECT_EQ(j["uuid"], item_.getUUID().toString());
    EXPECT_EQ(j["name"], "Iron Sword");
    EXPECT_EQ(j["description"], "A sturdy iron sword.");
    EXPECT_EQ(j["weight"], 3.5);
    EXPECT_EQ(j["rarity"], "Common");
    EXPECT_EQ(j["type"], "OneHandedPrimary");
    EXPECT_EQ(j["status"], "OWNED");
    EXPECT_FALSE(j.contains("owner"));
  }

  // With owner
  {
    UUID owner;
    const auto ownerStr = owner.toString();
    item_.setOwner(std::move(owner));

    const auto j = item_.toJson();
    EXPECT_TRUE(j.contains("owner"));
    EXPECT_EQ(j["owner"], ownerStr);
  }
}

// =============================================================================
//  toJson — golden-file comparisons against expected output
// =============================================================================
TEST_F(BaseItemTest, ToJson_MatchesExpectedFile_NoOwner)
{
  BaseItem potion{};
  potion.setName("Health Potion");
  potion.setDescription("Restores 50 HP.");
  potion.setWeight(0.5f);
  potion.setRarity(item::Rarity::Uncommon);
  potion.setType(item::Type::Consumable);
  potion.setStatus(item::Status::NOT_OWNDED);

  auto raw = readFile(dataPath("expected_item_no_owner.json"));
  replaceAll(raw, "00000000-0000-0000-0000-000000000000",
             potion.getUUID().toString());
  const json expected = json::parse(raw);

  EXPECT_EQ(potion.toJson(), expected);
}

TEST_F(BaseItemTest, ToJson_MatchesExpectedFile_WithOwner)
{
  BaseItem sword{};
  sword.setName("Iron Sword");
  sword.setDescription("A sturdy iron sword.");
  sword.setWeight(3.5f);
  sword.setRarity(item::Rarity::Common);
  sword.setType(item::Type::OneHandedPrimary);
  sword.setStatus(item::Status::OWNED);

  UUID owner;
  const auto ownerStr = owner.toString();
  sword.setOwner(std::move(owner));

  auto raw = readFile(dataPath("expected_item_full.json"));
  replaceAll(raw, "00000000-0000-0000-0000-000000000000",
             sword.getUUID().toString());
  replaceAll(raw, "11111111-1111-1111-1111-111111111111", ownerStr);
  const json expected = json::parse(raw);

  EXPECT_EQ(sword.toJson(), expected);
}

} // namespace

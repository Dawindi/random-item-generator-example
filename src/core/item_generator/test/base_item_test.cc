#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>

#include "base_item.h"
#include "item_builder.h"
#include "item_data_types/item_rarity.h"
#include "item_data_types/item_status.h"
#include "item_data_types/item_type.h"
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
    item_ = ItemBuilder()
              .withName("Iron Sword")
              .withDescription("A sturdy iron sword.")
              .withWeight(3.5f)
              .withRarity(item::Rarity::Common)
              .withType(item::Type::OneHandedPrimary)
              .withStatus(item::Status::OWNED)
              .build();
  }

  BaseItem item_{ItemBuilder().build()};
};

// =============================================================================
//  Construction
// =============================================================================
TEST_F(BaseItemTest, DefaultConstruction)
{
  BaseItem fresh{ItemBuilder().build()};
  EXPECT_FALSE(fresh.getUUID().toString().empty());
  // A default-constructed item is invalid — weight, rarity, and type
  // are uninitialised and therefore fail the isValid() checks.
  EXPECT_FALSE(fresh.isValid());
  EXPECT_TRUE(fresh.getName().empty());
  EXPECT_TRUE(fresh.getDescription().empty());
  EXPECT_FALSE(fresh.getOwner().has_value());
}

// =============================================================================
//  Name
// =============================================================================
TEST_F(BaseItemTest, SetAndGetName)
{
  auto item = ItemBuilder().withName("Excalibur").build();
  EXPECT_EQ(item.getName(), "Excalibur");

  auto empty = ItemBuilder().withName("").build();
  EXPECT_TRUE(empty.getName().empty());
}

// =============================================================================
//  Description
// =============================================================================
TEST_F(BaseItemTest, SetAndGetDescription)
{
  auto item = ItemBuilder().withDescription("The legendary sword.").build();
  EXPECT_EQ(item.getDescription(), "The legendary sword.");

  auto empty = ItemBuilder().withDescription("").build();
  EXPECT_TRUE(empty.getDescription().empty());
}

// =============================================================================
//  Weight
// =============================================================================
TEST_F(BaseItemTest, SetAndGetWeight)
{
  auto item = ItemBuilder().withWeight(12.75f).build();
  EXPECT_FLOAT_EQ(item.getWeight(), 12.75f);

  auto zero = ItemBuilder().withWeight(0.0f).build();
  EXPECT_FLOAT_EQ(zero.getWeight(), 0.0f);

  auto neg = ItemBuilder().withWeight(-5.0f).build();
  EXPECT_FLOAT_EQ(neg.getWeight(), -5.0f);
}

// =============================================================================
//  Owner
// =============================================================================
TEST_F(BaseItemTest, SetAndGetOwner)
{
  UUID owner;
  const auto ownerStr = owner.toString();

  auto withOwner = ItemBuilder().withOwner(std::move(owner)).build();
  ASSERT_TRUE(withOwner.getOwner().has_value());
  EXPECT_EQ(withOwner.getOwner()->toString(), ownerStr);

  auto withoutOwner = ItemBuilder().build();
  EXPECT_FALSE(withoutOwner.getOwner().has_value());
}

// =============================================================================
//  isValid
// =============================================================================
TEST_F(BaseItemTest, IsValid_FullyBuiltItem)
{
  // A fully-populated item passes all validity checks.
  EXPECT_TRUE(item_.isValid());
}

TEST_F(BaseItemTest, IsValid_ZeroWeight)
{
  auto item = ItemBuilder()
                .withWeight(0.0f)
                .withRarity(item::Rarity::Common)
                .withType(item::Type::OneHandedPrimary)
                .build();
  EXPECT_TRUE(item.isValid());
}

TEST_F(BaseItemTest, IsValid_NegativeWeight)
{
  auto item = ItemBuilder()
                .withWeight(-1.0f)
                .withRarity(item::Rarity::Common)
                .withType(item::Type::OneHandedPrimary)
                .build();
  EXPECT_FALSE(item.isValid());
}

// =============================================================================
//  isEquivalent — name, description, weight, rarity, and type must all match.
//  Status and owner are intentionally excluded from the comparison.
// =============================================================================
TEST_F(BaseItemTest, IsEquivalent)
{
  // Matching properties → true (status/owner ignored)
  auto other = ItemBuilder::duplicateItem(item_);
  EXPECT_TRUE(item_.isEquivalent(other));

  // Status differs but core fields match → true (status ignored)
  auto diffStatus = ItemBuilder()
                      .withName(item_.getName())
                      .withDescription(item_.getDescription())
                      .withWeight(item_.getWeight())
                      .withRarity(item_.getRarity())
                      .withType(item_.getType())
                      .withStatus(item::Status::EQUIPPED)
                      .build();
  EXPECT_TRUE(item_.isEquivalent(diffStatus));

  // Name differs → false
  auto diffName = ItemBuilder()
                    .withName("Steel Sword")
                    .withDescription(item_.getDescription())
                    .withWeight(item_.getWeight())
                    .withRarity(item_.getRarity())
                    .withType(item_.getType())
                    .build();
  EXPECT_FALSE(item_.isEquivalent(diffName));

  // Description differs → false
  auto diffDesc = ItemBuilder()
                    .withName(item_.getName())
                    .withDescription("A completely different description.")
                    .withWeight(item_.getWeight())
                    .withRarity(item_.getRarity())
                    .withType(item_.getType())
                    .build();
  EXPECT_FALSE(item_.isEquivalent(diffDesc));

  // Weight differs → false
  auto diffWeight = ItemBuilder()
                      .withName(item_.getName())
                      .withDescription(item_.getDescription())
                      .withWeight(99.0f)
                      .withRarity(item_.getRarity())
                      .withType(item_.getType())
                      .build();
  EXPECT_FALSE(item_.isEquivalent(diffWeight));

  // Rarity differs → false
  auto diffRarity = ItemBuilder()
                      .withName(item_.getName())
                      .withDescription(item_.getDescription())
                      .withWeight(item_.getWeight())
                      .withRarity(item::Rarity::Artifact)
                      .withType(item_.getType())
                      .build();
  EXPECT_FALSE(item_.isEquivalent(diffRarity));

  // Type differs → false
  auto diffType = ItemBuilder()
                    .withName(item_.getName())
                    .withDescription(item_.getDescription())
                    .withWeight(item_.getWeight())
                    .withRarity(item_.getRarity())
                    .withType(item::Type::Amulet)
                    .build();
  EXPECT_FALSE(item_.isEquivalent(diffType));

  // Owner differs but core fields match → true (owner ignored)
  UUID someOwner;
  auto otherWithOwner = ItemBuilder::duplicateItem(item_, std::move(someOwner));
  EXPECT_TRUE(item_.isEquivalent(otherWithOwner));
}

// =============================================================================
//  isEqual — equivalence plus matching UUID
// =============================================================================
TEST_F(BaseItemTest, IsEqual)
{
  // Self-comparison: equivalent + same UUID → true
  EXPECT_TRUE(item_.isEqual(item_));

  // Different properties, different UUID → false
  auto other = ItemBuilder()
                 .withName("Other Sword")
                 .withDescription("Different.")
                 .withWeight(10.0f)
                 .withRarity(item::Rarity::Epic)
                 .withType(item::Type::TwoHandedPrimary)
                 .withStatus(item::Status::EQUIPPED)
                 .build();
  EXPECT_FALSE(item_.isEqual(other));

  // Same core properties but different UUID → false
  auto clone = ItemBuilder::duplicateItem(item_);
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
    auto itemWithOwner = ItemBuilder::duplicateItem(item_, std::move(owner));

    const auto j = itemWithOwner.toJson();
    EXPECT_TRUE(j.contains("owner"));
    EXPECT_EQ(j["owner"], ownerStr);
  }
}

// =============================================================================
//  toJson — golden-file comparisons against expected output
// =============================================================================
TEST_F(BaseItemTest, ToJson_MatchesExpectedFile_NoOwner)
{
  auto potion = ItemBuilder()
                  .withName("Health Potion")
                  .withDescription("Restores 50 HP.")
                  .withWeight(0.5f)
                  .withRarity(item::Rarity::Uncommon)
                  .withType(item::Type::Consumable)
                  .withStatus(item::Status::NOT_OWNDED)
                  .build();

  auto raw = readFile(dataPath("expected_item_no_owner.json"));
  replaceAll(raw, "00000000-0000-0000-0000-000000000000",
             potion.getUUID().toString());
  const json expected = json::parse(raw);

  EXPECT_EQ(potion.toJson(), expected);
}

TEST_F(BaseItemTest, ToJson_MatchesExpectedFile_WithOwner)
{
  UUID owner;
  const auto ownerStr = owner.toString();
  auto sword = ItemBuilder()
                 .withName("Iron Sword")
                 .withDescription("A sturdy iron sword.")
                 .withWeight(3.5f)
                 .withRarity(item::Rarity::Common)
                 .withType(item::Type::OneHandedPrimary)
                 .withStatus(item::Status::OWNED)
                 .withOwner(std::move(owner))
                 .build();

  auto raw = readFile(dataPath("expected_item_full.json"));
  replaceAll(raw, "00000000-0000-0000-0000-000000000000",
             sword.getUUID().toString());
  replaceAll(raw, "11111111-1111-1111-1111-111111111111", ownerStr);
  const json expected = json::parse(raw);

  EXPECT_EQ(sword.toJson(), expected);
}

} // namespace

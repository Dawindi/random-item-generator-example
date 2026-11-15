#include <cstddef>
#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>
#include <utility>

#include "base_item.h"
#include "item_builder.h"
#include "item_data_types/item_rarity.h"
#include "item_data_types/item_status.h"
#include "item_data_types/item_type.h"
#include "nlohmann/json_fwd.hpp"
#include "uuid.h"

using nlohmann::json;

namespace
{

// gtest's TEST_F macro expands to `void TestBody()` and its ASSERT/EXPECT
// macros expand to `if` statements, so the trailing-return-type and
// cognitive-complexity checks fire on code we cannot edit. These two checks
// are suppressed for this test file for that reason.
// NOLINTBEGIN(modernize-use-trailing-return-type,
//             readability-function-cognitive-complexity)

constexpr float kIronSwordWeight = 3.5F;
constexpr float kHeavyTestWeight = 12.75F;
constexpr float kNegativeTestWeight = -5.0F;
constexpr float kDifferentWeight = 99.0F;
constexpr float kOtherSwordWeight = 10.0F;
constexpr float kPotionWeight = 0.5F;

/// Helper: read the full contents of a file as a string.
auto readFile(const std::string& path) -> std::string
{
  std::ifstream const input(path);
  EXPECT_TRUE(input.is_open()) << "Failed to open: " << path;
  std::ostringstream oss;
  oss << input.rdbuf();
  return oss.str();
}

/// Helper: construct a path into the test data directory.
auto dataPath(const std::string& filename) -> std::string
{
  return std::string(TEST_DATA_DIR) + "/" + filename;
}

/// Helper: replace every occurrence of a substring in the source string.
void replaceAll(std::string& source, const std::string& from,
                const std::string& replacement)
{
  size_t pos = 0;
  while ((pos = source.find(from, pos)) != std::string::npos)
  {
    source.replace(pos, from.length(), replacement);
    pos += replacement.length();
  }
}

// =============================================================================
//  Fixture — a BaseItem with all fields populated
// =============================================================================
class BaseItemTest : public ::testing::Test
{
  public:
  const BaseItem& getTestItem() const { return item_; }

  protected:
  void SetUp() override
  {
    item_ = ItemBuilder()
              .withName("Iron Sword")
              .withDescription("A sturdy iron sword.")
              .withWeight(kIronSwordWeight)
              .withRarity(item::Rarity::Common)
              .withType(item::Type::OneHandedPrimary)
              .withStatus(item::Status::NOT_OWNDED)
              .build();
  }

  private:
  BaseItem item_{ItemBuilder().build()};
};

// =============================================================================
//  Construction
// =============================================================================
TEST_F(BaseItemTest, DefaultConstruction)
{
  BaseItem const fresh{ItemBuilder().build()};
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
  auto item = ItemBuilder().withWeight(kHeavyTestWeight).build();
  EXPECT_FLOAT_EQ(item.getWeight(), kHeavyTestWeight);

  auto zero = ItemBuilder().withWeight(0.0F).build();
  EXPECT_FLOAT_EQ(zero.getWeight(), 0.0F);

  auto neg = ItemBuilder().withWeight(kNegativeTestWeight).build();
  EXPECT_FLOAT_EQ(neg.getWeight(), kNegativeTestWeight);
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
  EXPECT_EQ(withOwner.getOwner().value().toString(), ownerStr);

  auto withoutOwner = ItemBuilder().build();
  EXPECT_FALSE(withoutOwner.getOwner().has_value());
}

// =============================================================================
//  isValid
// =============================================================================
TEST_F(BaseItemTest, IsValid_FullyBuiltItem)
{
  // A fully-populated item passes all validity checks.
  EXPECT_TRUE(getTestItem().isValid());
}

TEST_F(BaseItemTest, IsValid_ZeroWeight)
{
  auto item = ItemBuilder()
                .withWeight(0.0F)
                .withRarity(item::Rarity::Common)
                .withType(item::Type::OneHandedPrimary)
                .withStatus(item::Status::NOT_OWNDED)
                .build();
  EXPECT_TRUE(item.isValid());
}

TEST_F(BaseItemTest, IsValid_NegativeWeight)
{
  auto item = ItemBuilder()
                .withWeight(-1.0F)
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
  const auto& item = getTestItem();

  // Matching properties → true (status/owner ignored)
  auto other = ItemBuilder::duplicateItem(item);
  EXPECT_TRUE(item.isEquivalent(other));

  // Status differs but core fields match → true (status ignored)
  auto diffStatus = ItemBuilder()
                      .withName(item.getName())
                      .withDescription(item.getDescription())
                      .withWeight(item.getWeight())
                      .withRarity(item.getRarity())
                      .withType(item.getType())
                      .withStatus(item::Status::EQUIPPED)
                      .build();
  EXPECT_TRUE(item.isEquivalent(diffStatus));

  // Name differs → false
  auto diffName = ItemBuilder()
                    .withName("Steel Sword")
                    .withDescription(item.getDescription())
                    .withWeight(item.getWeight())
                    .withRarity(item.getRarity())
                    .withType(item.getType())
                    .build();
  EXPECT_FALSE(item.isEquivalent(diffName));

  // Description differs → false
  auto diffDesc = ItemBuilder()
                    .withName(item.getName())
                    .withDescription("A completely different description.")
                    .withWeight(item.getWeight())
                    .withRarity(item.getRarity())
                    .withType(item.getType())
                    .build();
  EXPECT_FALSE(item.isEquivalent(diffDesc));

  // Weight differs → false
  auto diffWeight = ItemBuilder()
                      .withName(item.getName())
                      .withDescription(item.getDescription())
                      .withWeight(kDifferentWeight)
                      .withRarity(item.getRarity())
                      .withType(item.getType())
                      .build();
  EXPECT_FALSE(item.isEquivalent(diffWeight));

  // Rarity differs → false
  auto diffRarity = ItemBuilder()
                      .withName(item.getName())
                      .withDescription(item.getDescription())
                      .withWeight(item.getWeight())
                      .withRarity(item::Rarity::Artifact)
                      .withType(item.getType())
                      .build();
  EXPECT_FALSE(item.isEquivalent(diffRarity));

  // Type differs → false
  auto diffType = ItemBuilder()
                    .withName(item.getName())
                    .withDescription(item.getDescription())
                    .withWeight(item.getWeight())
                    .withRarity(item.getRarity())
                    .withType(item::Type::Amulet)
                    .build();
  EXPECT_FALSE(item.isEquivalent(diffType));

  // Owner differs but core fields match → true (owner ignored)
  UUID someOwner;
  auto otherWithOwner = ItemBuilder::duplicateItem(item, std::move(someOwner));
  EXPECT_TRUE(item.isEquivalent(otherWithOwner));
}

// =============================================================================
//  isEqual — equivalence plus matching UUID
// =============================================================================
TEST_F(BaseItemTest, IsEqual)
{
  const auto& item = getTestItem();

  // Self-comparison: equivalent + same UUID → true
  EXPECT_TRUE(item.isEqual(item));

  // Different properties, different UUID → false
  auto other = ItemBuilder()
                 .withName("Other Sword")
                 .withDescription("Different.")
                 .withWeight(kOtherSwordWeight)
                 .withRarity(item::Rarity::Epic)
                 .withType(item::Type::TwoHandedPrimary)
                 .withStatus(item::Status::EQUIPPED)
                 .build();
  EXPECT_FALSE(item.isEqual(other));

  // Same core properties but different UUID → false
  auto clone = ItemBuilder::duplicateItem(item);
  EXPECT_FALSE(item.isEqual(clone));
}

// =============================================================================
//  toJson — inline checks
// =============================================================================
TEST_F(BaseItemTest, ToJson)
{
  const auto& item = getTestItem();

  // Without owner
  {
    const auto result = item.toJson();
    EXPECT_EQ(result["uuid"], item.getUUID().toString());
    EXPECT_EQ(result["name"], "Iron Sword");
    EXPECT_EQ(result["description"], "A sturdy iron sword.");
    EXPECT_EQ(result["weight"], kIronSwordWeight);
    EXPECT_EQ(result["rarity"], "Common");
    EXPECT_EQ(result["type"], "OneHandedPrimary");
    EXPECT_EQ(result["status"], "NOT_OWNDED");
    EXPECT_FALSE(result.contains("owner"));
  }

  // With owner
  {
    UUID owner;
    const auto ownerStr = owner.toString();
    auto itemWithOwner = ItemBuilder::duplicateItem(item, std::move(owner));

    const auto result = itemWithOwner.toJson();
    EXPECT_TRUE(result.contains("owner"));
    EXPECT_EQ(result["owner"], ownerStr);
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
                  .withWeight(kPotionWeight)
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
                 .withWeight(kIronSwordWeight)
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

// NOLINTEND(modernize-use-trailing-return-type,
//           readability-function-cognitive-complexity)

} // namespace

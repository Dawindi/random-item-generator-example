#include <cstddef>
#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>
#include <utility>

#include "armor_item.h"
#include "armor_item_builder.h"
#include "item_builder.h"
#include "item_data_types/item_damage_type.h"
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

// Test fixture values. Weights are in arbitrary units and resistances are
// percentages (0-100).
constexpr float kIronHelmetWeight = 2.5F;
constexpr float kHelmetWeight = 2.0F;
constexpr float kTrainingHelmetWeight = 1.0F;
constexpr float kChestplateWeight = 10.0F;
constexpr float kGauntletsWeight = 3.0F;
constexpr float kGreavesWeight = 4.0F;
constexpr float kAmuletWeight = 0.5F;
constexpr float kRingWeight = 0.2F;
constexpr float kCoinWeight = 0.1F;
constexpr float kFrostAmuletWeight = 0.3F;
constexpr float kGreatswordWeight = 8.0F;
constexpr float kDragonChestplateWeight = 12.0F;

constexpr float kPhysicalResistance = 15.0F;
constexpr float kFireResistance = 5.0F;
constexpr float kBaseResistance = 10.0F;
constexpr float kOverwrittenResistance = 99.0F;
constexpr float kColdResistance = 20.0F;
constexpr float kChaosResistance = 30.0F;
constexpr float kArmGearResistance = 8.0F;
constexpr float kLegGearResistance = 12.0F;
constexpr float kAboveMaxResistance = 150.0F;
constexpr float kJustAboveZeroResistance = 0.001F;
constexpr float kJustBelowMaxResistance = 99.999F;
constexpr float kMaxResistance = 100.0F;
constexpr float kDragonPhysicalResistance = 25.0F;
constexpr float kDragonFireResistance = 50.0F;

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
//  Fixture — a fully-built ArmorItem
// =============================================================================
class ArmorItemTest : public ::testing::Test
{
  public:
  const ArmorItem& getTestItem() const { return item_; }

  protected:
  void SetUp() override
  {
    item_ = ArmorItemBuilder()
              .withName("Iron Helmet")
              .withDescription("A sturdy iron helmet.")
              .withWeight(kIronHelmetWeight)
              .withRarity(item::Rarity::Common)
              .withType(item::Type::HeadGear)
              .withStatus(item::Status::NOT_OWNDED)
              .withResistance(item::DamageType::Physical, kPhysicalResistance)
              .withResistance(item::DamageType::Fire, kFireResistance)
              .build();
  }

  private:
  ArmorItem item_{ArmorItemBuilder().build()};
};

// =============================================================================
//  Construction
// =============================================================================
TEST_F(ArmorItemTest, DefaultConstruction)
{
  ArmorItem const fresh{ArmorItemBuilder().build()};
  EXPECT_FALSE(fresh.getUUID().toString().empty());
  // A default-constructed ArmorItem is invalid — weight, rarity, and type
  // are uninitialised and therefore fail the isValid() checks.
  EXPECT_FALSE(fresh.isValid());
  EXPECT_TRUE(fresh.getName().empty());
  EXPECT_TRUE(fresh.getDescription().empty());
  EXPECT_FALSE(fresh.getOwner().has_value());
  EXPECT_TRUE(fresh.getDamageResistance().empty());
}

// =============================================================================
//  Resistance
// =============================================================================
TEST_F(ArmorItemTest, SetAndGetResistance)
{
  // Fixture item has Physical:15.0, Fire:5.0
  const auto& res = getTestItem().getDamageResistance();
  ASSERT_EQ(res.size(), 2U);
  EXPECT_FLOAT_EQ(res.at(item::DamageType::Physical), kPhysicalResistance);
  EXPECT_FLOAT_EQ(res.at(item::DamageType::Fire), kFireResistance);

  // Single resistance type
  auto coldOnly = ArmorItemBuilder()
                    .withName("Frost Amulet")
                    .withType(item::Type::Amulet)
                    .withRarity(item::Rarity::Common)
                    .withWeight(kFrostAmuletWeight)
                    .withResistance(item::DamageType::Cold, kColdResistance)
                    .build();
  ASSERT_EQ(coldOnly.getDamageResistance().size(), 1U);
  EXPECT_FLOAT_EQ(coldOnly.getDamageResistance().at(item::DamageType::Cold),
                  kColdResistance);

  // Overwrite an existing resistance entry
  auto helm =
    ArmorItemBuilder()
      .withName("Helm")
      .withType(item::Type::HeadGear)
      .withRarity(item::Rarity::Common)
      .withWeight(kHelmetWeight)
      .withResistance(item::DamageType::Physical, kBaseResistance)
      .withResistance(item::DamageType::Physical, kOverwrittenResistance)
      .build();
  ASSERT_EQ(helm.getDamageResistance().size(), 1U);
  EXPECT_FLOAT_EQ(helm.getDamageResistance().at(item::DamageType::Physical),
                  kOverwrittenResistance);
}

// =============================================================================
//  isValid
// =============================================================================
TEST_F(ArmorItemTest, IsValid_FullyBuiltItem)
{
  EXPECT_TRUE(getTestItem().isValid());
}

TEST_F(ArmorItemTest, IsValid_ValidArmorTypes)
{
  // All six armor-slot types should be valid
  auto headGear = ArmorItemBuilder()
                    .withName("Helmet")
                    .withType(item::Type::HeadGear)
                    .withRarity(item::Rarity::Common)
                    .withWeight(kHelmetWeight)
                    .withStatus(item::Status::NOT_OWNDED)
                    .withResistance(item::DamageType::Physical, kBaseResistance)
                    .build();
  EXPECT_TRUE(headGear.isValid());

  auto chestGear =
    ArmorItemBuilder()
      .withName("Chestplate")
      .withType(item::Type::ChestGear)
      .withRarity(item::Rarity::Common)
      .withWeight(kChestplateWeight)
      .withStatus(item::Status::NOT_OWNDED)
      .withResistance(item::DamageType::Physical, kColdResistance)
      .build();
  EXPECT_TRUE(chestGear.isValid());

  auto armGear =
    ArmorItemBuilder()
      .withName("Gauntlets")
      .withType(item::Type::ArmGear)
      .withRarity(item::Rarity::Common)
      .withWeight(kGauntletsWeight)
      .withStatus(item::Status::NOT_OWNDED)
      .withResistance(item::DamageType::Physical, kArmGearResistance)
      .build();
  EXPECT_TRUE(armGear.isValid());

  auto legGear =
    ArmorItemBuilder()
      .withName("Greaves")
      .withType(item::Type::LegGear)
      .withRarity(item::Rarity::Common)
      .withWeight(kGreavesWeight)
      .withStatus(item::Status::NOT_OWNDED)
      .withResistance(item::DamageType::Physical, kLegGearResistance)
      .build();
  EXPECT_TRUE(legGear.isValid());

  auto amulet = ArmorItemBuilder()
                  .withName("Necklace")
                  .withType(item::Type::Amulet)
                  .withRarity(item::Rarity::Common)
                  .withWeight(kAmuletWeight)
                  .withStatus(item::Status::NOT_OWNDED)
                  .withResistance(item::DamageType::Chaos, kChaosResistance)
                  .build();
  EXPECT_TRUE(amulet.isValid());

  auto ring =
    ArmorItemBuilder()
      .withName("Band")
      .withType(item::Type::Ring)
      .withRarity(item::Rarity::Common)
      .withWeight(kRingWeight)
      .withStatus(item::Status::NOT_OWNDED)
      .withResistance(item::DamageType::Lightning, kPhysicalResistance)
      .build();
  EXPECT_TRUE(ring.isValid());
}

TEST_F(ArmorItemTest, IsValid_InvalidArmorTypes)
{
  // Weapon types should be rejected for armor
  auto oneHanded =
    ArmorItemBuilder()
      .withName("Sword")
      .withType(item::Type::OneHandedPrimary)
      .withRarity(item::Rarity::Common)
      .withWeight(kHelmetWeight)
      .withStatus(item::Status::NOT_OWNDED)
      .withResistance(item::DamageType::Physical, kBaseResistance)
      .build();
  EXPECT_FALSE(oneHanded.isValid());

  auto twoHanded =
    ArmorItemBuilder()
      .withName("Greatsword")
      .withType(item::Type::TwoHandedPrimary)
      .withRarity(item::Rarity::Common)
      .withWeight(kGreatswordWeight)
      .withStatus(item::Status::NOT_OWNDED)
      .withResistance(item::DamageType::Physical, kBaseResistance)
      .build();
  EXPECT_FALSE(twoHanded.isValid());

  auto consumable =
    ArmorItemBuilder()
      .withName("Potion")
      .withType(item::Type::Consumable)
      .withRarity(item::Rarity::Common)
      .withWeight(kAmuletWeight)
      .withStatus(item::Status::NOT_OWNDED)
      .withResistance(item::DamageType::Physical, kBaseResistance)
      .build();
  EXPECT_FALSE(consumable.isValid());

  auto currency = ArmorItemBuilder()
                    .withName("Gold")
                    .withType(item::Type::Currency)
                    .withRarity(item::Rarity::Common)
                    .withWeight(kCoinWeight)
                    .withStatus(item::Status::NOT_OWNDED)
                    .withResistance(item::DamageType::Physical, kBaseResistance)
                    .build();
  EXPECT_FALSE(currency.isValid());
}

TEST_F(ArmorItemTest, IsValid_NoResistance)
{
  auto item = ArmorItemBuilder()
                .withName("Helmet")
                .withType(item::Type::HeadGear)
                .withRarity(item::Rarity::Common)
                .withWeight(kHelmetWeight)
                .withStatus(item::Status::NOT_OWNDED)
                .build();
  EXPECT_FALSE(item.isValid());
}

TEST_F(ArmorItemTest, IsValid_ZeroResistance)
{
  auto item = ArmorItemBuilder()
                .withName("Helmet")
                .withType(item::Type::HeadGear)
                .withRarity(item::Rarity::Common)
                .withWeight(kHelmetWeight)
                .withStatus(item::Status::NOT_OWNDED)
                .withResistance(item::DamageType::Physical, 0.0F)
                .build();
  EXPECT_FALSE(item.isValid());
}

TEST_F(ArmorItemTest, IsValid_NegativeResistance)
{
  auto item = ArmorItemBuilder()
                .withName("Helmet")
                .withType(item::Type::HeadGear)
                .withRarity(item::Rarity::Common)
                .withWeight(kHelmetWeight)
                .withStatus(item::Status::NOT_OWNDED)
                .withResistance(item::DamageType::Physical, -1.0F)
                .build();
  EXPECT_FALSE(item.isValid());
}

TEST_F(ArmorItemTest, IsValid_ResistanceAt100)
{
  auto item = ArmorItemBuilder()
                .withName("Helmet")
                .withType(item::Type::HeadGear)
                .withRarity(item::Rarity::Common)
                .withWeight(kHelmetWeight)
                .withStatus(item::Status::NOT_OWNDED)
                .withResistance(item::DamageType::Physical, kMaxResistance)
                .build();
  // 100.0f is not strictly less than 100.0f → invalid
  EXPECT_FALSE(item.isValid());
}

TEST_F(ArmorItemTest, IsValid_ResistanceAbove100)
{
  auto item = ArmorItemBuilder()
                .withName("Helmet")
                .withType(item::Type::HeadGear)
                .withRarity(item::Rarity::Common)
                .withWeight(kHelmetWeight)
                .withStatus(item::Status::NOT_OWNDED)
                .withResistance(item::DamageType::Physical, kAboveMaxResistance)
                .build();
  EXPECT_FALSE(item.isValid());
}

TEST_F(ArmorItemTest, IsValid_ResistanceJustAboveZero)
{
  auto item =
    ArmorItemBuilder()
      .withName("Helmet")
      .withType(item::Type::HeadGear)
      .withRarity(item::Rarity::Common)
      .withWeight(kHelmetWeight)
      .withStatus(item::Status::NOT_OWNDED)
      .withResistance(item::DamageType::Physical, kJustAboveZeroResistance)
      .build();
  EXPECT_TRUE(item.isValid());
}

TEST_F(ArmorItemTest, IsValid_ResistanceJustBelow100)
{
  auto item =
    ArmorItemBuilder()
      .withName("Helmet")
      .withType(item::Type::HeadGear)
      .withRarity(item::Rarity::Common)
      .withWeight(kHelmetWeight)
      .withStatus(item::Status::NOT_OWNDED)
      .withResistance(item::DamageType::Physical, kJustBelowMaxResistance)
      .build();
  EXPECT_TRUE(item.isValid());
}

TEST_F(ArmorItemTest, IsValid_NegativeWeight)
{
  auto item = ArmorItemBuilder()
                .withName("Helmet")
                .withType(item::Type::HeadGear)
                .withRarity(item::Rarity::Common)
                .withWeight(-1.0F)
                .withStatus(item::Status::NOT_OWNDED)
                .withResistance(item::DamageType::Physical, kBaseResistance)
                .build();
  EXPECT_FALSE(item.isValid());
}

TEST_F(ArmorItemTest, IsValid_ZeroWeight)
{
  auto item = ArmorItemBuilder()
                .withName("Helmet")
                .withType(item::Type::HeadGear)
                .withRarity(item::Rarity::Common)
                .withWeight(0.0F)
                .withStatus(item::Status::NOT_OWNDED)
                .withResistance(item::DamageType::Physical, kBaseResistance)
                .build();
  EXPECT_TRUE(item.isValid());
}

// =============================================================================
//  isEquivalent
// =============================================================================
TEST_F(ArmorItemTest, IsEquivalent_MatchingResistance)
{
  const auto& item = getTestItem();
  auto other =
    ArmorItemBuilder()
      .withName(item.getName())
      .withDescription(item.getDescription())
      .withWeight(item.getWeight())
      .withRarity(item.getRarity())
      .withType(item.getType())
      .withResistance(item::DamageType::Physical, kPhysicalResistance)
      .withResistance(item::DamageType::Fire, kFireResistance)
      .withStatus(item::Status::NOT_OWNDED)
      .build();
  EXPECT_TRUE(item.isEquivalent(other));
}

TEST_F(ArmorItemTest, IsEquivalent_DifferentResistanceAmount)
{
  const auto& item = getTestItem();
  auto other =
    ArmorItemBuilder()
      .withName(item.getName())
      .withDescription(item.getDescription())
      .withWeight(item.getWeight())
      .withRarity(item.getRarity())
      .withType(item.getType())
      .withResistance(item::DamageType::Physical, kOverwrittenResistance)
      .withResistance(item::DamageType::Fire, kFireResistance)
      .withStatus(item::Status::NOT_OWNDED)
      .build();
  EXPECT_FALSE(item.isEquivalent(other));
}

TEST_F(ArmorItemTest, IsEquivalent_DifferentResistanceTypes)
{
  const auto& item = getTestItem();
  auto other = ArmorItemBuilder()
                 .withName(item.getName())
                 .withDescription(item.getDescription())
                 .withWeight(item.getWeight())
                 .withRarity(item.getRarity())
                 .withType(item.getType())
                 .withResistance(item::DamageType::Cold, kPhysicalResistance)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_FALSE(item.isEquivalent(other));
}

TEST_F(ArmorItemTest, IsEquivalent_ExtraResistanceType)
{
  const auto& item = getTestItem();
  auto other =
    ArmorItemBuilder()
      .withName(item.getName())
      .withDescription(item.getDescription())
      .withWeight(item.getWeight())
      .withRarity(item.getRarity())
      .withType(item.getType())
      .withResistance(item::DamageType::Physical, kPhysicalResistance)
      .withResistance(item::DamageType::Fire, kFireResistance)
      .withResistance(item::DamageType::Cold, kBaseResistance)
      .withStatus(item::Status::NOT_OWNDED)
      .build();
  EXPECT_FALSE(item.isEquivalent(other));
}

TEST_F(ArmorItemTest, IsEquivalent_FewerResistanceType)
{
  const auto& item = getTestItem();
  auto other =
    ArmorItemBuilder()
      .withName(item.getName())
      .withDescription(item.getDescription())
      .withWeight(item.getWeight())
      .withRarity(item.getRarity())
      .withType(item.getType())
      .withResistance(item::DamageType::Physical, kPhysicalResistance)
      .withStatus(item::Status::NOT_OWNDED)
      .build();
  EXPECT_FALSE(item.isEquivalent(other));
}

TEST_F(ArmorItemTest, IsEquivalent_DifferentCoreFields)
{
  const auto& item = getTestItem();
  auto other =
    ArmorItemBuilder()
      .withName("Steel Helmet")
      .withDescription(item.getDescription())
      .withWeight(item.getWeight())
      .withRarity(item.getRarity())
      .withType(item.getType())
      .withResistance(item::DamageType::Physical, kPhysicalResistance)
      .withResistance(item::DamageType::Fire, kFireResistance)
      .withStatus(item::Status::NOT_OWNDED)
      .build();
  EXPECT_FALSE(item.isEquivalent(other));
}

TEST_F(ArmorItemTest, IsEquivalent_DifferentConcreteType)
{
  // An ArmorItem should not be equivalent to a BaseItem, even if core
  // fields match.
  const auto& item = getTestItem();
  auto base = ItemBuilder()
                .withName(item.getName())
                .withDescription(item.getDescription())
                .withWeight(item.getWeight())
                .withRarity(item.getRarity())
                .withType(item.getType())
                .withStatus(item::Status::NOT_OWNDED)
                .build();
  EXPECT_FALSE(item.isEquivalent(base));
}

// =============================================================================
//  isEqual
// =============================================================================
TEST_F(ArmorItemTest, IsEqual_Self)
{
  const auto& item = getTestItem();
  EXPECT_TRUE(item.isEqual(item));
}

TEST_F(ArmorItemTest, IsEqual_EquivalentButDifferentUUID)
{
  const auto& item = getTestItem();
  auto other =
    ArmorItemBuilder()
      .withName(item.getName())
      .withDescription(item.getDescription())
      .withWeight(item.getWeight())
      .withRarity(item.getRarity())
      .withType(item.getType())
      .withResistance(item::DamageType::Physical, kPhysicalResistance)
      .withResistance(item::DamageType::Fire, kFireResistance)
      .withStatus(item::Status::NOT_OWNDED)
      .build();
  // Same properties, different UUID → not equal
  EXPECT_FALSE(item.isEqual(other));
}

// =============================================================================
//  toJson — inline checks
// =============================================================================
TEST_F(ArmorItemTest, ToJson_WithResistance)
{
  const auto& item = getTestItem();
  const auto result = item.toJson();
  EXPECT_EQ(result["uuid"], item.getUUID().toString());
  EXPECT_EQ(result["name"], "Iron Helmet");
  EXPECT_EQ(result["description"], "A sturdy iron helmet.");
  EXPECT_EQ(result["weight"], kIronHelmetWeight);
  EXPECT_EQ(result["rarity"], "Common");
  EXPECT_EQ(result["type"], "HeadGear");
  EXPECT_EQ(result["status"], "NOT_OWNDED");
  EXPECT_TRUE(result.contains("damage_resistance"));
  EXPECT_FLOAT_EQ(result["damage_resistance"]["Physical"], kPhysicalResistance);
  EXPECT_FLOAT_EQ(result["damage_resistance"]["Fire"], kFireResistance);
}

TEST_F(ArmorItemTest, ToJson_WithoutResistance)
{
  auto noRes = ArmorItemBuilder()
                 .withName("Training Helmet")
                 .withDescription("A basic practice helmet.")
                 .withWeight(kTrainingHelmetWeight)
                 .withRarity(item::Rarity::Common)
                 .withType(item::Type::HeadGear)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  const auto result = noRes.toJson();
  EXPECT_FALSE(result.contains("damage_resistance"));
}

TEST_F(ArmorItemTest, ToJson_WithOwner)
{
  const auto& item = getTestItem();
  UUID owner;
  const auto ownerStr = owner.toString();
  auto itemWithOwner =
    ArmorItemBuilder()
      .withName(item.getName())
      .withDescription(item.getDescription())
      .withWeight(item.getWeight())
      .withRarity(item.getRarity())
      .withType(item.getType())
      .withStatus(item.getStatus())
      .withResistance(item::DamageType::Physical, kPhysicalResistance)
      .withResistance(item::DamageType::Fire, kFireResistance)
      .withOwner(std::move(owner))
      .build();

  const auto result = itemWithOwner.toJson();
  EXPECT_TRUE(result.contains("owner"));
  EXPECT_EQ(result["owner"], ownerStr);
}

// =============================================================================
//  toJson — golden-file comparison
// =============================================================================
TEST_F(ArmorItemTest, ToJson_MatchesExpectedFile)
{
  auto chestplate =
    ArmorItemBuilder()
      .withName("Dragon Scale Chestplate")
      .withDescription(
        "Chestplate forged from the scales of an ancient dragon.")
      .withWeight(kDragonChestplateWeight)
      .withRarity(item::Rarity::Epic)
      .withType(item::Type::ChestGear)
      .withStatus(item::Status::OWNED)
      .withResistance(item::DamageType::Physical, kDragonPhysicalResistance)
      .withResistance(item::DamageType::Fire, kDragonFireResistance)
      .build();

  auto raw = readFile(dataPath("expected_armor_item.json"));
  replaceAll(raw, "00000000-0000-0000-0000-000000000000",
             chestplate.getUUID().toString());
  auto expectedJson = json::parse(raw);

  auto actualJson = chestplate.toJson();

  EXPECT_EQ(actualJson, expectedJson);
}

// =============================================================================
//  ArmorItemBuilder::fromJson
// =============================================================================
TEST_F(ArmorItemTest, FromJson_RoundTrip)
{
  const auto& item = getTestItem();
  const auto result = item.toJson();
  auto restored = ArmorItemBuilder::fromJson(result);
  ASSERT_TRUE(restored.has_value());
  EXPECT_TRUE(restored.value().isValid());
  // Core fields match
  EXPECT_EQ(restored.value().getName(), item.getName());
  EXPECT_EQ(restored.value().getDescription(), item.getDescription());
  EXPECT_FLOAT_EQ(restored.value().getWeight(), item.getWeight());
  EXPECT_EQ(restored.value().getRarity(), item.getRarity());
  EXPECT_EQ(restored.value().getType(), item.getType());
  EXPECT_EQ(restored.value().getStatus(), item.getStatus());
  // Resistance matches
  const auto& origRes = item.getDamageResistance();
  const auto& restoredRes = restored.value().getDamageResistance();
  ASSERT_EQ(restoredRes.size(), origRes.size());
  for (const auto& [type, amount] : origRes)
  {
    auto resistanceIt = restoredRes.find(type);
    ASSERT_NE(resistanceIt, restoredRes.end());
    EXPECT_FLOAT_EQ(resistanceIt->second, amount);
  }
  // UUIDs differ (fromJson always creates a new UUID)
  EXPECT_NE(restored.value().getUUID(), item.getUUID());
}

TEST_F(ArmorItemTest, FromJson_MissingRequiredField)
{
  json jsonData = getTestItem().toJson();
  jsonData.erase("name");
  EXPECT_FALSE(ArmorItemBuilder::fromJson(jsonData).has_value());
}

TEST_F(ArmorItemTest, FromJson_MissingDamageResistance)
{
  json jsonData = getTestItem().toJson();
  jsonData.erase("damage_resistance");
  EXPECT_FALSE(ArmorItemBuilder::fromJson(jsonData).has_value());
}

TEST_F(ArmorItemTest, FromJson_InvalidRarity)
{
  json jsonData = getTestItem().toJson();
  jsonData["rarity"] = "Nonexistent";
  EXPECT_FALSE(ArmorItemBuilder::fromJson(jsonData).has_value());
}

TEST_F(ArmorItemTest, FromJson_InvalidResistanceType)
{
  json jsonData = getTestItem().toJson();
  jsonData["damage_resistance"] = {{"Nonexistent", kFireResistance}};
  auto result = ArmorItemBuilder::fromJson(jsonData);
  ASSERT_TRUE(result.has_value());
  // Unknown resistance types are silently ignored
  EXPECT_TRUE(result.value().getDamageResistance().empty());
}

TEST_F(ArmorItemTest, FromJson_NonNumericResistance)
{
  json jsonData = getTestItem().toJson();
  jsonData["damage_resistance"] = {{"Physical", "five"}};
  auto result = ArmorItemBuilder::fromJson(jsonData);
  ASSERT_TRUE(result.has_value());
  // Non-numeric resistance values are silently ignored
  EXPECT_TRUE(result.value().getDamageResistance().empty());
}

TEST_F(ArmorItemTest, FromJson_ResistanceCaseInsensitive)
{
  json jsonData = getTestItem().toJson();
  jsonData["damage_resistance"] = {{"physical", kPhysicalResistance},
                                   {"FIRE", kFireResistance}};
  auto result = ArmorItemBuilder::fromJson(jsonData);
  ASSERT_TRUE(result.has_value());
  EXPECT_FLOAT_EQ(
    result.value().getDamageResistance().at(item::DamageType::Physical),
    kPhysicalResistance);
  EXPECT_FLOAT_EQ(
    result.value().getDamageResistance().at(item::DamageType::Fire),
    kFireResistance);
}

TEST_F(ArmorItemTest, FromJson_FileNotFound)
{
  EXPECT_FALSE(
    ArmorItemBuilder::fromJson(std::filesystem::path{"nonexistent_file.json"})
      .has_value());
}

TEST_F(ArmorItemTest, FromJson_InvalidSyntax)
{
  EXPECT_FALSE(ArmorItemBuilder::fromJson(
                 std::filesystem::path{dataPath("invalid_syntax.json")})
                 .has_value());
}

// NOLINTEND(modernize-use-trailing-return-type,
//           readability-function-cognitive-complexity)

} // namespace

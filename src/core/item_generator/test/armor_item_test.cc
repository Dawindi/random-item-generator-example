#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>

#include "armor_item_builder.h"
#include "item_builder.h"
#include "item_data_types/item_damage_type.h"
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
//  Fixture — a fully-built ArmorItem
// =============================================================================
class ArmorItemTest : public ::testing::Test
{
  protected:
  void SetUp() override
  {
    item_ = ArmorItemBuilder()
              .withName("Iron Helmet")
              .withDescription("A sturdy iron helmet.")
              .withWeight(2.5f)
              .withRarity(item::Rarity::Common)
              .withType(item::Type::HeadGear)
              .withStatus(item::Status::NOT_OWNDED)
              .withResistance(item::DamageType::Physical, 15.0f)
              .withResistance(item::DamageType::Fire, 5.0f)
              .build();
  }

  ArmorItem item_{ArmorItemBuilder().build()};
};

// =============================================================================
//  Construction
// =============================================================================
TEST_F(ArmorItemTest, DefaultConstruction)
{
  ArmorItem fresh{ArmorItemBuilder().build()};
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
  const auto& res = item_.getDamageResistance();
  ASSERT_EQ(res.size(), 2u);
  EXPECT_FLOAT_EQ(res.at(item::DamageType::Physical), 15.0f);
  EXPECT_FLOAT_EQ(res.at(item::DamageType::Fire), 5.0f);

  // Single resistance type
  auto coldOnly = ArmorItemBuilder()
                    .withName("Frost Amulet")
                    .withType(item::Type::Amulet)
                    .withRarity(item::Rarity::Common)
                    .withWeight(0.3f)
                    .withResistance(item::DamageType::Cold, 20.0f)
                    .build();
  ASSERT_EQ(coldOnly.getDamageResistance().size(), 1u);
  EXPECT_FLOAT_EQ(coldOnly.getDamageResistance().at(item::DamageType::Cold),
                  20.0f);

  // Overwrite an existing resistance entry
  auto helm = ArmorItemBuilder()
                .withName("Helm")
                .withType(item::Type::HeadGear)
                .withRarity(item::Rarity::Common)
                .withWeight(2.0f)
                .withResistance(item::DamageType::Physical, 10.0f)
                .withResistance(item::DamageType::Physical, 99.0f)
                .build();
  ASSERT_EQ(helm.getDamageResistance().size(), 1u);
  EXPECT_FLOAT_EQ(helm.getDamageResistance().at(item::DamageType::Physical),
                  99.0f);
}

// =============================================================================
//  isValid
// =============================================================================
TEST_F(ArmorItemTest, IsValid_FullyBuiltItem) { EXPECT_TRUE(item_.isValid()); }

TEST_F(ArmorItemTest, IsValid_ValidArmorTypes)
{
  // All six armor-slot types should be valid
  auto headGear = ArmorItemBuilder()
                    .withName("Helmet")
                    .withType(item::Type::HeadGear)
                    .withRarity(item::Rarity::Common)
                    .withWeight(2.0f)
                    .withStatus(item::Status::NOT_OWNDED)
                    .withResistance(item::DamageType::Physical, 10.0f)
                    .build();
  EXPECT_TRUE(headGear.isValid());

  auto chestGear = ArmorItemBuilder()
                     .withName("Chestplate")
                     .withType(item::Type::ChestGear)
                     .withRarity(item::Rarity::Common)
                     .withWeight(10.0f)
                     .withStatus(item::Status::NOT_OWNDED)
                     .withResistance(item::DamageType::Physical, 20.0f)
                     .build();
  EXPECT_TRUE(chestGear.isValid());

  auto armGear = ArmorItemBuilder()
                   .withName("Gauntlets")
                   .withType(item::Type::ArmGear)
                   .withRarity(item::Rarity::Common)
                   .withWeight(3.0f)
                   .withStatus(item::Status::NOT_OWNDED)
                   .withResistance(item::DamageType::Physical, 8.0f)
                   .build();
  EXPECT_TRUE(armGear.isValid());

  auto legGear = ArmorItemBuilder()
                   .withName("Greaves")
                   .withType(item::Type::LegGear)
                   .withRarity(item::Rarity::Common)
                   .withWeight(4.0f)
                   .withStatus(item::Status::NOT_OWNDED)
                   .withResistance(item::DamageType::Physical, 12.0f)
                   .build();
  EXPECT_TRUE(legGear.isValid());

  auto amulet = ArmorItemBuilder()
                  .withName("Necklace")
                  .withType(item::Type::Amulet)
                  .withRarity(item::Rarity::Common)
                  .withWeight(0.5f)
                  .withStatus(item::Status::NOT_OWNDED)
                  .withResistance(item::DamageType::Chaos, 30.0f)
                  .build();
  EXPECT_TRUE(amulet.isValid());

  auto ring = ArmorItemBuilder()
                .withName("Band")
                .withType(item::Type::Ring)
                .withRarity(item::Rarity::Common)
                .withWeight(0.2f)
                .withStatus(item::Status::NOT_OWNDED)
                .withResistance(item::DamageType::Lightning, 15.0f)
                .build();
  EXPECT_TRUE(ring.isValid());
}

TEST_F(ArmorItemTest, IsValid_InvalidArmorTypes)
{
  // Weapon types should be rejected for armor
  auto oneHanded = ArmorItemBuilder()
                     .withName("Sword")
                     .withType(item::Type::OneHandedPrimary)
                     .withRarity(item::Rarity::Common)
                     .withWeight(2.0f)
                     .withStatus(item::Status::NOT_OWNDED)
                     .withResistance(item::DamageType::Physical, 10.0f)
                     .build();
  EXPECT_FALSE(oneHanded.isValid());

  auto twoHanded = ArmorItemBuilder()
                     .withName("Greatsword")
                     .withType(item::Type::TwoHandedPrimary)
                     .withRarity(item::Rarity::Common)
                     .withWeight(8.0f)
                     .withStatus(item::Status::NOT_OWNDED)
                     .withResistance(item::DamageType::Physical, 10.0f)
                     .build();
  EXPECT_FALSE(twoHanded.isValid());

  auto consumable = ArmorItemBuilder()
                      .withName("Potion")
                      .withType(item::Type::Consumable)
                      .withRarity(item::Rarity::Common)
                      .withWeight(0.5f)
                      .withStatus(item::Status::NOT_OWNDED)
                      .withResistance(item::DamageType::Physical, 10.0f)
                      .build();
  EXPECT_FALSE(consumable.isValid());

  auto currency = ArmorItemBuilder()
                    .withName("Gold")
                    .withType(item::Type::Currency)
                    .withRarity(item::Rarity::Common)
                    .withWeight(0.1f)
                    .withStatus(item::Status::NOT_OWNDED)
                    .withResistance(item::DamageType::Physical, 10.0f)
                    .build();
  EXPECT_FALSE(currency.isValid());
}

TEST_F(ArmorItemTest, IsValid_NoResistance)
{
  auto item = ArmorItemBuilder()
                .withName("Helmet")
                .withType(item::Type::HeadGear)
                .withRarity(item::Rarity::Common)
                .withWeight(2.0f)
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
                .withWeight(2.0f)
                .withStatus(item::Status::NOT_OWNDED)
                .withResistance(item::DamageType::Physical, 0.0f)
                .build();
  EXPECT_FALSE(item.isValid());
}

TEST_F(ArmorItemTest, IsValid_NegativeResistance)
{
  auto item = ArmorItemBuilder()
                .withName("Helmet")
                .withType(item::Type::HeadGear)
                .withRarity(item::Rarity::Common)
                .withWeight(2.0f)
                .withStatus(item::Status::NOT_OWNDED)
                .withResistance(item::DamageType::Physical, -1.0f)
                .build();
  EXPECT_FALSE(item.isValid());
}

TEST_F(ArmorItemTest, IsValid_ResistanceAt100)
{
  auto item = ArmorItemBuilder()
                .withName("Helmet")
                .withType(item::Type::HeadGear)
                .withRarity(item::Rarity::Common)
                .withWeight(2.0f)
                .withStatus(item::Status::NOT_OWNDED)
                .withResistance(item::DamageType::Physical, 100.0f)
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
                .withWeight(2.0f)
                .withStatus(item::Status::NOT_OWNDED)
                .withResistance(item::DamageType::Physical, 150.0f)
                .build();
  EXPECT_FALSE(item.isValid());
}

TEST_F(ArmorItemTest, IsValid_ResistanceJustAboveZero)
{
  auto item = ArmorItemBuilder()
                .withName("Helmet")
                .withType(item::Type::HeadGear)
                .withRarity(item::Rarity::Common)
                .withWeight(2.0f)
                .withStatus(item::Status::NOT_OWNDED)
                .withResistance(item::DamageType::Physical, 0.001f)
                .build();
  EXPECT_TRUE(item.isValid());
}

TEST_F(ArmorItemTest, IsValid_ResistanceJustBelow100)
{
  auto item = ArmorItemBuilder()
                .withName("Helmet")
                .withType(item::Type::HeadGear)
                .withRarity(item::Rarity::Common)
                .withWeight(2.0f)
                .withStatus(item::Status::NOT_OWNDED)
                .withResistance(item::DamageType::Physical, 99.999f)
                .build();
  EXPECT_TRUE(item.isValid());
}

TEST_F(ArmorItemTest, IsValid_NegativeWeight)
{
  auto item = ArmorItemBuilder()
                .withName("Helmet")
                .withType(item::Type::HeadGear)
                .withRarity(item::Rarity::Common)
                .withWeight(-1.0f)
                .withStatus(item::Status::NOT_OWNDED)
                .withResistance(item::DamageType::Physical, 10.0f)
                .build();
  EXPECT_FALSE(item.isValid());
}

TEST_F(ArmorItemTest, IsValid_ZeroWeight)
{
  auto item = ArmorItemBuilder()
                .withName("Helmet")
                .withType(item::Type::HeadGear)
                .withRarity(item::Rarity::Common)
                .withWeight(0.0f)
                .withStatus(item::Status::NOT_OWNDED)
                .withResistance(item::DamageType::Physical, 10.0f)
                .build();
  EXPECT_TRUE(item.isValid());
}

// =============================================================================
//  isEquivalent
// =============================================================================
TEST_F(ArmorItemTest, IsEquivalent_MatchingResistance)
{
  auto other = ArmorItemBuilder()
                 .withName(item_.getName())
                 .withDescription(item_.getDescription())
                 .withWeight(item_.getWeight())
                 .withRarity(item_.getRarity())
                 .withType(item_.getType())
                 .withResistance(item::DamageType::Physical, 15.0f)
                 .withResistance(item::DamageType::Fire, 5.0f)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_TRUE(item_.isEquivalent(other));
}

TEST_F(ArmorItemTest, IsEquivalent_DifferentResistanceAmount)
{
  auto other = ArmorItemBuilder()
                 .withName(item_.getName())
                 .withDescription(item_.getDescription())
                 .withWeight(item_.getWeight())
                 .withRarity(item_.getRarity())
                 .withType(item_.getType())
                 .withResistance(item::DamageType::Physical, 99.0f)
                 .withResistance(item::DamageType::Fire, 5.0f)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_FALSE(item_.isEquivalent(other));
}

TEST_F(ArmorItemTest, IsEquivalent_DifferentResistanceTypes)
{
  auto other = ArmorItemBuilder()
                 .withName(item_.getName())
                 .withDescription(item_.getDescription())
                 .withWeight(item_.getWeight())
                 .withRarity(item_.getRarity())
                 .withType(item_.getType())
                 .withResistance(item::DamageType::Cold, 15.0f)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_FALSE(item_.isEquivalent(other));
}

TEST_F(ArmorItemTest, IsEquivalent_ExtraResistanceType)
{
  auto other = ArmorItemBuilder()
                 .withName(item_.getName())
                 .withDescription(item_.getDescription())
                 .withWeight(item_.getWeight())
                 .withRarity(item_.getRarity())
                 .withType(item_.getType())
                 .withResistance(item::DamageType::Physical, 15.0f)
                 .withResistance(item::DamageType::Fire, 5.0f)
                 .withResistance(item::DamageType::Cold, 10.0f)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_FALSE(item_.isEquivalent(other));
}

TEST_F(ArmorItemTest, IsEquivalent_FewerResistanceType)
{
  auto other = ArmorItemBuilder()
                 .withName(item_.getName())
                 .withDescription(item_.getDescription())
                 .withWeight(item_.getWeight())
                 .withRarity(item_.getRarity())
                 .withType(item_.getType())
                 .withResistance(item::DamageType::Physical, 15.0f)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_FALSE(item_.isEquivalent(other));
}

TEST_F(ArmorItemTest, IsEquivalent_DifferentCoreFields)
{
  auto other = ArmorItemBuilder()
                 .withName("Steel Helmet")
                 .withDescription(item_.getDescription())
                 .withWeight(item_.getWeight())
                 .withRarity(item_.getRarity())
                 .withType(item_.getType())
                 .withResistance(item::DamageType::Physical, 15.0f)
                 .withResistance(item::DamageType::Fire, 5.0f)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_FALSE(item_.isEquivalent(other));
}

TEST_F(ArmorItemTest, IsEquivalent_DifferentConcreteType)
{
  // An ArmorItem should not be equivalent to a BaseItem, even if core
  // fields match.
  auto base = ItemBuilder()
                .withName(item_.getName())
                .withDescription(item_.getDescription())
                .withWeight(item_.getWeight())
                .withRarity(item_.getRarity())
                .withType(item_.getType())
                .withStatus(item::Status::NOT_OWNDED)
                .build();
  EXPECT_FALSE(item_.isEquivalent(base));
}

// =============================================================================
//  isEqual
// =============================================================================
TEST_F(ArmorItemTest, IsEqual_Self) { EXPECT_TRUE(item_.isEqual(item_)); }

TEST_F(ArmorItemTest, IsEqual_EquivalentButDifferentUUID)
{
  auto other = ArmorItemBuilder()
                 .withName(item_.getName())
                 .withDescription(item_.getDescription())
                 .withWeight(item_.getWeight())
                 .withRarity(item_.getRarity())
                 .withType(item_.getType())
                 .withResistance(item::DamageType::Physical, 15.0f)
                 .withResistance(item::DamageType::Fire, 5.0f)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  // Same properties, different UUID → not equal
  EXPECT_FALSE(item_.isEqual(other));
}

// =============================================================================
//  toJson — inline checks
// =============================================================================
TEST_F(ArmorItemTest, ToJson_WithResistance)
{
  const auto j = item_.toJson();
  EXPECT_EQ(j["uuid"], item_.getUUID().toString());
  EXPECT_EQ(j["name"], "Iron Helmet");
  EXPECT_EQ(j["description"], "A sturdy iron helmet.");
  EXPECT_EQ(j["weight"], 2.5);
  EXPECT_EQ(j["rarity"], "Common");
  EXPECT_EQ(j["type"], "HeadGear");
  EXPECT_EQ(j["status"], "NOT_OWNDED");
  EXPECT_TRUE(j.contains("damage_resistance"));
  EXPECT_FLOAT_EQ(j["damage_resistance"]["Physical"], 15.0);
  EXPECT_FLOAT_EQ(j["damage_resistance"]["Fire"], 5.0);
}

TEST_F(ArmorItemTest, ToJson_WithoutResistance)
{
  auto noRes = ArmorItemBuilder()
                 .withName("Training Helmet")
                 .withDescription("A basic practice helmet.")
                 .withWeight(1.0f)
                 .withRarity(item::Rarity::Common)
                 .withType(item::Type::HeadGear)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  const auto j = noRes.toJson();
  EXPECT_FALSE(j.contains("damage_resistance"));
}

TEST_F(ArmorItemTest, ToJson_WithOwner)
{
  UUID owner;
  const auto ownerStr = owner.toString();
  auto itemWithOwner = ArmorItemBuilder()
                         .withName(item_.getName())
                         .withDescription(item_.getDescription())
                         .withWeight(item_.getWeight())
                         .withRarity(item_.getRarity())
                         .withType(item_.getType())
                         .withStatus(item_.getStatus())
                         .withResistance(item::DamageType::Physical, 15.0f)
                         .withResistance(item::DamageType::Fire, 5.0f)
                         .withOwner(std::move(owner))
                         .build();

  const auto j = itemWithOwner.toJson();
  EXPECT_TRUE(j.contains("owner"));
  EXPECT_EQ(j["owner"], ownerStr);
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
      .withWeight(12.0f)
      .withRarity(item::Rarity::Epic)
      .withType(item::Type::ChestGear)
      .withStatus(item::Status::OWNED)
      .withResistance(item::DamageType::Physical, 25.0f)
      .withResistance(item::DamageType::Fire, 50.0f)
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
  const auto j = item_.toJson();
  auto restored = ArmorItemBuilder::fromJson(j);
  ASSERT_TRUE(restored.has_value());
  EXPECT_TRUE(restored->isValid());
  // Core fields match
  EXPECT_EQ(restored->getName(), item_.getName());
  EXPECT_EQ(restored->getDescription(), item_.getDescription());
  EXPECT_FLOAT_EQ(restored->getWeight(), item_.getWeight());
  EXPECT_EQ(restored->getRarity(), item_.getRarity());
  EXPECT_EQ(restored->getType(), item_.getType());
  EXPECT_EQ(restored->getStatus(), item_.getStatus());
  // Resistance matches
  const auto& origRes = item_.getDamageResistance();
  const auto& restoredRes = restored->getDamageResistance();
  ASSERT_EQ(restoredRes.size(), origRes.size());
  for (const auto& [type, amount] : origRes)
  {
    auto it = restoredRes.find(type);
    ASSERT_NE(it, restoredRes.end());
    EXPECT_FLOAT_EQ(it->second, amount);
  }
  // UUIDs differ (fromJson always creates a new UUID)
  EXPECT_NE(restored->getUUID(), item_.getUUID());
}

TEST_F(ArmorItemTest, FromJson_MissingRequiredField)
{
  json j = item_.toJson();
  j.erase("name");
  EXPECT_FALSE(ArmorItemBuilder::fromJson(j).has_value());
}

TEST_F(ArmorItemTest, FromJson_MissingDamageResistance)
{
  json j = item_.toJson();
  j.erase("damage_resistance");
  EXPECT_FALSE(ArmorItemBuilder::fromJson(j).has_value());
}

TEST_F(ArmorItemTest, FromJson_InvalidRarity)
{
  json j = item_.toJson();
  j["rarity"] = "Nonexistent";
  EXPECT_FALSE(ArmorItemBuilder::fromJson(j).has_value());
}

TEST_F(ArmorItemTest, FromJson_InvalidResistanceType)
{
  json j = item_.toJson();
  j["damage_resistance"] = {{"Nonexistent", 5.0}};
  auto result = ArmorItemBuilder::fromJson(j);
  ASSERT_TRUE(result.has_value());
  // Unknown resistance types are silently ignored
  EXPECT_TRUE(result->getDamageResistance().empty());
}

TEST_F(ArmorItemTest, FromJson_NonNumericResistance)
{
  json j = item_.toJson();
  j["damage_resistance"] = {{"Physical", "five"}};
  auto result = ArmorItemBuilder::fromJson(j);
  ASSERT_TRUE(result.has_value());
  // Non-numeric resistance values are silently ignored
  EXPECT_TRUE(result->getDamageResistance().empty());
}

TEST_F(ArmorItemTest, FromJson_ResistanceCaseInsensitive)
{
  json j = item_.toJson();
  j["damage_resistance"] = {{"physical", 15.0}, {"FIRE", 5.0}};
  auto result = ArmorItemBuilder::fromJson(j);
  ASSERT_TRUE(result.has_value());
  EXPECT_FLOAT_EQ(result->getDamageResistance().at(item::DamageType::Physical),
                  15.0f);
  EXPECT_FLOAT_EQ(result->getDamageResistance().at(item::DamageType::Fire),
                  5.0f);
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

} // namespace

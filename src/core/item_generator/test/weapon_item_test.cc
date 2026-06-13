#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>

#include "item_builder.h"
#include "item_data_types/item_damage_type.h"
#include "item_data_types/item_rarity.h"
#include "item_data_types/item_status.h"
#include "item_data_types/item_type.h"
#include "uuid.h"
#include "weapon_item_builder.h"

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
//  Fixture — a fully-built WeaponItem
// =============================================================================
class WeaponItemTest : public ::testing::Test
{
  protected:
  void SetUp() override
  {
    item_ = WeaponItemBuilder()
              .withName("Iron Sword")
              .withDescription("A sturdy iron sword.")
              .withWeight(3.5f)
              .withRarity(item::Rarity::Common)
              .withType(item::Type::OneHandedPrimary)
              .withStatus(item::Status::NOT_OWNDED)
              .withDamage(item::DamageType::Physical, 5)
              .withDamage(item::DamageType::Fire, 3)
              .withRangeInMeters(1.5f)
              .build();
  }

  WeaponItem item_{WeaponItemBuilder().build()};
};

// =============================================================================
//  Construction
// =============================================================================
TEST_F(WeaponItemTest, DefaultConstruction)
{
  WeaponItem fresh{WeaponItemBuilder().build()};
  EXPECT_FALSE(fresh.getUUID().toString().empty());
  // A default-constructed WeaponItem is invalid — weight, rarity, and type
  // are uninitialised and therefore fail the isValid() checks.
  EXPECT_FALSE(fresh.isValid());
  EXPECT_TRUE(fresh.getName().empty());
  EXPECT_TRUE(fresh.getDescription().empty());
  EXPECT_FALSE(fresh.getOwner().has_value());
  EXPECT_TRUE(fresh.getDamage().empty());
  EXPECT_FLOAT_EQ(fresh.getRangeInMeters(), 0.1f);
}

// =============================================================================
//  Damage
// =============================================================================
TEST_F(WeaponItemTest, SetAndGetDamage)
{
  // Fixture item has Physical:5, Fire:3
  const auto& dmg = item_.getDamage();
  ASSERT_EQ(dmg.size(), 2u);
  EXPECT_EQ(dmg.at(item::DamageType::Physical), 5);
  EXPECT_EQ(dmg.at(item::DamageType::Fire), 3);

  // Single damage type
  auto coldOnly = WeaponItemBuilder()
                    .withName("Ice Dagger")
                    .withType(item::Type::OneHandedSecondary)
                    .withRarity(item::Rarity::Common)
                    .withWeight(1.0f)
                    .withDamage(item::DamageType::Cold, 10)
                    .build();
  ASSERT_EQ(coldOnly.getDamage().size(), 1u);
  EXPECT_EQ(coldOnly.getDamage().at(item::DamageType::Cold), 10);

  // Overwrite an existing damage entry
  auto sword = WeaponItemBuilder()
                 .withName("Sword")
                 .withType(item::Type::OneHandedPrimary)
                 .withRarity(item::Rarity::Common)
                 .withWeight(2.0f)
                 .withDamage(item::DamageType::Physical, 5)
                 .withDamage(item::DamageType::Physical, 99)
                 .build();
  ASSERT_EQ(sword.getDamage().size(), 1u);
  EXPECT_EQ(sword.getDamage().at(item::DamageType::Physical), 99);
}

// =============================================================================
//  Range
// =============================================================================
TEST_F(WeaponItemTest, SetAndGetRange)
{
  EXPECT_FLOAT_EQ(item_.getRangeInMeters(), 1.5f);

  auto bow = WeaponItemBuilder()
               .withName("Longbow")
               .withType(item::Type::TwoHandedPrimary)
               .withRarity(item::Rarity::Common)
               .withWeight(2.0f)
               .withRangeInMeters(100.0f)
               .build();
  EXPECT_FLOAT_EQ(bow.getRangeInMeters(), 100.0f);
}

TEST_F(WeaponItemTest, IsValid_NegativeRange)
{
  auto item = WeaponItemBuilder()
                .withName("Sword")
                .withType(item::Type::OneHandedPrimary)
                .withRarity(item::Rarity::Common)
                .withWeight(2.0f)
                .withStatus(item::Status::NOT_OWNDED)
                .withRangeInMeters(-1.0f)
                .build();
  EXPECT_FALSE(item.isValid());
}

TEST_F(WeaponItemTest, IsValid_ZeroRange)
{
  auto item = WeaponItemBuilder()
                .withName("Sword")
                .withType(item::Type::OneHandedPrimary)
                .withRarity(item::Rarity::Common)
                .withWeight(2.0f)
                .withStatus(item::Status::NOT_OWNDED)
                .withRangeInMeters(0.0f)
                .build();
  EXPECT_FALSE(item.isValid());
}

TEST_F(WeaponItemTest, IsValid_PositiveRange)
{
  auto item = WeaponItemBuilder()
                .withName("Sword")
                .withType(item::Type::OneHandedPrimary)
                .withRarity(item::Rarity::Common)
                .withWeight(2.0f)
                .withStatus(item::Status::NOT_OWNDED)
                .withRangeInMeters(0.1f)
                .build();
  EXPECT_TRUE(item.isValid());
}

// =============================================================================
//  isValid
// =============================================================================
TEST_F(WeaponItemTest, IsValid_FullyBuiltItem) { EXPECT_TRUE(item_.isValid()); }

TEST_F(WeaponItemTest, IsValid_ValidWeaponTypes)
{
  // All three weapon-slot types should be valid
  auto oneHandedPrimary = WeaponItemBuilder()
                            .withName("Sword")
                            .withType(item::Type::OneHandedPrimary)
                            .withRarity(item::Rarity::Common)
                            .withWeight(2.0f)
                            .withStatus(item::Status::NOT_OWNDED)
                            .build();
  EXPECT_TRUE(oneHandedPrimary.isValid());

  auto oneHandedSecondary = WeaponItemBuilder()
                              .withName("Dagger")
                              .withType(item::Type::OneHandedSecondary)
                              .withRarity(item::Rarity::Common)
                              .withWeight(1.0f)
                              .withStatus(item::Status::NOT_OWNDED)
                              .build();
  EXPECT_TRUE(oneHandedSecondary.isValid());

  auto twoHanded = WeaponItemBuilder()
                     .withName("Greatsword")
                     .withType(item::Type::TwoHandedPrimary)
                     .withRarity(item::Rarity::Common)
                     .withWeight(8.0f)
                     .withStatus(item::Status::NOT_OWNDED)
                     .build();
  EXPECT_TRUE(twoHanded.isValid());
}

TEST_F(WeaponItemTest, IsValid_InvalidWeaponTypes)
{
  // Non-weapon types should be rejected
  auto headGear = WeaponItemBuilder()
                    .withName("Helmet")
                    .withType(item::Type::HeadGear)
                    .withRarity(item::Rarity::Common)
                    .withWeight(2.0f)
                    .withStatus(item::Status::NOT_OWNDED)
                    .build();
  EXPECT_FALSE(headGear.isValid());

  auto chestGear = WeaponItemBuilder()
                     .withName("Chestplate")
                     .withType(item::Type::ChestGear)
                     .withRarity(item::Rarity::Common)
                     .withWeight(10.0f)
                     .withStatus(item::Status::NOT_OWNDED)
                     .build();
  EXPECT_FALSE(chestGear.isValid());

  auto armGear = WeaponItemBuilder()
                   .withName("Gauntlets")
                   .withType(item::Type::ArmGear)
                   .withRarity(item::Rarity::Common)
                   .withWeight(3.0f)
                   .withStatus(item::Status::NOT_OWNDED)
                   .build();
  EXPECT_FALSE(armGear.isValid());

  auto legGear = WeaponItemBuilder()
                   .withName("Greaves")
                   .withType(item::Type::LegGear)
                   .withRarity(item::Rarity::Common)
                   .withWeight(4.0f)
                   .withStatus(item::Status::NOT_OWNDED)
                   .build();
  EXPECT_FALSE(legGear.isValid());

  auto amulet = WeaponItemBuilder()
                  .withName("Necklace")
                  .withType(item::Type::Amulet)
                  .withRarity(item::Rarity::Common)
                  .withWeight(0.5f)
                  .withStatus(item::Status::NOT_OWNDED)
                  .build();
  EXPECT_FALSE(amulet.isValid());

  auto ring = WeaponItemBuilder()
                .withName("Band")
                .withType(item::Type::Ring)
                .withRarity(item::Rarity::Common)
                .withWeight(0.2f)
                .withStatus(item::Status::NOT_OWNDED)
                .build();
  EXPECT_FALSE(ring.isValid());

  auto consumable = WeaponItemBuilder()
                      .withName("Potion")
                      .withType(item::Type::Consumable)
                      .withRarity(item::Rarity::Common)
                      .withWeight(0.5f)
                      .withStatus(item::Status::NOT_OWNDED)
                      .build();
  EXPECT_FALSE(consumable.isValid());

  auto currency = WeaponItemBuilder()
                    .withName("Gold")
                    .withType(item::Type::Currency)
                    .withRarity(item::Rarity::Common)
                    .withWeight(0.1f)
                    .withStatus(item::Status::NOT_OWNDED)
                    .build();
  EXPECT_FALSE(currency.isValid());
}

TEST_F(WeaponItemTest, IsValid_NegativeWeight)
{
  auto item = WeaponItemBuilder()
                .withName("Sword")
                .withType(item::Type::OneHandedPrimary)
                .withRarity(item::Rarity::Common)
                .withWeight(-1.0f)
                .withStatus(item::Status::NOT_OWNDED)
                .build();
  EXPECT_FALSE(item.isValid());
}

TEST_F(WeaponItemTest, IsValid_ZeroWeight)
{
  auto item = WeaponItemBuilder()
                .withName("Sword")
                .withType(item::Type::OneHandedPrimary)
                .withRarity(item::Rarity::Common)
                .withWeight(0.0f)
                .withStatus(item::Status::NOT_OWNDED)
                .build();
  EXPECT_TRUE(item.isValid());
}

// =============================================================================
//  isEquivalent
// =============================================================================
TEST_F(WeaponItemTest, IsEquivalent_MatchingDamage)
{
  auto other = WeaponItemBuilder()
                 .withName(item_.getName())
                 .withDescription(item_.getDescription())
                 .withWeight(item_.getWeight())
                 .withRarity(item_.getRarity())
                 .withType(item_.getType())
                 .withDamage(item::DamageType::Physical, 5)
                 .withDamage(item::DamageType::Fire, 3)
                 .withRangeInMeters(1.5f)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_TRUE(item_.isEquivalent(other));
}

TEST_F(WeaponItemTest, IsEquivalent_DifferentDamageAmount)
{
  auto other = WeaponItemBuilder()
                 .withName(item_.getName())
                 .withDescription(item_.getDescription())
                 .withWeight(item_.getWeight())
                 .withRarity(item_.getRarity())
                 .withType(item_.getType())
                 .withDamage(item::DamageType::Physical, 999)
                 .withDamage(item::DamageType::Fire, 3)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_FALSE(item_.isEquivalent(other));
}

TEST_F(WeaponItemTest, IsEquivalent_DifferentDamageTypes)
{
  auto other = WeaponItemBuilder()
                 .withName(item_.getName())
                 .withDescription(item_.getDescription())
                 .withWeight(item_.getWeight())
                 .withRarity(item_.getRarity())
                 .withType(item_.getType())
                 .withDamage(item::DamageType::Cold, 5)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_FALSE(item_.isEquivalent(other));
}

TEST_F(WeaponItemTest, IsEquivalent_DifferentRange)
{
  auto other = WeaponItemBuilder()
                 .withName(item_.getName())
                 .withDescription(item_.getDescription())
                 .withWeight(item_.getWeight())
                 .withRarity(item_.getRarity())
                 .withType(item_.getType())
                 .withDamage(item::DamageType::Physical, 5)
                 .withDamage(item::DamageType::Fire, 3)
                 .withRangeInMeters(99.0f)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_FALSE(item_.isEquivalent(other));
}

TEST_F(WeaponItemTest, IsEquivalent_DifferentCoreFields)
{
  auto other = WeaponItemBuilder()
                 .withName("Steel Sword")
                 .withDescription(item_.getDescription())
                 .withWeight(item_.getWeight())
                 .withRarity(item_.getRarity())
                 .withType(item_.getType())
                 .withDamage(item::DamageType::Physical, 5)
                 .withDamage(item::DamageType::Fire, 3)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_FALSE(item_.isEquivalent(other));
}

TEST_F(WeaponItemTest, IsEquivalent_DifferentConcreteType)
{
  // A WeaponItem should not be equivalent to a BaseItem, even if core
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
TEST_F(WeaponItemTest, IsEqual_Self) { EXPECT_TRUE(item_.isEqual(item_)); }

TEST_F(WeaponItemTest, IsEqual_EquivalentButDifferentUUID)
{
  auto other = WeaponItemBuilder()
                 .withName(item_.getName())
                 .withDescription(item_.getDescription())
                 .withWeight(item_.getWeight())
                 .withRarity(item_.getRarity())
                 .withType(item_.getType())
                 .withDamage(item::DamageType::Physical, 5)
                 .withDamage(item::DamageType::Fire, 3)
                 .withRangeInMeters(1.5f)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  // Same properties, different UUID → not equal
  EXPECT_FALSE(item_.isEqual(other));
}

// =============================================================================
//  toJson — inline checks
// =============================================================================
TEST_F(WeaponItemTest, ToJson_WithDamage)
{
  const auto j = item_.toJson();
  EXPECT_EQ(j["uuid"], item_.getUUID().toString());
  EXPECT_EQ(j["name"], "Iron Sword");
  EXPECT_EQ(j["description"], "A sturdy iron sword.");
  EXPECT_EQ(j["weight"], 3.5);
  EXPECT_EQ(j["rarity"], "Common");
  EXPECT_EQ(j["type"], "OneHandedPrimary");
  EXPECT_EQ(j["status"], "NOT_OWNDED");
  EXPECT_TRUE(j.contains("damage"));
  EXPECT_EQ(j["damage"]["Physical"], 5);
  EXPECT_EQ(j["damage"]["Fire"], 3);
  EXPECT_EQ(j["rangeInMeters"], 1.5);
}

TEST_F(WeaponItemTest, ToJson_WithoutDamage)
{
  auto noDamage = WeaponItemBuilder()
                    .withName("Training Sword")
                    .withDescription("A wooden practice sword.")
                    .withWeight(2.0f)
                    .withRarity(item::Rarity::Common)
                    .withType(item::Type::OneHandedPrimary)
                    .withStatus(item::Status::NOT_OWNDED)
                    .build();
  const auto j = noDamage.toJson();
  EXPECT_FALSE(j.contains("damage"));
}

TEST_F(WeaponItemTest, ToJson_WithOwner)
{
  UUID owner;
  const auto ownerStr = owner.toString();
  auto itemWithOwner = WeaponItemBuilder()
                         .withName(item_.getName())
                         .withDescription(item_.getDescription())
                         .withWeight(item_.getWeight())
                         .withRarity(item_.getRarity())
                         .withType(item_.getType())
                         .withStatus(item_.getStatus())
                         .withDamage(item::DamageType::Physical, 5)
                         .withDamage(item::DamageType::Fire, 3)
                         .withOwner(std::move(owner))
                         .build();

  const auto j = itemWithOwner.toJson();
  EXPECT_TRUE(j.contains("owner"));
  EXPECT_EQ(j["owner"], ownerStr);
}

// =============================================================================
//  toJson — golden-file comparison
// =============================================================================
TEST_F(WeaponItemTest, ToJson_MatchesExpectedFile)
{
  auto sword = WeaponItemBuilder()
                 .withName("Flaming Sword")
                 .withDescription("A blade wreathed in eternal flame.")
                 .withWeight(4.0f)
                 .withRarity(item::Rarity::Rare)
                 .withType(item::Type::OneHandedPrimary)
                 .withStatus(item::Status::OWNED)
                 .withDamage(item::DamageType::Physical, 8)
                 .withDamage(item::DamageType::Fire, 12)
                 .build();

  auto raw = readFile(dataPath("expected_weapon_item.json"));
  replaceAll(raw, "00000000-0000-0000-0000-000000000000",
             sword.getUUID().toString());
  auto expectedJson = json::parse(raw);

  auto actualJson = sword.toJson();

  // Float comparison: verify rangeInMeters within tolerance.
  ASSERT_TRUE(actualJson.contains("rangeInMeters"));
  ASSERT_TRUE(expectedJson.contains("rangeInMeters"));
  EXPECT_NEAR(actualJson["rangeInMeters"].get<float>(),
              expectedJson["rangeInMeters"].get<float>(), 1e-5f);

  // Normalise range so structural comparison succeeds.
  actualJson["rangeInMeters"] = 0;
  expectedJson["rangeInMeters"] = 0;

  EXPECT_EQ(actualJson, expectedJson);
}

// =============================================================================
//  WeaponItemBuilder::fromJson
// =============================================================================
TEST_F(WeaponItemTest, FromJson_RoundTrip)
{
  const auto j = item_.toJson();
  auto restored = WeaponItemBuilder::fromJson(j);
  ASSERT_TRUE(restored.has_value());
  EXPECT_TRUE(restored->isValid());
  // Core fields match
  EXPECT_EQ(restored->getName(), item_.getName());
  EXPECT_EQ(restored->getDescription(), item_.getDescription());
  EXPECT_FLOAT_EQ(restored->getWeight(), item_.getWeight());
  EXPECT_EQ(restored->getRarity(), item_.getRarity());
  EXPECT_EQ(restored->getType(), item_.getType());
  EXPECT_EQ(restored->getStatus(), item_.getStatus());
  // Damage matches
  EXPECT_EQ(restored->getDamage(), item_.getDamage());
  // Range matches
  EXPECT_FLOAT_EQ(restored->getRangeInMeters(), item_.getRangeInMeters());
  // UUIDs differ (fromJson always creates a new UUID)
  EXPECT_NE(restored->getUUID(), item_.getUUID());
}

TEST_F(WeaponItemTest, FromJson_MissingRequiredField)
{
  json j = item_.toJson();
  j.erase("name");
  EXPECT_FALSE(WeaponItemBuilder::fromJson(j).has_value());
}

TEST_F(WeaponItemTest, FromJson_InvalidRarity)
{
  json j = item_.toJson();
  j["rarity"] = "Nonexistent";
  EXPECT_FALSE(WeaponItemBuilder::fromJson(j).has_value());
}

TEST_F(WeaponItemTest, FromJson_InvalidDamageType)
{
  json j = item_.toJson();
  j["damage"] = {{"Nonexistent", 5}};
  auto result = WeaponItemBuilder::fromJson(j);
  ASSERT_TRUE(result.has_value());
  // Unknown damage types are silently ignored
  EXPECT_TRUE(result->getDamage().empty());
}

TEST_F(WeaponItemTest, FromJson_NonIntegerDamage)
{
  json j = item_.toJson();
  j["damage"] = {{"Physical", 3.14}};
  auto result = WeaponItemBuilder::fromJson(j);
  ASSERT_TRUE(result.has_value());
  // Non-integer damage values are silently ignored
  EXPECT_TRUE(result->getDamage().empty());
}

TEST_F(WeaponItemTest, FromJson_DamageCaseInsensitive)
{
  json j = item_.toJson();
  j["damage"] = {{"physical", 5}, {"FIRE", 3}};
  auto result = WeaponItemBuilder::fromJson(j);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->getDamage().at(item::DamageType::Physical), 5);
  EXPECT_EQ(result->getDamage().at(item::DamageType::Fire), 3);
}

TEST_F(WeaponItemTest, FromJson_FileNotFound)
{
  EXPECT_FALSE(
    WeaponItemBuilder::fromJson(std::filesystem::path{"nonexistent_file.json"})
      .has_value());
}

TEST_F(WeaponItemTest, FromJson_InvalidSyntax)
{
  EXPECT_FALSE(WeaponItemBuilder::fromJson(
                 std::filesystem::path{dataPath("invalid_syntax.json")})
                 .has_value());
}

} // namespace

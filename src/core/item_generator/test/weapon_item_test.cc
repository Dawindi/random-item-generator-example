#include <cstddef>
#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>
#include <utility>

#include "item_builder.h"
#include "item_data_types/item_damage_type.h"
#include "item_data_types/item_rarity.h"
#include "item_data_types/item_status.h"
#include "item_data_types/item_type.h"
#include "item_serialization.h"
#include "nlohmann/json_fwd.hpp"
#include "uuid.h"
#include "weapon_item.h"
#include "weapon_item_builder.h"

using nlohmann::json;

namespace
{

// gtest's TEST_F macro expands to `void TestBody()` and its ASSERT/EXPECT
// macros expand to `if` statements, so the trailing-return-type and
// cognitive-complexity checks fire on code we cannot edit. These two checks
// are suppressed for this test file for that reason.
// NOLINTBEGIN(modernize-use-trailing-return-type,
//             readability-function-cognitive-complexity)

// Test fixture values. Weights are in arbitrary units, damage amounts are
// integer damage per hit, and ranges are in meters.
constexpr float kIronSwordWeight = 3.5F;
constexpr float kSwordWeight = 2.0F;
constexpr float kDaggerWeight = 1.0F;
constexpr float kGreatswordWeight = 8.0F;
constexpr float kChestplateWeight = 10.0F;
constexpr float kGauntletsWeight = 3.0F;
constexpr float kGreavesWeight = 4.0F;
constexpr float kAmuletWeight = 0.5F;
constexpr float kRingWeight = 0.2F;
constexpr float kCoinWeight = 0.1F;
constexpr float kFlamingSwordWeight = 4.0F;

constexpr int kPhysicalDamage = 5;
constexpr int kFireDamage = 3;
constexpr int kColdDamage = 10;
constexpr int kOverwrittenDamage = 99;
constexpr int kVeryDifferentDamage = 999;
constexpr int kFlamingPhysicalDamage = 8;
constexpr int kFlamingFireDamage = 12;

constexpr float kMeleeRange = 1.5F;
constexpr float kLongbowRange = 100.0F;
constexpr float kDifferentRange = 99.0F;
constexpr float kMinimumRange = 0.1F;
constexpr double kNonIntegerDamage = 3.14;

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
//  Fixture — a fully-built WeaponItem
// =============================================================================
class WeaponItemTest : public ::testing::Test
{
  public:
  [[nodiscard]] auto geTesttWeaponItem() const -> const WeaponItem&
  {
    return item_;
  }

  protected:
  void SetUp() override
  {
    item_ = WeaponItemBuilder()
              .withName("Iron Sword")
              .withDescription("A sturdy iron sword.")
              .withWeight(kIronSwordWeight)
              .withRarity(item::Rarity::Common)
              .withType(item::Type::OneHandedPrimary)
              .withStatus(item::Status::NOT_OWNDED)
              .withDamage(item::DamageType::Physical, kPhysicalDamage)
              .withDamage(item::DamageType::Fire, kFireDamage)
              .withRangeInMeters(kMeleeRange)
              .build();
  }

  private:
  WeaponItem item_{WeaponItemBuilder().build()};
};

// =============================================================================
//  Construction
// =============================================================================
TEST_F(WeaponItemTest, DefaultConstruction)
{
  WeaponItem const fresh{WeaponItemBuilder().build()};
  EXPECT_FALSE(fresh.getUUID().toString().empty());
  // A default-constructed WeaponItem is invalid — weight, rarity, and type
  // are uninitialised and therefore fail the isValid() checks.
  EXPECT_FALSE(fresh.isValid());
  EXPECT_TRUE(fresh.getName().empty());
  EXPECT_TRUE(fresh.getDescription().empty());
  EXPECT_FALSE(fresh.getOwner().has_value());
  EXPECT_TRUE(fresh.getDamage().empty());
  EXPECT_FLOAT_EQ(fresh.getRangeInMeters(), kMinimumRange);
}

// =============================================================================
//  Damage
// =============================================================================
TEST_F(WeaponItemTest, SetAndGetDamage)
{
  // Fixture item has Physical:5, Fire:3
  const auto& dmg = geTesttWeaponItem().getDamage();
  ASSERT_EQ(dmg.size(), 2U);
  EXPECT_EQ(dmg.at(item::DamageType::Physical), kPhysicalDamage);
  EXPECT_EQ(dmg.at(item::DamageType::Fire), kFireDamage);

  // Single damage type
  auto coldOnly = WeaponItemBuilder()
                    .withName("Ice Dagger")
                    .withType(item::Type::OneHandedSecondary)
                    .withRarity(item::Rarity::Common)
                    .withWeight(kDaggerWeight)
                    .withDamage(item::DamageType::Cold, kColdDamage)
                    .build();
  ASSERT_EQ(coldOnly.getDamage().size(), 1U);
  EXPECT_EQ(coldOnly.getDamage().at(item::DamageType::Cold), kColdDamage);

  // Overwrite an existing damage entry
  auto sword = WeaponItemBuilder()
                 .withName("Sword")
                 .withType(item::Type::OneHandedPrimary)
                 .withRarity(item::Rarity::Common)
                 .withWeight(kSwordWeight)
                 .withDamage(item::DamageType::Physical, kPhysicalDamage)
                 .withDamage(item::DamageType::Physical, kOverwrittenDamage)
                 .build();
  ASSERT_EQ(sword.getDamage().size(), 1U);
  EXPECT_EQ(sword.getDamage().at(item::DamageType::Physical),
            kOverwrittenDamage);
}

// =============================================================================
//  Range
// =============================================================================
TEST_F(WeaponItemTest, SetAndGetRange)
{
  EXPECT_FLOAT_EQ(geTesttWeaponItem().getRangeInMeters(), kMeleeRange);

  auto bow = WeaponItemBuilder()
               .withName("Longbow")
               .withType(item::Type::TwoHandedPrimary)
               .withRarity(item::Rarity::Common)
               .withWeight(kSwordWeight)
               .withRangeInMeters(kLongbowRange)
               .build();
  EXPECT_FLOAT_EQ(bow.getRangeInMeters(), kLongbowRange);
}

TEST_F(WeaponItemTest, IsValid_NegativeRange)
{
  auto item = WeaponItemBuilder()
                .withName("Sword")
                .withType(item::Type::OneHandedPrimary)
                .withRarity(item::Rarity::Common)
                .withWeight(kSwordWeight)
                .withStatus(item::Status::NOT_OWNDED)
                .withRangeInMeters(-1.0F)
                .build();
  EXPECT_FALSE(item.isValid());
}

TEST_F(WeaponItemTest, IsValid_ZeroRange)
{
  auto item = WeaponItemBuilder()
                .withName("Sword")
                .withType(item::Type::OneHandedPrimary)
                .withRarity(item::Rarity::Common)
                .withWeight(kSwordWeight)
                .withStatus(item::Status::NOT_OWNDED)
                .withRangeInMeters(0.0F)
                .build();
  EXPECT_FALSE(item.isValid());
}

TEST_F(WeaponItemTest, IsValid_PositiveRange)
{
  auto item = WeaponItemBuilder()
                .withName("Sword")
                .withType(item::Type::OneHandedPrimary)
                .withRarity(item::Rarity::Common)
                .withWeight(kSwordWeight)
                .withStatus(item::Status::NOT_OWNDED)
                .withRangeInMeters(kMinimumRange)
                .build();
  EXPECT_TRUE(item.isValid());
}

// =============================================================================
//  isValid
// =============================================================================
TEST_F(WeaponItemTest, IsValid_FullyBuiltItem)
{
  EXPECT_TRUE(geTesttWeaponItem().isValid());
}

TEST_F(WeaponItemTest, IsValid_ValidWeaponTypes)
{
  // All three weapon-slot types should be valid
  auto oneHandedPrimary = WeaponItemBuilder()
                            .withName("Sword")
                            .withType(item::Type::OneHandedPrimary)
                            .withRarity(item::Rarity::Common)
                            .withWeight(kSwordWeight)
                            .withStatus(item::Status::NOT_OWNDED)
                            .build();
  EXPECT_TRUE(oneHandedPrimary.isValid());

  auto oneHandedSecondary = WeaponItemBuilder()
                              .withName("Dagger")
                              .withType(item::Type::OneHandedSecondary)
                              .withRarity(item::Rarity::Common)
                              .withWeight(kDaggerWeight)
                              .withStatus(item::Status::NOT_OWNDED)
                              .build();
  EXPECT_TRUE(oneHandedSecondary.isValid());

  auto twoHanded = WeaponItemBuilder()
                     .withName("Greatsword")
                     .withType(item::Type::TwoHandedPrimary)
                     .withRarity(item::Rarity::Common)
                     .withWeight(kGreatswordWeight)
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
                    .withWeight(kSwordWeight)
                    .withStatus(item::Status::NOT_OWNDED)
                    .build();
  EXPECT_FALSE(headGear.isValid());

  auto chestGear = WeaponItemBuilder()
                     .withName("Chestplate")
                     .withType(item::Type::ChestGear)
                     .withRarity(item::Rarity::Common)
                     .withWeight(kChestplateWeight)
                     .withStatus(item::Status::NOT_OWNDED)
                     .build();
  EXPECT_FALSE(chestGear.isValid());

  auto armGear = WeaponItemBuilder()
                   .withName("Gauntlets")
                   .withType(item::Type::ArmGear)
                   .withRarity(item::Rarity::Common)
                   .withWeight(kGauntletsWeight)
                   .withStatus(item::Status::NOT_OWNDED)
                   .build();
  EXPECT_FALSE(armGear.isValid());

  auto legGear = WeaponItemBuilder()
                   .withName("Greaves")
                   .withType(item::Type::LegGear)
                   .withRarity(item::Rarity::Common)
                   .withWeight(kGreavesWeight)
                   .withStatus(item::Status::NOT_OWNDED)
                   .build();
  EXPECT_FALSE(legGear.isValid());

  auto amulet = WeaponItemBuilder()
                  .withName("Necklace")
                  .withType(item::Type::Amulet)
                  .withRarity(item::Rarity::Common)
                  .withWeight(kAmuletWeight)
                  .withStatus(item::Status::NOT_OWNDED)
                  .build();
  EXPECT_FALSE(amulet.isValid());

  auto ring = WeaponItemBuilder()
                .withName("Band")
                .withType(item::Type::Ring)
                .withRarity(item::Rarity::Common)
                .withWeight(kRingWeight)
                .withStatus(item::Status::NOT_OWNDED)
                .build();
  EXPECT_FALSE(ring.isValid());

  auto consumable = WeaponItemBuilder()
                      .withName("Potion")
                      .withType(item::Type::Consumable)
                      .withRarity(item::Rarity::Common)
                      .withWeight(kAmuletWeight)
                      .withStatus(item::Status::NOT_OWNDED)
                      .build();
  EXPECT_FALSE(consumable.isValid());

  auto currency = WeaponItemBuilder()
                    .withName("Gold")
                    .withType(item::Type::Currency)
                    .withRarity(item::Rarity::Common)
                    .withWeight(kCoinWeight)
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
                .withWeight(-1.0F)
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
                .withWeight(0.0F)
                .withStatus(item::Status::NOT_OWNDED)
                .build();
  EXPECT_TRUE(item.isValid());
}

// =============================================================================
//  isEquivalent
// =============================================================================
TEST_F(WeaponItemTest, IsEquivalent_MatchingDamage)
{
  const auto& testWeaponItem = geTesttWeaponItem();
  auto other = WeaponItemBuilder()
                 .withName(testWeaponItem.getName())
                 .withDescription(testWeaponItem.getDescription())
                 .withWeight(testWeaponItem.getWeight())
                 .withRarity(testWeaponItem.getRarity())
                 .withType(testWeaponItem.getType())
                 .withDamage(item::DamageType::Physical, kPhysicalDamage)
                 .withDamage(item::DamageType::Fire, kFireDamage)
                 .withRangeInMeters(kMeleeRange)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_TRUE(testWeaponItem.isEquivalent(other));
}

TEST_F(WeaponItemTest, IsEquivalent_DifferentDamageAmount)
{
  const auto& testWeaponItem = geTesttWeaponItem();
  auto other = WeaponItemBuilder()
                 .withName(testWeaponItem.getName())
                 .withDescription(testWeaponItem.getDescription())
                 .withWeight(testWeaponItem.getWeight())
                 .withRarity(testWeaponItem.getRarity())
                 .withType(testWeaponItem.getType())
                 .withDamage(item::DamageType::Physical, kVeryDifferentDamage)
                 .withDamage(item::DamageType::Fire, kFireDamage)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_FALSE(testWeaponItem.isEquivalent(other));
}

TEST_F(WeaponItemTest, IsEquivalent_DifferentDamageTypes)
{
  const auto& testWeaponItem = geTesttWeaponItem();
  auto other = WeaponItemBuilder()
                 .withName(testWeaponItem.getName())
                 .withDescription(testWeaponItem.getDescription())
                 .withWeight(testWeaponItem.getWeight())
                 .withRarity(testWeaponItem.getRarity())
                 .withType(testWeaponItem.getType())
                 .withDamage(item::DamageType::Cold, kPhysicalDamage)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_FALSE(testWeaponItem.isEquivalent(other));
}

TEST_F(WeaponItemTest, IsEquivalent_DifferentRange)
{
  const auto& testWeaponItem = geTesttWeaponItem();
  auto other = WeaponItemBuilder()
                 .withName(testWeaponItem.getName())
                 .withDescription(testWeaponItem.getDescription())
                 .withWeight(testWeaponItem.getWeight())
                 .withRarity(testWeaponItem.getRarity())
                 .withType(testWeaponItem.getType())
                 .withDamage(item::DamageType::Physical, kPhysicalDamage)
                 .withDamage(item::DamageType::Fire, kFireDamage)
                 .withRangeInMeters(kDifferentRange)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_FALSE(testWeaponItem.isEquivalent(other));
}

TEST_F(WeaponItemTest, IsEquivalent_DifferentCoreFields)
{
  const auto& testWeaponItem = geTesttWeaponItem();
  auto other = WeaponItemBuilder()
                 .withName("Steel Sword")
                 .withDescription(testWeaponItem.getDescription())
                 .withWeight(testWeaponItem.getWeight())
                 .withRarity(testWeaponItem.getRarity())
                 .withType(testWeaponItem.getType())
                 .withDamage(item::DamageType::Physical, kPhysicalDamage)
                 .withDamage(item::DamageType::Fire, kFireDamage)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  EXPECT_FALSE(testWeaponItem.isEquivalent(other));
}

TEST_F(WeaponItemTest, IsEquivalent_DifferentConcreteType)
{
  // A WeaponItem should not be equivalent to a BaseItem, even if core
  // fields match.
  const auto& testWeaponItem = geTesttWeaponItem();
  auto base = ItemBuilder()
                .withName(testWeaponItem.getName())
                .withDescription(testWeaponItem.getDescription())
                .withWeight(testWeaponItem.getWeight())
                .withRarity(testWeaponItem.getRarity())
                .withType(testWeaponItem.getType())
                .withStatus(item::Status::NOT_OWNDED)
                .build();
  EXPECT_FALSE(testWeaponItem.isEquivalent(base));
}

// =============================================================================
//  isEqual
// =============================================================================
TEST_F(WeaponItemTest, IsEqual_Self)
{
  const auto& testWeaponItem = geTesttWeaponItem();
  EXPECT_TRUE(testWeaponItem.isEqual(testWeaponItem));
}

TEST_F(WeaponItemTest, IsEqual_EquivalentButDifferentUUID)
{
  const auto& testWeaponItem = geTesttWeaponItem();
  auto other = WeaponItemBuilder()
                 .withName(testWeaponItem.getName())
                 .withDescription(testWeaponItem.getDescription())
                 .withWeight(testWeaponItem.getWeight())
                 .withRarity(testWeaponItem.getRarity())
                 .withType(testWeaponItem.getType())
                 .withDamage(item::DamageType::Physical, kPhysicalDamage)
                 .withDamage(item::DamageType::Fire, kFireDamage)
                 .withRangeInMeters(kMeleeRange)
                 .withStatus(item::Status::NOT_OWNDED)
                 .build();
  // Same properties, different UUID → not equal
  EXPECT_FALSE(testWeaponItem.isEqual(other));
}

// =============================================================================
//  toJson — inline checks
// =============================================================================
TEST_F(WeaponItemTest, ToJson_WithDamage)
{
  const auto& testWeaponItem = geTesttWeaponItem();
  const auto envelope = testWeaponItem.toJson();
  EXPECT_TRUE(item::verifyItem(envelope).has_value());
  const auto result = envelope["payload"];
  EXPECT_EQ(result["uuid"], testWeaponItem.getUUID().toString());
  EXPECT_EQ(result["name"], "Iron Sword");
  EXPECT_EQ(result["description"], "A sturdy iron sword.");
  EXPECT_EQ(result["weight"], kIronSwordWeight);
  EXPECT_EQ(result["rarity"], "Common");
  EXPECT_EQ(result["type"], "OneHandedPrimary");
  EXPECT_EQ(result["status"], "NOT_OWNDED");
  EXPECT_TRUE(result.contains("damage"));
  EXPECT_EQ(result["damage"]["Physical"], kPhysicalDamage);
  EXPECT_EQ(result["damage"]["Fire"], kFireDamage);
  EXPECT_EQ(result["rangeInMeters"], kMeleeRange);
}

TEST_F(WeaponItemTest, ToJson_WithoutDamage)
{
  auto noDamage = WeaponItemBuilder()
                    .withName("Training Sword")
                    .withDescription("A wooden practice sword.")
                    .withWeight(kSwordWeight)
                    .withRarity(item::Rarity::Common)
                    .withType(item::Type::OneHandedPrimary)
                    .withStatus(item::Status::NOT_OWNDED)
                    .build();
  const auto envelope = noDamage.toJson();
  EXPECT_TRUE(item::verifyItem(envelope).has_value());
  const auto result = envelope["payload"];
  EXPECT_FALSE(result.contains("damage"));
}

TEST_F(WeaponItemTest, ToJson_WithOwner)
{
  const auto& testWeaponItem = geTesttWeaponItem();
  UUID owner;
  const auto ownerStr = owner.toString();
  auto itemWithOwner =
    WeaponItemBuilder()
      .withName(testWeaponItem.getName())
      .withDescription(testWeaponItem.getDescription())
      .withWeight(testWeaponItem.getWeight())
      .withRarity(testWeaponItem.getRarity())
      .withType(testWeaponItem.getType())
      .withStatus(testWeaponItem.getStatus())
      .withDamage(item::DamageType::Physical, kPhysicalDamage)
      .withDamage(item::DamageType::Fire, kFireDamage)
      .withOwner(std::move(owner))
      .build();

  const auto envelope = itemWithOwner.toJson();
  EXPECT_TRUE(item::verifyItem(envelope).has_value());
  const auto result = envelope["payload"];
  EXPECT_TRUE(result.contains("owner"));
  EXPECT_EQ(result["owner"], ownerStr);

  // The owner round-trips through deserialization.
  auto restored = WeaponItemBuilder::fromJson(envelope);
  ASSERT_TRUE(restored.has_value());
  ASSERT_TRUE(restored.value().getOwner().has_value());
  EXPECT_EQ(restored.value().getOwner().value().toString(), ownerStr);
}

// =============================================================================
//  toJson — golden-file comparison
// =============================================================================
TEST_F(WeaponItemTest, ToJson_MatchesExpectedFile)
{
  auto sword = WeaponItemBuilder()
                 .withName("Flaming Sword")
                 .withDescription("A blade wreathed in eternal flame.")
                 .withWeight(kFlamingSwordWeight)
                 .withRarity(item::Rarity::Rare)
                 .withType(item::Type::OneHandedPrimary)
                 .withStatus(item::Status::OWNED)
                 .withDamage(item::DamageType::Physical, kFlamingPhysicalDamage)
                 .withDamage(item::DamageType::Fire, kFlamingFireDamage)
                 .build();

  auto raw = readFile(dataPath("expected_weapon_item.json"));
  replaceAll(raw, "00000000-0000-0000-0000-000000000000",
             sword.getUUID().toString());
  auto expectedPayload = json::parse(raw);

  auto envelope = sword.toJson();
  ASSERT_TRUE(item::verifyItem(envelope).has_value());
  auto actualPayload = envelope["payload"];

  // Float comparison: verify rangeInMeters within tolerance.
  ASSERT_TRUE(actualPayload.contains("rangeInMeters"));
  ASSERT_TRUE(expectedPayload.contains("rangeInMeters"));
  EXPECT_NEAR(actualPayload["rangeInMeters"].get<float>(),
              expectedPayload["rangeInMeters"].get<float>(), 1e-5F);

  // Normalise range so structural comparison succeeds.
  actualPayload["rangeInMeters"] = 0;
  expectedPayload["rangeInMeters"] = 0;

  EXPECT_EQ(actualPayload, expectedPayload);
}

// =============================================================================
//  WeaponItemBuilder::fromJson
// =============================================================================
TEST_F(WeaponItemTest, FromJson_RoundTrip)
{
  const auto& testWeaponItem = geTesttWeaponItem();
  const auto result = testWeaponItem.toJson();
  auto restored = WeaponItemBuilder::fromJson(result);
  ASSERT_TRUE(restored.has_value());
  EXPECT_TRUE(restored.value().isValid());
  // Core fields match
  EXPECT_EQ(restored.value().getName(), testWeaponItem.getName());
  EXPECT_EQ(restored.value().getDescription(), testWeaponItem.getDescription());
  EXPECT_FLOAT_EQ(restored.value().getWeight(), testWeaponItem.getWeight());
  EXPECT_EQ(restored.value().getRarity(), testWeaponItem.getRarity());
  EXPECT_EQ(restored.value().getType(), testWeaponItem.getType());
  EXPECT_EQ(restored.value().getStatus(), testWeaponItem.getStatus());
  // Damage matches
  EXPECT_EQ(restored.value().getDamage(), testWeaponItem.getDamage());
  // Range matches
  EXPECT_FLOAT_EQ(restored.value().getRangeInMeters(),
                  testWeaponItem.getRangeInMeters());
  // UUID is preserved across the round trip.
  EXPECT_EQ(restored.value().getUUID(), testWeaponItem.getUUID());
}

TEST_F(WeaponItemTest, FromJson_MissingRequiredField)
{
  json payload = geTesttWeaponItem().toJson()["payload"];
  payload.erase("name");
  EXPECT_FALSE(
    WeaponItemBuilder::fromJson(item::signItem(payload)).has_value());
}

TEST_F(WeaponItemTest, FromJson_InvalidRarity)
{
  json payload = geTesttWeaponItem().toJson()["payload"];
  payload["rarity"] = "Nonexistent";
  EXPECT_FALSE(
    WeaponItemBuilder::fromJson(item::signItem(payload)).has_value());
}

TEST_F(WeaponItemTest, FromJson_InvalidDamageType)
{
  json payload = geTesttWeaponItem().toJson()["payload"];
  payload["damage"] = {{"Nonexistent", kPhysicalDamage}};
  auto result = WeaponItemBuilder::fromJson(item::signItem(payload));
  ASSERT_TRUE(result.has_value());
  // Unknown damage types are silently ignored
  EXPECT_TRUE(result.value().getDamage().empty());
}

TEST_F(WeaponItemTest, FromJson_NonIntegerDamage)
{
  json payload = geTesttWeaponItem().toJson()["payload"];
  payload["damage"] = {{"Physical", kNonIntegerDamage}};
  auto result = WeaponItemBuilder::fromJson(item::signItem(payload));
  ASSERT_TRUE(result.has_value());
  // Non-integer damage values are silently ignored
  EXPECT_TRUE(result.value().getDamage().empty());
}

TEST_F(WeaponItemTest, FromJson_DamageCaseInsensitive)
{
  json payload = geTesttWeaponItem().toJson()["payload"];
  payload["damage"] = {{"physical", kPhysicalDamage}, {"FIRE", kFireDamage}};
  auto result = WeaponItemBuilder::fromJson(item::signItem(payload));
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value().getDamage().at(item::DamageType::Physical),
            kPhysicalDamage);
  EXPECT_EQ(result.value().getDamage().at(item::DamageType::Fire), kFireDamage);
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

// NOLINTEND(modernize-use-trailing-return-type,
//           readability-function-cognitive-complexity)

} // namespace

#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>

#include "base_item.h"
#include "item_builder.h"
#include "item_rarity.h"
#include "item_status.h"
#include "item_type.h"

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

// =============================================================================
//  Fixture — a valid JSON object used across tests
// =============================================================================
class ItemBuilderFromJsonTest : public ::testing::Test
{
  protected:
  void SetUp() override
  {
    validJson_["name"] = "Iron Sword";
    validJson_["description"] = "A sturdy iron sword.";
    validJson_["weight"] = 3.5;
    validJson_["rarity"] = "Common";
    validJson_["type"] = "OneHandedPrimary";
    validJson_["status"] = "OWNED";
  }

  json validJson_;
};

// =============================================================================
//  fromJson(const json&) — success paths
// =============================================================================
TEST_F(ItemBuilderFromJsonTest, FromJsonObject_Success)
{
  auto item = ItemBuilder::fromJson(validJson_);
  ASSERT_TRUE(item.has_value());

  EXPECT_EQ(item->getName(), "Iron Sword");
  EXPECT_EQ(item->getDescription(), "A sturdy iron sword.");
  EXPECT_FLOAT_EQ(item->getWeight(), 3.5f);
  EXPECT_EQ(item->getRarity(), item::Rarity::Common);
  EXPECT_EQ(item->getType(), item::Type::OneHandedPrimary);
  EXPECT_EQ(item->getStatus(), item::Status::OWNED);

  // UUID is auto-generated — verify it's non-empty (valid).
  EXPECT_FALSE(item->getUUID().toString().empty());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_GeneratesNewUUID)
{
  auto a = ItemBuilder::fromJson(validJson_);
  auto b = ItemBuilder::fromJson(validJson_);
  ASSERT_TRUE(a.has_value());
  ASSERT_TRUE(b.has_value());

  // Two items built from the same JSON get different UUIDs.
  EXPECT_NE(a->getUUID().toString(), b->getUUID().toString());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_IgnoresUUID)
{
  validJson_["uuid"] = "00000000-0000-0000-0000-000000000000";
  auto item = ItemBuilder::fromJson(validJson_);
  ASSERT_TRUE(item.has_value());

  // UUID is not the one from JSON.
  EXPECT_NE(item->getUUID().toString(), "00000000-0000-0000-0000-000000000000");
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_IgnoresOwner)
{
  validJson_["owner"] = "11111111-1111-1111-1111-111111111111";
  auto item = ItemBuilder::fromJson(validJson_);
  ASSERT_TRUE(item.has_value());

  // Owner is ignored — should be nullopt.
  EXPECT_FALSE(item->getOwner().has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_IgnoresExtraFields)
{
  validJson_["extra"] = "unexpected";
  validJson_["another"] = 42;
  auto item = ItemBuilder::fromJson(validJson_);
  ASSERT_TRUE(item.has_value());

  EXPECT_EQ(item->getName(), "Iron Sword");
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_AllRarityValues)
{
  struct
  {
    const char* str;
    item::Rarity expected;
  } cases[] = {{"Common", item::Rarity::Common},
               {"Uncommon", item::Rarity::Uncommon},
               {"Rare", item::Rarity::Rare},
               {"Epic", item::Rarity::Epic},
               {"Legendary", item::Rarity::Legendary},
               {"Artifact", item::Rarity::Artifact}};

  for (const auto& [str, expected] : cases)
  {
    validJson_["rarity"] = str;
    auto item = ItemBuilder::fromJson(validJson_);
    ASSERT_TRUE(item.has_value()) << "Failed for rarity: " << str;
    EXPECT_EQ(item->getRarity(), expected);
  }
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_AllTypeValues)
{
  struct
  {
    const char* str;
    item::Type expected;
  } cases[] = {{"OneHandedPrimary", item::Type::OneHandedPrimary},
               {"OneHandedSecondary", item::Type::OneHandedSecondary},
               {"TwoHandedPrimary", item::Type::TwoHandedPrimary},
               {"HeadGear", item::Type::HeadGear},
               {"ChestGear", item::Type::ChestGear},
               {"ArmGear", item::Type::ArmGear},
               {"LegGear", item::Type::LegGear},
               {"Amulet", item::Type::Amulet},
               {"Ring", item::Type::Ring},
               {"Consumable", item::Type::Consumable},
               {"Currency", item::Type::Currency}};

  for (const auto& [str, expected] : cases)
  {
    validJson_["type"] = str;
    auto item = ItemBuilder::fromJson(validJson_);
    ASSERT_TRUE(item.has_value()) << "Failed for type: " << str;
    EXPECT_EQ(item->getType(), expected);
  }
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_AllStatusValues)
{
  struct
  {
    const char* str;
    item::Status expected;
  } cases[] = {{"NOT_OWNDED", item::Status::NOT_OWNDED},
               {"OWNED", item::Status::OWNED},
               {"EQUIPPED", item::Status::EQUIPPED}};

  for (const auto& [str, expected] : cases)
  {
    validJson_["status"] = str;
    auto item = ItemBuilder::fromJson(validJson_);
    ASSERT_TRUE(item.has_value()) << "Failed for status: " << str;
    EXPECT_EQ(item->getStatus(), expected);
  }
}

// =============================================================================
//  fromJson(const json&) — failure paths
// =============================================================================
TEST_F(ItemBuilderFromJsonTest, FromJsonObject_MissingName)
{
  validJson_.erase("name");
  EXPECT_FALSE(ItemBuilder::fromJson(validJson_).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_MissingDescription)
{
  validJson_.erase("description");
  EXPECT_FALSE(ItemBuilder::fromJson(validJson_).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_MissingWeight)
{
  validJson_.erase("weight");
  EXPECT_FALSE(ItemBuilder::fromJson(validJson_).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_MissingRarity)
{
  validJson_.erase("rarity");
  EXPECT_FALSE(ItemBuilder::fromJson(validJson_).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_MissingType)
{
  validJson_.erase("type");
  EXPECT_FALSE(ItemBuilder::fromJson(validJson_).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_MissingStatus)
{
  validJson_.erase("status");
  EXPECT_FALSE(ItemBuilder::fromJson(validJson_).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_WrongTypeForName)
{
  validJson_["name"] = 42;
  EXPECT_FALSE(ItemBuilder::fromJson(validJson_).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_WrongTypeForWeight)
{
  validJson_["weight"] = "not a number";
  EXPECT_FALSE(ItemBuilder::fromJson(validJson_).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_InvalidRarityString)
{
  validJson_["rarity"] = "Mythical";
  EXPECT_FALSE(ItemBuilder::fromJson(validJson_).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_InvalidTypeString)
{
  validJson_["type"] = "Spaceship";
  EXPECT_FALSE(ItemBuilder::fromJson(validJson_).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_InvalidStatusString)
{
  validJson_["status"] = "SOLD";
  EXPECT_FALSE(ItemBuilder::fromJson(validJson_).has_value());
}

// =============================================================================
//  fromJson(const std::string&) — file path overload
// =============================================================================
TEST_F(ItemBuilderFromJsonTest, FromJsonFile_Success)
{
  auto item = ItemBuilder::fromJson(dataPath("valid_item.json"));
  ASSERT_TRUE(item.has_value());

  EXPECT_EQ(item->getName(), "Iron Sword");
  EXPECT_EQ(item->getDescription(), "A sturdy iron sword.");
  EXPECT_FLOAT_EQ(item->getWeight(), 3.5f);
  EXPECT_EQ(item->getRarity(), item::Rarity::Common);
  EXPECT_EQ(item->getType(), item::Type::OneHandedPrimary);
  EXPECT_EQ(item->getStatus(), item::Status::OWNED);
}

TEST_F(ItemBuilderFromJsonTest, FromJsonFile_FileNotFound)
{
  EXPECT_FALSE(
    ItemBuilder::fromJson(dataPath("does_not_exist.json")).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonFile_InvalidJson)
{
  EXPECT_FALSE(
    ItemBuilder::fromJson(dataPath("invalid_syntax.json")).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonFile_MissingRequiredField)
{
  EXPECT_FALSE(
    ItemBuilder::fromJson(dataPath("missing_name.json")).has_value());
}

// =============================================================================
//  Round-trip: build → toJson → fromJson → equivalence
// =============================================================================
TEST_F(ItemBuilderFromJsonTest, RoundTrip_BuilderToJsonToItem)
{
  // Build an item with the fluent API
  auto original = ItemBuilder()
                    .withName("Excalibur")
                    .withDescription("The legendary sword.")
                    .withWeight(15.0f)
                    .withRarity(item::Rarity::Legendary)
                    .withType(item::Type::TwoHandedPrimary)
                    .withStatus(item::Status::EQUIPPED)
                    .build();

  const auto j = original.toJson();

  auto restored = ItemBuilder::fromJson(j);
  ASSERT_TRUE(restored.has_value());

  // Core fields match.
  EXPECT_EQ(restored->getName(), original.getName());
  EXPECT_EQ(restored->getDescription(), original.getDescription());
  EXPECT_FLOAT_EQ(restored->getWeight(), original.getWeight());
  EXPECT_EQ(restored->getRarity(), original.getRarity());
  EXPECT_EQ(restored->getType(), original.getType());
  EXPECT_EQ(restored->getStatus(), original.getStatus());

  // UUID differs (new one generated).
  EXPECT_NE(restored->getUUID().toString(), original.getUUID().toString());

  // Owner is not preserved.
  EXPECT_FALSE(restored->getOwner().has_value());

  // Cores are equivalent (isEquivalent ignores UUID and owner).
  EXPECT_TRUE(original.isEquivalent(*restored));
}

} // namespace

#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>

#include "base_item.h"
#include "item_builder.h"
#include "item_data_types/item_rarity.h"
#include "item_data_types/item_status.h"
#include "item_data_types/item_type.h"
#include "item_serialization.h"
#include "nlohmann/json_fwd.hpp"

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
constexpr float kLegendarySwordWeight = 15.0F;
constexpr int kWrongTypeValue = 42;
constexpr std::size_t kRarityCaseCount = 5;
constexpr std::size_t kTypeCaseCount = 11;
constexpr std::size_t kStatusCaseCount = 3;

const std::string kTestUuid{"123e4567-e89b-12d3-a456-426614174000"};
const std::string kAlternateUuid{"11111111-1111-1111-1111-111111111111"};

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

// =============================================================================
//  Fixture — a valid JSON object used across tests
// =============================================================================
class ItemBuilderFromJsonTest : public ::testing::Test
{
  public:
  auto getValidJson() -> json& { return validJson_; }

  protected:
  void SetUp() override
  {
    validJson_["uuid"] = kTestUuid;
    validJson_["name"] = "Iron Sword";
    validJson_["description"] = "A sturdy iron sword.";
    validJson_["weight"] = kIronSwordWeight;
    validJson_["rarity"] = "Common";
    validJson_["type"] = "OneHandedPrimary";
    validJson_["status"] = "OWNED";
  }

  private:
  json validJson_;
};

// =============================================================================
//  fromJson(const json&) — success paths
// =============================================================================
TEST_F(ItemBuilderFromJsonTest, FromJsonObject_Success)
{
  const auto item = ItemBuilder::fromJson(item::signItem(getValidJson()));
  ASSERT_TRUE(item.has_value());

  EXPECT_EQ(item.value().getName(), "Iron Sword");
  EXPECT_EQ(item.value().getDescription(), "A sturdy iron sword.");
  EXPECT_FLOAT_EQ(item.value().getWeight(), kIronSwordWeight);
  EXPECT_EQ(item.value().getRarity(), item::Rarity::Common);
  EXPECT_EQ(item.value().getType(), item::Type::OneHandedPrimary);
  EXPECT_EQ(item.value().getStatus(), item::Status::OWNED);

  // UUID is restored from the JSON.
  EXPECT_EQ(item.value().getUUID().toString(), kTestUuid);

  // No owner was serialized, so the deserialized owner stays empty.
  EXPECT_FALSE(item.value().getOwner().has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_PreservesUUID)
{
  auto firstItem = ItemBuilder::fromJson(item::signItem(getValidJson()));
  auto secondItem = ItemBuilder::fromJson(item::signItem(getValidJson()));
  ASSERT_TRUE(firstItem.has_value());
  ASSERT_TRUE(secondItem.has_value());

  // Two items deserialized from the same JSON keep the same UUID.
  EXPECT_EQ(firstItem.value().getUUID().toString(),
            secondItem.value().getUUID().toString());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_UsesUUID)
{
  auto& testJson = getValidJson();
  testJson["uuid"] = kAlternateUuid;
  auto item = ItemBuilder::fromJson(item::signItem(testJson));
  ASSERT_TRUE(item.has_value());

  EXPECT_EQ(item.value().getUUID().toString(), kAlternateUuid);
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_PreservesOwner)
{
  auto& testJson = getValidJson();
  testJson["owner"] = kAlternateUuid;
  auto item = ItemBuilder::fromJson(item::signItem(testJson));
  ASSERT_TRUE(item.has_value());

  ASSERT_TRUE(item.value().getOwner().has_value());
  EXPECT_EQ(item.value().getOwner().value().toString(), kAlternateUuid);
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_RejectsInvalidOwner)
{
  auto& testJson = getValidJson();
  testJson["owner"] = "not-a-uuid";
  EXPECT_FALSE(ItemBuilder::fromJson(item::signItem(testJson)).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_IgnoresExtraFields)
{
  auto& testJson = getValidJson();
  testJson["extra"] = "unexpected";
  testJson["another"] = kWrongTypeValue;
  auto item = ItemBuilder::fromJson(item::signItem(testJson));
  ASSERT_TRUE(item.has_value());

  EXPECT_EQ(item.value().getName(), "Iron Sword");
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_AllRarityValues)
{
  using TestPair = std::pair<std::string, item::Rarity>;
  const std::array<TestPair, kRarityCaseCount> cases = {
    TestPair("Uncommon", item::Rarity::Uncommon),
    TestPair("Rare", item::Rarity::Rare),
    TestPair("Epic", item::Rarity::Epic),
    TestPair("Legendary", item::Rarity::Legendary),
    TestPair("Artifact", item::Rarity::Artifact),
  };

  auto& testJson = getValidJson();
  for (const auto& [str, expected] : cases)
  {
    testJson["rarity"] = str;
    auto item = ItemBuilder::fromJson(item::signItem(testJson));
    ASSERT_TRUE(item.has_value()) << "Failed for rarity: " << str;
    EXPECT_EQ(item.value().getRarity(), expected);
  }
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_AllTypeValues)
{
  using TestPair = std::pair<std::string, item::Type>;
  const std::array<TestPair, kTypeCaseCount> cases = {
    TestPair("OneHandedPrimary", item::Type::OneHandedPrimary),
    TestPair("OneHandedSecondary", item::Type::OneHandedSecondary),
    TestPair("TwoHandedPrimary", item::Type::TwoHandedPrimary),
    TestPair("HeadGear", item::Type::HeadGear),
    TestPair("ChestGear", item::Type::ChestGear),
    TestPair("ArmGear", item::Type::ArmGear),
    TestPair("LegGear", item::Type::LegGear),
    TestPair("Amulet", item::Type::Amulet),
    TestPair("Ring", item::Type::Ring),
    TestPair("Consumable", item::Type::Consumable),
    TestPair("Currency", item::Type::Currency),
  };

  auto& testJson = getValidJson();
  for (const auto& [str, expected] : cases)
  {
    testJson["type"] = str;
    auto item = ItemBuilder::fromJson(item::signItem(testJson));
    ASSERT_TRUE(item.has_value()) << "Failed for type: " << str;
    EXPECT_EQ(item.value().getType(), expected);
  }
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_AllStatusValues)
{
  using TestPair = std::pair<std::string, item::Status>;
  const std::array<TestPair, kStatusCaseCount> cases = {
    TestPair("NOT_OWNDED", item::Status::NOT_OWNDED),
    TestPair("OWNED", item::Status::OWNED),
    TestPair("EQUIPPED", item::Status::EQUIPPED),
  };

  auto& testJson = getValidJson();
  for (const auto& [str, expected] : cases)
  {
    testJson["status"] = str;
    auto item = ItemBuilder::fromJson(item::signItem(testJson));
    ASSERT_TRUE(item.has_value()) << "Failed for status: " << str;
    EXPECT_EQ(item.value().getStatus(), expected);
  }
}

// =============================================================================
//  fromJson(const json&) — failure paths
// =============================================================================
TEST_F(ItemBuilderFromJsonTest, FromJsonObject_MissingName)
{
  auto& testJson = getValidJson();
  testJson.erase("name");
  EXPECT_FALSE(ItemBuilder::fromJson(item::signItem(testJson)).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_MissingDescription)
{
  auto& testJson = getValidJson();
  testJson.erase("description");
  EXPECT_FALSE(ItemBuilder::fromJson(item::signItem(testJson)).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_MissingWeight)
{
  auto& testJson = getValidJson();
  testJson.erase("weight");
  EXPECT_FALSE(ItemBuilder::fromJson(item::signItem(testJson)).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_MissingRarity)
{
  auto& testJson = getValidJson();
  testJson.erase("rarity");
  EXPECT_FALSE(ItemBuilder::fromJson(item::signItem(testJson)).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_MissingType)
{
  auto& testJson = getValidJson();
  testJson.erase("type");
  EXPECT_FALSE(ItemBuilder::fromJson(item::signItem(testJson)).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_MissingStatus)
{
  auto& testJson = getValidJson();
  testJson.erase("status");
  EXPECT_FALSE(ItemBuilder::fromJson(item::signItem(testJson)).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_WrongTypeForName)
{
  auto& testJson = getValidJson();
  testJson["name"] = kWrongTypeValue;
  EXPECT_FALSE(ItemBuilder::fromJson(item::signItem(testJson)).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_WrongTypeForWeight)
{
  auto& testJson = getValidJson();
  testJson["weight"] = "not a number";
  EXPECT_FALSE(ItemBuilder::fromJson(item::signItem(testJson)).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_InvalidRarityString)
{
  auto& testJson = getValidJson();
  testJson["rarity"] = "Mythical";
  EXPECT_FALSE(ItemBuilder::fromJson(item::signItem(testJson)).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_InvalidTypeString)
{
  auto& testJson = getValidJson();
  testJson["type"] = "Spaceship";
  EXPECT_FALSE(ItemBuilder::fromJson(item::signItem(testJson)).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_InvalidStatusString)
{
  auto& testJson = getValidJson();
  testJson["status"] = "SOLD";
  EXPECT_FALSE(ItemBuilder::fromJson(item::signItem(testJson)).has_value());
}

// =============================================================================
//  fromJson(const std::string&) — file path overload
// =============================================================================
TEST_F(ItemBuilderFromJsonTest, FromJsonFile_Success)
{
  auto item =
    ItemBuilder::fromJson(std::filesystem::path{dataPath("valid_item.json")});
  ASSERT_TRUE(item.has_value());

  EXPECT_EQ(item.value().getName(), "Iron Sword");
  EXPECT_EQ(item.value().getDescription(), "A sturdy iron sword.");
  EXPECT_FLOAT_EQ(item.value().getWeight(), kIronSwordWeight);
  EXPECT_EQ(item.value().getRarity(), item::Rarity::Common);
  EXPECT_EQ(item.value().getType(), item::Type::OneHandedPrimary);
  EXPECT_EQ(item.value().getStatus(), item::Status::OWNED);
  EXPECT_EQ(item.value().getUUID().toString(), kTestUuid);
}

TEST_F(ItemBuilderFromJsonTest, FromJsonFile_FileNotFound)
{
  EXPECT_FALSE(ItemBuilder::fromJson(
                 std::filesystem::path{dataPath("does_not_exist.json")})
                 .has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonFile_InvalidJson)
{
  EXPECT_FALSE(ItemBuilder::fromJson(
                 std::filesystem::path{dataPath("invalid_syntax.json")})
                 .has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonFile_MissingRequiredField)
{
  EXPECT_FALSE(
    ItemBuilder::fromJson(std::filesystem::path{dataPath("missing_name.json")})
      .has_value());
}

// =============================================================================
//  Round-trip: build → toJson → fromJson → equivalence
// =============================================================================
TEST_F(ItemBuilderFromJsonTest, RoundTrip_BuilderToJsonToItem)
{
  UUID owner;
  const auto ownerStr = owner.toString();

  // Build an item with the fluent API
  auto original = ItemBuilder()
                    .withName("Excalibur")
                    .withDescription("The legendary sword.")
                    .withWeight(kLegendarySwordWeight)
                    .withRarity(item::Rarity::Legendary)
                    .withType(item::Type::TwoHandedPrimary)
                    .withStatus(item::Status::EQUIPPED)
                    .withOwner(std::move(owner))
                    .build();

  const auto result = original.toJson();

  auto restored = ItemBuilder::fromJson(result);
  ASSERT_TRUE(restored.has_value());

  // Core fields match.
  EXPECT_EQ(restored.value().getName(), original.getName());
  EXPECT_EQ(restored.value().getDescription(), original.getDescription());
  EXPECT_FLOAT_EQ(restored.value().getWeight(), original.getWeight());
  EXPECT_EQ(restored.value().getRarity(), original.getRarity());
  EXPECT_EQ(restored.value().getType(), original.getType());
  EXPECT_EQ(restored.value().getStatus(), original.getStatus());

  // UUID is preserved across the round trip.
  EXPECT_EQ(restored.value().getUUID().toString(),
            original.getUUID().toString());

  // Owner is preserved across the round trip.
  ASSERT_TRUE(restored.value().getOwner().has_value());
  EXPECT_EQ(restored.value().getOwner().value().toString(), ownerStr);

  // Cores are equivalent (isEquivalent ignores UUID and owner).
  EXPECT_TRUE(original.isEquivalent(restored.value()));
}

// =============================================================================
//  fromJson — integrity verification
// =============================================================================
TEST_F(ItemBuilderFromJsonTest, FromJsonObject_RejectsMissingHash)
{
  auto envelope = item::signItem(getValidJson());
  envelope.erase("hash");
  EXPECT_FALSE(ItemBuilder::fromJson(envelope).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_RejectsTamperedPayload)
{
  auto envelope = item::signItem(getValidJson());
  envelope["payload"]["name"] = "Hacked";
  EXPECT_FALSE(ItemBuilder::fromJson(envelope).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_RejectsMissingUuid)
{
  auto& testJson = getValidJson();
  testJson.erase("uuid");
  EXPECT_FALSE(ItemBuilder::fromJson(item::signItem(testJson)).has_value());
}

TEST_F(ItemBuilderFromJsonTest, FromJsonObject_RejectsInvalidUuid)
{
  auto& testJson = getValidJson();
  testJson["uuid"] = "not-a-uuid";
  EXPECT_FALSE(ItemBuilder::fromJson(item::signItem(testJson)).has_value());
}

TEST_F(ItemBuilderFromJsonTest, SignItemAndVerifyItemRoundTrip)
{
  const json payload = getValidJson();
  const auto envelope = item::signItem(payload);
  const auto verified = item::verifyItem(envelope);
  ASSERT_TRUE(verified.has_value());
  EXPECT_EQ(*verified, payload);
}

TEST_F(ItemBuilderFromJsonTest, VerifyItemRejectsNonObject)
{
  EXPECT_FALSE(item::verifyItem(json::array()).has_value());
}

// NOLINTEND(modernize-use-trailing-return-type,
//           readability-function-cognitive-complexity)

} // namespace

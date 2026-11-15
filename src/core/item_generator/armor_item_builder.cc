#include "armor_item_builder.h"
#include "item_damage_type.h"
#include "item_rarity.h"
#include "item_status.h"
#include "item_type.h"
#include "nlohmann/json_fwd.hpp"
#include "uuid.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <utility>

ArmorItemBuilder::ArmorItemBuilder() = default;

auto ArmorItemBuilder::withName(const std::string& name) -> ArmorItemBuilder&
{
  item_.setName(name);
  return *this;
}

auto ArmorItemBuilder::withDescription(const std::string& description)
  -> ArmorItemBuilder&
{
  item_.setDescription(description);
  return *this;
}

auto ArmorItemBuilder::withWeight(float weight) -> ArmorItemBuilder&
{
  item_.setWeight(weight);
  return *this;
}

auto ArmorItemBuilder::withRarity(item::Rarity rarity) -> ArmorItemBuilder&
{
  item_.setRarity(rarity);
  return *this;
}

auto ArmorItemBuilder::withType(item::Type type) -> ArmorItemBuilder&
{
  item_.setType(type);
  return *this;
}

auto ArmorItemBuilder::withStatus(item::Status status) -> ArmorItemBuilder&
{
  item_.setStatus(status);
  return *this;
}

auto ArmorItemBuilder::withOwner(std::optional<UUID> owner) -> ArmorItemBuilder&
{
  item_.setOwner(std::move(owner));
  return *this;
}

auto ArmorItemBuilder::withOwner(UUID&& owner) -> ArmorItemBuilder&
{
  item_.setOwner(std::optional<UUID>(std::move(owner)));
  return *this;
}

auto ArmorItemBuilder::withResistance(item::DamageType type, float amount)
  -> ArmorItemBuilder&
{
  item_.setDamageResistance(type, amount);
  return *this;
}

auto ArmorItemBuilder::build() -> ArmorItem { return std::move(item_); }

auto ArmorItemBuilder::fromJson(const nlohmann::json& data)
  -> std::optional<ArmorItem>
{
  // Validate required fields
  if (!data.contains("name") || !data["name"].is_string())
  {
    return std::nullopt;
  }
  if (!data.contains("description") || !data["description"].is_string())
  {
    return std::nullopt;
  }
  if (!data.contains("weight") || !data["weight"].is_number())
  {
    return std::nullopt;
  }
  if (!data.contains("rarity") || !data["rarity"].is_string())
  {
    return std::nullopt;
  }
  if (!data.contains("type") || !data["type"].is_string())
  {
    return std::nullopt;
  }
  if (!data.contains("status") || !data["status"].is_string())
  {
    return std::nullopt;
  }
  if (!data.contains("damage_resistance") ||
      !data["damage_resistance"].is_object())
  {
    return std::nullopt;
  }

  auto rarity = item::rarityFromString(data["rarity"]);
  if (!rarity.has_value())
  {
    return std::nullopt;
  }

  auto type = item::typeFromString(data["type"]);
  if (!type.has_value())
  {
    return std::nullopt;
  }

  auto status = item::statusFromString(data["status"]);
  if (!status.has_value())
  {
    return std::nullopt;
  }

  ArmorItem item;
  item.setName(data["name"]);
  item.setDescription(data["description"]);
  item.setWeight(static_cast<float>(data["weight"]));
  item.setRarity(*rarity);
  item.setType(*type);
  item.setStatus(*status);

  // Parse damage_resistance object
  for (const auto& [key, value] : data["damage_resistance"].items())
  {
    if (!value.is_number())
    {
      continue;
    }
    auto damageType = item::damageTypeFromString(key);
    if (damageType.has_value())
    {
      item.setDamageResistance(*damageType, static_cast<float>(value));
    }
  }

  return item;
}

auto ArmorItemBuilder::fromJson(const std::filesystem::path& path)
  -> std::optional<ArmorItem>
{
  std::ifstream const input(path);
  if (!input.is_open())
  {
    return std::nullopt;
  }

  std::ostringstream oss;
  oss << input.rdbuf();

  try
  {
    return fromJson(nlohmann::json::parse(oss.str()));
  }
  catch (const nlohmann::json::parse_error&)
  {
    return std::nullopt;
  }
}

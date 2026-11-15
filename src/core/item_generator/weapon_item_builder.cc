#include "weapon_item_builder.h"
#include "item_damage_type.h"
#include "item_rarity.h"
#include "item_serialization.h"
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

WeaponItemBuilder::WeaponItemBuilder() = default;

auto WeaponItemBuilder::withName(const std::string& name) -> WeaponItemBuilder&
{
  item_.setName(name);
  return *this;
}

auto WeaponItemBuilder::withDescription(const std::string& description)
  -> WeaponItemBuilder&
{
  item_.setDescription(description);
  return *this;
}

auto WeaponItemBuilder::withWeight(float weight) -> WeaponItemBuilder&
{
  item_.setWeight(weight);
  return *this;
}

auto WeaponItemBuilder::withRarity(item::Rarity rarity) -> WeaponItemBuilder&
{
  item_.setRarity(rarity);
  return *this;
}

auto WeaponItemBuilder::withType(item::Type type) -> WeaponItemBuilder&
{
  item_.setType(type);
  return *this;
}

auto WeaponItemBuilder::withStatus(item::Status status) -> WeaponItemBuilder&
{
  item_.setStatus(status);
  return *this;
}

auto WeaponItemBuilder::withOwner(std::optional<UUID> owner)
  -> WeaponItemBuilder&
{
  item_.setOwner(std::move(owner));
  return *this;
}

auto WeaponItemBuilder::withOwner(UUID&& owner) -> WeaponItemBuilder&
{
  item_.setOwner(std::optional<UUID>(std::move(owner)));
  return *this;
}

auto WeaponItemBuilder::withDamage(item::DamageType type, int amount)
  -> WeaponItemBuilder&
{
  item_.setDamage(type, amount);
  return *this;
}

auto WeaponItemBuilder::withRangeInMeters(float range) -> WeaponItemBuilder&
{
  item_.setRangeInMeters(range);
  return *this;
}

auto WeaponItemBuilder::build() -> WeaponItem { return std::move(item_); }

auto WeaponItemBuilder::fromJson(const nlohmann::json& data)
  -> std::optional<WeaponItem>
{
  const auto payload = item::verifyItem(data);
  if (!payload.has_value())
  {
    return std::nullopt;
  }
  const nlohmann::json& jsonData = *payload;

  auto uuid = item::parseUuid(jsonData);
  if (!uuid.has_value())
  {
    return std::nullopt;
  }
  if (!item::hasValidCommonFields(jsonData))
  {
    return std::nullopt;
  }

  if (!jsonData.contains("damage") || !jsonData["damage"].is_object())
  {
    return std::nullopt;
  }
  if (!jsonData.contains("rangeInMeters") ||
      !jsonData["rangeInMeters"].is_number())
  {
    return std::nullopt;
  }

  auto rarity = item::rarityFromString(jsonData["rarity"]);
  if (!rarity.has_value())
  {
    return std::nullopt;
  }

  auto type = item::typeFromString(jsonData["type"]);
  if (!type.has_value())
  {
    return std::nullopt;
  }

  auto status = item::statusFromString(jsonData["status"]);
  if (!status.has_value())
  {
    return std::nullopt;
  }

  std::optional<UUID> owner;
  if (jsonData.contains("owner"))
  {
    if (!jsonData["owner"].is_string())
    {
      return std::nullopt;
    }
    auto parsedOwner = UUID::fromString(jsonData["owner"].get<std::string>());
    if (!parsedOwner.has_value())
    {
      return std::nullopt;
    }
    owner = std::move(*parsedOwner);
  }

  WeaponItem item;
  item.setUUID(std::move(*uuid));
  item.setName(jsonData["name"]);
  item.setDescription(jsonData["description"]);
  item.setWeight(static_cast<float>(jsonData["weight"]));
  item.setRarity(*rarity);
  item.setType(*type);
  item.setStatus(*status);
  item.setRangeInMeters(static_cast<float>(jsonData["rangeInMeters"]));
  item.setOwner(std::move(owner));

  // Parse damage object
  for (const auto& [key, value] : jsonData["damage"].items())
  {
    if (!value.is_number_integer())
    {
      continue;
    }
    auto damageType = item::damageTypeFromString(key);
    if (damageType.has_value())
    {
      item.setDamage(*damageType, static_cast<int>(value));
    }
  }

  return item;
}

auto WeaponItemBuilder::fromJson(const std::filesystem::path& path)
  -> std::optional<WeaponItem>
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

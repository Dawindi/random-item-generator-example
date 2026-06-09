#include "weapon_item_builder.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <optional>
#include <sstream>

WeaponItemBuilder::WeaponItemBuilder() : item_() {}

WeaponItemBuilder& WeaponItemBuilder::withName(const std::string& name)
{
  item_.setName(name);
  return *this;
}

WeaponItemBuilder& WeaponItemBuilder::withDescription(
  const std::string& description)
{
  item_.setDescription(description);
  return *this;
}

WeaponItemBuilder& WeaponItemBuilder::withWeight(float weight)
{
  item_.setWeight(weight);
  return *this;
}

WeaponItemBuilder& WeaponItemBuilder::withRarity(item::Rarity rarity)
{
  item_.setRarity(rarity);
  return *this;
}

WeaponItemBuilder& WeaponItemBuilder::withType(item::Type type)
{
  item_.setType(type);
  return *this;
}

WeaponItemBuilder& WeaponItemBuilder::withStatus(item::Status status)
{
  item_.setStatus(status);
  return *this;
}

WeaponItemBuilder& WeaponItemBuilder::withOwner(std::optional<UUID> owner)
{
  item_.setOwner(std::move(owner));
  return *this;
}

WeaponItemBuilder& WeaponItemBuilder::withOwner(UUID&& owner)
{
  item_.setOwner(std::optional<UUID>(std::move(owner)));
  return *this;
}

WeaponItemBuilder& WeaponItemBuilder::withDamage(item::DamageType type,
                                                 int amount)
{
  item_.setDamage(type, amount);
  return *this;
}

WeaponItem WeaponItemBuilder::build() { return std::move(item_); }

std::optional<WeaponItem> WeaponItemBuilder::fromJson(const nlohmann::json& j)
{
  // Validate required fields
  if (!j.contains("name") || !j["name"].is_string())
    return std::nullopt;
  if (!j.contains("description") || !j["description"].is_string())
    return std::nullopt;
  if (!j.contains("weight") || !j["weight"].is_number())
    return std::nullopt;
  if (!j.contains("rarity") || !j["rarity"].is_string())
    return std::nullopt;
  if (!j.contains("type") || !j["type"].is_string())
    return std::nullopt;
  if (!j.contains("status") || !j["status"].is_string())
    return std::nullopt;

  auto rarity = item::rarityFromString(j["rarity"]);
  if (!rarity.has_value())
    return std::nullopt;

  auto type = item::typeFromString(j["type"]);
  if (!type.has_value())
    return std::nullopt;

  auto status = item::statusFromString(j["status"]);
  if (!status.has_value())
    return std::nullopt;

  WeaponItem item;
  item.setName(j["name"]);
  item.setDescription(j["description"]);
  item.setWeight(static_cast<float>(j["weight"]));
  item.setRarity(*rarity);
  item.setType(*type);
  item.setStatus(*status);

  // Parse optional damage object
  if (j.contains("damage") && j["damage"].is_object())
  {
    for (const auto& [key, value] : j["damage"].items())
    {
      if (!value.is_number_integer())
        continue;
      auto dt = item::damageTypeFromString(key);
      if (dt.has_value())
        item.setDamage(*dt, static_cast<int>(value));
    }
  }

  return item;
}

std::optional<WeaponItem> WeaponItemBuilder::fromJson(
  const std::string& path)
{
  std::ifstream in(path);
  if (!in.is_open())
    return std::nullopt;

  std::ostringstream oss;
  oss << in.rdbuf();

  try
  {
    return fromJson(nlohmann::json::parse(oss.str()));
  }
  catch (const nlohmann::json::parse_error&)
  {
    return std::nullopt;
  }
}

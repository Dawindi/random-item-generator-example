#include "armor_item_builder.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <optional>
#include <sstream>

ArmorItemBuilder::ArmorItemBuilder() : item_() {}

ArmorItemBuilder& ArmorItemBuilder::withName(const std::string& name)
{
  item_.setName(name);
  return *this;
}

ArmorItemBuilder&
ArmorItemBuilder::withDescription(const std::string& description)
{
  item_.setDescription(description);
  return *this;
}

ArmorItemBuilder& ArmorItemBuilder::withWeight(float weight)
{
  item_.setWeight(weight);
  return *this;
}

ArmorItemBuilder& ArmorItemBuilder::withRarity(item::Rarity rarity)
{
  item_.setRarity(rarity);
  return *this;
}

ArmorItemBuilder& ArmorItemBuilder::withType(item::Type type)
{
  item_.setType(type);
  return *this;
}

ArmorItemBuilder& ArmorItemBuilder::withStatus(item::Status status)
{
  item_.setStatus(status);
  return *this;
}

ArmorItemBuilder& ArmorItemBuilder::withOwner(std::optional<UUID> owner)
{
  item_.setOwner(std::move(owner));
  return *this;
}

ArmorItemBuilder& ArmorItemBuilder::withOwner(UUID&& owner)
{
  item_.setOwner(std::optional<UUID>(std::move(owner)));
  return *this;
}

ArmorItemBuilder& ArmorItemBuilder::withResistance(item::DamageType type,
                                                   float amount)
{
  item_.setDamageResistance(type, amount);
  return *this;
}

ArmorItem ArmorItemBuilder::build() { return std::move(item_); }

std::optional<ArmorItem> ArmorItemBuilder::fromJson(const nlohmann::json& j)
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
  if (!j.contains("damage_resistance") || !j["damage_resistance"].is_object())
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

  ArmorItem item;
  item.setName(j["name"]);
  item.setDescription(j["description"]);
  item.setWeight(static_cast<float>(j["weight"]));
  item.setRarity(*rarity);
  item.setType(*type);
  item.setStatus(*status);

  // Parse damage_resistance object
  for (const auto& [key, value] : j["damage_resistance"].items())
  {
    if (!value.is_number())
      continue;
    auto dt = item::damageTypeFromString(key);
    if (dt.has_value())
      item.setDamageResistance(*dt, static_cast<float>(value));
  }

  return item;
}

std::optional<ArmorItem>
ArmorItemBuilder::fromJson(const std::filesystem::path& path)
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

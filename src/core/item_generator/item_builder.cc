#include "item_builder.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <optional>
#include <sstream>

ItemBuilder::ItemBuilder() : item_() {}

ItemBuilder& ItemBuilder::withName(const std::string& name)
{
  item_.setName(name);
  return *this;
}

ItemBuilder& ItemBuilder::withDescription(const std::string& description)
{
  item_.setDescription(description);
  return *this;
}

ItemBuilder& ItemBuilder::withWeight(float weight)
{
  item_.setWeight(weight);
  return *this;
}

ItemBuilder& ItemBuilder::withRarity(item::Rarity rarity)
{
  item_.setRarity(rarity);
  return *this;
}

ItemBuilder& ItemBuilder::withType(item::Type type)
{
  item_.setType(type);
  return *this;
}

ItemBuilder& ItemBuilder::withStatus(item::Status status)
{
  item_.setStatus(status);
  return *this;
}

ItemBuilder& ItemBuilder::withOwner(std::optional<UUID> owner)
{
  item_.setOwner(std::move(owner));
  return *this;
}

ItemBuilder& ItemBuilder::withOwner(UUID&& owner)
{
  item_.setOwner(std::optional<UUID>(std::move(owner)));
  return *this;
}

BaseItem ItemBuilder::build() { return std::move(item_); }

std::optional<BaseItem> ItemBuilder::fromJson(const nlohmann::json& j)
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

  BaseItem item;
  item.setName(j["name"]);
  item.setDescription(j["description"]);
  item.setWeight(static_cast<float>(j["weight"]));
  item.setRarity(*rarity);
  item.setType(*type);
  item.setStatus(*status);

  return item;
}

std::optional<BaseItem> ItemBuilder::fromJson(const std::filesystem::path& path)
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

BaseItem ItemBuilder::duplicateItem(const BaseItem& original,
                                    std::optional<UUID> owner)
{
  BaseItem item;
  item.setName(original.getName());
  item.setDescription(original.getDescription());
  item.setWeight(original.getWeight());
  item.setRarity(original.getRarity());
  item.setType(original.getType());
  item.setStatus(original.getStatus());
  item.setOwner(std::move(owner));
  return item;
}

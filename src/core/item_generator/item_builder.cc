#include "item_builder.h"
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

ItemBuilder::ItemBuilder() = default;

auto ItemBuilder::withName(const std::string& name) -> ItemBuilder&
{
  item_.setName(name);
  return *this;
}

auto ItemBuilder::withDescription(const std::string& description)
  -> ItemBuilder&
{
  item_.setDescription(description);
  return *this;
}

auto ItemBuilder::withWeight(float weight) -> ItemBuilder&
{
  item_.setWeight(weight);
  return *this;
}

auto ItemBuilder::withRarity(item::Rarity rarity) -> ItemBuilder&
{
  item_.setRarity(rarity);
  return *this;
}

auto ItemBuilder::withType(item::Type type) -> ItemBuilder&
{
  item_.setType(type);
  return *this;
}

auto ItemBuilder::withStatus(item::Status status) -> ItemBuilder&
{
  item_.setStatus(status);
  return *this;
}

auto ItemBuilder::withOwner(std::optional<UUID> owner) -> ItemBuilder&
{
  item_.setOwner(std::move(owner));
  return *this;
}

auto ItemBuilder::withOwner(UUID&& owner) -> ItemBuilder&
{
  item_.setOwner(std::optional<UUID>(std::move(owner)));
  return *this;
}

auto ItemBuilder::build() -> BaseItem { return std::move(item_); }

auto ItemBuilder::fromJson(const nlohmann::json& data)
  -> std::optional<BaseItem>
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

  BaseItem item;
  item.setName(data["name"]);
  item.setDescription(data["description"]);
  item.setWeight(static_cast<float>(data["weight"]));
  item.setRarity(*rarity);
  item.setType(*type);
  item.setStatus(*status);

  return item;
}

auto ItemBuilder::fromJson(const std::filesystem::path& path)
  -> std::optional<BaseItem>
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

auto ItemBuilder::duplicateItem(const BaseItem& original,
                                std::optional<UUID> owner) -> BaseItem
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

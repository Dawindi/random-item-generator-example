#include "item_builder.h"
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

  BaseItem item;
  item.setUUID(std::move(*uuid));
  item.setName(jsonData["name"]);
  item.setDescription(jsonData["description"]);
  item.setWeight(static_cast<float>(jsonData["weight"]));
  item.setRarity(*rarity);
  item.setType(*type);
  item.setStatus(*status);
  item.setOwner(std::move(owner));

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

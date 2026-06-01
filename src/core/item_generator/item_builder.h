#pragma once

#include <optional>
#include <string>

#include "base_item.h"
#include "item_rarity.h"
#include "item_status.h"
#include "item_type.h"

/// Fluent builder for constructing BaseItem objects.
///
/// Usage:
///   auto sword = ItemBuilder()
///     .withName("Excalibur")
///     .withType(item::Type::OneHandedPrimary)
///     .withRarity(item::Rarity::Legendary)
///     .build();
class ItemBuilder
{
  public:
  ItemBuilder();
  ~ItemBuilder() = default;

  // Non-copyable (holds a non-copyable UUID).
  ItemBuilder(const ItemBuilder&) = delete;
  ItemBuilder& operator=(const ItemBuilder&) = delete;

  // Movable.
  ItemBuilder(ItemBuilder&&) noexcept = default;
  ItemBuilder& operator=(ItemBuilder&&) noexcept = default;

  /// Sets the item's name.
  ItemBuilder& withName(const std::string& name);

  /// Sets the item's description.
  ItemBuilder& withDescription(const std::string& description);

  /// Sets the item's weight.
  ItemBuilder& withWeight(float weight);

  /// Sets the item's rarity.
  ItemBuilder& withRarity(item::Rarity rarity);

  /// Sets the item's type.
  ItemBuilder& withType(item::Type type);

  /// Sets the item's status.
  ItemBuilder& withStatus(item::Status status);

  /// Sets the item's owner from an optional UUID.
  ItemBuilder& withOwner(std::optional<UUID> owner);

  /// Sets the item's owner from a UUID rvalue. The UUID is moved into the
  /// item.
  ItemBuilder& withOwner(UUID&& owner);

  /// Builds and returns the constructed BaseItem. After calling build(),
  /// the builder is left in a moved-from state and should not be reused.
  BaseItem build();

  /// Creates a BaseItem from a JSON object. Returns std::nullopt if required
  /// fields are missing or values are invalid. The item receives a new UUID;
  /// any "uuid" field in the JSON is ignored. The "owner" field is also
  /// ignored (UUIDs cannot be constructed from strings).
  static std::optional<BaseItem> fromJson(const nlohmann::json& j);

  /// Creates a BaseItem from a JSON file at the given path. Returns
  /// std::nullopt if the file cannot be read or the JSON is invalid.
  static std::optional<BaseItem> fromJson(const std::string& path);

  private:
  BaseItem item_;
};

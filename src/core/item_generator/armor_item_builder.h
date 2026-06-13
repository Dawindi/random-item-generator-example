#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include "armor_item.h"
#include "item_data_types/item_damage_type.h"
#include "item_data_types/item_rarity.h"
#include "item_data_types/item_status.h"
#include "item_data_types/item_type.h"

/// Fluent builder for constructing ArmorItem objects.
///
/// Usage:
///   auto helmet = ArmorItemBuilder()
///     .withName("Iron Helmet")
///     .withType(item::Type::HeadGear)
///     .withRarity(item::Rarity::Common)
///     .withResistance(item::DamageType::Physical, 15.0f)
///     .withResistance(item::DamageType::Fire, 5.0f)
///     .build();
class ArmorItemBuilder
{
  public:
  ArmorItemBuilder();
  ~ArmorItemBuilder() = default;

  // Non-copyable (holds a non-copyable UUID via ArmorItem).
  ArmorItemBuilder(const ArmorItemBuilder&) = delete;
  ArmorItemBuilder& operator=(const ArmorItemBuilder&) = delete;

  // Movable.
  ArmorItemBuilder(ArmorItemBuilder&&) noexcept = default;
  ArmorItemBuilder& operator=(ArmorItemBuilder&&) noexcept = default;

  /// Sets the item's name.
  ArmorItemBuilder& withName(const std::string& name);

  /// Sets the item's description.
  ArmorItemBuilder& withDescription(const std::string& description);

  /// Sets the item's weight.
  ArmorItemBuilder& withWeight(float weight);

  /// Sets the item's rarity.
  ArmorItemBuilder& withRarity(item::Rarity rarity);

  /// Sets the item's type.
  ArmorItemBuilder& withType(item::Type type);

  /// Sets the item's status.
  ArmorItemBuilder& withStatus(item::Status status);

  /// Sets the item's owner from an optional UUID.
  ArmorItemBuilder& withOwner(std::optional<UUID> owner);

  /// Sets the item's owner from a UUID rvalue. The UUID is moved into the
  /// item.
  ArmorItemBuilder& withOwner(UUID&& owner);

  /// Adds or updates a damage-resistance entry for the given damage type.
  /// Multiple calls with different types accumulate resistance entries.
  ArmorItemBuilder& withResistance(item::DamageType type, float amount);

  /// Builds and returns the constructed ArmorItem. After calling build(),
  /// the builder is left in a moved-from state and should not be reused.
  ArmorItem build();

  /// Creates an ArmorItem from a JSON object. Returns std::nullopt if
  /// required fields are missing or values are invalid. The item receives a
  /// new UUID; any "uuid" field in the JSON is ignored. The "owner" field is
  /// also ignored (UUIDs cannot be constructed from strings). An optional
  /// "damage_resistance" object maps damage-type strings to float amounts.
  static std::optional<ArmorItem> fromJson(const nlohmann::json& j);

  /// Creates an ArmorItem from a JSON file at the given path. Returns
  /// std::nullopt if the file cannot be read or the JSON is invalid.
  static std::optional<ArmorItem> fromJson(const std::filesystem::path& path);

  private:
  ArmorItem item_;
};

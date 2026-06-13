#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include "item_data_types/item_damage_type.h"
#include "item_data_types/item_rarity.h"
#include "item_data_types/item_status.h"
#include "item_data_types/item_type.h"
#include "weapon_item.h"

/// Fluent builder for constructing WeaponItem objects.
///
/// Usage:
///   auto sword = WeaponItemBuilder()
///     .withName("Flaming Sword")
///     .withType(item::Type::OneHandedPrimary)
///     .withRarity(item::Rarity::Rare)
///     .withDamage(item::DamageType::Physical, 5)
///     .withDamage(item::DamageType::Fire, 5)
///     .build();
class WeaponItemBuilder
{
  public:
  WeaponItemBuilder();
  ~WeaponItemBuilder() = default;

  // Non-copyable (holds a non-copyable UUID via WeaponItem).
  WeaponItemBuilder(const WeaponItemBuilder&) = delete;
  WeaponItemBuilder& operator=(const WeaponItemBuilder&) = delete;

  // Movable.
  WeaponItemBuilder(WeaponItemBuilder&&) noexcept = default;
  WeaponItemBuilder& operator=(WeaponItemBuilder&&) noexcept = default;

  /// Sets the item's name.
  WeaponItemBuilder& withName(const std::string& name);

  /// Sets the item's description.
  WeaponItemBuilder& withDescription(const std::string& description);

  /// Sets the item's weight.
  WeaponItemBuilder& withWeight(float weight);

  /// Sets the item's rarity.
  WeaponItemBuilder& withRarity(item::Rarity rarity);

  /// Sets the item's type.
  WeaponItemBuilder& withType(item::Type type);

  /// Sets the item's status.
  WeaponItemBuilder& withStatus(item::Status status);

  /// Sets the item's owner from an optional UUID.
  WeaponItemBuilder& withOwner(std::optional<UUID> owner);

  /// Sets the item's owner from a UUID rvalue. The UUID is moved into the
  /// item.
  WeaponItemBuilder& withOwner(UUID&& owner);

  /// Adds or updates a damage entry for the given damage type. Multiple
  /// calls with different types accumulate damage entries.
  WeaponItemBuilder& withDamage(item::DamageType type, int amount);

  /// Sets the weapon's range in meters.
  WeaponItemBuilder& withRangeInMeters(float range);

  /// Builds and returns the constructed WeaponItem. After calling build(),
  /// the builder is left in a moved-from state and should not be reused.
  WeaponItem build();

  /// Creates a WeaponItem from a JSON object. Returns std::nullopt if
  /// required fields are missing or values are invalid. The item receives a
  /// new UUID; any "uuid" field in the JSON is ignored. The "owner" field is
  /// also ignored (UUIDs cannot be constructed from strings). An optional
  /// "damage" object maps damage-type strings to integer amounts.
  static std::optional<WeaponItem> fromJson(const nlohmann::json& j);

  /// Creates a WeaponItem from a JSON file at the given path. Returns
  /// std::nullopt if the file cannot be read or the JSON is invalid.
  static std::optional<WeaponItem> fromJson(const std::filesystem::path& path);

  private:
  WeaponItem item_;
};

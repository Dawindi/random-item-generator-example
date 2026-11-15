#pragma once

#include <map>

#include "base_item.h"
#include "item_data_types/item_damage_type.h"

class WeaponItemBuilder;

class WeaponItem : public BaseItem
{
  friend class WeaponItemBuilder;

  public:
  ~WeaponItem() override = default;

  // Non-copyable (holds a non-copyable UUID via BaseItem).
  WeaponItem(const WeaponItem&) = delete;
  WeaponItem& operator=(const WeaponItem&) = delete;

  // Movable.
  WeaponItem(WeaponItem&&) noexcept = default;
  WeaponItem& operator=(WeaponItem&&) noexcept = default;

  const std::map<item::DamageType, int>& getDamage() const;

  float getRangeInMeters() const;

  bool isValid() const override;
  bool isEquivalent(const InterfaceItem& other) const override;
  bool isEqual(const InterfaceItem& other) const override;

  protected:
  WeaponItem();

  json toJsonPayload() const override;

  void setDamage(item::DamageType type, int amount);
  void setRangeInMeters(float range);

  private:
  std::map<item::DamageType, int> damage_;
  static constexpr float kDefaultRangeInMeters = 0.1F;
  float itemRangeInMeters_ = kDefaultRangeInMeters;
};

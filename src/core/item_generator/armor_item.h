#pragma once

#include <map>

#include "base_item.h"
#include "item_data_types/item_damage_type.h"

class ArmorItemBuilder;

class ArmorItem : public BaseItem
{
  friend class ArmorItemBuilder;

  public:
  ~ArmorItem() override = default;

  // Non-copyable (holds a non-copyable UUID via BaseItem).
  ArmorItem(const ArmorItem&) = delete;
  ArmorItem& operator=(const ArmorItem&) = delete;

  // Movable.
  ArmorItem(ArmorItem&&) noexcept = default;
  ArmorItem& operator=(ArmorItem&&) noexcept = default;

  json toJson() const override;

  const std::map<item::DamageType, float>& getDamageResistance() const;

  bool isValid() const override;
  bool isEquivalent(const InterfaceItem& other) const override;
  bool isEqual(const InterfaceItem& other) const override;

  protected:
  ArmorItem();

  void setDamageResistance(item::DamageType type, float amount);

  std::map<item::DamageType, float> damageResistance_;
};

#include "weapon_item.h"
#include "base_item.h"
#include "interface_item.h"
#include "item_damage_type.h"
#include "item_type.h"

#include <cmath>
#include <map>

WeaponItem::WeaponItem() = default;

auto WeaponItem::toJsonPayload() const -> json
{
  auto result = BaseItem::toJsonPayload();
  if (!damage_.empty())
  {
    auto damageJson = json::object();
    for (const auto& [type, amount] : damage_)
    {
      damageJson[item::typeToString(type)] = amount;
    }
    result["damage"] = damageJson;
  }
  result["rangeInMeters"] = itemRangeInMeters_;
  return result;
}

auto WeaponItem::getDamage() const -> const std::map<item::DamageType, int>&
{
  return damage_;
}

void WeaponItem::setDamage(item::DamageType type, int amount)
{
  damage_[type] = amount;
}

auto WeaponItem::getRangeInMeters() const -> float
{
  return itemRangeInMeters_;
}

void WeaponItem::setRangeInMeters(float range) { itemRangeInMeters_ = range; }

auto WeaponItem::isValid() const -> bool
{
  if (!BaseItem::isValid())
  {
    return false;
  }

  // A weapon must have one of the allowed weapon-slot types.

  if (const auto itemType = getType();
      itemType != item::Type::OneHandedPrimary &&
      itemType != item::Type::OneHandedSecondary &&
      itemType != item::Type::TwoHandedPrimary)
  {
    return false;
  }

  if (itemRangeInMeters_ <= 0.0F)
  {
    return false;
  }

  return true;
}

auto WeaponItem::isEquivalent(const InterfaceItem& other) const -> bool
{
  if (!BaseItem::isEquivalent(other))
  {
    return false;
  }

  // Different concrete type → not equivalent.
  const auto* otherWeapon = dynamic_cast<const WeaponItem*>(&other);
  if (otherWeapon == nullptr)
  {
    return false;
  }

  // Compare range with a small tolerance to avoid float-precision
  // mismatches (e.g. 0.1f vs parsed double 0.1).
  constexpr float kEpsilon = 1e-5F;
  const bool rangeEquivalent =
    std::abs(itemRangeInMeters_ - otherWeapon->itemRangeInMeters_) < kEpsilon;

  return damage_ == otherWeapon->damage_ && rangeEquivalent;
}

auto WeaponItem::isEqual(const InterfaceItem& other) const -> bool
{
  return isEquivalent(other) && getUUID() == other.getUUID();
}

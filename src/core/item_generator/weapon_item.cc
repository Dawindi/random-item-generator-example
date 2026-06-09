#include "weapon_item.h"

WeaponItem::WeaponItem() : BaseItem() {}

json WeaponItem::toJson() const
{
  auto j = BaseItem::toJson();
  if (!damage_.empty())
  {
    auto damageJson = json::object();
    for (const auto& [type, amount] : damage_)
      damageJson[item::typeToString(type)] = amount;
    j["damage"] = damageJson;
  }
  return j;
}

const std::map<item::DamageType, int>& WeaponItem::getDamage() const
{
  return damage_;
}

void WeaponItem::setDamage(item::DamageType type, int amount)
{
  damage_[type] = amount;
}

bool WeaponItem::isValid() const
{
  if (!BaseItem::isValid())
    return false;

  // A weapon must have one of the allowed weapon-slot types.

  if (const auto t = getType(); t != item::Type::OneHandedPrimary &&
                                t != item::Type::OneHandedSecondary &&
                                t != item::Type::TwoHandedPrimary)
  {
    return false;
  }

  return true;
}

bool WeaponItem::isEquivalent(const InterfaceItem& other) const
{
  if (!BaseItem::isEquivalent(other))
    return false;

  // Different concrete type → not equivalent.
  const auto* otherWeapon = dynamic_cast<const WeaponItem*>(&other);
  if (!otherWeapon)
    return false;

  return damage_ == otherWeapon->damage_;
}

bool WeaponItem::isEqual(const InterfaceItem& other) const
{
  return isEquivalent(other) && getUUID() == other.getUUID();
}

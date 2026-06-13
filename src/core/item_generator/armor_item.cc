#include "armor_item.h"

#include <cmath>

ArmorItem::ArmorItem() : BaseItem() {}

json ArmorItem::toJson() const
{
  auto j = BaseItem::toJson();
  if (!damageResistance_.empty())
  {
    auto resistanceJson = json::object();
    for (const auto& [type, amount] : damageResistance_)
      resistanceJson[item::typeToString(type)] = amount;
    j["damage_resistance"] = resistanceJson;
  }
  return j;
}

const std::map<item::DamageType, float>& ArmorItem::getDamageResistance() const
{
  return damageResistance_;
}

void ArmorItem::setDamageResistance(item::DamageType type, float amount)
{
  damageResistance_[type] = amount;
}

bool ArmorItem::isValid() const
{
  if (!BaseItem::isValid())
    return false;

  // An armor item must have one of the allowed armor-slot types.
  if (const auto t = getType();
      t != item::Type::HeadGear && t != item::Type::ChestGear &&
      t != item::Type::ArmGear && t != item::Type::LegGear &&
      t != item::Type::Amulet && t != item::Type::Ring)
  {
    return false;
  }

  // Must have at least one resistance entry.
  if (damageResistance_.empty())
    return false;

  // Every resistance value must be > 0.0f and < 100.0f.
  for (const auto& [type, amount] : damageResistance_)
  {
    if (amount <= 0.0f || amount >= 100.0f)
      return false;
  }

  return true;
}

bool ArmorItem::isEquivalent(const InterfaceItem& other) const
{
  if (!BaseItem::isEquivalent(other))
    return false;

  // Different concrete type → not equivalent.
  const auto* otherArmor = dynamic_cast<const ArmorItem*>(&other);
  if (!otherArmor)
    return false;

  // Compare resistance maps with float tolerance.
  if (damageResistance_.size() != otherArmor->damageResistance_.size())
    return false;

  constexpr float kEpsilon = 1e-5f;
  for (const auto& [type, amount] : damageResistance_)
  {
    auto it = otherArmor->damageResistance_.find(type);
    if (it == otherArmor->damageResistance_.end())
      return false;
    if (std::abs(amount - it->second) >= kEpsilon)
      return false;
  }

  return true;
}

bool ArmorItem::isEqual(const InterfaceItem& other) const
{
  return isEquivalent(other) && getUUID() == other.getUUID();
}

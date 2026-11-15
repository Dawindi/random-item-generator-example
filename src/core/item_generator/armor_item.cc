#include "armor_item.h"
#include "base_item.h"
#include "interface_item.h"
#include "item_damage_type.h"
#include "item_type.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <utility>

ArmorItem::ArmorItem() = default;

auto ArmorItem::toJsonPayload() const -> json
{
  auto result = BaseItem::toJsonPayload();
  if (!damageResistance_.empty())
  {
    auto resistanceJson = json::object();
    for (const auto& [type, amount] : damageResistance_)
    {
      resistanceJson[item::typeToString(type)] = amount;
    }
    result["damage_resistance"] = resistanceJson;
  }
  return result;
}

auto ArmorItem::getDamageResistance() const
  -> const std::map<item::DamageType, float>&
{
  return damageResistance_;
}

void ArmorItem::setDamageResistance(item::DamageType type, float amount)
{
  damageResistance_[type] = amount;
}

auto ArmorItem::isValid() const -> bool
{
  if (!BaseItem::isValid())
  {
    return false;
  }

  // An armor item must have one of the allowed armor-slot types.
  if (const auto itemType = getType();
      itemType != item::Type::HeadGear && itemType != item::Type::ChestGear &&
      itemType != item::Type::ArmGear && itemType != item::Type::LegGear &&
      itemType != item::Type::Amulet && itemType != item::Type::Ring)
  {
    return false;
  }

  // Must have at least one resistance entry.
  if (damageResistance_.empty())
  {
    return false;
  }

  // Check if any damage resistance values are invalid
  // Every resistance value must be > 0.0f and < 100.0f.
  if (std::ranges::any_of(
        damageResistance_.begin(), damageResistance_.end(),
        [](const std::pair<item::DamageType, float>& entry) -> bool
        { return entry.second <= 0.0F || entry.second >= 100.0F; }))
  {
    return false;
  }

  return true;
}

auto ArmorItem::isEquivalent(const InterfaceItem& other) const -> bool
{
  if (!BaseItem::isEquivalent(other))
  {
    return false;
  }

  // Different concrete type means not equivalent
  const auto* otherArmor = dynamic_cast<const ArmorItem*>(&other);
  if (otherArmor == nullptr)
  {
    return false;
  }

  // Compare resistance maps sizes
  if (damageResistance_.size() != otherArmor->damageResistance_.size())
  {
    return false;
  }

  // Compare resistance maps with a tolerance: every entry must exist in the
  // other map and its amount must match within the tolerance.
  constexpr float kEpsilon = 1e-5F;
  return std::ranges::all_of(
    damageResistance_.begin(), damageResistance_.end(),
    [otherArmor,
     kEpsilon](const std::pair<item::DamageType, float>& entry) -> bool
    {
      const auto otherArmorResForType =
        otherArmor->damageResistance_.find(entry.first);
      return otherArmorResForType != otherArmor->damageResistance_.end() &&
             std::abs(entry.second - otherArmorResForType->second) < kEpsilon;
    });
}

auto ArmorItem::isEqual(const InterfaceItem& other) const -> bool
{
  return isEquivalent(other) && getUUID() == other.getUUID();
}

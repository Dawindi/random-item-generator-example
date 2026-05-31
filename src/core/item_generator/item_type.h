#pragma once

namespace item
{

enum class Type : int
{
  OneHandedPrimary,
  OneHandedSecondary,
  TwoHandedPrimary,
  HeadGear,
  ChestGear,
  ArmGear,
  LegGear,
  Amulet,
  Ring,
  Consumable,
  Currency
};

[[nodiscard]] static constexpr const char* typeToString(Type rarity)
{
  switch (rarity)
  {
    case Type::OneHandedPrimary:
      return "OneHandedPrimary";
    case Type::OneHandedSecondary:
      return "OneHandedSecondary";
    case Type::TwoHandedPrimary:
      return "TwoHandedPrimary";
    case Type::HeadGear:
      return "HeadGear";
    case Type::ChestGear:
      return "ChestGear";
    case Type::ArmGear:
      return "ArmGear";
    case Type::LegGear:
      return "LegGear";
    case Type::Amulet:
      return "Amulet";
    case Type::Ring:
      return "Ring";
    case Type::Consumable:
      return "Consumable";
    case Type::Currency:
      return "Currency";
  }
  return "";
}

} // namespace item

#pragma once

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>

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

[[nodiscard]] inline std::optional<Type> typeFromString(std::string_view s)
{
  if (s.empty())
    return std::nullopt;

  std::string lower(s);
  std::transform(lower.begin(), lower.end(), lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (lower == "onehandedprimary")
    return Type::OneHandedPrimary;
  else if (lower == "onehandedsecondary")
    return Type::OneHandedSecondary;
  else if (lower == "twohandedprimary")
    return Type::TwoHandedPrimary;
  else if (lower == "headgear")
    return Type::HeadGear;
  else if (lower == "chestgear")
    return Type::ChestGear;
  else if (lower == "armgear")
    return Type::ArmGear;
  else if (lower == "leggear")
    return Type::LegGear;
  else if (lower == "amulet")
    return Type::Amulet;
  else if (lower == "ring")
    return Type::Ring;
  else if (lower == "consumable")
    return Type::Consumable;
  else if (lower == "currency")
    return Type::Currency;
  return std::nullopt;
}

} // namespace item

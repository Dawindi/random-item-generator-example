#pragma once

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>

namespace item
{

enum class DamageType : int
{
  Physical,
  Fire,
  Cold,
  Lightning,
  Chaos,
};

[[nodiscard]] static constexpr const char* typeToString(DamageType damageType)
{
  switch (damageType)
  {
    case DamageType::Physical:
      return "Physical";
    case DamageType::Fire:
      return "Fire";
    case DamageType::Cold:
      return "Cold";
    case DamageType::Lightning:
      return "Lightning";
    case DamageType::Chaos:
      return "Chaos";
  }
  return "";
}

[[nodiscard]] inline std::optional<DamageType>
damageTypeFromString(std::string_view s)
{
  if (s.empty())
    return std::nullopt;

  std::string lower(s);
  std::transform(lower.begin(), lower.end(), lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (lower == "physical")
    return DamageType::Physical;
  else if (lower == "fire")
    return DamageType::Fire;
  else if (lower == "cold")
    return DamageType::Cold;
  else if (lower == "lightning")
    return DamageType::Lightning;
  else if (lower == "chaos")
    return DamageType::Chaos;
  return std::nullopt;
}

} // namespace item

#pragma once

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

} // namespace item

#pragma once

namespace item
{

enum class Rarity : int
{
  Common,
  Uncommon,
  Rare,
  Epic,
  Legendary,
  Artifact,
};

[[nodiscard]] static constexpr const char* rarityToString(Rarity rarity)
{
  switch (rarity)
  {
    case Rarity::Common:
      return "Common";
    case Rarity::Uncommon:
      return "Uncommon";
    case Rarity::Rare:
      return "Rare";
    case Rarity::Epic:
      return "Epic";
    case Rarity::Legendary:
      return "Legendary";
    case Rarity::Artifact:
      return "Artifact";
  }
  return "";
}

} // namespace item

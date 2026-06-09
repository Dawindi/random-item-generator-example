#pragma once

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>

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

[[nodiscard]] inline std::optional<Rarity> rarityFromString(std::string_view s)
{
  if (s.empty())
    return std::nullopt;

  std::string lower(s);
  std::transform(lower.begin(), lower.end(), lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (lower == "common")
    return Rarity::Common;
  else if (lower == "uncommon")
    return Rarity::Uncommon;
  else if (lower == "rare")
    return Rarity::Rare;
  else if (lower == "epic")
    return Rarity::Epic;
  else if (lower == "legendary")
    return Rarity::Legendary;
  else if (lower == "artifact")
    return Rarity::Artifact;
  return std::nullopt;
}

} // namespace item

#pragma once

#include <string_view>

#include "item_data_types/item_rarity.h"

namespace item
{

/// Per-rarity modifications applied to an item preset during item
/// generation. Rarity affects the generated item's name, description and
/// the likelihood of rolling the maximum of its value range.
///
/// Plain old data: trivially copyable and default-constructible.
struct RarityModifier
{
  /// Prefix prepended to the base name (e.g. "Rare" in "Rare Dagger").
  std::string_view namePrefix;

  /// Suffix appended to the base name.
  std::string_view nameSuffix;

  /// Full description replacement. Empty means "keep the base
  /// description".
  std::string_view description;

  /// Probability in [0, 1] that a generated roll yields the maximum of the
  /// preset's value range (damage for weapons, resistance for armor).
  double maxValueChance = 0.0;
};

/// Rarity modifiers for every item::Rarity value, mapped explicitly so the
/// rarity-to-modifier assignment cannot silently go out of sync.
///
/// Plain old data: trivially copyable and default-constructible.
struct RarityModifiers
{
  RarityModifier common;
  RarityModifier uncommon;
  RarityModifier rare;
  RarityModifier epic;
  RarityModifier legendary;
  RarityModifier artifact;

  /// Returns the modifiers for the given rarity.
  [[nodiscard]] constexpr const RarityModifier& modifierFor(Rarity rarity) const
  {
    switch (rarity)
    {
      case Rarity::Common:
        return common;
      case Rarity::Uncommon:
        return uncommon;
      case Rarity::Rare:
        return rare;
      case Rarity::Epic:
        return epic;
      case Rarity::Legendary:
        return legendary;
      case Rarity::Artifact:
        return artifact;
    }
    return common;
  }
};

} // namespace item

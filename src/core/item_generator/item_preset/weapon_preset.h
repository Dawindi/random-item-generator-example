#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

#include "item_data_types/item_damage_type.h"
#include "item_data_types/item_type.h"
#include "item_preset.h"

namespace item
{

/// Blueprint describing how a weapon item of a given kind is generated.
///
/// Mostly plain old data: a trivially copyable aggregate holding non-owning
/// views into static string data and scalars. Presets hold no randomness
/// themselves and are copied when an item is created from them.
struct WeaponPreset
{
  /// Base name of the generated item, before rarity modifiers are applied.
  std::string_view name;

  /// Base description of the generated item, before rarity modifiers are
  /// applied.
  std::string_view description;

  /// Item slot of the generated weapon.
  Type type = Type::OneHandedPrimary;

  /// Damage type of the preset's damage range.
  DamageType damageType = DamageType::Physical;

  /// Inclusive lower bound of the damage range.
  int minDamage = 1;

  /// Inclusive upper bound of the damage range.
  int maxDamage = 1;

  /// Attack range of the generated weapon, in meters.
  float rangeInMeters = 0.1f;

  /// Per-rarity modifications applied during generation.
  RarityModifiers rarityModifiers;

  /// Returns the modifiers for the given rarity.
  [[nodiscard]] constexpr const RarityModifier& modifierFor(Rarity rarity) const
  {
    return rarityModifiers.modifierFor(rarity);
  }

  /// Returns the item name with the given rarity's modifiers applied.
  [[nodiscard]] constexpr std::string nameFor(Rarity rarity) const
  {
    const RarityModifier& modifier = modifierFor(rarity);
    return std::string(modifier.namePrefix) + std::string(name) +
           std::string(modifier.nameSuffix);
  }

  /// Returns the item description with the given rarity's modifiers
  /// applied. The view refers to static storage owned by the database.
  [[nodiscard]] constexpr std::string_view descriptionFor(Rarity rarity) const
  {
    const RarityModifier& modifier = modifierFor(rarity);
    if (!modifier.description.empty())
      return modifier.description;
    return description;
  }

  /// Returns the probability in [0, 1] that a damage roll yields the
  /// maximum of the damage range for the given rarity.
  [[nodiscard]] constexpr double maxValueChanceFor(Rarity rarity) const
  {
    return modifierFor(rarity).maxValueChance;
  }
};

/// Identifiers for the presets known to the database.
enum class WeaponPresetId : int
{
  Dagger,
  Spear,
  OneHandedSword,
  TwoHandedSword,
};

// ---------------------------------------------------------------------------
//  Preset database
//
//  All entries are constexpr: they are fully accessible from anywhere and
//  cannot be modified at runtime, so no further encapsulation is needed.
// ---------------------------------------------------------------------------

/// Blueprint for a dagger: a simple dagger dealing 1 to 10 physical damage.
inline constexpr WeaponPreset kDaggerPreset = WeaponPreset{
  .name = "Dagger",
  .description = "A simple dagger",
  .type = Type::OneHandedSecondary,
  .damageType = DamageType::Physical,
  .minDamage = 1,
  .maxDamage = 10,
  .rangeInMeters = 1.0f,
  .rarityModifiers =
    RarityModifiers{
      .common =
        RarityModifier{
          .namePrefix = "",
          .nameSuffix = "",
          .description = "",
          .maxValueChance = 0.10,
        },
      .uncommon =
        RarityModifier{
          .namePrefix = "Fine ",
          .nameSuffix = "",
          .description = "A fine dagger",
          .maxValueChance = 0.15,
        },
      .rare =
        RarityModifier{
          .namePrefix = "Rare ",
          .nameSuffix = "",
          .description = "A well-crafted dagger",
          .maxValueChance = 0.20,
        },
      .epic =
        RarityModifier{
          .namePrefix = "Epic ",
          .nameSuffix = "",
          .description = "A masterwork dagger",
          .maxValueChance = 0.30,
        },
      .legendary =
        RarityModifier{
          .namePrefix = "Legendary ",
          .nameSuffix = "",
          .description = "A dagger of legend",
          .maxValueChance = 0.40,
        },
      .artifact =
        RarityModifier{
          .namePrefix = "Artifact ",
          .nameSuffix = "",
          .description = "An ancient dagger of immense power",
          .maxValueChance = 0.50,
        },
    },
};

/// Blueprint for a spear: excellent range, slightly below average damage.
inline constexpr WeaponPreset kSpearPreset = WeaponPreset{
  .name = "Spear",
  .description = "A simple spear",
  .type = Type::TwoHandedPrimary,
  .damageType = DamageType::Physical,
  .minDamage = 2,
  .maxDamage = 8,
  .rangeInMeters = 2.5f,
  .rarityModifiers =
    RarityModifiers{
      .common =
        RarityModifier{
          .namePrefix = "",
          .nameSuffix = "",
          .description = "",
          .maxValueChance = 0.10,
        },
      .uncommon =
        RarityModifier{
          .namePrefix = "Fine ",
          .nameSuffix = "",
          .description = "A fine spear",
          .maxValueChance = 0.15,
        },
      .rare =
        RarityModifier{
          .namePrefix = "Rare ",
          .nameSuffix = "",
          .description = "A well-crafted spear",
          .maxValueChance = 0.20,
        },
      .epic =
        RarityModifier{
          .namePrefix = "Epic ",
          .nameSuffix = "",
          .description = "A masterwork spear",
          .maxValueChance = 0.30,
        },
      .legendary =
        RarityModifier{
          .namePrefix = "Legendary ",
          .nameSuffix = "",
          .description = "A spear of legend",
          .maxValueChance = 0.40,
        },
      .artifact =
        RarityModifier{
          .namePrefix = "Artifact ",
          .nameSuffix = "",
          .description = "An ancient spear of immense power",
          .maxValueChance = 0.50,
        },
    },
};

/// Blueprint for a one-handed sword: balanced range and damage.
inline constexpr WeaponPreset kOneHandedSwordPreset = WeaponPreset{
  .name = "One-Handed Sword",
  .description = "A balanced one-handed sword",
  .type = Type::OneHandedPrimary,
  .damageType = DamageType::Physical,
  .minDamage = 2,
  .maxDamage = 10,
  .rangeInMeters = 1.2f,
  .rarityModifiers =
    RarityModifiers{
      .common =
        RarityModifier{
          .namePrefix = "",
          .nameSuffix = "",
          .description = "",
          .maxValueChance = 0.10,
        },
      .uncommon =
        RarityModifier{
          .namePrefix = "Fine ",
          .nameSuffix = "",
          .description = "A fine one-handed sword",
          .maxValueChance = 0.15,
        },
      .rare =
        RarityModifier{
          .namePrefix = "Rare ",
          .nameSuffix = "",
          .description = "A well-crafted one-handed sword",
          .maxValueChance = 0.20,
        },
      .epic =
        RarityModifier{
          .namePrefix = "Epic ",
          .nameSuffix = "",
          .description = "A masterwork one-handed sword",
          .maxValueChance = 0.30,
        },
      .legendary =
        RarityModifier{
          .namePrefix = "Legendary ",
          .nameSuffix = "",
          .description = "A one-handed sword of legend",
          .maxValueChance = 0.40,
        },
      .artifact =
        RarityModifier{
          .namePrefix = "Artifact ",
          .nameSuffix = "",
          .description = "An ancient one-handed sword of immense power",
          .maxValueChance = 0.50,
        },
    },
};

/// Blueprint for a two-handed sword: high damage, medium range.
inline constexpr WeaponPreset kTwoHandedSwordPreset = WeaponPreset{
  .name = "Two-Handed Sword",
  .description = "A heavy two-handed sword",
  .type = Type::TwoHandedPrimary,
  .damageType = DamageType::Physical,
  .minDamage = 5,
  .maxDamage = 15,
  .rangeInMeters = 1.6f,
  .rarityModifiers =
    RarityModifiers{
      .common =
        RarityModifier{
          .namePrefix = "",
          .nameSuffix = "",
          .description = "",
          .maxValueChance = 0.10,
        },
      .uncommon =
        RarityModifier{
          .namePrefix = "Fine ",
          .nameSuffix = "",
          .description = "A fine two-handed sword",
          .maxValueChance = 0.15,
        },
      .rare =
        RarityModifier{
          .namePrefix = "Rare ",
          .nameSuffix = "",
          .description = "A well-crafted two-handed sword",
          .maxValueChance = 0.20,
        },
      .epic =
        RarityModifier{
          .namePrefix = "Epic ",
          .nameSuffix = "",
          .description = "A masterwork two-handed sword",
          .maxValueChance = 0.30,
        },
      .legendary =
        RarityModifier{
          .namePrefix = "Legendary ",
          .nameSuffix = "",
          .description = "A two-handed sword of legend",
          .maxValueChance = 0.40,
        },
      .artifact =
        RarityModifier{
          .namePrefix = "Artifact ",
          .nameSuffix = "",
          .description = "An ancient two-handed sword of immense power",
          .maxValueChance = 0.50,
        },
    },
};

/// All weapon presets known to the database, in WeaponPresetId order. Keep
/// this registry in sync with the WeaponPresetId enum so it can be indexed
/// by it.
inline constexpr std::array<WeaponPreset, 4> kWeaponPresets = {
  kDaggerPreset,
  kSpearPreset,
  kOneHandedSwordPreset,
  kTwoHandedSwordPreset,
};

/// Returns a copy of the preset registered under the given id, or
/// std::nullopt if the id is not registered. Presets have no identity of
/// their own and are trivially cheap to copy (non-owning string views and
/// scalars only), so handing out copies cannot affect the database.
[[nodiscard]] constexpr std::optional<WeaponPreset>
getWeaponPreset(const WeaponPresetId id)
{
  const auto index = static_cast<std::size_t>(id);
  if (index >= kWeaponPresets.size())
    return std::nullopt;
  return kWeaponPresets[index];
}

} // namespace item

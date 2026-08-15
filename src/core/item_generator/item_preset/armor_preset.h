#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

#include "item_data_types/item_type.h"
#include "item_preset.h"

namespace item
{

/// Blueprint describing how an armor item of a given kind is generated.
///
/// Mostly plain old data: a trivially copyable aggregate holding non-owning
/// views into static string data and scalars. Presets hold no randomness
/// themselves and are copied when an item is created from them.
struct ArmorPreset
{
  /// Base name of the generated item, before rarity modifiers are applied.
  std::string_view name;

  /// Base description of the generated item, before rarity modifiers are
  /// applied.
  std::string_view description;

  /// Item slot of the generated armor piece.
  Type type = Type::ChestGear;

  /// Inclusive lower bound of the physical resistance range.
  int minResistance = 1;

  /// Inclusive upper bound of the physical resistance range.
  int maxResistance = 1;

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

  /// Returns the probability in [0, 1] that a resistance roll yields the
  /// maximum of the resistance range for the given rarity.
  [[nodiscard]] constexpr double maxValueChanceFor(Rarity rarity) const
  {
    return modifierFor(rarity).maxValueChance;
  }
};

/// Identifiers for the presets known to the database.
enum class ArmorPresetId : int
{
  Gloves,
  Boots,
  Helmet,
  Chestplate,
};

// ---------------------------------------------------------------------------
//  Preset database
//
//  All entries are constexpr: they are fully accessible from anywhere and
//  cannot be modified at runtime, so no further encapsulation is needed.
// ---------------------------------------------------------------------------

/// Blueprint for gloves: light protection for the arms.
inline constexpr ArmorPreset kGlovesPreset = ArmorPreset{
  .name = "Gloves",
  .description = "Simple gloves",
  .type = Type::ArmGear,
  .minResistance = 1,
  .maxResistance = 3,
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
          .description = "A fine pair of gloves",
          .maxValueChance = 0.15,
        },
      .rare =
        RarityModifier{
          .namePrefix = "Rare ",
          .nameSuffix = "",
          .description = "A well-crafted pair of gloves",
          .maxValueChance = 0.20,
        },
      .epic =
        RarityModifier{
          .namePrefix = "Epic ",
          .nameSuffix = "",
          .description = "A masterwork pair of gloves",
          .maxValueChance = 0.30,
        },
      .legendary =
        RarityModifier{
          .namePrefix = "Legendary ",
          .nameSuffix = "",
          .description = "Gloves of legend",
          .maxValueChance = 0.40,
        },
      .artifact =
        RarityModifier{
          .namePrefix = "Artifact ",
          .nameSuffix = "",
          .description = "Ancient gloves of immense power",
          .maxValueChance = 0.50,
        },
    },
};

/// Blueprint for boots: light protection for the legs.
inline constexpr ArmorPreset kBootsPreset = ArmorPreset{
  .name = "Boots",
  .description = "Simple boots",
  .type = Type::LegGear,
  .minResistance = 1,
  .maxResistance = 4,
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
          .description = "A fine pair of boots",
          .maxValueChance = 0.15,
        },
      .rare =
        RarityModifier{
          .namePrefix = "Rare ",
          .nameSuffix = "",
          .description = "A well-crafted pair of boots",
          .maxValueChance = 0.20,
        },
      .epic =
        RarityModifier{
          .namePrefix = "Epic ",
          .nameSuffix = "",
          .description = "A masterwork pair of boots",
          .maxValueChance = 0.30,
        },
      .legendary =
        RarityModifier{
          .namePrefix = "Legendary ",
          .nameSuffix = "",
          .description = "Boots of legend",
          .maxValueChance = 0.40,
        },
      .artifact =
        RarityModifier{
          .namePrefix = "Artifact ",
          .nameSuffix = "",
          .description = "Ancient boots of immense power",
          .maxValueChance = 0.50,
        },
    },
};

/// Blueprint for a helmet: protection for the head.
inline constexpr ArmorPreset kHelmetPreset = ArmorPreset{
  .name = "Helmet",
  .description = "A simple helmet",
  .type = Type::HeadGear,
  .minResistance = 2,
  .maxResistance = 5,
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
          .description = "A fine helmet",
          .maxValueChance = 0.15,
        },
      .rare =
        RarityModifier{
          .namePrefix = "Rare ",
          .nameSuffix = "",
          .description = "A well-crafted helmet",
          .maxValueChance = 0.20,
        },
      .epic =
        RarityModifier{
          .namePrefix = "Epic ",
          .nameSuffix = "",
          .description = "A masterwork helmet",
          .maxValueChance = 0.30,
        },
      .legendary =
        RarityModifier{
          .namePrefix = "Legendary ",
          .nameSuffix = "",
          .description = "A helmet of legend",
          .maxValueChance = 0.40,
        },
      .artifact =
        RarityModifier{
          .namePrefix = "Artifact ",
          .nameSuffix = "",
          .description = "An ancient helmet of immense power",
          .maxValueChance = 0.50,
        },
    },
};

/// Blueprint for a chestplate: heavy protection for the chest.
inline constexpr ArmorPreset kChestplatePreset = ArmorPreset{
  .name = "Chestplate",
  .description = "A simple chestplate",
  .type = Type::ChestGear,
  .minResistance = 4,
  .maxResistance = 8,
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
          .description = "A fine chestplate",
          .maxValueChance = 0.15,
        },
      .rare =
        RarityModifier{
          .namePrefix = "Rare ",
          .nameSuffix = "",
          .description = "A well-crafted chestplate",
          .maxValueChance = 0.20,
        },
      .epic =
        RarityModifier{
          .namePrefix = "Epic ",
          .nameSuffix = "",
          .description = "A masterwork chestplate",
          .maxValueChance = 0.30,
        },
      .legendary =
        RarityModifier{
          .namePrefix = "Legendary ",
          .nameSuffix = "",
          .description = "A chestplate of legend",
          .maxValueChance = 0.40,
        },
      .artifact =
        RarityModifier{
          .namePrefix = "Artifact ",
          .nameSuffix = "",
          .description = "An ancient chestplate of immense power",
          .maxValueChance = 0.50,
        },
    },
};

/// All armor presets known to the database, in ArmorPresetId order. Keep
/// this registry in sync with the ArmorPresetId enum so it can be indexed
/// by it.
inline constexpr std::array<ArmorPreset, 4> kArmorPresets = {
  kGlovesPreset,
  kBootsPreset,
  kHelmetPreset,
  kChestplatePreset,
};

/// Returns a copy of the preset registered under the given id, or
/// std::nullopt if the id is not registered. Presets have no identity of
/// their own and are trivially cheap to copy (non-owning string views and
/// scalars only), so handing out copies cannot affect the database.
[[nodiscard]] constexpr std::optional<ArmorPreset>
getArmorPreset(const ArmorPresetId id)
{
  const auto index = static_cast<std::size_t>(id);
  if (index >= kArmorPresets.size())
    return std::nullopt;
  return kArmorPresets[index];
}

} // namespace item

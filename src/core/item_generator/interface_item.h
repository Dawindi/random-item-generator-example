#pragma once

#include <nlohmann/json.hpp>
#include <optional>

#include "uuid.h"

using json = nlohmann::json;

namespace item
{
enum class Rarity;
enum class Type;
enum class Status;
} // namespace item

class InterfaceItem
{
  public:
  virtual ~InterfaceItem() = default;

  // - Conversions -
  // toJson returns a JSON representation of the item.
  virtual json toJson() const = 0;

  // - Getters/Setters -
  // UUID returns the item's unique identifier as a const reference to avoid
  // modification. There is no setter for UUID — each UUID should be unique.
  virtual const UUID& getUUID() const = 0;

  // name returns the item's name as a const reference to avoid modification.
  virtual const std::string& getName() const = 0;

  // name sets the item's name to the given value.
  virtual void setName(const std::string&) = 0;

  // description returns the item's description as a const reference to avoid
  // modification.
  virtual const std::string& getDescription() const = 0;

  // description sets the item's description to the given value.
  virtual void setDescription(const std::string&) = 0;

  // weight returns the item's weight as a const reference to avoid
  // modification.
  virtual const float& getWeight() const = 0;

  // weight sets the item's weight to the given value.
  virtual void setWeight(float) = 0;

  // rarity returns the item's rarity as a const reference to avoid
  // modification.
  virtual const item::Rarity& getRarity() const = 0;

  // rarity sets the item's rarity to the given value.
  virtual void setRarity(item::Rarity) = 0;

  // type returns the item's type as a const reference to avoid
  // modification.
  virtual const item::Type& getType() const = 0;

  // type sets the item's type to the given value.
  virtual void setType(item::Type) = 0;

  // status returns the item's status as a const reference to avoid
  // modification.
  virtual const item::Status& getStatus() const = 0;

  // status sets the item's status to the given value.
  virtual void setStatus(item::Status) = 0;

  // owner returns the item's owner as a const reference to avoid
  // modification.
  virtual const std::optional<UUID>& getOwner() const = 0;

  // owner sets the item's owner to the given value.
  virtual void setOwner(std::optional<UUID>) = 0;

  // - Checks -
  // isValid returns whether the item is valid (i.e. has a non-empty UUID).
  virtual bool isValid() const = 0;

  // isEquivalent returns whether the item is equivalent to another item without
  // considering UUID equality.
  virtual bool isEquivalent(const InterfaceItem&) const = 0;

  // isEqual returns whether the item is equal to another item without
  // considering UUID equality.
  virtual bool isEqual(const InterfaceItem&) const = 0;
};

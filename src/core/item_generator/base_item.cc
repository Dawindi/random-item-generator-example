#include "base_item.h"
#include "interface_item.h"
#include "item_rarity.h"
#include "item_serialization.h"
#include "item_status.h"
#include "item_type.h"

#include <optional>
#include <string>
#include <string_view>
#include <utility>

BaseItem::BaseItem() = default;

auto BaseItem::toJson() const -> json
{
  return item::signItem(toJsonPayload());
}

auto BaseItem::toJsonPayload() const -> json
{
  auto result = json::object();
  result["uuid"] = uuid_.toString();
  result["name"] = name_;
  result["description"] = description_;
  result["weight"] = weight_;
  result["rarity"] = item::rarityToString(rarity_);
  result["type"] = item::typeToString(type_);
  result["status"] = item::statusToString(status_);
  if (owner_.has_value())
  {
    result["owner"] = owner_->toString();
  }
  return result;
}

void BaseItem::setUUID(UUID uuid) { uuid_ = std::move(uuid); }

auto BaseItem::getUUID() const -> const UUID& { return uuid_; }

auto BaseItem::getName() const -> const std::string& { return name_; }
void BaseItem::setName(const std::string& name) { name_ = name; }

auto BaseItem::getDescription() const -> const std::string&
{
  return description_;
}
void BaseItem::setDescription(const std::string& description)
{
  description_ = description;
}

auto BaseItem::getWeight() const -> const float& { return weight_; }
void BaseItem::setWeight(float weight) { weight_ = weight; }

auto BaseItem::getRarity() const -> const item::Rarity& { return rarity_; }
void BaseItem::setRarity(item::Rarity rarity) { rarity_ = rarity; }

auto BaseItem::getType() const -> const item::Type& { return type_; }
void BaseItem::setType(item::Type type) { type_ = type; }

auto BaseItem::getStatus() const -> const item::Status& { return status_; }
void BaseItem::setStatus(item::Status status) { status_ = status; }

auto BaseItem::getOwner() const -> const std::optional<UUID>& { return owner_; }
void BaseItem::setOwner(std::optional<UUID> owner)
{
  owner_ = std::move(owner);
}

auto BaseItem::isValid() const -> bool
{
  using std::string_view;

  const bool valid_rarity = !string_view(item::rarityToString(rarity_)).empty();
  const bool valid_type = !string_view(item::typeToString(type_)).empty();
  const bool valid_weight = weight_ >= 0.0F;
  const bool valid_owner_when_owned =
    (owner_ != std::nullopt && status_ != item::Status::NOT_OWNDED) ||
    (owner_ == std::nullopt && status_ == item::Status::NOT_OWNDED);

  return valid_weight && valid_rarity && valid_type && valid_owner_when_owned;
}

auto BaseItem::isEquivalent(const InterfaceItem& other) const -> bool
{
  return name_ == other.getName() && description_ == other.getDescription() &&
         weight_ == other.getWeight() && rarity_ == other.getRarity() &&
         type_ == other.getType();
}

auto BaseItem::isEqual(const InterfaceItem& other) const -> bool
{
  return isEquivalent(other) && uuid_ == other.getUUID();
}

#include "base_item.h"
#include "item_type.h"

#include <string_view>

BaseItem::BaseItem() : uuid_() {}

json BaseItem::toJson() const
{
  auto j = json::object();
  j["uuid"] = uuid_.toString();
  j["name"] = name_;
  j["description"] = description_;
  j["weight"] = weight_;
  j["rarity"] = item::rarityToString(rarity_);
  j["type"] = item::typeToString(type_);
  j["status"] = item::statusToString(status_);
  if (owner_.has_value())
    j["owner"] = owner_->toString();
  return j;
}

const UUID& BaseItem::getUUID() const { return uuid_; }

const std::string& BaseItem::getName() const { return name_; }
void BaseItem::setName(const std::string& name) { name_ = name; }

const std::string& BaseItem::getDescription() const { return description_; }
void BaseItem::setDescription(const std::string& description)
{
  description_ = description;
}

const float& BaseItem::getWeight() const { return weight_; }
void BaseItem::setWeight(float weight) { weight_ = weight; }

const item::Rarity& BaseItem::getRarity() const { return rarity_; }
void BaseItem::setRarity(item::Rarity rarity) { rarity_ = rarity; }

const item::Type& BaseItem::getType() const { return type_; }
void BaseItem::setType(item::Type type) { type_ = type; }

const item::Status& BaseItem::getStatus() const { return status_; }
void BaseItem::setStatus(item::Status status) { status_ = status; }

const std::optional<UUID>& BaseItem::getOwner() const { return owner_; }
void BaseItem::setOwner(std::optional<UUID> owner)
{
  owner_ = std::move(owner);
}

bool BaseItem::isValid() const
{
  using std::string_view;

  return weight_ >= 0.0f &&
         !string_view(item::rarityToString(rarity_)).empty() &&
         !string_view(item::typeToString(type_)).empty();
}

bool BaseItem::isEquivalent(const InterfaceItem& other) const
{
  return name_ == other.getName() && description_ == other.getDescription() &&
         weight_ == other.getWeight() && rarity_ == other.getRarity() &&
         type_ == other.getType();
}

bool BaseItem::isEqual(const InterfaceItem& other) const
{
  return isEquivalent(other) && uuid_ == other.getUUID();
}

#pragma once

#include <optional>
#include <string>

#include "interface_item.h"
#include "item_rarity.h"
#include "item_status.h"
#include "item_type.h"

class BaseItem : public InterfaceItem
{
  public:
  BaseItem();
  ~BaseItem() override = default;

  json toJson() const override;

  const UUID& getUUID() const override;

  const std::string& getName() const override;
  void setName(const std::string& name) override;

  const std::string& getDescription() const override;
  void setDescription(const std::string& description) override;

  const float& getWeight() const override;
  void setWeight(float weight) override;

  const item::Rarity& getRarity() const override;
  void setRarity(item::Rarity rarity) override;

  const item::Type& getType() const override;
  void setType(item::Type type) override;

  const item::Status& getStatus() const override;
  void setStatus(item::Status status) override;

  const std::optional<UUID>& getOwner() const override;
  void setOwner(std::optional<UUID> owner) override;

  bool isValid() const override;
  bool isEquivalent(const InterfaceItem& other) const override;
  bool isEqual(const InterfaceItem& other) const override;

  protected:
  UUID uuid_;
  std::string name_;
  std::string description_;
  float weight_;
  item::Rarity rarity_;
  item::Type type_;
  item::Status status_;
  std::optional<UUID> owner_;
};

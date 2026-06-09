#pragma once

#include <optional>
#include <string>

#include "interface_item.h"
#include "item_rarity.h"
#include "item_status.h"
#include "item_type.h"

class BaseItem : public InterfaceItem
{
  friend class ItemBuilder;

  public:
  ~BaseItem() override = default;

  // Non-copyable (holds a non-copyable UUID).
  BaseItem(const BaseItem&) = delete;
  BaseItem& operator=(const BaseItem&) = delete;

  // Movable.
  BaseItem(BaseItem&&) noexcept = default;
  BaseItem& operator=(BaseItem&&) noexcept = default;

  json toJson() const override;

  const UUID& getUUID() const override;

  const std::string& getName() const override;
  const std::string& getDescription() const override;
  const float& getWeight() const override;
  const item::Rarity& getRarity() const override;
  const item::Type& getType() const override;
  const item::Status& getStatus() const override;
  const std::optional<UUID>& getOwner() const override;

  bool isValid() const override;
  bool isEquivalent(const InterfaceItem& other) const override;
  bool isEqual(const InterfaceItem& other) const override;

  protected:
  BaseItem();

  void setName(const std::string& name) final override;
  void setDescription(const std::string& description) final override;
  void setWeight(float weight) final override;
  void setRarity(item::Rarity rarity) final override;
  void setType(item::Type type) final override;
  void setStatus(item::Status status) final override;
  void setOwner(std::optional<UUID> owner) final override;

  UUID uuid_;
  std::string name_;
  std::string description_;
  float weight_;
  item::Rarity rarity_;
  item::Type type_;
  item::Status status_;
  std::optional<UUID> owner_;
};

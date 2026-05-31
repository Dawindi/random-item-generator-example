#pragma once

#include <string>

class UUID
{
  public:
  /// Generates a new random UUID v4 on construction.
  UUID();

  ~UUID() = default;

  // Non-copyable — each UUID should be unique.
  UUID(const UUID&) = delete;
  UUID& operator=(const UUID&) = delete;

  // Movable
  UUID(UUID&&) noexcept = default;
  UUID& operator=(UUID&&) noexcept = default;

  /// Returns the UUID as a string (e.g.
  /// "550e8400-e29b-41d4-a716-446655440000").
  std::string toString() const;

  /// Compares two UUIDs for equality.
  bool operator==(const UUID& other) const;
  bool operator!=(const UUID& other) const;

  private:
  std::string m_uuid;
};

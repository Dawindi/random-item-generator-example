#pragma once

#include <array>
#include <cstdint>
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

  // Movable — moved-from UUID is zeroed.
  UUID(UUID&& other) noexcept;
  UUID& operator=(UUID&& other) noexcept;

  /// Compares two UUIDs for equality (all 16 bytes equal).
  bool operator==(const UUID& other) const;
  bool operator!=(const UUID& other) const;

  /// Returns the UUID as a string (e.g.
  /// "550e8400-e29b-41d4-a716-446655440000").
  std::string toString() const;

  private:
  std::array<std::uint8_t, 16> uuid_;
};

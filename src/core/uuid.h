#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

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

  /// Parses a canonical UUID string ("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx").
  /// Returns std::nullopt when the string is malformed or contains
  /// non-hexadecimal digits.
  static std::optional<UUID> fromString(std::string_view text);

  private:
  static constexpr uint8_t kUUIDSize_ = 16;
  std::array<std::uint8_t, kUUIDSize_> uuid_;
};

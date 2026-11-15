#include "uuid.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <ios>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <string_view>

namespace
{
constexpr std::size_t kUuidHalfByteCount = 8;
constexpr std::size_t kVersionByteIndex = 6;
constexpr std::size_t kVariantByteIndex = 8;
constexpr std::uint8_t kVersionMask = 0x0F;
constexpr std::uint8_t kVersionBits = 0x40;
constexpr std::uint8_t kVariantMask = 0x3F;
constexpr std::uint8_t kVariantBits = 0x80;
constexpr std::size_t kFirstDashIndex = 4;
constexpr std::size_t kSecondDashIndex = 6;
constexpr std::size_t kThirdDashIndex = 8;
constexpr std::size_t kFourthDashIndex = 10;

constexpr std::size_t kUuidStringLength = 36;
constexpr std::array<std::size_t, 5> kUuidGroupSizes = {8, 4, 4, 4, 12};
constexpr int kHexDigitBase = 10;

/// Returns the value of a hex digit (0-15), or -1 when the character is not a
/// hexadecimal digit.
auto hexValue(char character) -> int
{
  if (character >= '0' && character <= '9')
  {
    return character - '0';
  }
  if (character >= 'a' && character <= 'f')
  {
    return character - 'a' + kHexDigitBase;
  }
  if (character >= 'A' && character <= 'F')
  {
    return character - 'A' + kHexDigitBase;
  }
  return -1;
}
} // namespace

UUID::UUID()
{
  // Thread-safe static locals (guaranteed by C++11).
  static std::random_device randomSource;
  static std::mt19937_64 gen(randomSource());
  static std::uniform_int_distribution<std::uint64_t> dis;

  const std::uint64_t firstHalf = dis(gen);
  const std::uint64_t secondHalf = dis(gen);
  std::memcpy(uuid_.data(), &firstHalf, kUuidHalfByteCount);
  std::memcpy(&uuid_[kUuidHalfByteCount], &secondHalf, kUuidHalfByteCount);

  // Stamp version 4 (byte 6, upper nibble → 0100 xxxx).
  uuid_[kVersionByteIndex] = static_cast<std::uint8_t>(
    (uuid_[kVersionByteIndex] & kVersionMask) | kVersionBits);

  // Stamp variant (byte 8, upper two bits → 10xx xxxx).
  uuid_[kVariantByteIndex] = static_cast<std::uint8_t>(
    (uuid_[kVariantByteIndex] & kVariantMask) | kVariantBits);
}

UUID::UUID(UUID&& other) noexcept : uuid_(other.uuid_) { other.uuid_.fill(0); }

auto UUID::operator=(UUID&& other) noexcept -> UUID&
{
  if (this != &other)
  {
    uuid_ = other.uuid_;
    other.uuid_.fill(0);
  }
  return *this;
}

auto UUID::operator==(const UUID& other) const -> bool
{
  return uuid_ == other.uuid_;
}

auto UUID::operator!=(const UUID& other) const -> bool
{
  return uuid_ != other.uuid_;
}

auto UUID::toString() const -> std::string
{
  std::ostringstream oss;
  oss << std::hex << std::setfill('0');

  for (std::size_t i = 0; i < uuid_.size(); ++i)
  {
    // Insert dashes after bytes 3, 5, 7, 9.
    if (i == kFirstDashIndex || i == kSecondDashIndex || i == kThirdDashIndex ||
        i == kFourthDashIndex)
    {
      oss << '-';
    }
    oss << std::setw(2) << static_cast<int>(uuid_[i]);
  }

  return oss.str();
}

auto UUID::fromString(std::string_view text) -> std::optional<UUID>
{
  if (text.size() != kUuidStringLength)
  {
    return std::nullopt;
  }

  UUID uuid;
  std::size_t position = 0;
  std::size_t byteIndex = 0;

  for (std::size_t group = 0; group < kUuidGroupSizes.size(); ++group)
  {
    if (group > 0)
    {
      if (text[position] != '-')
      {
        return std::nullopt;
      }
      ++position;
    }

    const std::size_t groupSize = kUuidGroupSizes[group];
    for (std::size_t i = 0; i < groupSize; i += 2)
    {
      const int high = hexValue(text[position]);
      const int low = hexValue(text[position + 1]);
      if (high < 0 || low < 0)
      {
        return std::nullopt;
      }
      uuid.uuid_[byteIndex] =
        static_cast<std::uint8_t>((high << 4) | low);
      ++byteIndex;
      position += 2;
    }
  }

  return uuid;
}

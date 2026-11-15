#include "uuid.h"

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <ios>
#include <random>
#include <sstream>
#include <string>

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

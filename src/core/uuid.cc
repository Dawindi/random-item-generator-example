#include "uuid.h"

#include <cstring>
#include <iomanip>
#include <random>
#include <sstream>

UUID::UUID()
{
  // Thread-safe static locals (guaranteed by C++11).
  static std::random_device rd;
  static std::mt19937_64 gen(rd());
  static std::uniform_int_distribution<std::uint64_t> dis;

  const std::uint64_t a = dis(gen);
  const std::uint64_t b = dis(gen);
  std::memcpy(&uuid_[0], &a, 8);
  std::memcpy(&uuid_[8], &b, 8);

  // Stamp version 4 (byte 6, upper nibble → 0100 xxxx).
  uuid_[6] = static_cast<std::uint8_t>((uuid_[6] & 0x0F) | 0x40);

  // Stamp variant (byte 8, upper two bits → 10xx xxxx).
  uuid_[8] = static_cast<std::uint8_t>((uuid_[8] & 0x3F) | 0x80);
}

UUID::UUID(UUID&& other) noexcept : uuid_(other.uuid_) { other.uuid_.fill(0); }

UUID& UUID::operator=(UUID&& other) noexcept
{
  if (this != &other)
  {
    uuid_ = other.uuid_;
    other.uuid_.fill(0);
  }
  return *this;
}

bool UUID::operator==(const UUID& other) const { return uuid_ == other.uuid_; }

bool UUID::operator!=(const UUID& other) const { return uuid_ != other.uuid_; }

std::string UUID::toString() const
{
  std::ostringstream oss;
  oss << std::hex << std::setfill('0');

  for (int i = 0; i < 16; ++i)
  {
    // Insert dashes after bytes 3, 5, 7, 9.
    if (i == 4 || i == 6 || i == 8 || i == 10)
      oss << '-';
    oss << std::setw(2) << static_cast<int>(uuid_[i]);
  }

  return oss.str();
}

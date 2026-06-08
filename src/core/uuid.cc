#include "uuid.h"

#include <random>
#include <sstream>

UUID::UUID()
{
  // Thread-safe static locals (guaranteed by C++11).
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> hex_dis(0, 15);
  static std::uniform_int_distribution<> variant_dis(8, 11); // 8, 9, A, B

  std::ostringstream oss;
  oss << std::hex;

  // Time-low               (8 hex digits)
  for (int i = 0; i < 8; ++i)
    oss << hex_dis(gen);
  oss << '-';

  // Time-mid               (4 hex digits)
  for (int i = 0; i < 4; ++i)
    oss << hex_dis(gen);
  oss << '-';

  // Version 4 + time-high  ("4" + 3 hex digits)
  oss << '4';
  for (int i = 0; i < 3; ++i)
    oss << hex_dis(gen);
  oss << '-';

  // Variant bits           (1 hex digit from [8,B] + 3 hex digits)
  oss << variant_dis(gen);
  for (int i = 0; i < 3; ++i)
    oss << hex_dis(gen);
  oss << '-';

  // Node                   (12 hex digits)
  for (int i = 0; i < 12; ++i)
    oss << hex_dis(gen);

  m_uuid = oss.str();
}

std::string UUID::toString() const { return m_uuid; }

bool UUID::isValid() const { return !m_uuid.empty() && m_uuid.size() == 36; }

bool UUID::operator==(const UUID& other) const
{
  return m_uuid == other.m_uuid;
}

bool UUID::operator!=(const UUID& other) const
{
  return m_uuid != other.m_uuid;
}

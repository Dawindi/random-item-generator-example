#include "item_serialization.h"
#include "uuid.h"

#include <openssl/evp.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

namespace
{

constexpr std::size_t kSha256DigestLength = 32;

auto digestToHex(const std::array<std::uint8_t, kSha256DigestLength>& digest)
  -> std::string
{
  std::ostringstream oss;
  oss << std::hex << std::setfill('0');
  for (const auto byte : digest)
  {
    oss << std::setw(2) << static_cast<int>(byte);
  }
  return oss.str();
}

} // namespace

namespace item
{

auto sha256Hex(std::string_view data) -> std::string
{
  std::array<std::uint8_t, kSha256DigestLength> digest{};
  std::size_t digestLength = digest.size();
  const int success = EVP_Q_digest(nullptr, "SHA256", nullptr, data.data(),
                                    data.size(), digest.data(), &digestLength);
  if (success != 1)
  {
    return {};
  }
  return digestToHex(digest);
}

auto signItem(const nlohmann::json& payload) -> nlohmann::json
{
  nlohmann::json envelope = nlohmann::json::object();
  envelope["payload"] = payload;
  envelope["hash"] = sha256Hex(payload.dump());
  return envelope;
}

auto verifyItem(const nlohmann::json& envelope)
  -> std::optional<nlohmann::json>
{
  if (!envelope.is_object())
  {
    return std::nullopt;
  }
  if (!envelope.contains("payload") || !envelope["payload"].is_object())
  {
    return std::nullopt;
  }
  if (!envelope.contains("hash") || !envelope["hash"].is_string())
  {
    return std::nullopt;
  }

  const auto& payload = envelope["payload"];
  const auto expectedHash = envelope["hash"].get<std::string>();
  const auto actualHash = sha256Hex(payload.dump());
  if (actualHash != expectedHash)
  {
    return std::nullopt;
  }
  return payload;
}

auto parseUuid(const nlohmann::json& payload) -> std::optional<UUID>
{
  if (!payload.contains("uuid") || !payload["uuid"].is_string())
  {
    return std::nullopt;
  }
  return UUID::fromString(payload["uuid"].get<std::string>());
}

auto hasValidCommonFields(const nlohmann::json& payload) -> bool
{
  if (!payload.contains("name") || !payload["name"].is_string())
  {
    return false;
  }
  if (!payload.contains("description") ||
      !payload["description"].is_string())
  {
    return false;
  }
  if (!payload.contains("weight") || !payload["weight"].is_number())
  {
    return false;
  }
  if (!payload.contains("rarity") || !payload["rarity"].is_string())
  {
    return false;
  }
  if (!payload.contains("type") || !payload["type"].is_string())
  {
    return false;
  }
  if (!payload.contains("status") || !payload["status"].is_string())
  {
    return false;
  }
  return true;
}

} // namespace item

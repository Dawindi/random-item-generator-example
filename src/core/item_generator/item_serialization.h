#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>

#include "uuid.h"

namespace item
{

/// Computes the SHA-256 digest of `data` and returns it as a lowercase hex
/// string.
[[nodiscard]] std::string sha256Hex(std::string_view data);

/// Wraps an item payload in a signed envelope:
///   { "payload": <payload>, "hash": "<sha256 of payload.dump()>" }
[[nodiscard]] nlohmann::json signItem(const nlohmann::json& payload);

/// Verifies a signed envelope and returns the payload when the hash matches.
/// Returns std::nullopt when the envelope is malformed or was tampered with.
[[nodiscard]] std::optional<nlohmann::json>
verifyItem(const nlohmann::json& envelope);

/// Parses the serialized UUID from a verified item payload. Returns
/// std::nullopt when the "uuid" field is absent, is not a string, or is not a
/// valid UUID.
[[nodiscard]] std::optional<UUID> parseUuid(const nlohmann::json& payload);

/// Validates the fields shared by every item payload.
[[nodiscard]] bool hasValidCommonFields(const nlohmann::json& payload);

} // namespace item

#pragma once

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>

namespace item
{

enum class Status : int
{
  NOT_OWNDED,
  OWNED,
  EQUIPPED
};

[[nodiscard]] static constexpr const char* statusToString(Status status)
{
  switch (status)
  {
    case Status::NOT_OWNDED:
      return "NOT_OWNDED";
    case Status::OWNED:
      return "OWNED";
    case Status::EQUIPPED:
      return "EQUIPPED";
  }
  return "";
}

[[nodiscard]] inline std::optional<Status> statusFromString(std::string_view s)
{
  if (s.empty())
    return std::nullopt;

  std::string lower(s);
  std::transform(lower.begin(), lower.end(), lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (lower == "not_ownded")
    return Status::NOT_OWNDED;
  else if (lower == "owned")
    return Status::OWNED;
  else if (lower == "equipped")
    return Status::EQUIPPED;
  return std::nullopt;
}

} // namespace item

#pragma once

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

} // namespace item

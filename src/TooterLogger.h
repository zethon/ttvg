#pragma once
#include <memory>

#include <spdlog/spdlog.h>

namespace tt
{

namespace log
{

constexpr auto GLOBAL_LOGGER = "tt";

using SpdLogPtr = std::shared_ptr<spdlog::logger>;

[[maybe_unused]] SpdLogPtr rootLogger();
SpdLogPtr initializeLogger(const std::string& name);

} // namespace log

} // namespace tt
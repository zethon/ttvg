#pragma once
#include <memory>

#include <spdlog/spdlog.h>

namespace ttvg
{

constexpr auto GLOBAL_LOGGER = "ttvg";

using SpdLogPtr = std::shared_ptr<spdlog::logger>;

[[maybe_unused]] SpdLogPtr rootLogger();
SpdLogPtr initializeLogger(const std::string& name);

} // namespace
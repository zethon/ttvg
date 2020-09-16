#include <spdlog/sinks/stdout_color_sinks.h>
#include "TooterLogger.h"

namespace ttvg
{

[[maybe_unused]] SpdLogPtr rootLogger()
{
    SpdLogPtr root = spdlog::get(GLOBAL_LOGGER);

    if (!root)
    {
        root = spdlog::stdout_color_mt(GLOBAL_LOGGER);

#ifdef RELEASE
        spdlog::set_level(spdlog::level::off);
#else
        spdlog::set_level(spdlog::level::trace);
#endif
    }

    return root;
}

SpdLogPtr initializeLogger(const std::string& name)
{
    rootLogger();

    SpdLogPtr logger = spdlog::get(name);
    if (!logger)
    {
        logger = spdlog::get(GLOBAL_LOGGER)->clone(name);
        spdlog::register_logger(logger);
    }

    return logger;
}

} // namespace

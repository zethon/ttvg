// Ash Crypto
// Copyright (c) 2017-2020, Adalid Claure <aclaure@gmail.com>

#include <fstream>
#include <iomanip>

#include <boost/filesystem.hpp>

#include "Settings.h"

namespace amb
{

inline std::string json_to_string(const nlohmann::json& j)
{
    if (j.type() == nlohmann::json::value_t::string) 
    {
        return j.get<std::string>();
    }

    return j.dump();
}

std::size_t Settings::load()
{
    return load(_currentFile);
}

std::size_t Settings::load(std::string_view filename)
{
    if (filename.empty())
    {
        throw std::runtime_error("cannot load empty settings file");
    }

    _currentFile = filename;

    std::size_t count = 0;

    boost::filesystem::path file{ filename.data() };

    if (boost::filesystem::exists(file))
    {
        nlohmann::json settings;
        std::ifstream in(filename.data());
        in >> settings;
        in.close();

        for (const auto& item : settings.items())
        {
            if (const auto key = item.key();
                _settings.find(key) == _settings.end())
            {
                throw std::invalid_argument(
                    fmt::format("unregistered config value '{}'", item.key()));
            }
            else
            {
                if (!_validators[key]
                    || _validators[key]->isValid(json_to_string(item.value())))
                {
                    _settings[key] = item.value();
                    count++;
                }
                else
                {
                    throw std::invalid_argument(
                        fmt::format("error loading config item '{}': invalid value '{}'", key, item.value().get<std::string>()));
                }
            }
        }
    }

    return count;
}

void Settings::save()
{
    return save(_currentFile);
}

void Settings::save(std::string_view filename)
{
    if (filename.empty())
    {
        throw std::runtime_error("cannot save empty settings file");
    }

    _currentFile = filename;

    std::ofstream out;
    out.open(filename.data(),
        std::ofstream::out | std::ofstream::trunc);

    out << std::setw(4) << _settings;
    out.close();
}

bool Settings::exists(const std::string & name) const
{
    return _settings.find(name) != _settings.end();
}

void Settings::registerString(const std::string & name, const std::string & value, std::shared_ptr<Validator> val)
{
    if (val)
    {
        val->validate(value);
    }

    _validators[name] = val;
    _settings[name] = value;
    _defaults[name] = value;
}

void Settings::registerBool(const std::string & name, bool value)
{
    _validators[name] = std::make_shared<BoolValidator>();
    _settings[name] = value;
    _defaults[name] = value;
}

void Settings::registerEnum(const std::string & name, const std::string & value, const std::vector<std::string>& possibleVals)
{
    auto validator = std::make_shared<EnumValidator>(possibleVals);
    validator->validate(value);
    _validators[name] = validator;
    _settings[name] = value;
    _defaults[name] = value;
}

void Settings::set(const std::string& name, const std::string& value)
{
    if (_settings.find(name) == _settings.end())
    {
        throw std::invalid_argument(fmt::format("setting '{}' has not been registered", name));
    }

    _validators[name]->validate(value);

    if (_settings[name].is_number_unsigned())
    {
        _settings[name] = boost::lexical_cast<std::uint32_t>(value);
    }
    else if (_settings[name].is_number_integer())
    {
        _settings[name] = boost::lexical_cast<std::int32_t>(value);
    }
    else if (_settings[name].is_boolean())
    {
        _settings[name] = utils::convertToBool(value);
    }
    else if (_settings[name].is_string())
    {
        _settings[name] = value;
    }
    else
    {
        throw std::invalid_argument(fmt::format("unknown setting type '{}'", name));
    }
}

bool Settings::setNoThrow(const std::string& name, const std::string& value)
{
    if (_settings.find(name) == _settings.end())
    {
        return false;
    }

    if (_settings[name].is_number_unsigned())
    {
        if (_validators[name]->isValid(value))
        {
            _settings[name] = boost::lexical_cast<std::uint32_t>(value);
            return true;
        }
    }
    else if (_settings[name].is_number_integer())
    {
        if (_validators[name]->isValid(value))
        {
            _settings[name] = boost::lexical_cast<std::int32_t>(value);
            return true;
        }
    }
    else if (_settings[name].is_boolean())
    {
        if (_validators[name]->isValid(value))
        {
            _settings[name] = utils::convertToBool(value);
            return true;
        }
    }
    else if (_settings[name].is_string())
    {
        if (_validators[name]->isValid(value))
        {
            _settings[name] = value;
            return true;
        }
    }
    else
    {
        throw std::invalid_argument(fmt::format("unknown setting type '{}'", name));
    }

    return false;
}

std::uint32_t Settings::value(const std::string & name, std::uint32_t defval)
{
    if (_settings.find(name) == _settings.end()
        || !_settings[name].is_number_unsigned())
    {
        return defval;
    }

    return _settings[name].get<std::uint32_t>();
}

std::int32_t Settings::value(const std::string & name, std::int32_t defval)
{
    if (_settings.find(name) == _settings.end()
        || !_settings[name].is_number_integer())
    {
        return defval;
    }

    return _settings[name].get<std::int32_t>();
}

bool Settings::value(const std::string & name, bool defval)
{
    if (_settings.find(name) == _settings.end()
        || !_settings[name].is_boolean())
    {
        return defval;
    }

    return _settings[name].get<bool>();
}

std::string Settings::value(const std::string & name, std::string defval)
{
    if (_settings.find(name) == _settings.end()
        || !_settings[name].is_string())
    {
        return defval;
    }

    return _settings[name].get<std::string>();
}

void Settings::reset()
{
    _settings.clear();

    for (const auto& defval : _defaults.items())
    {
        _settings[defval.key()] = _defaults[defval.key()];
    }
}

} // namespace

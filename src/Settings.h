// Ash Crypto
// Copyright (c) 2017-2020, Adalid Claure <aclaure@gmail.com>

#pragma once

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/range/join.hpp>

#include <nlohmann/json.hpp>
#include <fmt/core.h>

using namespace std::string_literals;

namespace amb
{

namespace utils
{

static const std::vector<std::string> trueStrings = { "true"s, "on"s, "1"s };
static const std::vector<std::string> falseStrings = { "false"s, "off"s, "0"s };

inline bool isBoolean(const std::string& s)
{
    auto temp = boost::range::join(trueStrings, falseStrings);
    return std::find_if(std::begin(temp), std::end(temp),
        [s](const std::string& val) -> bool
        {
            return boost::iequals(val,s);
        })
        != std::end(temp);
}

inline bool convertToBool(const std::string_view s)
{
    if (std::find_if(
            std::begin(trueStrings),
            std::end(trueStrings),
            [&s](const std::string& data)
            {
                return boost::iequals(data, s);
            }) != std::end(trueStrings))
    {
        return true;
    }
    else if (std::find_if(
            std::begin(falseStrings),
            std::end(falseStrings),
            [&s](const std::string& data)
            {
                return boost::iequals(data, s);
            }) != std::end(falseStrings))
    {
        return false;
    }

    throw std::runtime_error(fmt::format("invalid value '{}'", s));
}

} // namespace utils

class Settings;
using SettingsPtr = std::shared_ptr<Settings>;

class Validator
{
public:

    virtual bool isValid(const std::string& value) = 0;
    virtual std::string error(const std::string& value) const = 0;

    virtual void validate(const std::string& value)
    {
        if (!isValid(value))
        {
            throw std::invalid_argument(error(value));
        }
    }


    virtual ~Validator() = default;
};

class NotEmptyValidator : public Validator 
{
public:
    bool isValid(const std::string& value) override
    {
        return !value.empty();
    }

    std::string error(const std::string& value) const override
    {
        return "the setting cannot be empty"s;
    }
};

class LengthValidator : public Validator
{
    std::size_t _maxlen;

public:
    LengthValidator(std::size_t maxlen)
        : _maxlen{ maxlen }
    {}

    virtual ~LengthValidator() = default;

    bool isValid(const std::string& value) override
    {
        return value.size() <= _maxlen;
    }

    std::string error(const std::string& value) const override
    {
        return fmt::format("the value '{}' exceeds the max legnth '{}'", value, _maxlen);
    }
};

template<typename T>
class NumberValidator : public Validator
{
public:
    virtual bool isValid(const std::string& value) override
    {
        bool retval = false;

        try
        {
            boost::lexical_cast<T>(value);
            retval = true;
        }
        catch (const boost::bad_lexical_cast&)
        {
            return false;
        }

        return retval;
    }

    std::string error(const std::string& value) const override
    {
        return fmt::format("the value '{}' is not a numeric value");
    }
};

template<typename T>
class RangeValidator : public NumberValidator<T>
{
    T    _minval;
    T    _maxval;

public:
    RangeValidator(T min, T max)
        : _minval{ min },
          _maxval{ max }
    {}

    virtual bool isValid(const std::string& value) override
    {
        bool retval = false;

        try
        {
            auto x = boost::lexical_cast<T>(value);
            retval = _minval <= x && x <= _maxval;
        }
        catch (const boost::bad_lexical_cast&)
        {
            return false;
        }

        return retval;
    }

    void validate(const std::string& value) override
    {
        if (!NumberValidator<T>::isValid(value))
        {
            throw std::invalid_argument(NumberValidator<T>::error(value));
        }
        else if (!isValid(value))
        {
            throw std::invalid_argument(this->error(value));
        }
    }

    std::string error(const std::string& value) const override
    {
        return fmt::format("the value '{}' falls outside the range {}-{}", 
            value, _minval, _maxval);
    }
};

class BoolValidator : public Validator
{

public:

    bool isValid(const std::string& value) override
    {
        return utils::isBoolean(value);
    }

    std::string error(const std::string& value) const override
    {
        return fmt::format("the value '{}' is not a boolean value", value);
    }
};

class EnumValidator : public Validator
{
    std::vector<std::string>    _values;

public:

    EnumValidator(const std::vector<std::string>& vals)
        : _values{ vals } // copying a vector!!
    {}

    bool isValid(const std::string& value) override
    {
        return std::find_if(std::begin(_values), std::end(_values),
            [&value](const std::string& val) -> bool
            {
                return boost::iequals(val, value);
            })
            != std::end(_values);
    }

    std::string error(const std::string& value) const override
    {
        return fmt::format("invalid value '{}', possible values are: {}",
            value, boost::algorithm::join(_values,","));
    }
};

class Settings final
{
    using Validators = std::map<std::string, std::shared_ptr<Validator>>;

    nlohmann::json  _settings;
    nlohmann::json  _defaults;
    Validators      _validators;
    std::string     _currentFile;

public:

    auto begin() const -> decltype(_settings.items().begin())
    {
        return _settings.items().begin();
    }

    auto end() const -> decltype(_settings.items().begin())
    {
        return _settings.items().end();
    }

    std::size_t load();
    std::size_t load(std::string_view filename);

    void save();
    void save(std::string_view filename);

    std::string filename() const
    {
        return _currentFile;
    }

    bool exists(const std::string& name) const;
    std::size_t size() const { return _settings.size(); }

    void registerString(const std::string& name, const std::string& value, 
        std::shared_ptr<Validator> val = nullptr);

    template<typename T>
    void registerNumber(const std::string& name, T value,
        std::shared_ptr<Validator> val = nullptr)
    {
        if (val)
        {
            _validators[name] = val;
        }
        else
        {
            _validators[name] = std::make_shared<NumberValidator<T>>();
        }

        _settings[name] = value;
        _defaults[name] = value;
    }

    void registerInt(const std::string& name, std::int32_t value,
        std::shared_ptr<Validator> val = nullptr)
    {
        registerNumber<std::int32_t>(name, value, val);
    }

    void registerUInt(const std::string& name, std::uint32_t value,
        std::shared_ptr<Validator> val = nullptr)
    {
        registerNumber<std::uint32_t>(name, value, val);
    }

    void registerBool(const std::string& name, bool value);

    void registerEnum(const std::string& name, const std::string& value,
        const std::vector<std::string>& possibleVals);

    void set(const std::string& name, const std::string& value);
    bool setNoThrow(const std::string& name, const std::string& value);

    std::uint32_t value(const std::string& name, std::uint32_t defval);
    std::int32_t value(const std::string& name, std::int32_t defval);
    bool value(const std::string& name, bool defval);

    std::string value(const std::string& name, std::string defval);
    std::string value(const std::string& name, const char* defval)
    {
        return value(name, std::string{ defval });
    }

    void reset();
};

} // namespace
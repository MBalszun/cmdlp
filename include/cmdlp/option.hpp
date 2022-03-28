/// @file option.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#pragma once

#include <string>

namespace cmdlp
{

class Option {
public:
    /// The character that identifies the option.
    std::string optc;
    /// The entire word for the option.
    std::string opts;
    /// Description of the option.
    std::string description;

    Option(const std::string &_optc,
           const std::string &_opts,
           const std::string &_description)
        : optc(_optc),
          opts(_opts),
          description(_description)
    {
        // Nothing to do.
    }

    virtual unsigned get_value_length() const = 0;

    virtual ~Option()
    {
        // Nothing to do.
    }
};

class ToggleOption : public Option {
public:
    /// If the option is toggled.
    bool toggled;

    ToggleOption(const std::string &_optc,
                 const std::string &_opts,
                 const std::string &_description,
                 bool _toggled)
        : Option(_optc, _opts, _description),
          toggled(_toggled)
    {
        // Nothing to do.
    }

    virtual ~ToggleOption()
    {
        // Nothing to do.
    }

    virtual unsigned get_value_length() const
    {
        return 5;
    }
};

class ValueOption : public Option {
public:
    /// The actual value.
    std::string value;
    /// The option is required.
    bool required;

    ValueOption(const std::string &_optc,
                const std::string &_opts,
                const std::string &_description,
                const std::string &_value,
                bool _required)
        : Option(_optc, _opts, _description),
          value(_value),
          required(_required)
    {
        // Nothing to do.
    }

    virtual ~ValueOption()
    {
        // Nothing to do.
    }

    virtual unsigned get_value_length() const
    {
        return value.size();
    }
};

} // namespace cmdlp

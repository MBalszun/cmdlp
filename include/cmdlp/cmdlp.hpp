/// @file command_line_parser.hpp
/// @date 26/08/2021

#pragma once

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace cmdlp
{
class LineParser {
private:
    std::vector<std::string> tokens;

public:
    LineParser(int &argc, char **argv)
    {
        for (int i = 1; i < argc; ++i) {
            tokens.emplace_back(std::string(argv[i]));
        }
    }

    /// @brief Retrieves the given option.
    /// @param option The option to search.
    /// @return The value for the given option.
    inline const std::string &getOption(const std::string &option) const
    {
        auto it = std::find(tokens.begin(), tokens.end(), option);
        if ((it != tokens.end()) && (std::next(it) != tokens.end())) {
            ++it;
            if (!__starts_with(*it, "-")) {
                return (*it);
            }
        }
        static const std::string empty_string;
        return empty_string;
    }

    /// @brief Retrieves the given option.
    /// @param option The option to search.
    /// @return The value for the given option.
    inline const std::string &getOption(char option) const
    {
        static char sopt[2];
        sopt[0] = option;
        sopt[1] = '\0';
        return this->getOption(sopt);
    }

    /// @brief Checks if the given option exists.
    /// @param option The option to search.
    /// @return If the option exists.
    inline bool hasOption(const std::string &option) const
    {
        return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
    }

    /// @brief Checks if the given option exists.
    /// @param option The option to search.
    /// @return If the option exists.
    inline bool hasOption(char option) const
    {
        std::string strOption;
        strOption.push_back('-');
        strOption.push_back(option);
        return this->hasOption(strOption);
    }

private:
    /// @brief Checks if the given string s starts with the given prefix.
    /// @param source    the source string.
    /// @param prefix    the prefix to check.
    /// @param sensitive turn on/off sensitivity for capits/non-capitals
    /// @param n         specify the minimum number of characters to check.
    /// @return true  if the source starts with the prefix, give all the conditions.
    /// @return false if the source does not start with the prefix, give all the conditions.
    static inline bool __starts_with(
        const std::string &source,
        const std::string &prefix,
        bool sensitive = false,
        int n          = -1)
    {
        if (&prefix == &source) {
            return true;
        }
        if (prefix.length() > source.length()) {
            return false;
        }
        if (source.empty() || prefix.empty()) {
            return false;
        }
        std::string::const_iterator it0 = source.begin(), it1 = prefix.begin();
        if (sensitive) {
            while ((it1 != prefix.end()) && ((*it1) == (*it0))) {
                if ((n > 0) && (--n <= 0)) {
                    return true;
                }
                ++it0, ++it1;
            }
        } else {
            while ((it1 != prefix.end()) && (tolower(*it1) == tolower(*it0))) {
                if ((n > 0) && (--n <= 0)) {
                    return true;
                }
                ++it0, ++it1;
            }
        }
        return it1 == prefix.end();
    }
};

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

    virtual ~Option() = default;
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

    virtual ~ToggleOption() = default;
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

    virtual ~ValueOption() = default;
};

class OptionParser {
public:
    OptionParser(int &argc, char **argv)
        : _parser(argc, argv),
          _options(),
          _optionParsed()
    {
        _options.emplace_back(new ToggleOption("-h", "--help", "Shows this help for the program.", false));
    }

    OptionParser(OptionParser &&other) = default;

    virtual ~OptionParser()
    {
        for (auto option : _options) {
            delete option;
        }
    }

    void addOption(char optc,
                   const std::string &opts,
                   const std::string &_description = "",
                   const std::string &_value       = "",
                   bool _required                  = false)
    {
        auto __optc = this->optc_to_string(optc);
        for (auto &it : _options) {
            if (it->optc == __optc) {
                std::cerr << "Duplicate option in (addOption) : " << it->optc << "[" << it->opts << "]\n";
                std::exit(1);
            }
            if (it->opts == opts) {
                std::cerr << "Duplicate option in (addOption) : " << it->opts << "[" << it->optc << "]\n";
                std::exit(1);
            }
        }
        _options.emplace_back(new ValueOption(__optc, opts, _description, _value, _required));
    }

    template <typename T>
    void addOption(char optc,
                   const std::string &opts,
                   const std::string &_description,
                   const T &_value,
                   bool _required = false)
    {
        std::stringstream ss;
        ss << _value;
        this->addOption(optc, opts, _description, ss.str(), _required);
    }

    void parseOptions()
    {
        std::string value;
        for (auto option : _options) {
            auto vopt = dynamic_cast<ValueOption *>(option);
            if (vopt) {
                // Try to search '-*'
                if (!(value = _parser.getOption(vopt->optc)).empty()) {
                    vopt->value = value;
                }
                // Try to search '--***'
                else if (!(value = _parser.getOption(vopt->opts)).empty()) {
                    vopt->value = value;
                }
                // If we did not find the option, and the option is required, print an error.
                else if (vopt->required) {
                    std::cerr << "Cannot find required option : " << vopt->opts << "[" << vopt->optc << "]\n";
                    std::cerr << this->getHelp() << "\n";
                    std::exit(1);
                }
            } else {
                auto topt = dynamic_cast<ToggleOption *>(option);
                if (topt) {
                    // Try to search '-*'
                    if (_parser.hasOption(topt->optc)) {
                        topt->toggled = true;
                    }
                    // Try to search '--***'
                    else if (_parser.hasOption(topt->opts)) {
                        topt->toggled = true;
                    }
                }
            }
        }
        _optionParsed = true;
    }

    template <typename T>
    T getOption(const std::string &opts) const
    {
        auto option = this->findOption(opts);
        if (option) {
            auto vopt = dynamic_cast<ValueOption *>(option);
            auto topt = dynamic_cast<ToggleOption *>(option);
            std::stringstream ss;
            if (vopt) {
                ss << vopt->value;
            } else if (topt) {
                ss << topt->toggled;
            }
            T data;
            ss >> data;
            return data;
        }
        return T(0);
    }

    template <typename T>
    T getOption(char optc) const
    {
        auto option = this->findOption(optc);
        if (option) {
            auto vopt = dynamic_cast<ValueOption *>(option);
            auto topt = dynamic_cast<ToggleOption *>(option);
            std::stringstream ss;
            if (vopt) {
                ss << vopt->value;
            } else if (topt) {
                ss << topt->toggled;
            }
            T data;
            ss >> data;
            return data;
        }
        return T(0);
    }

    /// @brief Checks if the given option exists.
    /// @param option The option to search.
    /// @return If the option exists.
    inline bool hasOption(const std::string &opts) const
    {
        auto option = this->findOption(opts);
        if (option) {
            auto vopt = dynamic_cast<ValueOption *>(option);
            if (vopt) {
                return !vopt->value.empty();
            }
            auto topt = dynamic_cast<ToggleOption *>(option);
            if (topt) {
                return topt->toggled;
            }
        }
        return false;
    }

    /// @brief Checks if the given option exists.
    /// @param option The option to search.
    /// @return If the option exists.
    inline bool hasOption(char optc) const
    {
        auto option = this->findOption(optc);
        if (option) {
            auto vopt = dynamic_cast<ValueOption *>(option);
            if (vopt) {
                return !vopt->value.empty();
            }
            auto topt = dynamic_cast<ToggleOption *>(option);
            if (topt) {
                return topt->toggled;
            }
        }
        return false;
    }

    std::string getHelp() const
    {
        size_t longest_option = 0, longest_value = 0;
        for (auto option : _options) {
            // Compute the longest option.
            auto option_length = option->opts.size();
            if (longest_option < option_length) {
                longest_option = option_length;
            }
            // Compute the longest value.
            auto vopt = dynamic_cast<ValueOption *>(option);
            if (vopt) {
                auto value_length = vopt->value.length();
                if (longest_value < value_length) {
                    longest_value = value_length;
                }
            } else {
                auto topt = dynamic_cast<ToggleOption *>(option);
                if (topt) {
                    if (longest_value < 4) {
                        longest_value = 4;
                    }
                }
            }
        }
        // Print the arguments.
        std::stringstream ss;
        for (auto option : _options) {
            ss << "    ";
            ss << "[" << option->optc << "] ";
            ss << std::setw(longest_option) << std::left << option->opts;
            ss << " (" << std::setw(longest_value) << std::right;
            auto vopt = dynamic_cast<ValueOption *>(option);
            if (vopt) {
                ss << vopt->value;
            } else {
                auto topt = dynamic_cast<ToggleOption *>(option);
                if (topt) {
                    ss << (topt->toggled ? "true" : "false");
                }
            }
            ss << ") : ";
            ss << option->description << "\n";
        }
        return ss.str();
    }

private:
    static inline std::string &optc_to_string(char c)
    {
        static std::string optcs = "-*";
        optcs[1]                 = c;
        return optcs;
    }

    Option *findOption(char optc) const
    {
        assert(_optionParsed && "You must parse the options first.");
        auto __optc = this->optc_to_string(optc);
        for (auto option : _options) {
            if (option->optc == __optc) {
                return option;
            }
        }
        return nullptr;
    }

    Option *findOption(const std::string &opts) const
    {
        assert(_optionParsed && "You must parse the options first.");
        for (auto option : _options) {
            if (option->opts == opts) {
                return option;
            }
        }
        return nullptr;
    }

    LineParser _parser;
    std::vector<Option *> _options;
    bool _optionParsed;
};

template <>
std::string OptionParser::getOption(const std::string &opts) const
{
    auto option = this->findOption(opts);
    if (option) {
        auto vopt = dynamic_cast<ValueOption *>(option);
        if (vopt)
            return vopt->value;
        auto topt = dynamic_cast<ToggleOption *>(option);
        if (topt)
            return topt->toggled ? "true" : "false";
    }
    return "";
}

template <>
std::string OptionParser::getOption(char optc) const
{
    auto option = this->findOption(optc);
    if (option) {
        auto vopt = dynamic_cast<ValueOption *>(option);
        if (vopt)
            return vopt->value;
        auto topt = dynamic_cast<ToggleOption *>(option);
        if (topt)
            return topt->toggled ? "true" : "false";
    }
    return "";
}

} // namespace cmdlp
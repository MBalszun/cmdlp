/// @file cmdlp.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#pragma once

#include "line_parser.hpp"
#include "option.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>

namespace cmdlp
{
class OptionParser {
public:
    typedef std::vector<Option *> option_list_t;
    typedef std::vector<Option *>::iterator iterator_t;
    typedef std::vector<Option *>::const_iterator const_iterator_t;

    OptionParser(int &argc, char **argv)
        : _parser(argc, argv),
          _options(),
          _optionParsed(),
          _longest_option(),
          _longest_value()
    {
        _options.push_back(new ToggleOption("-h", "--help", "Shows this help for the program.", false));
    }

    virtual ~OptionParser()
    {
        for (iterator_t it = _options.begin(); it != _options.end(); ++it) {
            delete *it;
        }
    }

    inline void addOption(Option *option)
    {
        for (iterator_t it = _options.begin(); it != _options.end(); ++it) {
            if ((*it)->optc == option->optc) {
                std::cerr << "Duplicate option : " << (*it)->optc << "[" << (*it)->opts << "]\n";
                std::exit(1);
            }
            if ((*it)->opts == option->opts) {
                std::cerr << "Duplicate option : " << (*it)->opts << "[" << (*it)->optc << "]\n";
                std::exit(1);
            }
        }
        _options.push_back(option);
        if (option->opts.length() > _longest_option) {
            _longest_option = option->opts.length();
        }
        if (option->get_value_length() > _longest_value) {
            _longest_value = option->get_value_length();
        }
    }

    template <typename T>
    void addVOption(char optc,
                    const std::string &opts,
                    const std::string &_description,
                    const T &_value,
                    bool _required)
    {
        std::stringstream ss;
        ss << _value;
        this->addOption(new ValueOption(this->optc_to_string(optc), opts, _description, ss.str(), _required));
    }

    void addTOption(char optc,
                    const std::string &opts,
                    const std::string &_description,
                    bool _toggled)
    {
        this->addOption(new ToggleOption(this->optc_to_string(optc), opts, _description, _toggled));
    }

    void parseOptions()
    {
        ValueOption *vopt;
        ToggleOption *topt;
        std::string value;
        for (iterator_t it = _options.begin(); it != _options.end(); ++it) {
            if ((vopt = dynamic_cast<ValueOption *>(*it))) {
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
            } else if ((topt = dynamic_cast<ToggleOption *>(*it))) {
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
        _optionParsed = true;
    }

    template <typename T>
    T getOption(const std::string &opts) const
    {
        Option *option = this->findOption(opts);
        if (option) {
            ValueOption *vopt  = dynamic_cast<ValueOption *>(option);
            ToggleOption *topt = dynamic_cast<ToggleOption *>(option);
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
        Option *option = this->findOption(optc);
        if (option) {
            ValueOption *vopt  = dynamic_cast<ValueOption *>(option);
            ToggleOption *topt = dynamic_cast<ToggleOption *>(option);
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
        Option *option = this->findOption(opts);
        if (option) {
            ValueOption *vopt = dynamic_cast<ValueOption *>(option);
            if (vopt) {
                return !vopt->value.empty();
            }
            ToggleOption *topt = dynamic_cast<ToggleOption *>(option);
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
        Option *option = this->findOption(optc);
        if (option) {
            ValueOption *vopt = dynamic_cast<ValueOption *>(option);
            if (vopt) {
                return !vopt->value.empty();
            }
            ToggleOption *topt = dynamic_cast<ToggleOption *>(option);
            if (topt) {
                return topt->toggled;
            }
        }
        return false;
    }

    std::string getHelp() const
    {
        ValueOption *vopt;
        ToggleOption *topt;
        // Print the arguments.
        std::stringstream ss;
        for (const_iterator_t it = _options.begin(); it != _options.end(); ++it) {
            ss << "    ";
            ss << "[" << (*it)->optc << "] ";
            ss << std::setw(_longest_option) << std::left << (*it)->opts;
            ss << " (" << std::setw(_longest_value) << std::right;
            if ((vopt = dynamic_cast<ValueOption *>(*it))) {
                ss << vopt->value;
            } else if ((topt = dynamic_cast<ToggleOption *>(*it))) {
                ss << (topt->toggled ? "true" : "false");
            }
            ss << ") : ";
            ss << (*it)->description << "\n";
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
        if (!_optionParsed) {
            std::cerr << "You must parse the options first.\n";
            std::exit(1);
        }
        std::string __optc = this->optc_to_string(optc);
        for (const_iterator_t it = _options.begin(); it != _options.end(); ++it) {
            if ((*it)->optc == __optc) {
                return (*it);
            }
        }
        return NULL;
    }

    Option *findOption(const std::string &opts) const
    {
        if (!_optionParsed) {
            std::cerr << "You must parse the options first.\n";
            std::exit(1);
        }
        for (const_iterator_t it = _options.begin(); it != _options.end(); ++it) {
            if ((*it)->opts == opts) {
                return (*it);
            }
        }
        return NULL;
    }

    LineParser _parser;
    option_list_t _options;
    bool _optionParsed;
    unsigned _longest_option;
    unsigned _longest_value;
};

template <>
std::string OptionParser::getOption(const std::string &opts) const
{
    Option *option = this->findOption(opts);
    if (option) {
        ValueOption *vopt = dynamic_cast<ValueOption *>(option);
        if (vopt)
            return vopt->value;
        ToggleOption *topt = dynamic_cast<ToggleOption *>(option);
        if (topt)
            return topt->toggled ? "true" : "false";
    }
    return "";
}

template <>
std::string OptionParser::getOption(char optc) const
{
    Option *option = this->findOption(optc);
    if (option) {
        ValueOption *vopt = dynamic_cast<ValueOption *>(option);
        if (vopt)
            return vopt->value;
        ToggleOption *topt = dynamic_cast<ToggleOption *>(option);
        if (topt)
            return topt->toggled ? "true" : "false";
    }
    return "";
}

} // namespace cmdlp
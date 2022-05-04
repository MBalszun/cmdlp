/// @file option_list.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#pragma once

#include "option.hpp"

#include <exception>
#include <sstream>
#include <utility>

namespace cmdlp
{
class OptionExistException : public std::exception {
private:
    std::string msg;

public:
    OptionExistException(const Option &_new_option, const Option &_existing_option)
        : std::exception()
    {
        std::stringstream ss;
        ss << "Option (" << _new_option.optc << ", " << _new_option.opts << ") already exists: "
           << "(" << _existing_option.optc << ", " << _existing_option.opts << ")\n";
        msg = ss.str();
    }

    const char *what() const throw()
    {
        return msg.c_str();
    }
};

class OptionList {
public:
    typedef std::vector<std::unique_ptr<Option>> option_list_t;
    typedef std::vector<std::unique_ptr<Option>>::iterator iterator_t;
    typedef std::vector<std::unique_ptr<Option>>::const_iterator const_iterator_t;

    /// @brief Construct a new Option List object.
    OptionList()
        : options(),
          longest_option(),
          longest_value()
    {
    }

    OptionList(const OptionList &other)
        : options(),
          longest_option(other.longest_option),
          longest_value(other.longest_value)
    {
        for (const auto &option : other.options) {
            this->options.push_back(option->clone());
        }
    }

    OptionList(OptionList &&other) noexcept
        : options{ std::move(other.options) },
          longest_option{ std::exchange(other.longest_option, 0) },
          longest_value{ std::exchange(other.longest_value, 0) }
    {
    }

    // both move and copy assignment
    OptionList &operator=(OptionList other)
    {
        this->swap(other);
        return *this;
    }

    void swap(OptionList &other)
    {
        std::swap(this->options, other.options);
        std::swap(this->longest_option, other.longest_option);
        std::swap(this->longest_value, other.longest_value);
    }

    virtual ~OptionList() = default;

    inline bool optionExhists(Option *option) const
    {
        return this->optionExhists(option->optc, option->opts);
    }

    inline bool optionExhists(char optc) const
    {
        return this->optionExhists(cmdlp::optc_to_string(optc), "");
    }

    inline bool optionExhists(const std::string &opts) const
    {
        return this->optionExhists("", opts);
    }

    inline Option *findOption(char optc) const
    {
        return this->findOption(cmdlp::optc_to_string(optc), "");
    }

    inline Option *findOption(const std::string &opts) const
    {
        return this->findOption("", opts);
    }

    template <typename T>
    inline T getOption(Option *option) const
    {
        return this->getOption<T>(option->optc, option->opts);
    }

    template <typename T>
    inline T getOption(char optc) const
    {
        return this->getOption<T>(cmdlp::optc_to_string(optc), "");
    }

    template <typename T>
    inline T getOption(const std::string &opts) const
    {
        return this->getOption<T>("", opts);
    }

    inline void addOption(std::unique_ptr<Option> option)
    {
        for (const auto &opt_ptr : options) {
            if (opt_ptr->optc == option->optc)
                throw OptionExistException(*option, *opt_ptr);
            if (opt_ptr->opts == option->opts)
                throw OptionExistException(*option, *opt_ptr);
        }
        if (option->opts.length() > longest_option)
            longest_option = option->opts.length();
        if (option->get_value_length() > longest_value)
            longest_value = option->get_value_length();
        options.push_back(std::move(option));
    }

    inline const_iterator_t begin() const
    {
        return options.begin();
    }

    inline const_iterator_t end() const
    {
        return options.end();
    }

    inline std::size_t getLongestOption() const
    {
        return longest_option;
    }

    inline std::size_t getLongestValue() const
    {
        return longest_value;
    }

    inline void updateLongestValue(std::size_t length)
    {
        if (length > longest_value)
            longest_value = length;
    }

private:
    inline bool optionExhists(const std::string &optc, const std::string &opts) const
    {
        return findOption(optc, opts) != nullptr;
    }

    Option *findOption(const std::string &optc, const std::string &opts) const
    {
        for (const auto &o : options) {
            if (!optc.empty() && o->optc == optc)
                return o.get();
            if (!opts.empty() && o->opts == opts)
                return o.get();
        }
        return nullptr;
    }

    template <typename T>
    T getOption(const std::string &optc, const std::string &opts) const
    {
        Option *option = this->findOption(optc, opts);
        if (option) {
            // Create a stringstream to parse the values.
            std::stringstream ss;
            // First try to check if it is a value option.
            ValueOption *vopt = dynamic_cast<ValueOption *>(option);
            if (vopt) {
                ss << vopt->value;
            } else {
                // Then, check if it is a toggle option.
                ToggleOption *topt = dynamic_cast<ToggleOption *>(option);
                if (topt) {
                    ss << topt->toggled;
                }
            }
            // Parse the data.
            T data;
            ss >> data;
            return data;
        }
        return T(0);
    }

    option_list_t options;
    std::size_t longest_option;
    std::size_t longest_value;
};

template <>
std::string OptionList::getOption(const std::string &optc, const std::string &opts) const
{
    Option *option = this->findOption(optc, opts);
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
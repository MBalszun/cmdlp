/// @file line_parser.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#pragma once

#include <algorithm>
#include <vector>
#include <string>

namespace cmdlp
{

class LineParser {
private:
    typedef std::vector<std::string> token_list_t;

    token_list_t tokens;

public:
    LineParser(int argc, char **argv)
    {
        for (int i = 1; i < argc; ++i) {
            tokens.push_back(std::string(argv[i]));
        }
    }

    /// @brief Retrieves the given option.
    /// @param option The option to search.
    /// @return The value for the given option.
    inline const std::string &getOption(const std::string &option) const
    {
        token_list_t::const_iterator it = std::find(tokens.begin(), tokens.end(), option);
        if ((it != tokens.end()) && (++it != tokens.end())) {
            if (!_starts_with(*it, "-")) {
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
    static inline bool _starts_with(
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

} // namespace cmdlp

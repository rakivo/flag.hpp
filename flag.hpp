#pragma once

#include <string>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <utility>
#include <optional>
#include <type_traits>

template <class T>
struct Flag {
    const char *shrt, *lng;
    const char *help = "[EMPTY]";
    bool mandatory = false;

    constexpr Flag(const char *shrt, const char *lng)
        : shrt(shrt),
          lng(lng)
    {};

    constexpr Flag(const char *shrt, const char *lng, const char *help)
        : shrt(shrt),
          lng(lng),
          help(help)
    {};

    constexpr Flag(const char *shrt, const char *lng, bool mandatory)
        : shrt(shrt),
          lng(lng),
          mandatory(mandatory)
    {};

    constexpr Flag(const char *shrt, const char *lng, const char *help, bool mandatory)
        : shrt(shrt),
          lng(lng),
          help(help),
          mandatory(mandatory)
    {};

    constexpr Flag(const char *shrt, const char *lng, bool mandatory, const char *help)
        : shrt(shrt),
          lng(lng),
          help(help),
          mandatory(mandatory)
    {};

    ~Flag() = default;
};

template <class T>
std::ostream&
operator<<(std::ostream& os, const Flag<T>& flag)
{
    const int HELP_WIDTH = 24;

    os << std::right;
    os << std::right
       << "[" << flag.shrt << ", " << flag.lng << "]"
       << std::setw(HELP_WIDTH - (std::strlen(flag.shrt) + std::strlen(flag.lng) + 4))
       << " " << flag.help;

    return os;
}

// Template for string types: `std::string` or `const char *`
template <class T, class = void>
struct is_string_or_cstr : std::false_type {};

template <class T>
struct is_string_or_cstr<
    T,
    std::enable_if_t<std::is_same<T, std::string>::value>
> : std::true_type {};

template <>
struct is_string_or_cstr<const char*> : std::true_type {};

// Template for integer types: `int`, `int32_t`, `uint32_t`, etc...
template <class T, class = void>
struct is_constructible_from_int : std::false_type {};

template <class T>
struct is_constructible_from_int<
    T,
    std::void_t<decltype(T { std::declval<int>() })>
> : std::true_type {};

struct Parser {
    size_t argc;
    const char *const *const argv;

    constexpr Parser(int argc, const char *const *argv)
        : argc(argc),
          argv(argv)
    {};

    [[noreturn]] static void
    mandatory_flag_hasnt_been_passed(const char *shrt, const char *lng)
    {
        std::cerr << "Mandatory flag `" << shrt
                  << "` or `"           << lng
                  << "` hasn't been passed" << std::endl;

        std::exit(1);
    }

    [[noreturn]] static void
    flag_hasnt_been_passed(const char *shrt, const char *lng)
    {
        std::cerr << "Flag `" << shrt
                  << "` or `" << lng
                  << "` hasn't been passed" << std::endl;

        std::exit(1);
    }

    [[noreturn]] static void
    failed_to_parse_flag_to_int(const char *shrt, const char *lng, const char *v)
    {
        std::cerr << "Failed to parse `" << v << "` to integer"
                  << ", value of the `"  << shrt
                  << "` or `"            << lng << "` flag." << std::endl;

        std::exit(1);
    }

    std::optional<const char *>
    parse_str(const char *shrt, const char *lng) const noexcept
    {
        for (size_t i = 0; i < this->argc; i++) {
            if (0 == std::strcmp(this->argv[i], shrt) or
                0 == std::strcmp(this->argv[i], lng))
            {
                if (i + 1 >= this->argc) return std::nullopt;
                return this->argv[i + 1];
            }
        }

        return std::nullopt;
    }

    inline const char *
    parse_str_or_exit(const char *shrt, const char *lng) const noexcept
    {
        const auto ret = this->parse_str(shrt, lng);
        if (ret == std::nullopt) flag_hasnt_been_passed(shrt, lng);
        return *ret;
    }

    template <class T>
    inline std::optional<class std::enable_if<is_string_or_cstr<T>::value>::type>
    parse(const Flag<T>& flag) const noexcept
    {
        return this->parse_str(flag.shrt, flag.lng);
    }

    template <class T>
    inline class std::enable_if<is_string_or_cstr<T>::value>::type
    parse(const Flag<T>& flag, T def) const noexcept
    {
        const auto ret_ = this->parse_str(flag.shrt, flag.lng);
        if (ret_ == std::nullopt) {
            if (flag.mandatory) mandatory_flag_hasnt_been_passed(flag.shrt, flag.lng);
            return def;
        }
        return *ret_;
    }

    template <class T>
    inline std::optional<class std::enable_if<std::is_floating_point<T>::value, T>::type>
    parse(const Flag<T>& flag) const noexcept
    {
        const auto ret = this->parse_str(flag.shrt, flag.lng);
        if (ret == std::nullopt) return std::nullopt;

        try {
            return std::stof(*ret);
        } catch (...) {
            return std::nullopt;
        }
    }

    template <class T>
    inline class std::enable_if<std::is_floating_point<T>::value, T>::type
    parse(const Flag<T>& flag, T def) const noexcept
    {
        const auto ret_ = this->parse_str(flag.shrt, flag.lng);
        if (ret_ == std::nullopt) {
            if (flag.mandatory) mandatory_flag_hasnt_been_passed(flag.shrt, flag.lng);
            return def;
        }

        const auto ret = *ret_;
        try {
            return std::stof(ret);
        } catch (...) {
            failed_to_parse_flag_to_int(flag.shrt, flag.lng, ret);
        }
    }

    template <class T>
    inline std::optional<std::enable_if_t<is_constructible_from_int<T>::value, T>>
    parse(const Flag<T>& flag) const noexcept
    {
        const auto ret = this->parse_str(flag.shrt, flag.lng);
        if (ret == std::nullopt) return std::nullopt;

        try {
            return std::stoi(*ret);
        } catch (...) {
            return std::nullopt;
        }
    }

    template <class T>
    inline std::enable_if_t<is_constructible_from_int<T>::value, T>
    parse(const Flag<T>& flag, T def) const noexcept
    {
        const auto ret_ = this->parse_str(flag.shrt, flag.lng);
        if (ret_ == std::nullopt) {
            if (flag.mandatory) mandatory_flag_hasnt_been_passed(flag.shrt, flag.lng);
            return def;
        }

        const auto ret = *ret_;
        try {
            return std::stoi(ret);
        } catch (...) {
            failed_to_parse_flag_to_int(flag.shrt, flag.lng, ret);
        }
    }
};

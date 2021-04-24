#pragma once
#include "for_each.hpp"
#include <string>

/// \arg enumOrEnumClass Shall be only one of the value:
///     - \code enum X \endcode
///     - \code enum class X \endcode
/// Where \code X \endcode may be any word.
/// In both cases, expand to \code X \endcode.
/// So, extract the type from the declaration.
#define {{ macro("EXTRACT_ENUM_TYPE_FROM_DECL") }}(enumOrEnumClass) {{ macro("REMOVE_CLASS_PREFIX_IF_NECESSARY") }}( \
    {{ macro("CAT") }}({{ macro("REMOVE_PREFIX_ENUM_") }}, enumOrEnumClass))


// Remove first the "enum" keyword
// both "enum" and "enum class" start with "enum"
#define {{ macro("REMOVE_PREFIX_ENUM_enum") }}

// Then, The argument of this macro x, may be "class X" or just "X".
// We can identify thanks to this macro to know if its either "class X" or just "X".
// If x is "class X", expands to 2 arguments (expression containing a comma).
// If x is just "X", expands to 1 argument.
#define {{ macro("REMOVE_CLASS_PREFIX_IF_NECESSARY") }}(classOrX) {{ macro("CAT") }}( \
        {{ macro("REMOVE_CLASS_PREFIX_IF_NECESSARY_ARGS_") }}, \
        {{ macro("N") }}({{ macro("CAT") }}( \
            {{ macro("IDENTIFY_PREFIX_CLASS_") }}, classOrX) \
        ) \
    )(classOrX)

#define {{ macro("REMOVE_PREFIX_CLASS_class") }}
#define {{ macro("IDENTIFY_PREFIX_CLASS_class") }} , // Comma acts like 2 arguments
#define {{ macro("REMOVE_CLASS_PREFIX_IF_NECESSARY_ARGS_2") }}(classX) {{ macro("CAT") }}({{ macro("REMOVE_PREFIX_CLASS_") }}, classX) // 2 arguments, it's a "class X"
#define {{ macro("REMOVE_CLASS_PREFIX_IF_NECESSARY_ARGS_1") }}(x) x // 1 argument, it's only "X".

template<typename Enum>
inline constexpr const char* get_enum_name();

/// \arg EnumName The name of the enum.
/// \arg ... The value of the enum, comma separated. May have default value with "= <value>".
///          The effective declaration order is the same as in an standard enum declaration.
///          So the effect of assigning with a value is the same in the standard as if the fields where declared in the same order.
///
/// \exemple
/// \code
/// enum class Key { A, B, C };
/// enum Day { Mon, Tue, Wed, Thu, Fri, Sat = -1, Sun = -2 };
/// \endcode
/// Is the same as
/// \code
/// {{ macro("ENUM") }}(enum class Key, A, B, C)
/// {{ macro("ENUM") }}(enum Day, Mon, Tue, Wed, Thu, Frid, Sat = -1, Sun = -2)
/// Can't use switch statement if there is multiple enums with same value
/// \endcode
#define OPP_ENUM(EnumName, ...) EnumName { \
        OPP_FOR_EACH(OPP_ENUM_APPEND_FIELD, __VA_ARGS__) \
    };                                     \
    inline const std::string& to_string(OPP_EXTRACT_ENUM_TYPE_FROM_DECL(EnumName) value) { \
        using enum_type = decltype(value);         \
        if(0) {}                                   \
        OPP_FOR_EACH(OPP_ENUM_TO_STRING_IF, __VA_ARGS__)                            \
        else { static const std::string name(OPP_INVALID_ENUM_STR); return name; };                              \
                                     \
    }                                      \
                                           \
    template<>                                       \
    inline constexpr const char* get_enum_name<OPP_EXTRACT_ENUM_TYPE_FROM_DECL(EnumName)>()    \
    { return OPP_STR(OPP_EXTRACT_ENUM_TYPE_FROM_DECL(EnumName)); }\
                                           \
    inline const char* get_long_name(OPP_EXTRACT_ENUM_TYPE_FROM_DECL(EnumName) value) {    \
        using namespace {{ macro("NAMESPACE") }};                                   \
        using enum_type = decltype(value); \
        using enum_name_metastring = decltype(OPP_CAT(OPP_STR(OPP_EXTRACT_ENUM_TYPE_FROM_DECL(EnumName)), _ms));                                   \
        if(0) {}                                   \
        OPP_FOR_EACH(OPP_ENUM_LONG_NAME_IF, __VA_ARGS__)                            \
        else { static const char *name(OPP_INVALID_ENUM_STR); return name; };                              \
    }                                      \
                                           \
    OPP_REQUIRE_SEMICOLON                                      \

#define OPP_ENUM_APPEND_FIELD(x) x,
#define OPP_ENUM_TO_STRING_IF(x) else if(value == ({{ macro("NAMESPACE") }}::EnumHelper<enum_type>{} << enum_type::x)) \
    { static const std::string name(OPP_STR(x), {{ macro("NAMESPACE") }}::findEnumNameEnd(#x)); return name; }
#define OPP_ENUM_LONG_NAME_IF(x) else if(value == ({{ macro("NAMESPACE") }}::EnumHelper<enum_type>{} << enum_type::x)) \
    { \
    constexpr static const char* enumerator_expr = OPP_STR(x);                                                                                                   \
    constexpr static const char* name = enum_name_metastring::extend("::"_ms)                                    \
        .extend(OPP_CAT(OPP_STR(x), _ms).template front<{{ macro("NAMESPACE") }}::findEnumNameEnd(enumerator_expr) - enumerator_expr>())                                                                                                    \
        .char_array;                                                     \
    return name; }

#ifndef {{ macro("INVALID_ENUM_STR") }}
#   define {{ macro("INVALID_ENUM_STR") }} "INVALID_ENUM_VALUE"
#endif

namespace {{ macro("NAMESPACE") }}
{
    // case enum_type::field: --> OK
    // case enum_type::field = const_expression:--> NOT OK

    // case (EnumHelper<enum_type>{} << enum_type::field): --> OK
    // case (EnumHelper<enum_type>{} << enum_type::field = const_expression): --> OK

    template<typename Enum>
    struct EnumHelper
    {
        Enum value;

        constexpr EnumHelper() = default;

        constexpr EnumHelper &operator<<(Enum rhs)
        {
            value = rhs;
            return *this;
        }

        template<typename T>
        constexpr Enum operator=(T &&) const
        {
            return value;
        }

        constexpr operator Enum() const
        {
            return value;
        }
    };

    constexpr bool is_space(char c)
    {
        // https://en.cppreference.com/w/cpp/string/byte/isspace
        switch (c)
        {
            case ' ':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
            case '\v':
                return true;

            default:
                return false;
        }
    }

    constexpr const char *findEnumNameEnd(const char *expr)
    {
        // expr is either "variable" or "variable = <expression>"
        while (*expr != '=' && *expr != '\0' && !is_space(*expr)) { ++expr; }

        return expr;
    }

    template<char... c>
    struct MetaString
    {
        // VERY IMPORTANT to add null char
        // because the compiler will likely compact all char arrays to optimize
        // but it also destroy the fact to iterate until end character because string are on top of each other
        static constexpr char char_array[] = { c..., '\0' };

        template<char... end>
        static constexpr MetaString<c..., end...> extend(MetaString<end...> = {}) {
            return {};
        }

        template<int n, char... inConstruct>
        struct Front
        {
            using value_type = typename MetaString::template Front<n - 1, inConstruct..., char_array[n - 1]>::value_type;
        };

        template<char... inConstruct>
        struct Front<0, inConstruct...>
        {
            using value_type = MetaString<inConstruct...>;
        };

        template<int i>
        static constexpr auto front()
        {
            static_assert(i >= 0 && i <= sizeof...(c), "front(): requested length longer than the length of the string");
            return typename Front<i>::value_type{};
        }
    };

    template<typename T, T... chars>
    constexpr MetaString<chars...> operator"" _ms() { return {}; }
}
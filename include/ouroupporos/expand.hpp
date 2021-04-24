/// AUTOMATICALLY GENERATED
/// DO NOT MODIFY

#pragma once

// "##" operator expansion
#define OPP_CAT2(a, b) a##b
#define OPP_CAT(a, b) OPP_CAT2(a, b)


// "#" operator expansion
#define OPP_STR2(...) #__VA_ARGS__
#define OPP_STR(...) OPP_STR2(__VA_ARGS__)

// https://stackoverflow.com/questions/35530850/how-to-require-a-semicolon-after-a-macro
#define OPP_REQUIRE_SEMICOLON static_assert(true, "Requires a semicolon at the end of the expression")
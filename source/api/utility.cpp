#include "utility.hpp"

int Utility::iequals(const std::string a, const std::string b)
{
    std::string a_lower = Utility::str_tolower(a);
    std::string b_lower = Utility::str_tolower(b);

    return std::strcmp(a_lower.c_str(), b_lower.c_str());
}

std::string Utility::str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); }
    );
    return s;
}
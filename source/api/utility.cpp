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

float Utility::getRandomFloat(float min, float max) {

    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<> distr(min, max);
    std::setprecision(10);
    return distr(eng);
}

int Utility::getRandomInt(int min, int max) {

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(min, max);

    return dist6(rng);
}
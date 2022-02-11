#pragma once

#include<string>
#include<cctype>
#include <algorithm>
#include <random>
#include <iomanip>

class Utility {

private:

public:

	static int iequals(const std::string a, const std::string b);

	static std::string str_tolower(std::string s);

	static float getRandomFloat(float min, float max);

	static int getRandomInt(int min, int max);
};
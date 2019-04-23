#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
using namespace std;

template<typename type_one>
string vectertojson(vector<type_one> input)
{
	string result{ "[" };
	stringstream ss;
	for (auto &i_input : input)
	{
		ss << i_input;
		if(&i_input != &input.back())
		{
			ss << ", ";
		}
	}
	result += ss.str();
	result += "]";
	return result;
}
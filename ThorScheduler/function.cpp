#include "function.h"

namespace Functions {
	function::function()
	{

	};
	function::~function()
	{
		printf("function.cpp closed.\n");
	}

	int8_t function::getDirection(int32_t a, int32_t b)
	{
		int8_t direction = (a - b);
		return (direction == 0 ? 0 : (direction < 0 ? -1 : 1));
	}

	bool function::isValue(string str)
	{
		for (const char c : str)
		{
			if (static_cast<int>(c) < ASCII_ZERO || static_cast<int>(c) > ASCII_NINE)
			{
				return false;
			}
		}
		return true;
	}

	vector<string> function::spaceOut(string s)
	{
		vector<string> result;
		string str = "";
		int index = 0;
		for (int i = index; i < s.length(); i++)
		{
			if (s[i] != ' ')
			{
				str += s.c_str()[i];
			}
			else
			{
				if (!str.empty())
				{
					result.push_back(str);
					str = "";
					index++;
				}
			}
		}
		if (!str.empty())
		{
			result.push_back(str);
		}

		return result;
	}

	int32_t function::abs(int32_t val)
	{
		return (val < 0 ? -val : val);
	}

	int32_t function::min(int32_t a, int32_t b)
	{
		return (a < b ? a : b);
	}

	unsigned long long function::getSum(int32_t a, int32_t b, int32_t c)
	{
		return abs(a - b) + abs(b - a);
	}

	void function::msgError(const exception& e, string str)
	{
		printf("[ Error, %s ] exception is as follows:\n\t\t%s\n", str.c_str(), e.what());
	}
}
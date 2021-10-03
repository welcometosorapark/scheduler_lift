#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

#define ASCII_ZERO		48
#define ASCII_NINE		57

using namespace std;
namespace Functions
{
	class function
	{
		public:
			function();
			~function();

			int8_t getDirection(int32_t a, int32_t b);
			vector<string> spaceOut(string s);
			int32_t abs(int32_t val);
			int32_t min(int32_t a, int32_t b);
			unsigned long long getSum(int32_t a, int32_t b, int32_t c);
			bool isValue(string str);

			void msgError(const exception& e, string str);
		private:
	};
}


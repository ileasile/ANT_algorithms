#pragma once
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <exception>
#include <ctime>
#include "BigInt.h"

double getCPUTime();

namespace Testing {
	typedef BigInt_t<32> BI;

	class Table {
		std::list<std::list<std::string>> data;

	public:
		Table() {}
		Table(std::list<std::string> header) {
			data.push_back(header);
		}
		friend std::ostream & operator<< (std::ostream & s, const Table & t) {
			for (auto row : t.data) {
				if (!row.empty()) {
					s << row.front();
					auto it = row.begin();
					++it;
					for (; it != row.end(); ++it) {
						s << ", " << *it;
					}
				}
				s << "\n";
			}
			return s;
		}
		Table & add_row(std::list<std::string> row) {
			data.push_back(row);
			return *this;
		}
	};

	class TestingException : public std::exception {
		const char * what() const noexcept {
			return "Testing: Unknown exception";
		}
	};
	class BadInputTestingException : public TestingException {
		const char * what() const noexcept {
			return "Testing: Unable to open input file";
		}
	};
	class NoSuchTestTypeTestingException : public TestingException {
		const char * what() const noexcept {
			return "Testing: Unknown test type given in the beginning of testing series";
		}
	};

	enum class TestType {
		IO, UNARY, BINARY, SHIFTS, COMP
	};

	auto MAX = std::numeric_limits<int>::max();

	std::map<std::string, TestType> test_type = {
		{ "io", TestType::IO},
		{ "ua", TestType::UNARY },
		{ "ba", TestType::BINARY },
		{ "shifts", TestType::SHIFTS },
		{ "comp", TestType::COMP } };
	std::map<TestType, int> res_number = {
		{ TestType::IO, 1 },
		{ TestType::UNARY, 1 },
		{ TestType::BINARY, 5 },
		{ TestType::SHIFTS, 2 },
		{ TestType::COMP, 1 } };
	std::string txt_res[] = { "FAIL", "OK" };

	Table test_correctness(std::string tests_path, int tests_limit = MAX) {
		std::ifstream f(tests_path);
		if (!f) throw BadInputTestingException();

		Table t;

		int NSERIES;
		f >> NSERIES;
		for (int i = 1; i <= NSERIES; ++i) {
			std::string type_s;
			f >> type_s;
			auto it = test_type.find(type_s);
			if (it == test_type.end()) 
				throw NoSuchTestTypeTestingException();

			t.add_row({ type_s });
			TestType type = it->second;
			int NTESTS;
			f >> NTESTS;

			std::string inp;
			std::string tmp;
			bool res[10];
			BI A, B;
			int n;

			for (int j = 0; j < NTESTS; ++j) {
				if (j >= tests_limit) {
					std::getline(f, inp);
					//std::cout << inp<< "\n";
					continue;
				}
				std::cout << i << ":" << (j+1) << " ... ";
				std::list<std::string> row;
				row.push_back(std::to_string(j + 1));

				switch (type)
				{
				case TestType::IO:
					f >> inp;
					A = BI(inp);
					tmp = A.to_string();
					res[0] = tmp == inp;
					break;

				case TestType::UNARY:
					f >> A;
					tmp = (-A).to_string();
					f >> inp;
					res[0] = tmp == inp;
					break;

				case TestType::COMP:
					f >> A >> B >> n;
					res[0] = A.compare(B) == n;
					break;

				case TestType::SHIFTS:
					f >> A >> n;

					tmp = (A << n).to_string();
					f >> inp;
					res[0] = tmp == inp;

					tmp = (A >> n).to_string();
					f >> inp;
					res[1] = tmp == inp;
					break;

				case TestType::BINARY:
					f >> A >> B;

					f >> inp;
					res[0] = (A + B).to_string() == inp;

					f >> inp;
					res[1] = (A - B).to_string() == inp;

					f >> inp;
					res[2] = (A * B).to_string() == inp;

					f >> inp;
					res[3] = (A / B).to_string() == inp;

					f >> inp;
					res[4] = (A % B).to_string() == inp;
					break;
				}

				for (int k = 0; k < res_number[type]; ++k) {
					row.push_back(txt_res[res[k]]);
				}
				t.add_row(row);
				std::getline(f, inp);
			}
		}

		return t;
	}
	
	class Timer {
		double st;
	public:
		void start() {
			st = getCPUTime();
		}
		double get() {
			auto fn = getCPUTime();
			return (fn - st);
		}
	};

	enum class TestTypeTime {
		INPUT, OUTPUT, UNARY, ADD, SUB, RSHIFT, LSHIFT, COMP, MULT, DIV
	};
	std::map<TestTypeTime, std::tuple<std::string, int, int, int>> type_to_s = {
		{ TestTypeTime::INPUT,		{"input",		5000, 20,	25} },
		{ TestTypeTime::OUTPUT,		{"output",		500, 20,	8 } },
		{ TestTypeTime::UNARY,		{"unary",		500, 1,		100000 } },
		{ TestTypeTime::ADD,		{"add",			500, 100,	100000 } },
		{ TestTypeTime::SUB,		{"sub",			500, 100,	100000 } },
		{ TestTypeTime::RSHIFT,		{"right_sh",	500, 100,	50000 } },
		{ TestTypeTime::LSHIFT,		{"left_sh",		500, 100,	50000 } },
		{ TestTypeTime::COMP,		{"comparison",	500, 100,	200000 } },
		{ TestTypeTime::MULT,		{"mult",		500, 100,	50 } },
		{ TestTypeTime::DIV,		{"div",			500, 100,	50 } }
	};

	void test_time(std::string prefix, std::set<TestTypeTime> set_of_types) {
		srand((unsigned)time(0));
		for (auto type_p : type_to_s) {
			auto type = type_p.first;
			if (set_of_types.find(type) == set_of_types.end())
				continue;

			auto type_str = std::get<0>(type_p.second);
			auto ntests = std::get<1>(type_p.second);
			auto startlen = std::get<2>(type_p.second);
			auto inclen = std::get<3>(type_p.second);
			Table t;
			
			for (int k = 1, len = startlen; k <= ntests; len += inclen, ++k) {
				std::cout << k<< "... ";
				std::string inp(len, '0');
				BI A, B, C;
				int sh, p = 0;
				Timer tim;
				double res;
				int rlen = len;

				if (type == TestTypeTime::DIV)
					A = BI::get_random(len * 3 / 2);
				else if (type != TestTypeTime::INPUT) {
					A = BI::get_random(len);
				}
				if (type == TestTypeTime::ADD ||
					type == TestTypeTime::SUB ||
					type == TestTypeTime::MULT ||
					type == TestTypeTime::DIV ||
					type == TestTypeTime::COMP)
					B = BI::get_random(len);
				if (type == TestTypeTime::LSHIFT ||
					type == TestTypeTime::RSHIFT)
					sh = rand() % 10000;

				switch (type)
				{
				case Testing::TestTypeTime::INPUT:
					std::generate(inp.begin()+1, inp.end(), []() {return '0'+ rand()%10; });
					inp[0] = '1' + rand() % 9;
					if (rand() % 2)
						inp = "-" + inp;
					tim.start();
					A = BI(inp);
					res = tim.get();
					rlen = (int)A.dig();
					break;
				case Testing::TestTypeTime::OUTPUT:
					tim.start();
					inp = A.to_string();
					res = tim.get();
					break;
				case Testing::TestTypeTime::UNARY:
					tim.start();
					C = - A;
					res = tim.get();
					break;
				case Testing::TestTypeTime::ADD:
					tim.start();
					C = A+B;
					res = tim.get();
					break;
				case Testing::TestTypeTime::SUB:
					tim.start();
					C = A - B;
					res = tim.get();
					break;
				case Testing::TestTypeTime::RSHIFT:
					tim.start();
					C = A >> sh;
					res = tim.get();
					break;
				case Testing::TestTypeTime::LSHIFT:
					tim.start();
					C = A << sh;
					res = tim.get();
					break;
				case Testing::TestTypeTime::COMP:
					tim.start();
					C = A.compare(B);
					res = tim.get();
					break;
				case Testing::TestTypeTime::MULT:
					tim.start();
					C = A * B;
					res = tim.get();
					break;
				case Testing::TestTypeTime::DIV:
					tim.start();
					C = A / B;
					res = tim.get();
					break;
				default:
					break;
				}
				
				p += C.isNeg();
				t.add_row({std::to_string(k), std::to_string(rlen), std::to_string(res)});
			}

			std::ofstream of(prefix + type_str + ".csv");
			of << t;
			of.close();
		}
	}
}
#pragma once
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <exception>
#include <ctime>
#include "BigInt.h"

namespace Testing {
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
						s << "; " << *it;
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
			BigInt A, B;
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
					A = BigInt(inp);
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
		clock_t st;
	public:
		void start() {
			st = clock();
		}
		double get() {
			auto fn = clock();
			return (fn - st) / (double)CLOCKS_PER_SEC;
		}
	};

	std::list<std::pair<std::string, Table>> test_time(
		std::string tests_path, 
		std::map<TestType, int> tests_limit = {
			{ TestType::IO, MAX },
			{ TestType::UNARY, MAX },
			{ TestType::BINARY, MAX },
			{ TestType::SHIFTS, MAX },
			{ TestType::COMP, MAX } })
	{
		std::ifstream f(tests_path);
		if (!f) throw BadInputTestingException();

		std::list<std::pair<std::string,Table>> ts;
		
		Timer timer;
		int WORKAROUND;

		int NSERIES;
		f >> NSERIES;
		for (int i = 1; i <= NSERIES; ++i) {
			std::string type_s;
			f >> type_s;
			auto it = test_type.find(type_s);
			if (it == test_type.end())
				throw NoSuchTestTypeTestingException();


			Table t;
			TestType type = it->second;
			int NTESTS;
			f >> NTESTS;

			std::string inp;
			std::string tmp;
			double res[10];
			BigInt A, B, C;
			int n, m;
			std::getline(f, inp);

			for (int j = 0; j < NTESTS; ++j) {
				if (j >= tests_limit[type]) {
					std::getline(f, inp);
					//std::cout << inp<< "\n";
					continue;
				}
				std::cout << i << ":" << (j + 1) << " ... ";
				std::list<std::string> row;
				row.push_back(std::to_string(j + 1));

				int legend_val = 0;
				switch (type)
				{
				case TestType::IO:
					f >> inp;
					timer.start();
					A = BigInt(inp);
					inp = A.to_string();
					res[0] = timer.get();
					legend_val = A.dig();
					break;

				case TestType::UNARY:
					f >> A;
					timer.start();
					C = -A;
					res[0] = timer.get();
					legend_val = C.dig();
					break;

				case TestType::COMP:
					f >> A >> B;
					timer.start();
					m = A.compare(B);
					WORKAROUND += m;
					res[0] = timer.get();
					legend_val = std::max(A.dig(), B.dig());
					break;

				case TestType::SHIFTS:
					f >> A >> n;

					timer.start();
					C = A << n;
					WORKAROUND += C.isPos();
					res[0] = timer.get();

					timer.start();
					C = A >> n;
					WORKAROUND += C.isPos();
					res[1] = timer.get();
					legend_val = A.dig();
					break;

				case TestType::BINARY:
					f >> A >> B;

					timer.start();
					C = A + B;
					WORKAROUND += C.isPos();
					res[0] = timer.get();

					timer.start();
					C = A - B;
					WORKAROUND += C.isPos();
					res[1] = timer.get();

					timer.start();
					C = A * B;
					WORKAROUND += C.isPos();
					res[2] = timer.get();

					timer.start();
					C = A / B;
					WORKAROUND += C.isPos();
					res[3] = timer.get();

					timer.start();
					C = A % B;
					WORKAROUND += C.isPos();
					res[4] = timer.get();

					legend_val = A.dig();
					break;
				}

				row.push_back(std::to_string(legend_val));
				for (int k = 0; k < res_number[type]; ++k) {
					row.push_back(std::to_string(res[k]));
				}
				t.add_row(row);
				std::getline(f, inp);
			}
			ts.push_back(std::make_pair(type_s, t));
		}
		std::cout << WORKAROUND;
		f.close();
		return ts;
	}
}
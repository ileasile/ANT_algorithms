#pragma once
#include <list>
#include <iostream>

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
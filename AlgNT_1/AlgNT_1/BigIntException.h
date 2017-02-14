#pragma once
#include <exception>

class BigIntException : public std::exception {
	const char * what() const noexcept {
		return "Big Integer: Unknown exception";
	}
};

class BigIntDivideByZeroException : public BigIntException {
	const char * what() const noexcept {
		return "Big Integer: Division by zero";
	}
};
#pragma once
#include <exception>

class MatrixException : public std::exception {
	const char * what() const noexcept {
		return "Square matrix: Unknown exception";
	}
};

class SingularMatrixException : public MatrixException {
	const char * what() const noexcept {
		return "Square matrix: Matrix is singular, there is no inverse";
	}
};
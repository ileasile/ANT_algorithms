#pragma once
#include <algorithm>
#include "MatrixException.h"

template<class T>
class SquareMatrix {
	T * data;
	size_t n;
	void swap_rows(size_t i, size_t j);

	void mult_row(size_t i, T & a);
	void divide_row(size_t i, T & a);

	//M[i][k] += M[j][k] for all k.
	void add_row(size_t i, size_t j);

	//M[i][k] += M[j][k] * a for all k.
	void add_row_multiplied(size_t i, size_t j, const T & a);
public:
	SquareMatrix(size_t n);
	SquareMatrix(const SquareMatrix<T> & mtx);
	~SquareMatrix();
	T * operator[] (size_t i);
	const T * operator[] (size_t i) const;
	SquareMatrix<T> inverse();
};

template<class T>
void SquareMatrix<T>::swap_rows(size_t i, size_t j) {
	if (i == j) return;
	T * pi = (*this)[i], *pj = (*this)[j];
	for (size_t k = 0; k < n; ++k) {
		std::swap(pi[k], pj[k]);
	}
}

template<class T>
void SquareMatrix<T>::mult_row(size_t i, T & a) {
	T * p = (*this)[i];
	for (size_t k = 0; k < n; ++k) {
		p[k] *= a;
	}
}

template<class T>
void SquareMatrix<T>::divide_row(size_t i, T & a) {
	T * p = (*this)[i];
	for (size_t k = 0; k < n; ++k) {
		p[k] /= a;
	}
}

template<class T>
void SquareMatrix<T>::add_row(size_t i, size_t j) {
	T * pi = (*this)[i], *pj = (*this)[j];
	for (size_t k = 0; k < n; ++k) {
		pi[k] += pj[k];
	}
}

template<class T>
void SquareMatrix<T>::add_row_multiplied(size_t i, size_t j, const T & a) {
	T * pi = (*this)[i], *pj = (*this)[j];
	for (size_t k = 0; k < n; ++k) {
		pi[k] += pj[k] * a;
	}
}

template<class T>
inline SquareMatrix<T>::SquareMatrix(size_t n) {
	this->n = n;
	data = new T[n*n];
}

template<class T>
SquareMatrix<T>::SquareMatrix(const SquareMatrix<T>& mtx) {
	n = mtx.n;
	data = new T[n * n];
	for (size_t i = 0; i < n; ++i) {
		for (size_t j = 0; j < n; ++j) {
			(*this)[i][j] = mtx[i][j];
		}
	}
}

template<class T>
SquareMatrix<T>::~SquareMatrix() {
	delete[] data;
}

template<class T>
inline T * SquareMatrix<T>::operator[](size_t i){
	return data + i*n;
}

template<class T>
inline const T * SquareMatrix<T>::operator[](size_t i) const{
	return data + i*n;
}

template<class T>
SquareMatrix<T> SquareMatrix<T>::inverse() {
	SquareMatrix<T> E(n);
	SquareMatrix<T> A(*this);

	for (size_t i = 0; i < n; ++i) {
		for (size_t j = 0; j < n; ++j) {
			E[i][j] = (i == j) ? 1 : 0;
		}
	}

	for (size_t k = 0; k < n; ++k) {
		size_t p = k;
		for (; p < n && A[k][p] == (T)0; ++p);
		if (p == n)
			throw SingularMatrixException();

		A.swap_rows(p, k);
		E.swap_rows(p, k);

		T a = A[k][k];
		for (size_t j = k + 1; j < n; ++j) {
			T b = -A[j][k];

			A.mult_row(j, a);
			E.mult_row(j, a);
			A.add_row_multiplied(j, k, b);
			E.add_row_multiplied(j, k, b);
		}
	}

	for (size_t k = n; k-- > 0;) {
		T a = A[k][k];
		for (size_t j = k; j-- > 0;) {
			T b = -A[j][k];

			A.mult_row(j, a);
			E.mult_row(j, a);
			A[k][j] = (T)0;
			E.add_row_multiplied(j, k, b);
		}
		E.divide_row(k, a);
	}

	return E;
}


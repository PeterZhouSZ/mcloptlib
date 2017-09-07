// The MIT License (MIT)
// Copyright (c) 2017 Matt Overby
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "MCL/Problem.hpp"

// Assumes DIM has been set before including

// min |Ax-b|
class ConstProblem : public mcl::optlib::Problem<double,DIM> {
public:
	typedef Eigen::Matrix<double,DIM,1> VectorX;
	typedef Eigen::Matrix<double,DIM,DIM> MatrixX;

	MatrixX A;
	VectorX b;
	ConstProblem(){
		A = MatrixX::Random();
		A = A.transpose()*A;
		b = VectorX::Random();
	}

	double value(const VectorX &x){ return (A*x-b).norm(); }
	double gradient(const VectorX &x, VectorX &grad){ grad = A*x-b; return value(x); }
};

class Rosenbrock : public mcl::optlib::Problem<double,2> {
public:
	typedef Eigen::Matrix<double,2,1> VectorX;
	double value(const VectorX &x){
		double a = 1.0 - x[0];
		double b = x[1] - x[0]*x[0];
		return a*a + b*b*100.0;
	}

};

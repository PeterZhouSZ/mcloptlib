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

#ifndef MCL_CONJGRADDESCENT_H
#define MCL_CONJGRADDESCENT_H

#include "Armijo.hpp"

namespace mcl {
namespace optimize {

template<typename Scalar, int DIM>
class ConjugateGradient {
private:
	typedef Eigen::Matrix<Scalar,DIM,1> VectorX;
	typedef Eigen::Matrix<Scalar,DIM,DIM> MatrixX;

public:
	int max_iters;
	Scalar eps;

	// Struct of solver parameters for L-BFGS
	struct Init {
		int max_iters;
		Scalar eps; // 0 = run full iterations
		Init() : max_iters(100), eps(0) {}
	};

	ConjugateGradient( const Init &init = Init() ) : max_iters(init.max_iters), eps(init.eps) {}

	void minimize(Problem<Scalar,DIM> &problem, VectorX &x){
		VectorX grad, grad_old, p;
		for( int iter=0; iter<max_iters; ++iter ){

			problem.gradient(x, grad);
			Scalar gradNorm = grad.template lpNorm<Eigen::Infinity>();
			if( gradNorm <= eps ){ break; }

			if( iter==0 ){ p = -grad; }
			else {
				Scalar beta = grad.dot(grad) / (grad_old.dot(grad_old));
				p = -grad + beta*p;
			}

			Scalar alpha = Armijo<Scalar, DIM, decltype(problem)>::linesearch(x, p, problem, 1);
			x = x + alpha*p;
			grad_old = grad;
		}
	} // end minimize

};

}
}

#endif

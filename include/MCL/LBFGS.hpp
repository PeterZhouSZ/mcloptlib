// The MIT License (MIT)
// Copyright (c) 2017 University of Minnesota
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

#include "Problem.hpp"
#include "MoreThuente.hpp"
#include "Armijo.hpp"

#ifndef MCL_LBFGS_H
#define MCL_LBFGS_H

namespace mcl {
namespace optlib {

// L-BFGS implementation based on Nocedal & Wright Numerical Optimization book (Section 7.2)
// DIM = dimension of the problem
// M = history window
//
// Original Author: Ioannis Karamouzas
//
template<typename Scalar, int DIM, int M=8>
class LBFGS {
private:
	typedef Eigen::Matrix<Scalar,DIM,1> VectorX;
	typedef Eigen::Matrix<Scalar,DIM,DIM> MatrixX;
	typedef Eigen::Matrix<Scalar,DIM,M> MatrixM;
	typedef Eigen::Matrix<Scalar,M,1> VectorM;

public:
	int max_iters;
	Scalar eps; // tolerance
	Scalar init_hess; // initial hessian guess

	// Struct of solver parameters for L-BFGS
	struct Init {
		int max_iters;
		Scalar eps; // 0 = run full iterations
		Scalar init_hess;
		Init() : max_iters(30), eps(0), init_hess(1) {}
	};

	LBFGS( const Init &init = Init() ) : max_iters(init.max_iters), eps(init.eps), init_hess(init.init_hess) {}

	// Returns number of iterations used
	inline int minimize(Problem<Scalar,DIM> &problem, VectorX &x){

		int dim = x.rows();
		MatrixM s = MatrixM::Zero(dim,M);
		MatrixM y = MatrixM::Zero(dim,M);
		VectorM alpha = VectorM::Zero(M);
		VectorM rho = VectorM::Zero(M);
		VectorX grad = VectorX::Zero(dim);
		VectorX q = VectorX::Zero(dim);
		VectorX grad_old = VectorX::Zero(dim);
		VectorX x_old = VectorX::Zero(dim);

		problem.gradient(x, grad);
		Scalar gamma_k = init_hess;
		Scalar gradNorm = 0.0;
		Scalar alpha_init = 1.0;

		int global_iter = 0;
		int maxIter = max_iters;

		for( int k=0; k<maxIter; ++k ){

			x_old = x;
			grad_old = grad;
			q = grad;
			global_iter++;
	
			// L-BFGS first - loop recursion		
			int iter = std::min(M, k);
			for(int i = iter - 1; i >= 0; --i){
				rho(i) = 1.0 / ((s.col(i)).dot(y.col(i)));
				alpha(i) = rho(i)*(s.col(i)).dot(q);
				q = q - alpha(i)*y.col(i);
			}

			// L-BFGS second - loop recursion			
			q = gamma_k*q;
			for(int i = 0; i < iter; ++i){
				Scalar beta = rho(i)*q.dot(y.col(i));
				q = q + (alpha(i) - beta)*s.col(i);
			}

			// is there a descent
			Scalar dir = q.dot(grad);
			if(dir <= eps ){
				q = grad;
				maxIter -= k;
				k = 0;
				alpha_init = std::min(1.0, 1.0 / grad.template lpNorm<Eigen::Infinity>() );
			}

			Scalar rate =
				Armijo<Scalar, DIM, decltype(problem)>::linesearch(x, -q, problem, alpha_init);
//				MoreThuente<Scalar, DIM, decltype(problem)>::linesearch(x, -q, problem, alpha_init);

			x = x - rate * q;
			if( rate*q.squaredNorm() <= eps ){ break; }

			problem.gradient(x,grad);
			gradNorm = grad.template lpNorm<Eigen::Infinity>();
			if(gradNorm <= eps){ break; }


			VectorX s_temp = x - x_old;
			VectorX y_temp = grad - grad_old;

			// update the history
			if(k < M){
				s.col(k) = s_temp;
				y.col(k) = y_temp;
			}
			else {
				s.leftCols(M - 1) = s.rightCols(M - 1).eval();
				s.rightCols(1) = s_temp;
				y.leftCols(M - 1) = y.rightCols(M - 1).eval();
				y.rightCols(1) = y_temp;
			}
		
			Scalar denom = y_temp.dot(y_temp);
			if(denom <= 0){ break; }
			gamma_k = s_temp.dot(y_temp) / denom;
			alpha_init = 1.0;

		}

		return global_iter;

	} // end minimize
};

}
}

#endif

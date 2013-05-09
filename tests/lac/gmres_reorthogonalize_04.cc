//----------------------------------------------------------------------
//    $Id$
//
//    Copyright (C) 2013 by the deal.II authors
//
//    This file is subject to QPL and may not be  distributed
//    without copyright and license information. Please refer
//    to the file deal.II/doc/license.html for the  text  and
//    further information on this license.
//
//----------------------------------------------------------------------

// same as gmres_reorthogonalize_02 but with worse inner product

#include "../tests.h"
#include <deal.II/lac/vector.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/solver_gmres.h>
#include <deal.II/lac/precondition.h>



// reimplements things from vector.templates.h in another way which is more
// prone to roundoff, the linker will select this version instead of the one
// in the deal.II library
namespace dealii{
template <typename Number>
template <typename Number2>
Number Vector<Number>::operator* (const Vector<Number2> &v) const
{
  Number sum = 0;
  for (unsigned int i=0; i<size(); ++i)
    sum += val[i] * v.val[i];
  return sum;
}
template <typename Number>
typename Vector<Number>::real_type Vector<Number>::l2_norm () const
{
  real_type sum = 0;
  for (unsigned int i=0; i<size(); ++i)
    sum += val[i] * val[i];
  return std::sqrt(sum);
}
}



template <typename number>
void test ()
{
  const unsigned int n = 200;
  Vector<number> rhs(n), sol(n);
  rhs = 1.;

  // only add diagonal entries
  SparsityPattern sp(n, n);
  sp.compress();
  SparseMatrix<number> matrix(sp);

  for (unsigned int i=0; i<n; ++i)
    matrix.diag_element(i) = (i+1);

  SolverControl control(1000, 1e2*std::numeric_limits<number>::epsilon());
  typename SolverGMRES<Vector<number> >::AdditionalData data;
  data.max_n_tmp_vectors = 202;

  SolverGMRES<Vector<number> > solver(control, data);
  solver.solve(matrix, sol, rhs, PreconditionIdentity());
}

int main()
{
  std::ofstream logfile("gmres_reorthogonalize_04/output");
  deallog << std::setprecision(3);
  deallog.attach(logfile);
  deallog.depth_console(0);
  deallog.threshold_double(1.e-10);

  deallog.push("double");
  test<double>();
  deallog.pop();
  deallog.threshold_double(1.e-4);
  deallog.push("float");
  test<float>();
  deallog.pop();
}


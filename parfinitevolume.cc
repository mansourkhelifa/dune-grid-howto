// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#include "config.h"               // know what grids are present
#include <iostream>               // for input/output to shell
#include <fstream>                // for input/output to files
#include <vector>                 // STL vector class
#include "dune/grid/common/mcmgmapper.hh" // mapper class

#include "unitcube.hh"
#include "transportproblem.hh"
#include "initialize.hh"
#include "parfvdatahandle.hh"
#include "parevolve.hh"
#include "vtkout.hh"

//===============================================================
// the time loop function working for all types of grids
//===============================================================

//! Parameter for mapper class
template<int dim>
struct P0Layout
{
  bool contains (int codim, Dune::GeometryType gt)
  {
    if (codim==0) return true;
    return false;
  }
};

template<class G>
void partimeloop (const G& grid, double tend)
{
  // get leaf index set type needed for mapper
  typedef typename G::template Codim<0>::LeafIndexSet IS;

  // make a mapper for codim 0 entities in the leaf grid
  Dune::MultipleCodimMultipleGeomTypeMapper<G,IS,P0Layout>
  mapper(grid,grid.leafIndexSet());

  // allocate a vector for the concentration
  std::vector<double> c(mapper.size());

  // initialize concentration with initial values
  initialize(grid,mapper,c);
  vtkout(grid,c,"pconc",0);

  // now do the time steps
  double t=0,dt;
  int k=0;
  while (t<tend)
  {
    k++;
    parevolve(grid,mapper,c,t,dt);
    t += dt;
    if (grid.comm().rank()==0)
      std::cout << "k=" << k << " t=" << t << " dt=" << dt << std::endl;
    if (k%20==0) vtkout(grid,c,"pconc",k/20);
  }
  vtkout(grid,c,"pconc",k/20);
}

//===============================================================
// The main function creates objects and does the time loop
//===============================================================

int main (int argc , char ** argv)
{
#if HAVE_MPI
  MPI_Init(&argc,&argv);
#endif

  UnitCube<Dune::YaspGrid<2,2>,64> uc;
  uc.grid().globalRefine(2);
  partimeloop(uc.grid(),0.5);

#if HAVE_MPI
  MPI_Finalize();
#endif

  // done
  return 0;
}
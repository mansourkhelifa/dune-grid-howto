// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#include "dune/io/file/vtk/vtkwriter.hh"
#include "dune/disc/functions/p0function.hh"
#include <stdio.h>

template<class G, class V>
void vtkout (const G& grid, const V& c, char* name, int k)
{
  Dune::VTKWriter<G> vtkwriter(grid);
  Dune::LeafP0FunctionWrapper<G,std::vector<double> > cc(grid,c);
  char fname[128];
  sprintf(fname,"%s-%05d",name,k);
  vtkwriter.addCellData(cc,"celldata");
  vtkwriter.write(fname,Dune::VTKOptions::binaryappended);
}
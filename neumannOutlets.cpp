/* This file is part of the Palabos library.
 *
 * Copyright (C) 2011-2017 FlowKit Sarl
 * Route d'Oron 2
 * 1010 Lausanne, Switzerland
 * E-mail contact: contact@flowkit.com
 *
 * The most recent release of Palabos can be downloaded at 
 * <http://www.palabos.org/>
 *
 * The library Palabos is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * The library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "palabos2D.h"
#include "palabos2D.hh"
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "poiseuille.h"
#include "poiseuille.hh"
#include "couette.h"
#include "couette.hh"
#include "cylinder.h"
#include "cylinder.hh"

using namespace plb;
using namespace plb::descriptors;
using namespace std;

typedef double T;
#define DESCRIPTOR D2Q9Descriptor

void setupCouetteWallsWithPeriodicity( MultiBlockLattice2D<T,DESCRIPTOR>& lattice,
                  IncomprFlowParam<T> const& parameters,
                  OnLatticeBoundaryCondition2D<T,DESCRIPTOR>& boundaryCondition )
{
    const plint nx = parameters.getNx();
    const plint ny = parameters.getNy();
	
	Box2D bottomWall(0, nx-1, 0, 0);
	Box2D topWall(0, nx-1, ny-1, ny-1);
	
	lattice.periodicity().toggle(0, true);

    boundaryCondition.setVelocityConditionOnBlockBoundaries(lattice, topWall);
	boundaryCondition.setVelocityConditionOnBlockBoundaries(lattice, bottomWall); // , boundary::dirichlet );
	
    // boundaryCondition.setVelocityConditionOnBlockBoundaries(lattice);

    setBoundaryVelocity(lattice, bottomWall, Array<T,2>((T)0.,(T)0.) );
    initializeAtEquilibrium(lattice, bottomWall, (T)1., Array<T,2>((T)0.,(T)0.) );

    T u = parameters.getLatticeU();
    setBoundaryVelocity(lattice, topWall, Array<T,2>(u,(T)0.) );
    initializeAtEquilibrium(lattice, topWall, (T)1., Array<T,2>(u,(T)0.) );

    // initializeAtEquilibrium(lattice, Box2D(0, nx-1, 1, ny-2), (T)1., Array<T,2>((T)0.,(T)0.) );

    // T u = parameters.getLatticeU();
    // setBoundaryVelocity(lattice, topWall, Array<T,2>(u,(T)0.) );
    // initializeAtEquilibrium(lattice, Box2D(0, nx-1, ny-1, ny-1), (T)1., Array<T,2>(u,(T)0.) );

    lattice.initialize();
}

void setupCouetteWallsWithInOutlets( MultiBlockLattice2D<T,DESCRIPTOR>& lattice,
                  IncomprFlowParam<T> const& parameters,
                  OnLatticeBoundaryCondition2D<T,DESCRIPTOR>& boundaryCondition )
{
    const plint nx = parameters.getNx();
    const plint ny = parameters.getNy();
	
	Box2D bottomWall(0, nx - 1, 0, 0);
	Box2D topWall(0, nx - 1, ny - 1, ny - 1);
    Box2D inlet(0, 0, 0, ny - 1);
    Box2D outlet(nx - 1, nx - 1, 0, ny - 1);
	
	boundaryCondition.addVelocityBoundary0N(inlet, lattice);

    setBoundaryVelocity(lattice, inlet, CouetteVelocity<T>(parameters));

    integrateProcessingFunctional(new CopyUnknownPopulationsFunctional2D<T,DESCRIPTOR, 0, +1>,
                                  outlet, lattice);

    boundaryCondition.setVelocityConditionOnBlockBoundaries(lattice, topWall);
	
	boundaryCondition.setVelocityConditionOnBlockBoundaries(lattice, bottomWall); //, boundary::freeslip);
	
    initializeAtEquilibrium(lattice, Box2D(0, nx-1, 1, ny-2), (T)1., Array<T,2>((T)0.,(T)0.) );

    T u = parameters.getLatticeU();
    setBoundaryVelocity(lattice, topWall, Array<T,2>(u,(T)0.) );
    initializeAtEquilibrium(lattice, topWall, (T)1., Array<T,2>(u,(T)0.) );

    lattice.initialize();
}

void writeGifs(MultiBlockLattice2D<T,DESCRIPTOR>& lattice, plint iter)
{
    const plint imSize = 600;

    ImageWriter<T> imageWriter("leeloo");
    imageWriter.writeScaledGif(createFileName("u", iter, 6),
                               *computeVelocityNorm(lattice),
                               imSize, imSize );
}

void writeVTK(MultiBlockLattice2D<T,DESCRIPTOR>& lattice,
              IncomprFlowParam<T> const& parameters, plint iter)
{
    T dx = parameters.getDeltaX();
    T dt = parameters.getDeltaT();
    VtkImageOutput2D<T> vtkOut(createFileName("vtk", iter, 6), dx);
    // VtkStructuredImageOutput2D<T> vtkOut(createFileName("vtk", iter, 6), dx);
    vtkOut.writeData<float>(*computeVelocityNorm(lattice), "velocityNorm", dx / dt);
    vtkOut.writeData<2,float>(*computeVelocity(lattice), "velocity", dx / dt);
}

int main(int argc, char* argv[]) {
    plbInit(&argc, &argv);

    global::directories().setOutputDir("./tmp/");

    IncomprFlowParam<T> parameters(
            (T) 1e-2,  // uMax
            (T) 1.5,  // Re 0.5 1.5     100
            100,       // N             128  
            2.,        // lx
            1.         // ly 
    );
    const T logT     = (T)0.02;  //0.5; //0.02; //      0.1
    const T imSave   = (T)0.1;  //10.; //0.1;  //      0.2
    const T vtkSave  = (T)1.;  //30.;   //      1.
    const T maxT     = (T)10.1; //100.1;

    writeLogFile(parameters, "Couette flow");

    MultiBlockLattice2D<T, DESCRIPTOR> lattice (
            parameters.getNx(), parameters.getNy(),
            new BGKdynamics<T,DESCRIPTOR>(parameters.getOmega()) );

    OnLatticeBoundaryCondition2D<T,DESCRIPTOR>*
        //boundaryCondition = createInterpBoundaryCondition2D<T,DESCRIPTOR>();
        boundaryCondition = createLocalBoundaryCondition2D<T,DESCRIPTOR>();

    setupCouetteWallsWithPeriodicity(lattice, parameters, *boundaryCondition);
    // setupCouetteWallsWithInOutlets(lattice, parameters, *boundaryCondition);

    // Main loop over time iterations.
    T prev = computeAverageEnergy(lattice); //getStoredAverageVelocity(lattice);
    T curr = 0.;
    for (plint iT=0; iT*parameters.getDeltaT()<maxT; ++iT) {
        if ((iT+1)%parameters.nStep(logT)==0) {
            curr = computeAverageEnergy(lattice);
            pcout << computeAverageDensity(lattice) << endl;
            pcout << computeAverageEnergy(lattice) << endl;
            // pcout << (iT+1) << " " << iT*parameters.getDeltaT() 
            //         <<  " " << setprecision(10) << computeAverageDensity(lattice) 
            //         << " " << setprecision(10) << curr
            //         << endl;

            // if (fabs(prev - curr) < 1e-5)
            // {
            //     writeVTK(lattice, parameters, iT);
            //     break;
            // }
            // else
            // {
            //     prev = curr;
            // }
        }

        if (iT%parameters.nStep(logT)==0) {
            pcout << "step " << iT
                  << "; lattice time=" << lattice.getTimeCounter().getTime()
                  << "; t=" << iT*parameters.getDeltaT()
                  << "; av energy="
                  << setprecision(10) << getStoredAverageEnergy<T>(lattice)
                  << "; av rho="
                  << getStoredAverageDensity<T>(lattice) << endl;
        }

        if (iT%parameters.nStep(imSave)==0) {
            pcout << "Saving Gif ..." << endl;
            writeGifs(lattice, iT);
        }

        if (iT%parameters.nStep(vtkSave)==0 && iT>0) {
            pcout << "Saving VTK file ..." << endl;
            writeVTK(lattice, parameters, iT);
        }

        // Lattice Boltzmann iteration step.
        lattice.collideAndStream();
    }

    delete boundaryCondition;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file    sgp.cpp
/// @brief   The main function.
///
/// @author  William Liao
/// @author  Yuhsiang Mike Tsai

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <timer.hpp>
#include "sgp.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  Main function for spectral graph partitioning.
///
int main( int argc, char** argv ){
    args setting;
    setting.target = Target::LOBPCG;
    setting.sipm = SIPM::HOST;
    setting.ls = LS::MAGMA;
    setting.solver_settings = "--solver LOBPCG --ev 4 --precond ILU";
    setting.file = "input.obj";
    setting.output = "output.obj";
    setting.sigma = 1e-5;
    setting.tol = 1e-12;
    setting.eig_maxiter = 1000;
    LS ls;
    std::string solver_settings, file, output;
    
    // Flags to check certain conditions
	// Read arguments
    readArgs(argc, argv, &setting);

    // Read file
    int E_size_r, E_size_c, *E;
    double *W;
    Network network_type = Network::UNDEFINED;
    Edge edge_type = Edge::UNDEFINED;
    cout << "Read the graph data from file..............." << flush;
    readGraph(setting.file, &E_size_r, &E_size_c, &E, &W, &network_type, &edge_type);
    cout << " Done.  " << endl;
    cout << "Size of data is " << E_size_r << "x" << E_size_c << endl;
    printKonectHeader(network_type, edge_type);

    // Construct adjacency matrix of graph
    int nnz, *cooRowIndA, *cooColIndA, n;
    double *cooValA;
    cout << "Construct adjacency matrix of graph........." << flush;
    GraphAdjacency(E_size_r, E, W, &n, &nnz,
        &cooValA, &cooRowIndA, &cooColIndA);
    cout << " Done.  " << endl;
    cout << "size of matrix = " << n << endl;
    cout << "nnz of A = " << nnz << endl;


    // Construct Laplacian
    int *csrRowIndA, *csrColIndA;
    double  *csrValA;
    cout << "Construct Laplacian matrix of graph........." << flush;
    // setting.sigma = 0;
    GraphLaplacian(&nnz, cooRowIndA, cooColIndA, cooValA, n, &csrRowIndA, &csrColIndA, &csrValA, setting.sigma);
    cout << " Done.  " << endl;
    cout << "nnz of L = " << nnz << endl;

    switch (setting.target) {
        case Target::LOBPCG : {
            double *eigval = nullptr, *eigvec = nullptr;
            int eignum = 0;
            solveSMEVP(setting.solver_settings, n, nnz,
                csrValA, csrRowIndA, csrColIndA,
                &eignum, &eigval, &eigvec);
            writePartition(n, E_size_r, E,
                eignum, eigval, eigvec, setting.output);
            break;
        }
        case Target::SIPM : {
            double mu;
            double *x, timer;
            x = new double[n];
            cout << "Solving Eigenvalue Problem.................." << flush;
            tic(&timer);
            switch (setting.sipm) {
                case SIPM::HOST :
                    solveShiftEVPHost(n, nnz, csrValA, csrRowIndA, csrColIndA,
                        setting.sigma, setting.eig_maxiter, setting.tol,
                        &mu, x);
                    break;
                case SIPM::DEVICE :
                    solveShiftEVP(n, nnz, csrValA, csrRowIndA, csrColIndA,
                        setting.sigma, setting.eig_maxiter, setting.tol,
                        &mu, x);
                    break;
            }
            cout << " Done.  ";
            toc(&timer);
            cout << "The estimated eigenvalue near "  << setting.sigma << " = ";
            cout << fixed << setprecision(13) << mu << endl;
            break;
        }
        case Target::LS : {
            double *b;
            b = new double[n];
            genRHS(b, n, nnz, csrValA, csrRowIndA, csrColIndA);
            double *x, timer;
            double res;
            x = new double[n];
            cout << "Solving Linear System......................." << flush;
            tic(&timer);
            if (setting.ls == LS::MAGMA) {
                solveGraph(solver_settings, n, nnz,
                    csrValA, csrRowIndA, csrColIndA, b, x);
            } else {
                solvels(setting.ls, n, nnz, csrValA, csrRowIndA, csrColIndA,
                    b, x, setting.tol);
            }
            cout << " Done.  ";
            toc(&timer);
            res = residual(n, nnz, csrValA, csrRowIndA, csrColIndA, b, x);
            cout << "||Ax - b|| =  "  << res << endl;
        }
    }
    return 0;
}

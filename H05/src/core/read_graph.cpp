////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file    read_graph.cpp
/// @brief   Read graph from file
///
/// @author  William Liao
///

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int readGraph(char *input, int **E, int *E_size_r, int *E_size_c){
	std::fstream pfile;
	int count = 0, n = 0;
	int *a, *b;
	int tmp;

	pfile.open(input,std::ios::in);
    assert( pfile );

    // skip comment lines
    std::string str;
    int linecount = 0, i;
    //pfile.ignore(4096, '\n');
    while( std::getline(pfile, str) ) {
        if ( str[0] == '%' )
        {
            linecount++;
            continue;
        }else{
            break;
        }
    }
    pfile.clear();
    pfile.seekg(0, std::ios::beg);
    for (i = 0; i < linecount; i++)
    {
        pfile.ignore(4096, '\n');
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Count size
    // row
    std::getline(pfile,str);
    std::istringstream sin(str);
    double k;
    while (sin >> k) {
      ++count;
    }
    *E_size_c = count;

    // col
    pfile.clear();
    pfile.seekg(0, std::ios::beg);
    //pfile.ignore(4096, '\n');
    for (i = 0; i < linecount; i++)
    {
        pfile.ignore(4096, '\n');
    }
    count = 0;
    while( !pfile.eof() && pfile.peek()!=EOF ) {
    	count++;
    	pfile.ignore(4096, '\n');
        pfile.get();
    }
    //std::cout << "Size of data is " << count << "x" << *E_size_c << std::endl;

    // Return to top of file
    pfile.clear();
  	pfile.seekg(0, std::ios::beg);

    if ( *E_size_c == 3 )
    {
        int *c;
        a  = new int[count];
        b  = new int[count];
        c  = new int[count];
        *E = new int[3*count];

        // Read graph
        for (i = 0; i < linecount; i++)
        {
            pfile.ignore(4096, '\n');
        }
        while( !pfile.eof() && pfile.peek()!=EOF ) {
            pfile >> a[n];
            pfile >> b[n];
            pfile >> c[n];
            n++;
            file.get();
        }
        // Change to zero base
        for (int i = 0; i < count; i++)
        {
            a[i] = a[i] - 1;
            b[i] = b[i] - 1;
        }
        std::copy(a, a+count, *E);
        std::copy(b, b+count, *E+count);
        std::copy(c, c+count, *E+2*count);
        *E_size_r = count;

        delete a;
        delete b;
        delete c;
    }else if ( *E_size_c == 2 )
    {
        a  = new int[count];
        b  = new int[count];
        *E = new int[2*count];

        // Read graph
        for (i = 0; i < linecount; i++)
        {
            pfile.ignore(4096, '\n');
        }
        while( !pfile.eof() && pfile.peek()!=EOF ) {
            pfile >> a[n];
            pfile >> b[n];
            n++;
            pfile.get();
        }
        // Change to zero base
        for (int i = 0; i < count; i++)
        {
            a[i] = a[i] - 1;
            b[i] = b[i] - 1;
        }
        std::copy(a, a+count, *E);
        std::copy(b, b+count, *E+count);
        *E_size_r = count;

        delete a;
        delete b;
    }

    pfile.close();

    return 0;
}
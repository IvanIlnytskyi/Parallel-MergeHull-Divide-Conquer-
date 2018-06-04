#include "ConvexHull.h"
#include <iostream>
#include <chrono>
#include <math.h>
#include <fstream>

using namespace std;
using namespace std::chrono;


void main()
{
	srand(static_cast <unsigned> (time(0)));
	ofstream results("results.txt");
	results << "power\tseq\tpar\n";
	for (size_t i = 7; i < 19; ++i)
	{	
		size_t N = pow(2, i);
		for (int j = 0; j < 10; ++j)
		{
			results << i << '\t';

			vector<Point> seq_points = create_test_vector(N);
			vector<Point> par_points(seq_points);

			high_resolution_clock::time_point seq_start = high_resolution_clock::now();
			Hull seq_result;
			grahamScan(seq_points, seq_result);
			high_resolution_clock::time_point seq_end = high_resolution_clock::now();


			high_resolution_clock::time_point par_start = high_resolution_clock::now();
			Hull par_result;
			parallelMergeHull(par_points, par_result);
			high_resolution_clock::time_point par_end = high_resolution_clock::now();

			cout << "seq execution time " << duration_cast<microseconds>(seq_end - seq_start).count() << endl;
			cout << "par execution time " << duration_cast<microseconds>(par_end - par_start).count() << endl;
			results << duration_cast<microseconds>(seq_end - seq_start).count() << '\t';
			results << duration_cast<microseconds>(par_end - par_start).count() << '\n';
		}	
	}
	system("Pause");
}

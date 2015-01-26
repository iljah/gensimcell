/*
Program which runs the performance tests and decides whether they succeeded.

Copyright 2014, 2015 Ilja Honkonen
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of copyright holders nor the names of their contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "chrono"
#include "cmath"
#include "cstdlib"
#include "fstream"
#include "iostream"
#include "string"

using namespace std;
using namespace std::chrono;

/*!
Runs the given program and returns the execution time in seconds.
*/
double run_mpi_test(const string& mpi_command, const string& path)
{
	const string command(mpi_command + " " + path);
	cout << "Running " + command + "...  ";
	cout.flush();

	const auto start = high_resolution_clock::now();
	if (system(command.c_str()) != EXIT_SUCCESS) {
		cout <<
			"Running " + command + " probably failed, "
			"was this program run from the gensimcell directory?\n";
		abort();
	}
	const auto end = high_resolution_clock::now();

	return duration_cast<duration<double>>(end - start).count();

}

/*!
First argument given to the program after potential filtering
by MPI should be the command to use to launch 
*/
int main(int argc, char* argv[])
{
	if (argc < 2) {
		cout << "MPI command must be given as argument, e.g. \"mpirun -n 2\"" << endl;
		abort();
	}
	const string mpi(argv[1]);

	const double allowed_relative_difference = 0.1;
	double run_time, ref_run_time;
	const string prefix("tests/parallel/particle_propagation/");

	run_time = run_mpi_test(mpi, prefix + "mpi_speed.dexe"),
	ref_run_time = run_mpi_test(mpi, prefix + "mpi_speed_reference.dexe");


	// check that output files are identical
	ifstream
		infile(prefix + "particle_3157.dc", ios::in | ios::binary),
		infile_ref(prefix + "reference_3157.dc", ios::in | ios::binary);

	if (not infile.is_open()) {
		std::cerr << __FILE__ << ":" << __LINE__ << " FAILED" << std::endl;
		abort();
	}
	if (not infile_ref.is_open()) {
		std::cerr << __FILE__ << ":" << __LINE__ << " FAILED" << std::endl;
		abort();
	}

	while (true) {
		char c = 0, c_ref = 1;

		infile.read(&c, 1);
		infile_ref.read(&c_ref, 1);

		if (infile.bad()) {
			std::cerr << __FILE__ << ":" << __LINE__ << " FAILED" << std::endl;
			abort();
		}

		if (infile_ref.bad()) {
			std::cerr << __FILE__ << ":" << __LINE__ << " FAILED" << std::endl;
			abort();
		}

		if (infile.eof() and infile_ref.eof()) {
			break;
		} else if (infile.eof() or infile_ref.eof()) {
			std::cerr << __FILE__ << ":" << __LINE__ << " FAILED" << std::endl;
			abort();
		}

		if (c != c_ref) {
			std::cerr << __FILE__ << ":" << __LINE__ << " FAILED" << std::endl;
			abort();
		}
	}


	if (
		fabs(run_time - ref_run_time)
		<= allowed_relative_difference * max(run_time, ref_run_time)
	) {
		return EXIT_SUCCESS;
	}

	cout << "Too large difference in speeds, trying again just in case" << endl;
	run_time = run_mpi_test(mpi, prefix + "mpi_speed.dexe"),
	ref_run_time = run_mpi_test(mpi, prefix + "mpi_speed_reference.dexe");
	if (
		fabs(run_time - ref_run_time)
		> allowed_relative_difference * max(run_time, ref_run_time)
	) {
		cout << "Speed difference too large: "
			<< run_time << " s vs. " << ref_run_time
			<< " s\nFAILED" << endl;
		abort();
	}

	return EXIT_SUCCESS;
}

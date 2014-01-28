#include "boost/logic/tribool.hpp"
#include "cstdlib"
#include "iostream"
#include "mpi.h"
#include "vector"

#include "check_true.hpp"
#include "gensimcell.hpp"

using namespace std;

struct test_variable1 {
	typedef int data_type;
};

struct test_variable2 {
	typedef std::array<double, 3> data_type;
};

struct test_variable3 {
	typedef float data_type;
};

struct test_variable4 {
	typedef unsigned int data_type;
};


#define PRINT_ERROR(rank, msg) \
std::cerr \
	<< __FILE__ << ":" << __LINE__ \
	<< " Process " << rank << ": " msg \
	<< std::endl;


void transfer_c1(MPI_Comm comm, const int rank)
{
	const test_variable1 v1;
	const test_variable2 v2;
	const test_variable3 v3;
	gensimcell::Cell<
		test_variable1,
		test_variable2,
		test_variable3
	> c1_1;

	void* address = NULL;
	int count = -1;
	MPI_Datatype datatype = MPI_DATATYPE_NULL;

	if (rank == 0) {

		// send all from process 0 to 1
		c1_1.set_transfer_all(true, v1, v2, v3);
		c1_1.set_transfer(true, v1, v2, v3);

		c1_1(v1) = 1;
		c1_1(v2) = {1.5, 2.25, 3.75};
		c1_1(v3) = 1.5;
		std::tie(address, count, datatype) = c1_1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c1_1.")
			abort();
		}
		if (MPI_Type_commit(&datatype) != MPI_SUCCESS) {
			PRINT_ERROR(rank, "Couldn't commit datatype of c1_1.")
			abort();
		}
		if (
			MPI_Send(
				address,
				count,
				datatype,
				1,
				0,
				comm
			) != MPI_SUCCESS
		) {
			PRINT_ERROR(rank, "Couldn't send c1_1 to process 1.")
			abort();
		}
		if (MPI_Type_free(&datatype) != MPI_SUCCESS) {
			PRINT_ERROR(rank, "Couldn't free datatype of c1_1.")
			abort();
		}
		CHECK_TRUE(c1_1(v1) == 1)
		CHECK_TRUE(c1_1(v2)[0] == 1.5)
		CHECK_TRUE(c1_1(v2)[1] == 2.25)
		CHECK_TRUE(c1_1(v2)[2] == 3.75)
		CHECK_TRUE(c1_1(v3) == 1.5)


		// don't send v1 and v3
		c1_1.set_transfer_all(false, v1, v3);
		std::tie(address, count, datatype) = c1_1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c1_1.")
			abort();
		}
		if (
			MPI_Send(
				address,
				count,
				datatype,
				1,
				0,
				comm
			) != MPI_SUCCESS
		) {
			PRINT_ERROR(rank, "Couldn't send c1_1 to process 1.")
			abort();
		}
		CHECK_TRUE(c1_1(v1) == 1)
		CHECK_TRUE(c1_1(v2)[0] == 1.5)
		CHECK_TRUE(c1_1(v2)[1] == 2.25)
		CHECK_TRUE(c1_1(v2)[2] == 3.75)
		CHECK_TRUE(c1_1(v3) == 1.5)


		// send v3 from process 1 to 0
		c1_1.set_transfer_all(true, v3);
		c1_1.set_transfer_all(false, v2);
		c1_1(v1) = -1;
		c1_1(v2) = {-1, -1, -1};
		c1_1(v3) = -1;
		std::tie(address, count, datatype) = c1_1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c1_1.")
			abort();
		}
		if (
			MPI_Recv(
				address,
				count,
				datatype,
				1,
				0,
				comm,
				MPI_STATUS_IGNORE
			) != MPI_SUCCESS
		) {
			PRINT_ERROR(rank, "Couldn't send c1_1 to process 1.")
			abort();
		}
		CHECK_TRUE(c1_1(v1) == -1)
		CHECK_TRUE(c1_1(v2)[0] == -1)
		CHECK_TRUE(c1_1(v2)[1] == -1)
		CHECK_TRUE(c1_1(v2)[2] == -1)
		CHECK_TRUE(c1_1(v3) == 8.5)


	} else if (rank == 1) {

		// comments in the proc == 0 part
		c1_1.set_transfer_all(true, v1, v2, v3);
		c1_1.set_transfer(true, v1, v2, v3);

		c1_1(v1) = -1;
		c1_1(v2) = {-1, -1, -1};
		c1_1(v3) = -1;
		std::tie(address, count, datatype) = c1_1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c1_1.")
			abort();
		}
		if (MPI_Type_commit(&datatype) != MPI_SUCCESS) {
			PRINT_ERROR(rank, "Couldn't commit datatype of c1_1.")
			abort();
		}
		if (
			MPI_Recv(
				address,
				count,
				datatype,
				0,
				0,
				comm,
				MPI_STATUS_IGNORE
			) != MPI_SUCCESS
		) {
			PRINT_ERROR(rank, "Couldn't receive c1_1 from process 0.")
			abort();
		}
		if (MPI_Type_free(&datatype) != MPI_SUCCESS) {
			PRINT_ERROR(rank, "Couldn't free datatype of c1_1.")
			abort();
		}
		CHECK_TRUE(c1_1(v1) == 1)
		CHECK_TRUE(c1_1(v2)[0] == 1.5)
		CHECK_TRUE(c1_1(v2)[1] == 2.25)
		CHECK_TRUE(c1_1(v2)[2] == 3.75)
		CHECK_TRUE(c1_1(v3) == 1.5)
		c1_1(v1) = -1;
		c1_1(v2) = {-1, -1, -1};
		c1_1(v3) = -1;


		c1_1.set_transfer_all(false, v1, v3);
		std::tie(address, count, datatype) = c1_1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c1_1.")
			abort();
		}
		if (
			MPI_Recv(
				address,
				count,
				datatype,
				0,
				0,
				comm,
				MPI_STATUS_IGNORE
			) != MPI_SUCCESS
		) {
			PRINT_ERROR(rank, "Couldn't receive c1_1 from process 0.")
			abort();
		}
		CHECK_TRUE(c1_1(v1) == -1)
		CHECK_TRUE(c1_1(v2)[0] == 1.5)
		CHECK_TRUE(c1_1(v2)[1] == 2.25)
		CHECK_TRUE(c1_1(v2)[2] == 3.75)
		CHECK_TRUE(c1_1(v3) == -1)
		c1_1(v2) = {-1, -1, -1};


		c1_1.set_transfer_all(true, v3);
		c1_1.set_transfer_all(false, v2);
		c1_1(v1) = 2;
		c1_1(v2) = {5.5, 6.25, 7.75};
		c1_1(v3) = 8.5;
		std::tie(address, count, datatype) = c1_1.get_mpi_datatype();
		if (count < 0) {
			PRINT_ERROR(rank, "Couldn't get datatype from c1_1.")
			abort();
		}
		if (
			MPI_Send(
				address,
				count,
				datatype,
				0,
				0,
				comm
			) != MPI_SUCCESS
		) {
			PRINT_ERROR(rank, "Couldn't receive c1_1 from process 0.")
			abort();
		}
	}
}


void transfer_cN(MPI_Comm comm, const int rank)
{
	const test_variable1 v1;
	const test_variable2 v2;
	const test_variable3 v3;
	const test_variable4 v4;
	vector<
		gensimcell::Cell<
			test_variable1,
			test_variable2,
			test_variable3,
			test_variable4
		>
	> c1s(4);

	void* address = NULL;
	int count = -1;
	MPI_Datatype datatype = MPI_DATATYPE_NULL;


	if (rank == 0) {


		// send all cells from 0 to 1
		c1s[0].set_transfer_all(true, v1, v2, v3, v4);

		for (size_t i = 0; i < c1s.size(); i++) {
			c1s[i].set_transfer(true, v1, v2, v3, v4);
			c1s[i](v1) = int(i);
			c1s[i](v2) = {i + 0.25, i + 0.5, i + 0.75};
			c1s[i](v3) = i + 5.5;
			c1s[i](v4) = i + 5;
		}

		for (auto& cell: c1s) {
			std::tie(address, count, datatype) = cell.get_mpi_datatype();
			if (count < 0) {
				PRINT_ERROR(rank, "Couldn't get datatype.")
				abort();
			}
			if (MPI_Type_commit(&datatype) != MPI_SUCCESS) {
				PRINT_ERROR(rank, "Couldn't commit datatype.")
				abort();
			}
			if (
				MPI_Send(
					address,
					count,
					datatype,
					1,
					0,
					comm
				) != MPI_SUCCESS
			) {
				PRINT_ERROR(rank, "Couldn't send.")
				abort();
			}
			if (MPI_Type_free(&datatype) != MPI_SUCCESS) {
				PRINT_ERROR(rank, "Couldn't free datatype.")
				abort();
			}
		}

		for (size_t i = 0; i < c1s.size(); i++) {
			CHECK_TRUE(c1s[i](v1) == int(i))
			CHECK_TRUE(c1s[i](v2)[0] == i + 0.25)
			CHECK_TRUE(c1s[i](v2)[1] == i + 0.5)
			CHECK_TRUE(c1s[i](v2)[2] == i + 0.75)
			CHECK_TRUE(c1s[i](v3) == i + 5.5)
			CHECK_TRUE(c1s[i](v4) == i + 5)
		}


		// send every other variable from every other cell
		c1s[0].set_transfer_all(boost::logic::indeterminate, v1, v2, v3, v4);
		for (size_t i = 0; i < c1s.size(); i++) {
			if (i % 2 == 0) {
				c1s[i].set_transfer(false, v1, v3);
				c1s[i].set_transfer(true, v2, v4);
			} else {
				c1s[i].set_transfer(true, v1, v3);
				c1s[i].set_transfer(false, v2, v4);
			}
		}

		for (auto& cell: c1s) {
			std::tie(address, count, datatype) = cell.get_mpi_datatype();
			if (count < 0) {
				PRINT_ERROR(rank, "Couldn't get datatype.")
				abort();
			}
			if (MPI_Type_commit(&datatype) != MPI_SUCCESS) {
				PRINT_ERROR(rank, "Couldn't commit datatype.")
				abort();
			}
			if (
				MPI_Send(
					address,
					count,
					datatype,
					1,
					0,
					comm
				) != MPI_SUCCESS
			) {
				PRINT_ERROR(rank, "Couldn't send.")
				abort();
			}
			if (MPI_Type_free(&datatype) != MPI_SUCCESS) {
				PRINT_ERROR(rank, "Couldn't free datatype.")
				abort();
			}
		}

		for (size_t i = 0; i < c1s.size(); i++) {
			CHECK_TRUE(c1s[i](v1) == int(i))
			CHECK_TRUE(c1s[i](v2)[0] == i + 0.25)
			CHECK_TRUE(c1s[i](v2)[1] == i + 0.5)
			CHECK_TRUE(c1s[i](v2)[2] == i + 0.75)
			CHECK_TRUE(c1s[i](v3) == i + 5.5)
			CHECK_TRUE(c1s[i](v4) == i + 5)
		}


	} else if (rank == 1) {

		// comments in the proc == 0 part
		c1s[0].set_transfer_all(true, v1, v2, v3, v4);

		for (size_t i = 0; i < c1s.size(); i++) {
			c1s[i].set_transfer(true, v1, v2, v3, v4);
			c1s[i](v1) = -1;
			c1s[i](v2) = {-1, -1, -1};
			c1s[i](v3) = -1;
			c1s[i](v4) = 0;
		}

		for (auto& cell: c1s) {
			std::tie(address, count, datatype) = cell.get_mpi_datatype();
			if (count < 0) {
				PRINT_ERROR(rank, "Couldn't get datatype.")
				abort();
			}
			if (MPI_Type_commit(&datatype) != MPI_SUCCESS) {
				PRINT_ERROR(rank, "Couldn't commit datatype.")
				abort();
			}
			if (
				MPI_Recv(
					address,
					count,
					datatype,
					0,
					0,
					comm,
					MPI_STATUS_IGNORE
				) != MPI_SUCCESS
			) {
				PRINT_ERROR(rank, "Couldn't receive.")
				abort();
			}
			if (MPI_Type_free(&datatype) != MPI_SUCCESS) {
				PRINT_ERROR(rank, "Couldn't free datatype.")
				abort();
			}
		}

		for (size_t i = 0; i < c1s.size(); i++) {
			CHECK_TRUE(c1s[i](v1) == int(i))
			CHECK_TRUE(c1s[i](v2)[0] == i + 0.25)
			CHECK_TRUE(c1s[i](v2)[1] == i + 0.5)
			CHECK_TRUE(c1s[i](v2)[2] == i + 0.75)
			CHECK_TRUE(c1s[i](v3) == i + 5.5)
			CHECK_TRUE(c1s[i](v4) == i + 5)
			c1s[i](v1) = -1;
			c1s[i](v2) = {-1, -1, -1};
			c1s[i](v3) = -1;
			c1s[i](v4) = 0;
		}


		c1s[0].set_transfer_all(boost::logic::indeterminate, v1, v2, v3, v4);
		for (size_t i = 0; i < c1s.size(); i++) {
			if (i % 2 == 0) {
				c1s[i].set_transfer(false, v1, v3);
				c1s[i].set_transfer(true, v2, v4);
			} else {
				c1s[i].set_transfer(true, v1, v3);
				c1s[i].set_transfer(false, v2, v4);
			}
		}

		for (auto& cell: c1s) {
			std::tie(address, count, datatype) = cell.get_mpi_datatype();
			if (count < 0) {
				PRINT_ERROR(rank, "Couldn't get datatype.")
				abort();
			}
			if (MPI_Type_commit(&datatype) != MPI_SUCCESS) {
				PRINT_ERROR(rank, "Couldn't commit datatype.")
				abort();
			}
			if (
				MPI_Recv(
					address,
					count,
					datatype,
					0,
					0,
					comm,
					MPI_STATUS_IGNORE
				) != MPI_SUCCESS
			) {
				PRINT_ERROR(rank, "Couldn't receive.")
				abort();
			}
			if (MPI_Type_free(&datatype) != MPI_SUCCESS) {
				PRINT_ERROR(rank, "Couldn't free datatype.")
				abort();
			}
		}

		for (size_t i = 0; i < c1s.size(); i++) {
			if (i % 2 == 0) {
				CHECK_TRUE(c1s[i](v1) == -1)
				CHECK_TRUE(c1s[i](v2)[0] == i + 0.25)
				CHECK_TRUE(c1s[i](v2)[1] == i + 0.5)
				CHECK_TRUE(c1s[i](v2)[2] == i + 0.75)
				CHECK_TRUE(c1s[i](v3) == -1)
				CHECK_TRUE(c1s[i](v4) == i + 5)
			} else {
				CHECK_TRUE(c1s[i](v1) == int(i))
				CHECK_TRUE(c1s[i](v2)[0] == -1)
				CHECK_TRUE(c1s[i](v2)[1] == -1)
				CHECK_TRUE(c1s[i](v2)[2] == -1)
				CHECK_TRUE(c1s[i](v3) == i + 5.5)
				CHECK_TRUE(c1s[i](v4) == 0)
			}
		}
	}
}


int main(int argc, char* argv[])
{
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		cerr << "Couldn't initialize MPI." << endl;
		abort();
	}

	MPI_Comm comm = MPI_COMM_WORLD;

	int rank = 0, comm_size = 0;
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &comm_size);

	if (comm_size < 2) {
		cerr << "This test must be run with at least 2 processes" << endl;
		abort();
	}

	transfer_c1(comm, rank);
	transfer_cN(comm, rank);

	MPI_Finalize();

	return EXIT_SUCCESS;
}

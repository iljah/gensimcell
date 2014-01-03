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


#define PRINT_ERROR(rank, msg) \
std::cerr \
	<< __FILE__ << ":" << __LINE__ \
	<< " Process " << rank << ": " msg \
	<< std::endl;


void transfer_c1_v1(MPI_Comm comm, const int rank)
{
	const test_variable1 v1;
	gensimcell::Cell<test_variable1> c1_1;

	void* address = NULL;
	int count = -1;
	MPI_Datatype datatype = MPI_DATATYPE_NULL;

	if (rank == 0) {

		// send v1 from process 0 to 1
		c1_1(v1) = 1;
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


		// don't send v1
		c1_1.set_transfer_all(v1, false);
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


		// send v1
		c1_1.set_transfer_all(v1, boost::logic::indeterminate);
		c1_1.set_transfer(v1, true);
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


		// send v1 from process 1 to 0
		c1_1(v1) = -1;
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
		CHECK_TRUE(c1_1(v1) == 2)
		c1_1(v1) = -1;


	} else if (rank == 1) {

		// comments in the proc == 0 part

		c1_1(v1) = -1;
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
		CHECK_TRUE(c1_1(v1) == 1)
		c1_1(v1) = -1;


		c1_1.set_transfer_all(v1, false);
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


		c1_1.set_transfer_all(v1, boost::logic::indeterminate);
		c1_1.set_transfer(v1, true);
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
		CHECK_TRUE(c1_1(v1) == 1)
		c1_1(v1) = -1;


		c1_1(v1) = 2;
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
		CHECK_TRUE(c1_1(v1) == 2)
		c1_1(v1) = -1;

	}
}


void transfer_cN_v1(MPI_Comm comm, const int rank)
{
	const test_variable1 v1;
	const test_variable2 v2;
	vector<gensimcell::Cell<test_variable1>> c1s(3);
	vector<gensimcell::Cell<test_variable2>> c2s(3);

	void* address = NULL;
	int count = -1;
	MPI_Datatype datatype = MPI_DATATYPE_NULL;

	if (rank == 0) {


		// send all cells from 0 to 1
		for (size_t i = 0; i < c1s.size(); i++) {
			c1s[i](v1) = i;
		}

		for (auto& cell: c1s) {
			std::tie(address, count, datatype) = cell.get_mpi_datatype();
			if (count < 0) {
				PRINT_ERROR(rank, "Couldn't get datatype.")
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
		}

		for (size_t i = 0; i < c1s.size(); i++) {
			CHECK_TRUE(c1s[i](v1) == int(i))
		}


		c1s[0].set_transfer_all(v1, false);
		for (auto& cell: c1s) {
			std::tie(address, count, datatype) = cell.get_mpi_datatype();
			if (count < 0) {
				PRINT_ERROR(rank, "Couldn't get datatype.")
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
		}


		c1s[0].set_transfer_all(v1, boost::logic::indeterminate);
		for (size_t i = 0; i < c1s.size(); i++) {
			if (i % 2 == 0) {
				c1s[i].set_transfer(v1, true);
			} else {
				c1s[i].set_transfer(v1, false);
			}
		}

		for (size_t i = 0; i < c1s.size(); i++) {
			std::tie(address, count, datatype) = c1s[i].get_mpi_datatype();
			if (count < 0) {
				PRINT_ERROR(rank, "Couldn't get datatype.")
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
		}

		for (size_t i = 0; i < c1s.size(); i++) {
			CHECK_TRUE(c1s[i](v1) == int(i))
		}


		for (size_t i = 0; i < c2s.size(); i++) {
			for (size_t j = 0; j < c2s[i](v2).size(); j++) {
				c2s[i](v2)[j] = i * 10 + j;
			}
		}

		for (auto& cell: c2s) {
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
		}

		for (size_t i = 0; i < c2s.size(); i++) {
			for (size_t j = 0; j < c2s[i](v2).size(); j++) {
				CHECK_TRUE(c2s[i](v2)[j] == i * 10 + j)
			}
		}


		c2s[0].set_transfer_all(v2, false);
		for (auto& cell: c2s) {
			std::tie(address, count, datatype) = cell.get_mpi_datatype();
			if (count < 0) {
				PRINT_ERROR(rank, "Couldn't get datatype.")
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
		}


	} else if (rank == 1) {

		// comments in the proc == 0 part
 
		for (auto& cell: c1s) {
			cell(v1) = -1;
		}

		for (auto& cell: c1s) {
			std::tie(address, count, datatype) = cell.get_mpi_datatype();
			if (count < 0) {
				PRINT_ERROR(rank, "Couldn't get datatype.")
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
		}

		for (size_t i = 0; i < c1s.size(); i++) {
			CHECK_TRUE(c1s[i](v1) == int(i))
			c1s[i](v1) = -1;
		}


		c1s[0].set_transfer_all(v1, false);
		for (auto& cell: c1s) {
			std::tie(address, count, datatype) = cell.get_mpi_datatype();
			if (count < 0) {
				PRINT_ERROR(rank, "Couldn't get datatype.")
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
		}
		for (auto& cell: c1s) {
			CHECK_TRUE(cell(v1) == -1)
		}


		c1s[0].set_transfer_all(v1, boost::logic::indeterminate);
		for (size_t i = 0; i < c1s.size(); i++) {
			if (i % 2 == 0) {
				c1s[i].set_transfer(v1, true);
			} else {
				c1s[i].set_transfer(v1, false);
			}
		}

		for (size_t i = 0; i < c1s.size(); i++) {
			std::tie(address, count, datatype) = c1s[i].get_mpi_datatype();
			if (count < 0) {
				PRINT_ERROR(rank, "Couldn't get datatype.")
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
		}

		for (size_t i = 0; i < c1s.size(); i++) {
			if (i % 2 == 0) {
				CHECK_TRUE(c1s[i](v1) == int(i))
			} else {
				CHECK_TRUE(c1s[i](v1) == -1)
			}
			c1s[i](v1) = -1;
		}


		for (size_t i = 0; i < c2s.size(); i++) {
			for (size_t j = 0; j < c2s[i](v2).size(); j++) {
				c2s[i](v2)[j] = -1;
			}
		}

		for (auto& cell: c2s) {
			std::tie(address, count, datatype) = cell.get_mpi_datatype();
			if (count < 0) {
				PRINT_ERROR(rank, "Couldn't get datatype.")
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
		}


		for (size_t i = 0; i < c2s.size(); i++) {
			for (size_t j = 0; j < c2s[i](v2).size(); j++) {
				CHECK_TRUE(c2s[i](v2)[j] == i * 10 + j)
				c2s[i](v2)[j] = -1;
			}
		}

		c2s[0].set_transfer_all(v2, false);
		for (auto& cell: c2s) {
			std::tie(address, count, datatype) = cell.get_mpi_datatype();
			if (count < 0) {
				PRINT_ERROR(rank, "Couldn't get datatype.")
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
		}

		for (size_t i = 0; i < c2s.size(); i++) {
			for (size_t j = 0; j < c2s[i](v2).size(); j++) {
				CHECK_TRUE(c2s[i](v2)[j] == -1)
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

	transfer_c1_v1(comm, rank);
	transfer_cN_v1(comm, rank);

	MPI_Finalize();

	return EXIT_SUCCESS;
}

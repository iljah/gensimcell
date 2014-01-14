#include "boost/function_types/property_tags.hpp"
#include "boost/tti/has_member_function.hpp"

#include "gensimcell.hpp"

BOOST_TTI_HAS_MEMBER_FUNCTION(get_mpi_datatype)

struct variable_t {
	typedef int data_type;
};

typedef gensimcell::Cell<variable_t> cell_t;

static_assert(
	has_member_function_get_mpi_datatype<
		cell_t,
		std::tuple<void*, int, MPI_Datatype>,
		boost::mpl::vector<>,
		boost::function_types::const_qualified
	>::value,
	"No get_mpi_datatype() const found given cell class."
);

int main(int, char**) { return 0; }

include makefiles/macosx_macports

CPPFLAGS += \
  -I source \
  -I tests \
  -I examples/game_of_life/parallel \
  -I examples/advection/parallel \
  -I examples/particle_propagation/parallel

CXXFLAGS += -std=c++0x -W -Wall -Wextra -pedantic -O3

HEADERS = \
  examples/advection/parallel/advection_initialize.hpp \
  examples/advection/parallel/advection_save.hpp \
  examples/advection/parallel/advection_solve.hpp \
  examples/advection/parallel/advection_variables.hpp \
  examples/combined/combined_variables.hpp \
  examples/game_of_life/parallel/gol_initialize.hpp \
  examples/game_of_life/parallel/gol_save.hpp \
  examples/game_of_life/parallel/gol_solve.hpp \
  examples/game_of_life/parallel/gol_variables.hpp \
  examples/particle_propagation/parallel/particle_initialize.hpp \
  examples/particle_propagation/parallel/particle_save.hpp \
  examples/particle_propagation/parallel/particle_solve.hpp \
  examples/particle_propagation/parallel/particle_variables.hpp \
  source/gensimcell.hpp \
  source/gensimcell_impl.hpp \
  source/get_var_mpi_datatype.hpp \
  source/operators.hpp \
  tests/check_true.hpp \
  tests/parallel/recursive_cell_gol/gol_initialize.hpp \
  tests/parallel/recursive_cell_gol/gol_save.hpp \
  tests/parallel/recursive_cell_gol/gol_solve.hpp \
  tests/parallel/recursive_cell_gol/gol_variables.hpp \
  tests/parallel/particle_propagation/reference_cell.hpp \
  tests/parallel/particle_propagation/reference_initialize.hpp \
  tests/parallel/particle_propagation/reference_save.hpp \
  tests/parallel/particle_propagation/reference_solve.hpp


## Compilation rules ##
# these require (some parts of) boost
%.exe: %.cpp $(HEADERS) Makefile
	@echo "CXX "$< && $(CXX) $(CPPFLAGS) $(CXXFLAGS) $(BOOST_CPPFLAGS) $< -o $@

# these require MPI (for example open-mpi.org)
%.mexe: %.cpp $(HEADERS) Makefile
	@echo "MPICXX "$< && $(MPICXX) -DHAVE_MPI $(CPPFLAGS) $(CXXFLAGS) $(BOOST_CPPFLAGS) $< -o $@

# these require Eigen and MPI
%.eexe: %.cpp $(HEADERS) Makefile
	@echo "MPICXX "$< && $(MPICXX) -DHAVE_MPI -DHAVE_EIGEN $(CPPFLAGS) $(CXXFLAGS) $(BOOST_CPPFLAGS) $(EIGEN_CPPFLAGS) $< -o $@

# these require dccrg (the c++11 version from c++11 branch,
# https://gitorious.org/dccrg) which also requires Zoltan
%.dexe: %.cpp $(HEADERS) Makefile
	@echo "MPICXX "$< && $(MPICXX) $(CPPFLAGS) $(CXXFLAGS) $(BOOST_CPPFLAGS) $(DCCRG_CPPFLAGS) $(ZOLTAN_CPPFLAGS) $(ZOLTAN_LDFLAGS) $(ZOLTAN_LIBS) $< -o $@


## Execution rules ##
%.tst: %.exe
	@echo "RUN "$< && $(RUN) ./$< && echo PASS && touch $@

%.mtst: %.mexe
	@echo "MPIRUN "$< && $(MPIRUN) ./$< && echo PASS && touch $@

%.etst: %.eexe
	@echo "MPIRUN "$< && $(MPIRUN) ./$< && echo PASS && touch $@

# these are launched serially but execute MPI themselves
%.mmtst: %.exe
	@echo "RUN "$< && $(RUN) ./$< "$(MPIRUN)" && echo PASS && touch $@


EXECUTABLES = \
  tests/compile/get_var_mpi_datatype_included.exe \
  tests/compile/one_variable.exe \
  tests/compile/two_variables.exe \
  tests/compile/many_variables.exe \
  tests/compile/one_variable_recursive.exe \
  tests/compile/many_variables_recursive.exe \
  tests/compile/identical_names.exe \
  tests/serial/one_variable.exe \
  tests/serial/many_variables.exe \
  tests/serial/one_variable_recursive.exe \
  tests/serial/many_variables_recursive.exe \
  tests/serial/operators/plus.exe \
  tests/serial/operators/minus.exe \
  tests/serial/operators/mul.exe \
  tests/serial/operators/div.exe \
  tests/serial/game_of_life/speed.exe \
  tests/serial/game_of_life/speed_reference.exe \
  tests/serial/game_of_life/main.exe \
  tests/parallel/particle_propagation/main.exe \
  examples/game_of_life/serial.exe \
  examples/game_of_life/non_cellular.exe \
  examples/advection/serial.exe \
  examples/particle_propagation/serial.exe

MPI_EXECS = \
  tests/compile/enable_if.mexe \
  tests/compile/get_var_mpi_datatype_included.mexe \
  tests/compile/cell_included.mexe \
  tests/compile/one_variable.mexe \
  tests/compile/two_variables.mexe \
  tests/compile/many_variables.mexe \
  tests/compile/one_variable_recursive.mexe \
  tests/compile/many_variables_recursive.mexe \
  tests/compile/identical_names.mexe \
  tests/serial/get_var_datatype_std.mexe \
  tests/serial/transfer_one_cell_one_variable.mexe \
  tests/serial/transfer_one_cell_many_variables.mexe \
  tests/serial/transfer_many_cells_one_variable.mexe \
  tests/serial/transfer_many_cells_many_variables.mexe \
  tests/serial/transfer_recursive.mexe \
  tests/parallel/one_variable.mexe \
  tests/parallel/many_variables.mexe \
  tests/parallel/memory_ordering.mexe

EIGEN_EXECS = \
  tests/compile/get_var_mpi_datatype_included.eexe \
  tests/serial/get_var_datatype_eigen.eexe \
  tests/parallel/eigen.eexe

DCCRG_EXECS = \
  tests/compile/dccrg/get_cell_mpi_datatype.dexe \
  tests/compile/dccrg/included.dexe \
  tests/compile/dccrg/instantiated.dexe \
  tests/compile/dccrg/initialized.dexe \
  tests/compile/dccrg/updated.dexe \
  tests/compile/dccrg/saved.dexe \
  tests/parallel/recursive_cell_gol/main.dexe \
  tests/parallel/particle_propagation/mpi_speed.dexe \
  tests/parallel/particle_propagation/mpi_speed_reference.dexe \
  examples/game_of_life/parallel/main.dexe \
  examples/game_of_life/parallel/gol2gnuplot.dexe \
  examples/advection/parallel/main.dexe \
  examples/advection/parallel/advection2gnuplot.dexe \
  examples/particle_propagation/parallel/main.dexe \
  examples/particle_propagation/parallel/particle2gnuplot.dexe \
  examples/combined/parallel.dexe

TESTS = \
  tests/serial/get_var_datatype_std.mtst \
  tests/serial/get_var_datatype_eigen.etst \
  tests/serial/one_variable.tst \
  tests/serial/many_variables.tst \
  tests/serial/one_variable_recursive.tst \
  tests/serial/many_variables_recursive.tst \
  tests/serial/transfer_one_cell_one_variable.mtst \
  tests/serial/transfer_one_cell_many_variables.mtst \
  tests/serial/transfer_many_cells_one_variable.mtst \
  tests/serial/transfer_many_cells_many_variables.mtst \
  tests/serial/transfer_recursive.mtst \
  tests/serial/operators/plus.tst \
  tests/serial/operators/minus.tst \
  tests/serial/operators/mul.tst \
  tests/serial/operators/div.tst \
  tests/serial/game_of_life/main.tst \
  tests/parallel/one_variable.mtst \
  tests/parallel/many_variables.mtst \
  tests/parallel/memory_ordering.mtst \
  tests/parallel/eigen.etst \
  tests/parallel/particle_propagation/main.mmtst

all: test

t: test
test: serial mpi eigen dccrg $(TESTS)
	@echo && echo "All tests passed."

serial: $(EXECUTABLES)

mpi: $(MPI_EXECS)

eigen: $(EIGEN_EXECS)

dccrg: $(DCCRG_EXECS)

d: data
data:
	@echo "CLEAN DATA" && rm -f \
	examples/advection/advection*dat \
	examples/advection/advection*png \
	examples/particle_propagation/particle*dat \
	examples/particle_propagation/particle*png \
	examples/*/parallel/*.dc \
	examples/*/parallel/*.png \
	examples/*/parallel/*.dat \
	examples/combined/*.dc \
	examples/combined/*.dat \
	examples/combined/*.png \
	tests/parallel/particle_propagation/*.dc \
	tests/parallel/particle_propagation/*.dat \
	tests/parallel/particle_propagation/*.png

c: clean
clean: data
	@echo "CLEAN" && rm -f $(EXECUTABLES) $(MPI_EXECS) $(EIGEN_EXECS) $(DCCRG_EXECS) $(TESTS)

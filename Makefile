CPPFLAGS = -I source
CXXFLAGS = -std=c++0x

include makefiles/macosx_macports

HEADERS = \
  source/gensimcell.hpp \
  source/gensimcell_impl.hpp \
  source/get_var_datatype.hpp

%.exe: %.cpp $(HEADERS) Makefile
	@echo "CXX "$< && $(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.exempi: %.cpp $(HEADERS) Makefile
	@echo "MPICXX "$< && $(MPICXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.tst: %.exe
	@echo "RUN "$< && ./$< && echo PASS && touch $@

%.tstmpi: %.exempi
	@echo "MPIRUN "$< && $(MPIRUN) ./$< && echo PASS && touch $@

EXECUTABLES = \
  tests/compile/get_var_datatype_included.exempi \
  tests/compile/cell_included.exe \
  tests/compile/one_variable.exe \
  tests/compile/two_variables.exe \
  tests/compile/many_variables.exe \
  tests/compile/one_variable_recursive.exe \
  tests/compile/many_variables_recursive.exe \
  tests/serial/get_var_datatype_std.exempi \
  tests/serial/one_variable.exe \
  tests/serial/many_variables.exe \
  tests/serial/one_variable_recursive.exe \
  tests/serial/many_variables_recursive.exe

TESTS = \
  tests/serial/get_var_datatype_std.tstmpi \
  tests/serial/one_variable.tst \
  tests/serial/many_variables.tst \
  tests/serial/one_variable_recursive.tst \
  tests/serial/many_variables_recursive.tst

all: test

t: test
test: $(EXECUTABLES) $(TESTS)
	@echo "All tests passed."

c: clean
clean:
	@echo "CLEAN" && rm -f $(EXECUTABLES) $(TESTS)

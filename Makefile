CPPFLAGS = -I source
CXXFLAGS = -std=c++0x

include makefiles/macosx_macports

HEADERS = \
  source/gensimcell.hpp \
  source/gensimcell_impl.hpp

%.exe: %.cpp $(HEADERS) Makefile
	@echo "CXX "$< && $(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.tst: %.exe
	@echo "RUN "$< && ./$< && echo PASS && touch $@

EXECUTABLES = \
  tests/compile/cell_included.exe \
  tests/compile/one_variable.exe \
  tests/compile/two_variables.exe \
  tests/compile/many_variables.exe \
  tests/compile/one_variable_recursive.exe \
  tests/compile/many_variables_recursive.exe \
  tests/serial/one_variable.exe \
  tests/serial/many_variables.exe \
  tests/serial/one_variable_recursive.exe \
  tests/serial/many_variables_recursive.exe

TESTS = \
  tests/serial/one_variable.tst \
  tests/serial/many_variables.tst \
  tests/serial/one_variable_recursive.tst \
  tests/serial/many_variables_recursive.tst

all: $(EXECUTABLES) $(TESTS)

t: test
test: $(TESTS)
	@echo "All tests passed."

c: clean
clean:
	@echo "CLEAN" && rm -f $(EXECUTABLES) $(TESTS)

CPPFLAGS = -I source
CXXFLAGS = -std=c++0x

include makefiles/macosx_macports

HEADERS = \
  source/gensimcell.hpp \
  source/gensimcell_impl.hpp

%.exe: %.cpp $(HEADERS) Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

EXECUTABLES = \
  tests/compile/cell_included.exe \
  tests/compile/one_variable.exe \
  tests/compile/two_variables.exe \
  tests/compile/many_variables.exe \
  tests/compile/one_variable_recursive.exe \
  tests/compile/many_variables_recursive.exe \
  tests/serial/one_variable.exe \
  tests/serial/many_variables.exe

all: $(EXECUTABLES)

t: test
test: $(EXECUTABLES)
	@for exe in $(EXECUTABLES); do echo "RUN "$$exe"...\t\t" && ./$$exe && echo "PASS"; done

c: clean
clean:
	rm -f $(EXECUTABLES)

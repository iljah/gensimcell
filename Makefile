CPPFLAGS = -I source
CXXFLAGS = -std=c++0x

include makefiles/macosx_macports

%.exe: %.cpp Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

EXECUTABLES = \
  tests/compile/cell_included.exe

all: $(EXECUTABLES)

t: test
test: $(EXECUTABLES)

c: clean
clean:
	rm -f $(EXECUTABLES)


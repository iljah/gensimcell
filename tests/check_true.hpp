#ifndef CHECK_TRUE_HPP
#define CHECK_TRUE_HPP

#include "cstdlib"
#include "iostream"

#define CHECK_TRUE(x) \
if (not (x)) { \
	std::cerr << __FILE__ << ":" << __LINE__ << std::endl; \
 	abort(); \
}

#endif

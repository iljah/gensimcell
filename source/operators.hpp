/*
+, -, * and / operators for generic simulation cell.

Copyright 2014 Ilja Honkonen
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

#ifndef GENSIMCELL_OPERATORS_HPP
#define GENSIMCELL_OPERATORS_HPP


namespace gensimcell {


#define GENSIMCELL_COMMA ,
#define GENSIMCELL_MAKE_FREE_OPERATOR(FREE_OPERATOR, MEMBER_OPERATOR) \
template <class Cell> Cell operator FREE_OPERATOR ( \
	Cell lhs GENSIMCELL_COMMA \
	const Cell& rhs \
) { \
	lhs MEMBER_OPERATOR rhs; \
	return lhs; \
} \
\
template < \
	class Cell GENSIMCELL_COMMA \
	class Scalar \
> Cell operator FREE_OPERATOR ( \
	Cell lhs GENSIMCELL_COMMA \
	const Scalar& rhs \
) { \
	lhs MEMBER_OPERATOR rhs; \
	return lhs; \
} \
\
template < \
	class Scalar GENSIMCELL_COMMA \
	class Cell \
> Cell operator FREE_OPERATOR ( \
	const Scalar& lhs GENSIMCELL_COMMA \
	Cell rhs \
) { \
	rhs MEMBER_OPERATOR lhs; \
	return rhs; \
}

GENSIMCELL_MAKE_FREE_OPERATOR(+, +=)
GENSIMCELL_MAKE_FREE_OPERATOR(-, -=)
GENSIMCELL_MAKE_FREE_OPERATOR(*, *=)
GENSIMCELL_MAKE_FREE_OPERATOR(/, /=)

#undef GENSIMCELL_MAKE_FREE_OPERATOR
#undef GENSIMCELL_COMMA

} // namespace


#endif // ifndef GENSIMCELL_OPERATORS_HPP

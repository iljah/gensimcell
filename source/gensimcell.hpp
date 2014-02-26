/*
Copyright (c) 2013, 2014, Ilja Honkonen
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

#ifndef GENSIMCELL_HPP
#define GENSIMCELL_HPP


#include "gensimcell_impl.hpp"


/*!
Namespace where everything related to gensimcell is defined.
*/
namespace gensimcell {


/*!
\todo Write documentation.
*/
template <
	class... Variables
> class Cell :
	public detail::Cell_impl<sizeof...(Variables), Variables...>
{
public:
	// allow the cell to be used as a variable
	using data_type = detail::Cell_impl<sizeof...(Variables), Variables...>;


	#if defined(MPI_VERSION) && (MPI_VERSION >= 2)

	// boost::tti has_member_function doesn't see the inherited one
	std::tuple<
		void*,
		int,
		MPI_Datatype
	> get_mpi_datatype() const
	{
		return detail::Cell_impl<
			sizeof...(Variables),
			Variables...
		>::get_mpi_datatype();
	}

	#endif // ifdef MPI_VERSION
};


} // namespace


#endif // ifndef GENSIMCELL_HPP

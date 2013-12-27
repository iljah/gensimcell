/*
Copyright (c) 2013, Ilja Honkonen
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of NASA nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

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

#ifndef GENSIMCELL_IMPL_HPP
#define GENSIMCELL_IMPL_HPP


#include "cstdlib"


namespace gensimcell {

/*!
Everything in this namespace should be considered as an
implementation detail that is subject to change without notice.
*/
namespace detail {



/*!
Generic version of the implementation that doesn't do anything.

See the version that stops the iteration
over Variables for documentation.
*/
template <
	size_t number_of_variables,
	class... Variables
> class Cell_impl {};



/*!
Starts or continues the iteration over variables.

See the version that stops the iteration for documentation.
*/
template <
	size_t number_of_variables,
	class Current_Variable,
	class... Rest_Of_Variables
> class Cell_impl<
	number_of_variables,
	Current_Variable,
	Rest_Of_Variables...
> :
	public Cell_impl<number_of_variables, Rest_Of_Variables...>
{

private:


	typename Current_Variable::data_type data;



public:


	Current_Variable& operator()(const Current_Variable&)
	{
		return this->data;
	};

	const Current_Variable& operator()(const Current_Variable&) const
	{
		return this->data;
	};

};



/*!
Stops the iteration over variables given by the user.

Operator () provides access to the user's data.
*/
template <
	size_t number_of_variables,
	class Variable
> class Cell_impl<
	number_of_variables,
	Variable
> {


private:


	typename Variable::data_type data;



public:


	Variable& operator()(const Variable&)
	{
		return this->data;
	}

	const Variable& operator()(const Variable&) const
	{
		return this->data;
	}
};



} // namespace detail
} // namespace gensimcell


#endif // ifndef GENSIMCELL_IMPL_HPP

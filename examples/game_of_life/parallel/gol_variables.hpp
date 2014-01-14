/*
Copyright (c) 2014, Ilja Honkonen
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


#ifndef GOL_VARIABLES_HPP
#define GOL_VARIABLES_HPP

#include "gensimcell.hpp"

//! see ../serial.cpp for the basics

/*!
Avoid collisions between the names of
variables of different simulations by
using a unique namespace for each one.
*/
namespace gol {

struct is_alive
{
	typedef bool data_type;
};

struct live_neighbors
{
	typedef int data_type;
};

typedef gensimcell::Cell<
	gol::is_alive,
	gol::live_neighbors
> cell_t;


/*!
Stops the MPI transfer of all variables
used in the game of life simulation.

The actual variables of the game of life
simulation are given as template parameters.
*/
template<
	class Cell_T,
	class Is_Alive,
	class Live_Neighbors
> void transfer_none()
{
	Cell_T::set_transfer_all(Is_Alive(), false);
	Cell_T::set_transfer_all(Live_Neighbors(), false);
}


/*!
Starts the MPI transfer of all variables
required in a parallel game of life simulation.

The actual variables of the game of life
simulation are given as template parameters.
*/
template<
	class Cell_T,
	class Is_Alive,
	class Live_Neighbors
> void transfer_all()
{
	Cell_T::set_transfer_all(Is_Alive(), true);
}


} // namespace

#endif // ifndef GOL_VARIABLES_HPP

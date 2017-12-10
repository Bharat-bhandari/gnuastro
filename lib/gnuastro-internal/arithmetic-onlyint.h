/*********************************************************************
Arithmetic operations on data structures.
This is part of GNU Astronomy Utilities (Gnuastro) package.

Original author:
     Mohammad Akhlaghi <akhlaghi@gnu.org>
Contributing author(s):
Copyright (C) 2015, Free Software Foundation, Inc.

Gnuastro is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

Gnuastro is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with Gnuastro. If not, see <http://www.gnu.org/licenses/>.
**********************************************************************/
#ifndef __GAL_ARITHMETIC_ONLYINT_H__
#define __GAL_ARITHMETIC_ONLYINT_H__


gal_data_t *
arithmetic_onlyint_binary(int operator, int flags, gal_data_t *lo,
                          gal_data_t *ro);

gal_data_t *
arithmetic_onlyint_bitwise_not(int flags, gal_data_t *in);


#endif

/*********************************************************************
NoiseChisel - Detect and segment signal in noise.
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
along with gnuastro. If not, see <http://www.gnu.org/licenses/>.
**********************************************************************/
#ifndef LABEL_H
#define LABEL_H

size_t
BF_concmp(unsigned char *byt, long *lab, size_t s0, size_t s1,
          const size_t connectivity);

size_t
BF_concomp_AdjMatrix(int *adj, size_t numside, long **outnewlabs);

void
removesmallarea_relabel(long *in, unsigned char *byt, size_t size,
			size_t *numlabs, size_t minarea);

void
detlabelsn(struct noisechiselparams *p, long *labinmesh, size_t numlabs,
           size_t start, size_t s0, size_t s1, float **outsntable);

#endif

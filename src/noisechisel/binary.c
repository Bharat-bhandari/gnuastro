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
#include <config.h>

#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <stdlib.h>

#include "main.h"

#include "label.h"
#include "fitsarrayvv.h"







/****************************************************************
 *****************          Counting         ********************
 ****************************************************************/
/* A region in a larger image is defined by its starting pixel
   (`start`), its height (s0) and width (s1). This function will count
   how many nonmasked (==0 in mask[]), foregound (==1 in byt[]) and
   background (==0 in byt[]) pixels there are in a given region of
   these two large arrays. mask[] and byt[] should have the same size
   and their width is here called `is1`. */
void
count_f_b_onregion(unsigned char *byt, size_t startind, size_t s0,
                   size_t s1, size_t is1, size_t *numf, size_t *numb)
{
  size_t nf=0, nb=0, r;
  unsigned char *b, *fb;

  for(r=0;r<s0;++r)
    {
      fb=(b=byt+startind)+s1;
      do
        *b ? ++nf : ++nb;
      while(++b<fb);
      startind+=is1;
    }
  *numf=nf;
  *numb=nb;
}




















/****************************************************************
 *****************      Dilate and Erode     ********************
 ****************************************************************/
/* 4 connected dilation and erosion: b0_f1==0: Dilate the
   foreground. b0_f1==1: Erode the foreground.  */
void
dilate0_erode1_4con(unsigned char *byt, size_t nr, size_t nc,
                    unsigned char b0_f1)
{
  size_t i, j, ind;
  unsigned char f, b, *pt, *fpt;

  /* Do a sanity check: */
  if(b0_f1!=1 && b0_f1!=0)
    error(EXIT_FAILURE, 0, "A bug! Please contact us at %s so we can fix "
          "this problem. In dilate0_erode1_4con (binary.c), the value to "
          "b0_f1 is %u while it should be 0 or 1.", PACKAGE_BUGREPORT, b0_f1);

  /* Set the foreground and background values. */
  if(b0_f1==0) {f=1; b=0;}
  else         {f=0; b=1;}

  /* Check the 4 corners: */
  if(byt[0]==b && (byt[1]==f || byt[nc]==f) )
    byt[0]=2;

  if(byt[nc-1]==b && (byt[nc-2]==f || byt[2*nc-1]==f))
    byt[nc-1]=2;

  if(byt[(nr-1)*nc]==b
     && (byt[(nr-2)*nc]==f || byt[(nr-1)*nc+1]==f) )
    byt[(nr-1)*nc]=2;

  if(byt[nr*nc-1]==b
     && (byt[nr*nc-2]==f || byt[nr*nc-1-nc]==f) )
    byt[nr*nc-1]=2;

  /* Check the 4 sides: */
  for(j=1;j<nc-1;++j)
    if(byt[j]==b
       && (byt[j+1]==f || byt[j-1]==f || byt[j+nc]==f) )
      byt[j]=2;

  for(j=1;j<nc-1;++j)
    {
      ind=(nr-1)*nc+j;
      if(byt[ind]==b
	 && (byt[ind+1]==f || byt[ind-1]==f || byt[ind-nc]==f) )
	byt[ind]=2;
    }

  for(i=1;i<nr-1;++i)
    {
      ind=i*nc;
      if(byt[ind]==b
	 && (byt[ind+1]==f || byt[ind+nc]==f || byt[ind-nc]==f) )
	byt[ind]=2;
    }

  for(i=1;i<nr-1;++i)
    {
      ind=(i+1)*nc-1;
      if(byt[ind]==b
	 && (byt[ind-1]==f || byt[ind+nc]==f || byt[ind-nc]==f) )
	byt[ind]=2;
    }

  /* Check the body: */
  for(i=1;i<nr-1;++i)
    for(j=1;j<nc-1;++j)
      {
	ind=i*nc+j;
	if(byt[ind]==b
	   && (byt[ind-1]==f     || byt[ind+1]==f
	       || byt[ind+nc]==f || byt[ind-nc]==f) )
	  byt[ind]=2;
      }

  /* Set all the changed pixels to the proper values: */
  fpt=(pt=byt)+nr*nc;
  do *pt = *pt==2 ? f : *pt; while(++pt<fpt);
}





/* 8 connected dilation and erosion. b0_f1==0: Dilate the
   foreground. b0_f1==1: Erode the foreground. */
void
dilate0_erode1_8con(unsigned char *byt, size_t nr, size_t nc,
                    unsigned char b0_f1)
{
  size_t i, j, ind;
  unsigned char f, b, *pt, *fpt;

  /* Do a sanity check: */
  if(b0_f1!=1 && b0_f1!=0)
    error(EXIT_FAILURE, 0, "A bug! Please contact us at %s so we can fix "
          "this problem. In dilate0_erode1_4con (binary.c), the value to "
          "b0_f1 is %u while it should be 0 or 1.", PACKAGE_BUGREPORT, b0_f1);

  /* Set the foreground and background values: */
  if(b0_f1==0) {f=1; b=0;}
  else         {f=0; b=1;}

  /* Check the 4 corners: */
  if(byt[0]==b && (byt[1]==f
		   || byt[nc]==f || byt[nc+1]==f) )
    byt[0]=2;

  if(byt[nc-1]==b && (byt[nc-2]==f
		      || byt[2*nc-1]==f
		      || byt[2*nc-2]==f) )
    byt[nc-1]=2;

  if(byt[(nr-1)*nc]==b
     && ( byt[(nr-2)*nc]==f || byt[(nr-1)*nc+1]==f
	  || byt[(nr-2)*nc+1]==f) )
    byt[(nr-1)*nc]=2;

  if(byt[nr*nc-1]==b
     && ( byt[nr*nc-2]==f || byt[nr*nc-1-nc]==f
	  || byt[nr*nc-2-nc]==f) )
    byt[nr*nc-1]=2;

  /* Check the 4 sides: */
  for(j=1;j<nc-1;++j)
    if(byt[j]==b
       && ( byt[j+1]==f || byt[j-1]==f || byt[j+nc]==f
	    || byt[j-1+nc]==f || byt[j+1+nc]==f) )
      byt[j]=2;

  for(j=1;j<nc-1;++j)
    {
      ind=(nr-1)*nc+j;
      if(byt[ind]==b
	 && ( byt[ind+1]==f || byt[ind-1]==f || byt[ind-nc]==f
	      || byt[ind-1-nc]==f || byt[ind+1-nc]==f) )
	byt[ind]=2;
    }

  for(i=1;i<nr-1;++i)
    {
      ind=i*nc;
      if(byt[ind]==b
	 && ( byt[ind+1]==f || byt[ind+nc]==f || byt[ind-nc]==f
	      || byt[ind+1-nc]==f || byt[ind+1+nc]==f) )
	byt[ind]=2;
    }

  for(i=1;i<nr-1;++i)
    {
      ind=(i+1)*nc-1;
      if(byt[ind]==b
	 && (byt[ind-1]==f || byt[ind+nc]==f || byt[ind-nc]==f
	     || byt[ind-1-nc]==f || byt[ind-1+nc]==f) )
	byt[ind]=2;
    }

  /* Check the body: */
  for(i=1;i<nr-1;++i)
    for(j=1;j<nc-1;++j)
      {
	ind=i*nc+j;
	if(byt[ind]==b
	   && (byt[ind-1]==f        || byt[ind+1]==f
	       || byt[ind+nc]==f    || byt[ind-nc]==f
	       || byt[ind-1-nc]==f  || byt[ind+1+nc]==f
	       || byt[ind-1+nc]==f  || byt[ind+1+nc]==f) )
	  byt[ind]=2;
      }

  /* Set all the changed pixels to the proper values: */
  fpt=(pt=byt)+nr*nc;
  do *pt = *pt==2 ? f : *pt; while(++pt<fpt);
}






/* Opening: erode n times then dilate n times. */
void
opening(unsigned char *byt, size_t s0, size_t s1,
        size_t depth, int con_type)
{
  size_t i;
  void (*func)(unsigned char *, size_t, size_t, unsigned char);

  /* Sanity check: */
  if(con_type!=4 && con_type!=8)
    error(EXIT_FAILURE, 0, "A bug! Please contact us at %s so we can fix "
          "this problem. For some reason, the value to con_type in opening "
          "(binary.c) is %d while it should be 4 or 8.", PACKAGE_BUGREPORT,
          con_type);

  /* Do the opening: */
  if(con_type==4)
    func=dilate0_erode1_4con;
  else func=dilate0_erode1_8con;

  for(i=0;i<depth;++i)
    func(byt, s0, s1, 1);

  for(i=0;i<depth;++i)
    func(byt, s0, s1, 0);
}




















/****************************************************************
 *****************          Fill holes       ********************
 ****************************************************************/
void
fillleftside(unsigned char *inv, size_t idx, size_t idy,
             size_t maxfill)
{
  size_t i, j, min_o, end, index;

  min_o=1;
  end=idx-2;
  for(i=2;i<end;++i)
    {
      index=i*idy+2;
      if(inv[index]==1 && inv[index+idy]==0)
        {
	  if(i+1-min_o<maxfill)
	    for(j=min_o;j<=i+1;++j)
	      inv[j*idy+1]=2;
	  min_o=i+1;
        }
      else if(inv[index]==0 && inv[index+idy]==1)
	min_o=i;
    }
  if(min_o!=1 && end-min_o<maxfill)
    for(j=min_o;j<=end;++j)
      inv[j*idy+1]=2;
}





void
fillbottomside(unsigned char *inv, size_t idy,
               size_t maxfill)
{
  size_t i, j, min_o, end, index;

  min_o=1;
  end=idy-2;
  for(i=2;i<end;++i)
    {
      index=2*idy+i;
      if(inv[index]==1 && inv[index+1]==0)
        {
	  if(i+1-min_o<maxfill)
	    for(j=min_o;j<=i+1;++j)
	      inv[idy+j]=2;
	  min_o=i+1;
        }
      else if(inv[index]==0 && inv[index+1]==1)
	min_o=i;
    }
  if(min_o!=1 && end-min_o<maxfill)
    for(j=min_o;j<=end;++j)
      inv[idy+j]=2;
}





void
fillrightside(unsigned char *inv, size_t idx, size_t idy,
              size_t maxfill)
{
  size_t i, j, min_o, end, index;

  min_o=1;
  end=idx-2;
  for(i=2;i<end;++i)
    {
      index=i*idy+idy-3;
      if(inv[index]==1 && inv[index+idy]==0)
        {
	  if(i+1-min_o<maxfill)
	    for(j=min_o;j<=i+1;++j)
	      inv[j*idy+idy-2]=2;
	  min_o=i+1;
        }
      else if(inv[index]==0 && inv[index+idy]==1)
	min_o=i;
    }
  if(min_o!=1 && end-min_o<maxfill)
    for(j=min_o;j<=end;++j)
      inv[j*idy+idy-2]=2;
}





void
filltopside(unsigned char *inv, size_t idx, size_t idy,
            size_t maxfill)
{
  size_t i, j, min_o, end, index;

  min_o=1;
  end=idy-2;
  for(i=2;i<end;++i)
    {
      index=(idx-3)*idy+i;
      if(inv[index]==1 && inv[index+1]==0)
        {
	  if(i+1-min_o<maxfill)
	    for(j=min_o;j<=i+1;++j)
	      inv[(idx-2)*idy+j]=2;
	  min_o=i+1;
        }
      else if(inv[index]==0 && inv[index+1]==1)
	min_o=i;
    }
  if(min_o!=1 && end-min_o<maxfill)
    for(j=min_o;j<=end;++j)
      inv[(idx-2)*idy+j]=2;
}





/* Make the array that is the inverse of the input byt of fill
   holes. The inverse array will also be 4 pixels larger in both
   dimensions. This is because we also want to fill those holes that
   are touching the side of the image. One pixel for a pixel that is
   one pixel away from the image border. Another pixel for those
   objects that are touching the image border. */
void
fh_makeinv(unsigned char *byt, size_t s0, size_t s1,
	   unsigned char **inv, size_t *oidx, size_t *oidy,
	   size_t l, size_t b, size_t r, size_t t)
{
  unsigned char *tinv;
  size_t i, j, idx, idy, size, tdiff=2;

  idx=s0+2*tdiff;
  idy=s1+2*tdiff;
  size=idx*idy;

  /* Allocate the temporary inverse array: */
  errno=0; tinv=malloc(size*sizeof *tinv);
  if(tinv==NULL)
    error(EXIT_FAILURE, errno, "%lu bytes for tinv (label.c)",
          size*sizeof *tinv);

  for(i=0;i<size;++i) tinv[i]=1;
  for(i=0;i<s0;++i)
    for(j=0;j<s1;++j)
      if(byt[i*s1+j]==1)
	tinv[(i+tdiff)*idy+j+tdiff]=0;

  if(l!=0) fillleftside(tinv, idx, idy, l);
  if(b!=0) fillbottomside(tinv, idy, b);
  if(r!=0) fillrightside(tinv, idx, idy, r);
  if(t!=0) filltopside(tinv, idx, idy, t);

  if(l!=0 || b!=0 || r!=0 || t!=0)
    for(i=0;i<size;++i)
      if(tinv[i]==2) tinv[i]=0;

  *inv=tinv;
  *oidx=idx;
  *oidy=idy;
}





/* Fill all the holes in an input unsigned char array that are bounded
   within a 4-connected region.

   The basic method is this:

   1. An inverse image is created:

        * For every pixel in the input that is 1, the inverse is 0.

        * The inverse image has two extra pixels on each edge to
          ensure that all the inv[i]==1 pixels around the image are
          touching each other and a diagonal object passing through
          the image does not cause the inv[i]==1 pixels on the edges
          of the image to get a different label.

   2. The 8 connected regions in this inverse image are found.

   3. Since we had a 2 pixel padding on the edges of the image, we
      know for sure that all labeled regions with a label of 1 are
      actually connected `holes' in the input image.

      Any pixel with a label larger than 1, is therefore a bounded
      hole that is not 8-connected to the rest of the holes.

*/
void
fillboundedholes(unsigned char *in, size_t s0, size_t s1)
{
  long *hlab;
  unsigned char *inv;
  size_t i, j, idx, idy, diff;


  /* Make the inverse array: */
  fh_makeinv(in, s0, s1, &inv, &idx, &idy, 0, 0, 0, 0);
  diff=(idx-s0)/2;


  /* Allocate the array to keep the hole indexs */
  errno=0; hlab=calloc(idx*idy, sizeof *hlab);
  if(hlab==NULL)
    error(EXIT_FAILURE, errno, "%lu bytes for hlab in filllabeledholes "
          "(label.c)", idx*idy*sizeof *hlab);


  /* Find the hole labels */
  BF_concmp(inv, hlab, idx, idy, 8);


  /* For a check
  arraytofitsimg("fbh.fits", "INPUT", BYTE_IMG, in,
                 s0, s1, 0, NULL, NULL, "tmp");
  arraytofitsimg("fbh.fits", "INV", BYTE_IMG, inv,
                 idx, idy, 0, NULL, NULL, "tmp");
  arraytofitsimg("fbh.fits", "HLAB", LONG_IMG, hlab,
                 idx, idy, 0, NULL, NULL, "tmp");
  */

  /* Correct the labels: */
  for(i=diff;i<idx-diff;++i)	/* Note that holes will always  */
    for(j=diff;j<idy-diff;++j)	/* be in the inner diff pixels. */
      if(hlab[i*idy+j]>1)
	in[(i-diff)*s1+j-diff]=1;

  /* For a check:
  arraytofitsimg("fbh.fits", "INPUT", BYTE_IMG, in,
                 s0, s1, 0, NULL, NULL, "tmp");
  */

  free(inv);
  free(hlab);
}

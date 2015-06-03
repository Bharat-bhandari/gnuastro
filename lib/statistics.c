/*********************************************************************
Statistical functions.
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

#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <float.h>
#include <stdlib.h>

#include "forqsort.h"
#include "statistics.h"
#include "arraymanip.h"









/****************************************************************
 *****************    Mininum and Maximum    ********************
 ****************************************************************/
void
floatmin(float *in, size_t size, float *min)
{
  float tmin=FLT_MAX, *fpt;
  fpt=in+size;
  do   /* Works for NAN, since NAN is not smaller than any number. */
    if(*in<tmin) tmin=*in;
  while(++in<fpt);
  *min=tmin;
}





void
floatmax(float *in, size_t size, float *max)
{
  float tmax=-FLT_MAX, *fpt;
  fpt=in+size;
  do /* Works for NAN, since NAN is not larger than any number. */
    if(*in>tmax) tmax=*in;
  while(++in<fpt);
  *max=tmax;
}





void
doublemin(double *in, size_t size, double *min)
{
  double tmin=FLT_MAX, *fpt;
  fpt=in+size;
  do   /* Works for NAN, since NAN is not smaller than any number. */
    if(*in<tmin) tmin=*in;
  while(++in<fpt);
  *min=tmin;
}





void
doublemax(double *in, size_t size, double *max)
{
  double tmax=-FLT_MAX, *fpt;
  fpt=in+size;
  do   /* Works for NAN, since NAN is not larger than any number. */
    if(*in>tmax) tmax=*in;
  while(++in<fpt);
  *max=tmax;
}




void
floatmaxmasked(float *in, unsigned char *mask, size_t size, float *max)
{
  float tmax=-FLT_MAX, *fpt;
  fpt=in+size;
  do
    if(*mask++==0 && *in>tmax)
      tmax=*in;
  while(++in<fpt);
  *max=tmax;
}





void
floatsecondmax(float *in, size_t size, float *secondmax)
{
  float smax=-FLT_MAX, max=-FLT_MAX, *fpt;
  fpt=in+size;
  do
    { /* Works for NAN, since NAN is not larger than any number. */
      if(*in>max)
	{
	  smax=max;
	  max=*in;
	}
      else if(*in>smax) smax=*in;
    }
  while(++in<fpt);
  *secondmax=smax;
}





void
floatsecondmin(float *in, size_t size, float *secondmin)
{
  float smin=FLT_MAX, min=FLT_MAX, *fpt;
  fpt=in+size;
  do
    { /* Works for NAN, since NAN is not smaller than any number. */
      if(*in<min)
	{
	  smin=min;
	  min=*in;
	}
      else if(*in<smin) smin=*in;
    }
  while(++in<fpt);
  *secondmin=smin;
}





void
fminmax(float *in, size_t size, float *min, float *max)
{
  float tmin=FLT_MAX, tmax=-FLT_MAX, *f, *fpt;

  fpt=(f=in)+size;
  do
    {   /* Works for NAN, because NaN values are not greater or
           smaller than any number. */
      if (*f>tmax) tmax=*f;
      if (*f<tmin) tmin=*f;
    }
  while(++f<fpt);

  /* If the whole data was a NaN, then tmin and tmax did not change
     from their initial values. */
  if(tmin==FLT_MAX || tmax==-FLT_MAX)
    *min=*max=NAN;
  else
    {
      *max=tmax;
      *min=tmin;
    }
}





void
dminmax(double *in, size_t size, double *min, double *max)
{
  double tmin=FLT_MAX, tmax=-FLT_MAX, *d, *dpt;

  dpt=(d=in)+size;
  do
    { /* Works for NAN */
      if (*d>tmax) tmax=*d;
      if (*d<tmin) tmin=*d;
    }
  while(++d<dpt);

  /* If all the data was a NaN */
  if(tmin==FLT_MAX || tmax==-FLT_MAX)
    *min=*max=NAN;
  else
    {
      *max=tmax;
      *min=tmin;
    }
}




void
dmax_withindex(double *in, size_t size, double *max, size_t *index)
{
  size_t tindex=0;
  double *fpt, *pt=in, tmax=-FLT_MAX;

  fpt=pt+size;
  do  /*  Works for NAN, see comments above. */
    if(*pt>tmax)
      {
	tmax=*pt;
	tindex=pt-in;
      }
  while(++pt<fpt);
  *index=tindex;
  *max=tmax;
}





void
fmax_withindex(float *in, size_t size,
	       float *max, size_t *index)
{
  size_t tindex=0;
  float *pt=in, *fpt, tmax=-FLT_MAX;

  fpt=pt+size;
  do  /* Works for NAN, see comments above.x */
    if(*pt>tmax)
      {
	tmax=*pt;
	tindex=pt-in;
      }
  while(++pt<fpt);
  *index=tindex;
  *max=tmax;
}





void
dmin_withindex(double *in, size_t size,
	       double *min, size_t *index)
{
  size_t tindex=0;
  double *pt=in, *fpt, tmin=FLT_MAX;

  fpt=pt+size;
  do  /* Works for NAN, see comments above. */
    if(*pt<tmin)
      {
	tmin=*pt;
	tindex=pt-in;
      }
  while(++pt<fpt);
  *index=tindex;
  *min=tmin;
}





void
fmin_withindex(float *in, size_t size,
	       float *min, size_t *index)
{
  size_t tindex=0;
  float *pt=in, *fpt, tmin=FLT_MAX;

  fpt=pt+size;
  do /* Works for NAN, see comments above. */
    if(*pt<tmin)
      {
	tmin=*pt;
	tindex=pt-in;
      }
  while(++pt<fpt);
  *index=tindex;
  *min=tmin;
}




















/****************************************************************
 *****************            Sum            ********************
 ****************************************************************/
float
floatsum(float *in, size_t size)
{
  float *fpt;
  double sum=0;
  fpt=in+size;
  do
    if(!isnan(*in))
      sum+=*in;
  while(++in<fpt);
  return sum;
}





float
floatsumsquared(float *in, size_t size)
{
  float *fpt;
  double sum=0;
  fpt=in+size;
  do
    if(!isnan(*in))
      sum+=*in * *in;
  while(++in<fpt);
  return sum;
}





/* Sum over all elements of the array that are not covered by a
   mask. Any non-zero masked pixel is considered to be a masked
   pixel. */
float
floatsummask(float *in, unsigned char *mask,
	     size_t size, size_t *nsize)
{
  double sum=0;
  size_t counter=0;
  unsigned char *pt=mask, *fpt;

  fpt=pt+size;
  do
    if(*pt==0)
      {
	sum+=in[pt-mask];
	++counter;
      }
  while(++pt<fpt);

  *nsize=counter;
  return sum;
}





float
floatsummaskl(float *in, long *mask,
              size_t size, size_t *nsize)
{
  double sum=0;
  size_t counter=0;
  long *pt=mask, *fpt;

  fpt=pt+size;
  do
    if(*pt==0)
      {
	sum+=in[pt-mask];
	++counter;
      }
  while(++pt<fpt);

  *nsize=counter;
  return sum;
}





float
floatsumsquaredmask(float *in, unsigned char *mask,
                    size_t size, size_t *nsize)
{
  double sum=0;
  size_t counter=0;
  unsigned char *pt=mask, *fpt;

  fpt=pt+size;
  do
    if(*pt==0)
      {
	sum+=in[pt-mask] * in[pt-mask];
	++counter;
      }
  while(++pt<fpt);

  *nsize=counter;
  return sum;
}






float
floatsumsquaredmaskl(float *in, long *mask,
                     size_t size, size_t *nsize)
{
  double sum=0;
  size_t counter=0;
  long *pt=mask, *fpt;

  fpt=pt+size;
  do
    if(*pt==0)
      {
	sum+=in[pt-mask] * in[pt-mask];
	++counter;
      }
  while(++pt<fpt);

  *nsize=counter;
  return sum;
}





















/****************************************************************
 *****************      Average and          ********************
 ****************    Standard deviation      ********************
 ****************************************************************/
void
fave(float *in, size_t size, float *ave, unsigned char *mask)
{
  float sum;
  size_t nsize;
  if(mask==NULL)
    sum=floatsum(in, size);
  else
    {
      sum=floatsummask(in, mask, size, &nsize);
      size=nsize;
    }
  *ave=sum/size;
}





void
favel(float *in, size_t size, float *ave, long *mask)
{
  float sum;
  size_t nsize;
  if(mask==NULL)
    sum=floatsum(in, size);
  else
    {
      sum=floatsummaskl(in, mask, size, &nsize);
      size=nsize;
    }
  *ave=sum/size;
}





/* Find the average and standard deviation of an array, assuming that
   there is a mask array. Any mask array pixel that is not zero will
   not be included in the average and standard deviations.  Here the
   mask is assumed to be unsigned char.  */
void
favestd(float *in, size_t size, float *ave, float *std, unsigned char *mask)
{
  size_t nsize1, nsize2;
  float sum, sum2;
  if(mask)
    {
      sum=floatsummask(in, mask, size, &nsize1);
      sum2=floatsumsquaredmask(in, mask, size, &nsize2);
      if(nsize1!=nsize2)
	error(EXIT_FAILURE, 0, "A bug in favestd (lib/statistics.h). "
	      "Somehow the number of masked pixels is measured "
	      "differently. Please contact us so we can find the cause.");
      size=nsize1;
    }
  else
    {
      sum=floatsum(in, size);
      sum2=floatsumsquared(in, size);
    }
  *ave=sum/size;
  *std=sqrt( (sum2-sum*sum/size)/size );
}





/* Similar to favestd, but when the mask is assumed to be a long
   array.  */
void
favestdl(float *in, size_t size, float *ave, float *std, long *mask)
{
  size_t nsize1, nsize2;
  float sum, sum2;
  if(mask==NULL)
    {
      sum=floatsum(in, size);
      sum2=floatsumsquared(in, size);
    }
  else
    {
      sum=floatsummaskl(in, mask, size, &nsize1);
      sum2=floatsumsquaredmaskl(in, mask, size, &nsize2);
      if(nsize1!=nsize2)
	error(EXIT_FAILURE, 0, "A bug in favestl (lib/statistics.h). "
	      "Somehow the number of masked pixels is measured "
	      "differently. Please contact us so we can find the cause.");
      size=nsize1;
    }
  *ave=sum/size;
  *std=sqrt( (sum2-sum*sum/size)/size );
}





/* Find the average and standard deviation of all pixels on a
   region. A region in a larger image is defined by its starting pixel
   (`start`), its height (s0) and width (s1). This function will find
   the sum and sumsquared of all the nonmasked (==0 in mask[]) and non
   marked (==0 in byt) pixels in the region. */
void
floatavestdmaskbyt0inregion(float *in, unsigned char *byt,
			    unsigned char *mask, size_t startind,
			    size_t s0, size_t s1, size_t is1,
			    float *ave, float *std)
{
  float *i;
  size_t r, size=0;
  double sum=0, sumsq=0;
  unsigned char *m, *b, *fb;

  for(r=0;r<s0;++r)
    {
      i=in+startind;
      m=mask+startind;
      fb=(b=byt+startind)+s1;
      do
	{
	if(*m++==0 && *b==0)	/* `m` will definitely be checked and */
	  {			/* incremented, while `b` might not.  */
	    size++;
	    sum   += *i;
	    sumsq += *i * *i;
	  }
	++i;
	}
      while(++b<fb);
      startind+=is1;
    }

  *ave = sum/size;
  *std = sqrt( (sumsq-sum*sum/size)/size );
}




















/****************************************************************
 ********     Histogram and Cumulative Frequency Plot     *******
 ****************************************************************/
/* Set the bin lower values for all the bins. If the minimum and
   maximum are equal, then use the quantile. */
void
setbins(float *sorted, size_t size, size_t numbins, float min,
	float max, int binonzero, float quant, float **obins)
{
  size_t i;
  float tosubtract, *bins, binwidth;

  /* Allocate space for the array. The extra bin is only for internal
     purposes (so the loops for the histogram and CFP can see the end
     of the last bin). It will never be seen by the user. */
  errno=0;
  bins=*obins=calloc((numbins+1)*2, sizeof *bins);
  if(bins==NULL)
    error(EXIT_FAILURE, errno, "%lu bytes for bins in setbins (statistics.c)",
          (numbins+1)*2*sizeof *bins);

  /* If the range is not defined, find it and set the bin width. */
  if(min==max)
    {
      if(quant!=0.0f)
	{
	  min=sorted[ indexfromquantile(size, quant)   ];
	  max=sorted[ indexfromquantile(size, 1-quant) ];
	}
      else
	{
	  min=sorted[0];
	  max=sorted[size-1];
	}
    }
  binwidth=(max-min)/numbins;

  /* Set all the bin smaller sides: */
  for(i=0;i<numbins+1;++i)
    bins[i*2]=min+i*binwidth;

  /* If one bin is to be placed on zero. */
  if(binonzero)
    {
      /* Go over all the bins and stop when the sign of the two sides
         of one bin are different. */
      for(i=0;i<numbins;++i)
	if(bins[i*2]*bins[(i+1)*2]<0.0f) break;
      if(i!=numbins)
        {
          tosubtract=bins[(i+1)*2];
          for(i=0;i<numbins+1;++i)
            bins[i*2]-=tosubtract;
        }
    }

  /* For a check
  for(i=0;i<numbins;++i)
    printf("%lu: %.4f\n", i+1, bins[i*2]);
  */
}





void
histogram(float *sorted, size_t size, float *bins, size_t numbins,
	  int normhist, int maxhistone)
{
  float max=-FLT_MAX;
  size_t histrow=0, i;

  if((long)numbins<=0)
    error(EXIT_FAILURE, 0, "The number of bins in histogram (statistics.h) "
          "must be >0. You have given asked for %ld.", (long)numbins);

  /* Fill the histogram. */
  for(i=0;i<size;++i)
    {
      /* For a check:
      printf("%f:\n  histrow: %lu, numbins: %lu\n",
             sorted[i], histrow, numbins);
      */
      /* Data has not reached bins yet: */
      if(sorted[i]<bins[histrow*2]) continue;

      /* Jump bins until sorted[i] is smaller than the larger value of
         one bin. If we are on the last bin (where
         histrow==numbins-1), then you don't have to increase histrow
         any more, as soon as sorted[i] becomes larger than the lagest
         bin, quit the search. The 1e-6f is to account for floating
         point error. NOTE: the last interval is closed on both
         sides.*/
      if(histrow==numbins-1)
        { if(sorted[i]>bins[numbins*2]+1e-6f) break; }
      else
        while(sorted[i]>=bins[(histrow+1)*2])
          if(++histrow==numbins-1) break;

      /* For a check:
      printf("  histrow: %lu\n", histrow);
      */
      ++bins[histrow*2+1];
    }

  /* In case a normalized histogram is desired: */
  if(normhist)
    for(i=0;i<numbins;++i)
      bins[i*2+1]/=size;

  /* In case the maximum value is to become one. */
  if(maxhistone)
    {
      for(i=0;i<numbins;++i)
	if(bins[i*2+1]>max)
	  max=bins[i*2+1];
      for(i=0;i<numbins;++i)
	bins[i*2+1]/=max;
    }

  /* In case you want to see the histogram:
  for(i=0;i<numbins;++i)
    printf("%lu: %.4f %.4F\n", i+1, bins[i*2], bins[i*2+1]);
  */
}





void
cumulativefp(float *sorted, size_t size, float *bins, size_t numbins,
	     int normcfp)
{
  float prevind=0;
  size_t cfprow=0, i, numinds=0;


  /* Fill the Cumulative frequency plot. The steps are just like the
     histogram above so we won't explain similar concepts here
     again. */
  for(i=0;i<size;++i)
    {
      if(sorted[i]<bins[cfprow*2]) continue;

      if(cfprow==numbins-1)
        { if(sorted[i]>bins[numbins*2]+1e-6f) break; }
      else
        while(sorted[i]>=bins[(cfprow+1)*2])
          {
            /* We have not yet reached the last bin. But we have
               reached the sorted[i] that is larger than the current
               bin and we want to switch to the next bin. So we have
               to finalize the current bin value.  bins[cfprow*2+1]
               has kept the sum of all the indexs that lie within this
               bin. Using numinds, we also kept a count of how many
               indexs there was.

               In case there were no indexs in this bin, then we have
               to pass on the previous value (since this bin did not
               contribute any data). We set numinds=0 and start
               working on the next bin. */
            if(numinds>0)
              prevind=bins[cfprow*2+1]/=numinds; /* Divide by num indexs */
            else
              bins[cfprow*2+1]=prevind;
            numinds=0;
            if(++cfprow==numbins-1) break;
          }

      /* We can't plot every index, also in every bin, there might be
         a sharp gradient. So in order to make things smooth, the
         value given to each bin will be the average of all the indexs
         that like over that bin. Here we keep the sum and number of
         indexs so in the end we can find the average. */
      bins[cfprow*2+1]+=i;
      ++numinds;
    }


  /* The last bin was not finalized. So we will do that here: */
  bins[cfprow*2+1] = numinds>0 ? bins[cfprow*2+1]/numinds : prevind;


  /* For a normalized CFP: */
  if(normcfp)
    for(i=0;i<numbins;++i)
      bins[i*2+1]/=size;


  /* In a CFP, all bins that are possibly left behind (there was no
     data to fill them) should get the same value as the lastly filled
     value. They should not be zero. Note that this has to be done
     after the possible normalization. */
  for(i=cfprow;i<numbins;++i)
    bins[i*2+1]=bins[(cfprow-1)*2+1];


  /* In case you want to see the CFP:
  for(i=0;i<numbins;++i)
    printf("%lu: %.4f %.4F\n", i+1, bins[i*2], bins[i*2+1]);
  */
}





void
cumulativefp_old(float *sorted, size_t size, float *bins, size_t numbins,
	     int normcfp)
{
  int lastrow=0;
  float prevind=0;
  size_t cfprow=0, i, numinds=0;

  /* Fill the Cumulative frequency plot: */
  for(i=0;i<size;++i)
    {
      if(sorted[i]<bins[cfprow*2]) continue;
      while ( (lastrow && sorted[i]>bins[(cfprow+1)*2]+1e-6f)
              || sorted[i]>=bins[(cfprow+1)*2] )
	{
	  if(numinds>0)
	    prevind=bins[cfprow*2+1]/=numinds; /* Divide by num indexs */
	  else
	    bins[cfprow*2+1]=prevind;
	  numinds=0;
	  if(++cfprow>=numbins)
	    break;              /* To break out of this while loop. */
          else if (cfprow==numbins-1) lastrow=1;
	}
      if(cfprow>=numbins) break; /* To break out of the for loop.   */
      bins[cfprow*2+1]+=i;	/* Sum of indexs (see above for average) */
      ++numinds;
    }

  /* For a normalized CFP: */
  if(normcfp)
    for(i=0;i<numbins;++i)
      bins[i*2+1]/=size;

  /* In a CFP, all bins that are possibly left behind (there was no
     data to fill them) should get the same value as the lastly filled
     value. They should not be zero. Note that this has to be done
     after the possible normalization. */
  for(i=cfprow;i<numbins;++i)
    bins[i*2+1]=bins[(cfprow-1)*2+1];

  /* In case you want to see the CFP:
  for(i=0;i<numbins;++i)
    printf("%lu: %.4f %.4F\n", i+1, bins[i*2], bins[i*2+1]);
  */
}





void
savehist(float *sorted, size_t size, size_t numbins,
	 char *filename, char *comment)
{
  FILE *fp;
  size_t i;
  int binonzero=0, normhist=0, maxhistone=0;
  float d, *bins, min=0.0f, max=0.0f, quant=0.0f;

  /* Set the bin sides: */
  setbins(sorted, size, numbins, min, max, binonzero, quant, &bins);

  /* Set the size of half a bin width:*/
  d=(bins[2]-bins[0])/2;

  /* Fill the histogram: */
  histogram(sorted, size, bins, numbins, normhist, maxhistone);

  /* Open the file for writing and save the histogram: */
  errno=0;
  fp=fopen(filename, "w");
  if(fp==NULL)
    error(EXIT_FAILURE, errno, "Couldn't open file %s", filename);
  fprintf(fp, "%s\n", comment);
  fprintf(fp, "# The input %lu points binned in %lu bins\n#\n",
          size, numbins);
  fprintf(fp, "# Column 0: Value in the middle of this bin.\n");
  fprintf(fp, "# Column 1: Number of points in this bin.\n");
  for(i=0;i<numbins;++i)
    fprintf(fp, "%-15.6f%.0f\n", bins[i*2]+d, bins[i*2+1]);
  fclose(fp);
  free(bins);
}




















/****************************************************************
 *****************         Quantiles         ********************
 ****************************************************************/
/* Find the index corresponding to a certain quantile, considering the
   rounding that might be needed. */
size_t
indexfromquantile(size_t size, float quant)
{
  float floatindex;

  if(quant>1.0f)
    error(EXIT_FAILURE, 0, "The quantile in indexfromquantile (statistics.c) "
          "Should be smaller.");

  /* Find the index of the quantile. */
  floatindex=(float)size*quant;
  /*
  printf("quant: %f, size: %lu, findex: %f\n", quant, size, floatindex);
  */
  /* Note that in the conversion from float to size_t, the floor
     integer value of the float will be used. */
  if( floatindex - (int)floatindex > 0.5 )
    return floatindex+1;
  else
    return floatindex;
}




















/****************************************************************
 *****************       Sigma clip          ********************
 ****************************************************************/
/* This function will repeatedly sigma clip the data and return the
   median. It is assumed that the data have been ordered.

   o1_n0: Ordered (1), not ordered (0).
*/
int
sigmaclip_converge(float *array, int o1_n0, size_t num_elem,
		   float sigma_multiple, float accuracy, float *outave,
                   float *outmed, float *outstd, int print)
{
  size_t counter=0;
  float *start, *oldstart, *dpt;
  float ave=*outave, med=*outmed, std=*outstd;
  float oldstd=0, oldmed=0, oldave=0, *orderedarray;

  if(o1_n0==0)
    {
      floatcopy(array, num_elem, &orderedarray);
      qsort(orderedarray, num_elem, sizeof*orderedarray,
	    floatincreasing);
    }
  else orderedarray=array;

  start=orderedarray;
  while(counter<MAXSIGCLIPCONVERGE)
    {
      oldstart=start;

      med=*(start+num_elem/2);
      favestd(start, num_elem, &ave, &std, NULL);

      if(print)
        printf("      %lu: %f, %f, %f, %lu\n",
               counter+1, med, ave, std, num_elem);

      /* It might happen that ave and std are NaN. If so, stop the
	 process here (the user has not given a mask and some pixels
	 have nan values!). */
      if(isnan(ave) || isnan(std))
	return 0;

      /* Normally, oldstd should be larger than std, because the
	 possible outliers have been removed. If it is not, it means
	 that we have clipped too much! */
      if(counter>0 && (oldstd-std)/std<accuracy)
	{
	  *outstd=oldstd; *outave=oldave; *outmed=oldmed;
	  return 1;
	}

      for(dpt=start; dpt<start+num_elem; ++dpt)
	if (*dpt>med-sigma_multiple*std)
	  {
	    start=dpt;
	    break;
	  }

      for(dpt=oldstart+num_elem-1;dpt>start;dpt--)
	if (*dpt<med+sigma_multiple*std)
	  {
	    num_elem=dpt-start+1;
	    break;
	  }

      oldave=ave;
      oldmed=med;
      oldstd=std;
      ++counter;
    }
  return 0;
}





/* This function will do a certain number of sigma clips and return
   the final average, median and std. o1_n0: 1: initially ordered. 2:
   initially not ordered.*/
int
sigmaclip_certainnum(float *array, int o1_n0, size_t num_elem,
		     float sigma_multiple, size_t numtimes,
		     float *outave, float *outmed, float *outstd,
                     int print)
{
  size_t counter=0;
  float ave=*outave, med=*outmed, std=*outstd;
  float *start, *oldstart, *dpt, *orderedarray;

  if(o1_n0==0)
    {
      floatcopy(array, num_elem, &orderedarray);
      qsort(orderedarray, num_elem, sizeof*orderedarray,
	    floatincreasing);
    }
  else orderedarray=array;

  start=orderedarray;
  for(counter=0;counter<numtimes;++counter)
    {
      oldstart=start;

      med=*(start+num_elem/2);
      favestd(start, num_elem, &ave, &std, NULL);

      if(print)
        printf("      %lu: %f, %f, %f, %lu\n",
               counter+1, med, ave, std, num_elem);

      /* It might happen that ave and std are nan. If so, stop the
	 process here (the user has not given a mask and some pixels
	 have nan values!). */
      if(isnan(ave) || isnan(std))
	return 0;


      for(dpt=start; dpt<start+num_elem; ++dpt)
	if (*dpt>med-sigma_multiple*std)
	  {
	    start=dpt;
	    break;
	  }


      for(dpt=oldstart+num_elem-1;dpt>start;dpt--)
	if (*dpt<med+sigma_multiple*std)
	  {
	    num_elem=dpt-start+1;
	    break;
	  }
    }

  if(o1_n0==0)
    free(orderedarray);

  *outave=ave;
  *outmed=med;
  *outstd=std;
  return 1;
}



















/****************************************************************/
/*************         Identify outliers         ****************/
/****************************************************************/
/* Using the cumulative distribution function this funciton will
   remove outliers from a dataset. */
void
removeoutliers_flatcdf(float *sorted, size_t *outsize)
{
  int firstfound=0;
  size_t size=*outsize, i, maxind;
  float *slopes, minslope, maxslope;

  /* Find a slopes array, think of the cumulative frequency plot when
     you want to think about slopes. */
  errno=0; slopes=malloc(size*sizeof *slopes);
  if(slopes==NULL)
    error(EXIT_FAILURE, errno, "%lu bytes for slopes in "
          "removeoutliers_flatcdf (statistics.c)", size*sizeof *slopes);

  /* Calcuate the slope of the CDF and put it in the slopes array. */
  for(i=1;i<size-1;++i)
    slopes[i]=2/(sorted[i+1]-sorted[i-1]);

  /* Find the position of the maximum slope, note that around the
     distribution mode, the difference between the values varies less,
     so two neighbouring elements have the closest values, hence the
     largest slope (when their difference is in the denominator). */
  fmax_withindex(slopes+1, size-2, &maxslope, &maxind);

  /* Find the minimum slope from the second element (for the first the
     slope is not defined. NOTE; maxind is one smaller than it should
     be because the input array to find it began from the second
     element. */
  floatsecondmin(slopes+1, maxind+1, &minslope);

  /* Find the second place where the slope falls below `minslope`
     after the maximum position. When found, add it with one to
     account for error. Note that incase there are no outliers by this
     definition, then the final size will be equal to size! */
  for(i=maxind+1;i<size-1;++i)
    if(slopes[i]<minslope)
      {
	if(firstfound)
	  break;
	else
	  firstfound=1;
      }
  *outsize=i+1;

  /*
  for(i=0;i<size;++i)
    printf("%lu\t%.3f\t%.3f\n", i, arr[i], slopes[i]);
  printf("\n\nPlace to cut off for outliers is: %lu\n\n", *outsize);
  */

  free(slopes);
}

/*********************************************************************
Table - View and manipulate a FITS table structures.
Table is part of GNU Astronomy Utilities (Gnuastro) package.

Original author:
     Mohammad Akhlaghi <mohammad@akhlaghi.org>
Contributing author(s):
Copyright (C) 2019-2021, Free Software Foundation, Inc.

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
#include <config.h>

#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>

#include <gnuastro/wcs.h>

#include <gnuastro-internal/checkset.h>

#include "main.h"
#include "arithmetic.h"





/*********************************************************************/
/********************       List operations      *********************/
/*********************************************************************/
static struct arithmetic_token *
arithmetic_add_new_to_end(struct arithmetic_token **list)
{
  struct arithmetic_token *tmp, *node;

  /* Allocate a new node. */
  errno=0;
  node=malloc(sizeof *node);
  if(node==NULL)
    error(EXIT_FAILURE, errno, "%s: couldn't allocate new node (%zu bytes)",
          __func__, sizeof *node);

  /* Initialize its elements. */
  node->next=NULL;
  node->constant=NULL;
  node->index=GAL_BLANK_SIZE_T;
  node->operator=GAL_ARITHMETIC_OP_INVALID;

  /* If the list already has elements, go to the last node in the list and
     add this node. */
  if(*list)
    {
      for(tmp=*list;tmp->next!=NULL;tmp=tmp->next) {}
      tmp->next=node;
    }
  else
    *list=node;

  /* Return a pointer to this node (to use temporarily). */
  return node;
}





void
arithmetic_token_free(struct arithmetic_token *list)
{
  struct arithmetic_token *tmp;
  while(list!=NULL)
    {
      tmp=list->next;
      free(list);
      list=tmp;
    }
}


















/*********************************************************************/
/********************       User-interface       *********************/
/*********************************************************************/
static char *
arithmetic_operator_name(int operator)
{
  char *out=gal_arithmetic_operator_string(operator);

  /* If the operator wasn't in the library, see if it was defined here. */
  if(out==NULL)
    switch(operator)
      {
      case ARITHMETIC_TABLE_OP_SIN: out="sin"; break;
      case ARITHMETIC_TABLE_OP_COS: out="cos"; break;
      case ARITHMETIC_TABLE_OP_TAN: out="tan"; break;
      case ARITHMETIC_TABLE_OP_ASIN: out="asin"; break;
      case ARITHMETIC_TABLE_OP_ACOS: out="acos"; break;
      case ARITHMETIC_TABLE_OP_ATAN: out="atan"; break;
      case ARITHMETIC_TABLE_OP_SINH: out="sinh"; break;
      case ARITHMETIC_TABLE_OP_COSH: out="cosh"; break;
      case ARITHMETIC_TABLE_OP_TANH: out="tanh"; break;
      case ARITHMETIC_TABLE_OP_ASINH: out="asinh"; break;
      case ARITHMETIC_TABLE_OP_ACOSH: out="acosh"; break;
      case ARITHMETIC_TABLE_OP_ATANH: out="atanh"; break;
      case ARITHMETIC_TABLE_OP_ATAN2: out="atan2"; break;
      case ARITHMETIC_TABLE_OP_WCSTOIMG: out="wcstoimg"; break;
      case ARITHMETIC_TABLE_OP_IMGTOWCS: out="imgtowcs"; break;
      case ARITHMETIC_TABLE_OP_DISTANCEFLAT: out="distance-flat"; break;
      case ARITHMETIC_TABLE_OP_DISTANCEONSPHERE: out="distance-on-sphere"; break;
      default:
        error(EXIT_FAILURE, 0, "%s: a bug! Please contact us at %s to fix "
              "the problem. %d is not a recognized operator code", __func__,
              PACKAGE_BUGREPORT, operator);
      }

  return out;
}





static void
arithmetic_init_wcs(struct tableparams *p, char *operator)
{
  /* If a WCS hasn't been read yet, read it.*/
  if(p->wcs==NULL)
    {
      /* A small sanity check. */
      if(p->wcsfile==NULL || p->wcshdu==NULL)
        error(EXIT_FAILURE, 0, "'--wcsfile' and '--wcshdu' are necessary "
              "for the '%s' operator", operator);

      /* Read the WCS. */
      p->wcs=gal_wcs_read(p->wcsfile, p->wcshdu, 0, 0, &p->nwcs);
      if(p->wcs==NULL)
        error(EXIT_FAILURE, 0, "%s (hdu: %s): no WCS could be read by "
              "WCSLIB", p->wcsfile, p->wcshdu);
    }
}





/* Set the operator code from the given string. */
static int
arithmetic_set_operator(struct tableparams *p, char *string,
                        size_t *num_operands)
{
  int op=gal_arithmetic_set_operator(string, num_operands);;

  /* Set the operator and number of operands. */
  if( op==GAL_ARITHMETIC_OP_INVALID )
    {
      if(      !strcmp(string, "sin"))
        { op=ARITHMETIC_TABLE_OP_SIN; *num_operands=0; }
      else if( !strcmp(string, "cos"))
        { op=ARITHMETIC_TABLE_OP_COS; *num_operands=0; }
      else if( !strcmp(string, "tan"))
        { op=ARITHMETIC_TABLE_OP_TAN; *num_operands=0; }
      else if( !strcmp(string, "asin"))
        { op=ARITHMETIC_TABLE_OP_ASIN; *num_operands=0; }
      else if( !strcmp(string, "acos"))
        { op=ARITHMETIC_TABLE_OP_ACOS; *num_operands=0; }
      else if( !strcmp(string, "atan"))
        { op=ARITHMETIC_TABLE_OP_ATAN; *num_operands=0; }
      else if( !strcmp(string, "atan2"))
        { op=ARITHMETIC_TABLE_OP_ATAN2; *num_operands=0; }
      else if( !strcmp(string, "sinh"))
        { op=ARITHMETIC_TABLE_OP_SINH; *num_operands=0; }
      else if( !strcmp(string, "cosh"))
        { op=ARITHMETIC_TABLE_OP_COSH; *num_operands=0; }
      else if( !strcmp(string, "tanh"))
        { op=ARITHMETIC_TABLE_OP_TANH; *num_operands=0; }
      else if( !strcmp(string, "asinh"))
        { op=ARITHMETIC_TABLE_OP_ASINH; *num_operands=0; }
      else if( !strcmp(string, "acosh"))
        { op=ARITHMETIC_TABLE_OP_ACOSH; *num_operands=0; }
      else if( !strcmp(string, "atanh"))
        { op=ARITHMETIC_TABLE_OP_ATANH; *num_operands=0; }
      else if( !strcmp(string, "wcstoimg"))
        { op=ARITHMETIC_TABLE_OP_WCSTOIMG; *num_operands=0; }
      else if( !strcmp(string, "imgtowcs"))
        { op=ARITHMETIC_TABLE_OP_IMGTOWCS; *num_operands=0; }
      else if( !strcmp(string, "distance-flat"))
        { op=ARITHMETIC_TABLE_OP_DISTANCEFLAT; *num_operands=0; }
      else if( !strcmp(string, "distance-on-sphere"))
        { op=ARITHMETIC_TABLE_OP_DISTANCEONSPHERE; *num_operands=0; }
      else
        { op=GAL_ARITHMETIC_OP_INVALID; *num_operands=GAL_BLANK_INT; }
    }

  /* Operator specific operations. */
  switch(op)
    {
    case ARITHMETIC_TABLE_OP_WCSTOIMG:
    case ARITHMETIC_TABLE_OP_IMGTOWCS:
      arithmetic_init_wcs(p, string);
      break;
    }

  /* Return the operator. */
  return op;
}





/* Initialize each column from an arithmetic operation. */
void
arithmetic_init(struct tableparams *p, struct arithmetic_token **arith,
                gal_list_str_t **toread, size_t *totcalled, char *expression)
{
  void *num;
  size_t one=1;
  uint8_t ntype;
  char *str, *delimiter=" \t";
  struct arithmetic_token *node=NULL;
  char *token=NULL, *lasttoken=NULL, *saveptr;

  /* Parse all the given tokens. */
  token=strtok_r(expression, delimiter, &saveptr);
  while(token!=NULL)
    {
      /* Allocate and initialize this arithmetic token. */
      lasttoken=token;
      node=arithmetic_add_new_to_end(arith);

      /* See if the token is an operator, if not check other cases.  */
      node->operator=arithmetic_set_operator(p, token, &node->num_operands);
      if(node->operator==GAL_ARITHMETIC_OP_INVALID)
        {
          /* Token is a single number.*/
          if( (num=gal_type_string_to_number(token, &ntype)) )
            node->constant=gal_data_alloc(num, ntype, 1, &one, NULL, 0, -1, 1,
                                          NULL, NULL, NULL);

          /* Token is a column operand (column number or name). */
          else
            {
              str = ( (token[0]=='$' && isdigit(token[1]))
                      ? &token[1]   /* Column number (starting with '$'). */
                      : token );    /* Column name, just add it.          */
              gal_list_str_add(toread, str, 1);
              node->index=*totcalled;
              *totcalled+=1;
            }
        }

      /* Go to the next token. */
      token=strtok_r(NULL, delimiter, &saveptr);
    }

  /* A small sanity check: the last added token must be an operator. */
  if( node==NULL || node->operator==GAL_ARITHMETIC_OP_INVALID )
    error(EXIT_FAILURE, 0, "last token in arithmetic column ('%s') is not a "
          "recognized operator", lasttoken);
}





/* Set the final index of each package of columns (possibly containing
   processing columns that will change in number and contents).  */
void
arithmetic_indexs_final(struct tableparams *p, size_t *colmatch)
{
  size_t startind;
  size_t i, numcols;
  struct column_pack *tmp;
  struct arithmetic_token *atmp;

  /* Set the column array that will allow removal of some read columns
     (where operations will be done). */
  p->colarray=gal_list_data_to_array_ptr(p->table, &p->numcolarray);

  /* go over each package of columns. */
  for(tmp=p->outcols;tmp!=NULL;tmp=tmp->next)
    {
      /* If we are on an arithmetic operation. */
      if(tmp->tokens)
        {
          for(atmp=tmp->tokens;atmp!=NULL;atmp=atmp->next)
            if(atmp->index!=GAL_BLANK_SIZE_T)
              {
                /* Small sanity check. */
                if(colmatch[atmp->index]!=1)
                  error(EXIT_FAILURE, 0, "arithmetic operands can (currently) "
                        "only correspond to a single column");

                /* Update the index in the full list of read columns. */
                numcols=0; for(i=0;i<atmp->index;++i) numcols+=colmatch[i];
                atmp->index=numcols;
              }
        }
      /* A simple column. */
      else
        {
          /* See where the starting column for this patch of simple columns
             is. */
          startind=0;
          for(i=0;i<tmp->start;++i) startind+=colmatch[i];

          /* How many of the read columns are associated with the this
             patch of columns. */
          numcols=0;
          for(i=0;i<tmp->numsimple;++i) numcols+=colmatch[tmp->start+i];

          /* Update the values. */
          tmp->start=startind;
          tmp->numsimple=numcols;
        }

    }
}




















/*********************************************************************/
/********************       Low-level tools      *********************/
/*********************************************************************/
static gal_data_t *
arithmetic_stack_pop(gal_data_t **stack, int operator, char *errormsg)
{
  gal_data_t *out=*stack;

  /* Update the stack. */
  if(*stack)
    *stack=(*stack)->next;
  else
    error(EXIT_FAILURE, 0, "not enough operands for '%s'%s",
          arithmetic_operator_name(operator), errormsg?errormsg:"");

  /* Remove the 'next' element to break from the stack and return. */
  out->next=NULL;
  return out;
}





/* Set the converted column metadata. */
static void
arithmetic_update_metadata(gal_data_t *col, char *name, char *unit,
                           char *comment)
{
  if(col)
    {
      if(col->name)    free(col->name);
      if(col->unit)    free(col->unit);
      if(col->comment) free(col->comment);
      gal_checkset_allocate_copy(name, &col->name);
      gal_checkset_allocate_copy(unit, &col->unit);
      gal_checkset_allocate_copy(comment, &col->comment);
    }
}




















/*********************************************************************/
/********************          Operations        *********************/
/*********************************************************************/
static void
arithmetic_wcs(struct tableparams *p, gal_data_t **stack, int operator)
{
  gal_data_t *tmp;
  char errormsg[100];
  struct wcsprm *wcs=p->wcs;
  size_t i, ndim=p->wcs->naxis;
  gal_data_t *coord[3]={NULL, NULL, NULL};

  /* Prepare the (possibly necessaary!) error message for the number of
     popped operand. */
  sprintf(errormsg, " (input WCS has %zu dimensions)", ndim);

  /* Pop all the necessary datasets and make sure they are
     double-precision. NOTE: the top dataset on the stack is the
     highest-dimensional dataset. */
  for(i=0;i<ndim;++i)
    {
      tmp=arithmetic_stack_pop(stack, operator, errormsg);
      tmp=gal_data_copy_to_new_type_free(tmp, GAL_TYPE_FLOAT64);
      coord[ndim-i-1]=tmp;
    }

  /* Define the list of coordinates. */
  if(coord[1]) coord[0]->next=coord[1];
  if(coord[2]) coord[1]->next=coord[2];

  /* Final preparations. */
  if(operator==ARITHMETIC_TABLE_OP_WCSTOIMG)
    {
      /* Do the conversion. */
      gal_wcs_world_to_img(coord[0], wcs, 1);

      /* For image coordinates, we don't need much precision. */
      for(i=0;i<ndim;++i)
        coord[i]=gal_data_copy_to_new_type_free(coord[i], GAL_TYPE_FLOAT32);

      /* Set the names, units and comments for each dataset. */
      arithmetic_update_metadata(coord[0],"X","pixel","Converted from WCS");
      arithmetic_update_metadata(coord[1],"Y","pixel","Converted from WCS");
      arithmetic_update_metadata(coord[2],"Z","pixel","Converted from WCS");
    }
  else
    {
      gal_wcs_img_to_world(coord[0], wcs, 1);
      arithmetic_update_metadata(coord[0], wcs->ctype[0], wcs->cunit[0],
                                 "Converted from pixel coordinates");
      arithmetic_update_metadata(coord[1], coord[1]?wcs->ctype[1]:NULL,
                                 coord[1]?wcs->cunit[1]:NULL,
                                 "Converted from pixel coordinates");
      arithmetic_update_metadata(coord[2], coord[2]?wcs->ctype[2]:NULL,
                                 coord[2]?wcs->cunit[2]:NULL,
                                 "Converted from pixel coordinates");
    }

  /* Reverse the column orders and put them on the stack. */
  for(i=0;i<ndim;++i)
    {
      coord[i]->next=NULL;
      gal_list_data_add(stack, coord[i]);
    }
}





static double
arithmetic_distance_flat(double a1, double a2, double b1, double b2)
{
  double d1=a1-b1, d2=a2-b2;
  return sqrt(d1*d1 + d2*d2);
}





static void
arithmetic_distance(struct tableparams *p, gal_data_t **stack, int operator)
{
  size_t i, j;
  double *o, *a1, *a2, *b1, *b2;
  gal_data_t *a, *b, *tmp, *out;
  char *colname=NULL, *colcomment=NULL;
  double (*distance_func)(double, double, double, double);

  /* Pop the columns for point 'b'.*/
  tmp=arithmetic_stack_pop(stack, operator, NULL);
  tmp=gal_data_copy_to_new_type_free(tmp, GAL_TYPE_FLOAT64);
  b=arithmetic_stack_pop(stack, operator, NULL);
  b=gal_data_copy_to_new_type_free(b, GAL_TYPE_FLOAT64);
  b->next=tmp;

  /* Pop the columns for point 'a'.*/
  tmp=arithmetic_stack_pop(stack, operator, NULL);
  tmp=gal_data_copy_to_new_type_free(tmp, GAL_TYPE_FLOAT64);
  a=arithmetic_stack_pop(stack, operator, NULL);
  a=gal_data_copy_to_new_type_free(a, GAL_TYPE_FLOAT64);
  a->next=tmp;

  /* Make sure the sizes are consistant: note that one point can have a
     single coordinate, but we don't know which one. */
  if(a->size!=a->next->size)
    error(EXIT_FAILURE, 0, "the sizes of the third and fourth operands "
          "of the '%s' operator (respectively containing %zu and %zu "
          "numbers) must be equal", arithmetic_operator_name(operator),
          a->next->size, a->size);
  if(b->size!=b->next->size)
    error(EXIT_FAILURE, 0, "the sizes of the first and second operands "
          "of the '%s' operator (respectively containing %zu and %zu "
          "numbers) must be equal", arithmetic_operator_name(operator),
          b->next->size, b->size);

  /* Basic settings based on the operator. */
  switch(operator)
    {
    case ARITHMETIC_TABLE_OP_DISTANCEFLAT:
      colname="dist-flat";
      distance_func=arithmetic_distance_flat;
      colcomment="Distance measured on a flat surface.";
      break;
    case ARITHMETIC_TABLE_OP_DISTANCEONSPHERE:
      colname="dist-spherical";
      distance_func=gal_wcs_angular_distance_deg;
      colcomment="Distance measured on a great circle.";
      break;
    default:
      error(EXIT_FAILURE, 0, "%s: a bug! Please contact us at %s to "
            "fix the problem. The operator code %d isn't recognized",
            __func__, PACKAGE_BUGREPORT, operator);
    }

  /* Make the output array based on the largest size. */
  out=gal_data_alloc(NULL, GAL_TYPE_FLOAT64, 1,
                     (a->size>b->size ? &a->size : &b->size), NULL, 0,
                     p->cp.minmapsize, p->cp.quietmmap, colname, NULL,
                     colcomment);

  /* Measure the distances.  */
  o=out->array;
  a1=a->array; a2=a->next->array;
  b1=b->array; b2=b->next->array;
  if(a->size==1 || b->size==1) /* One of them is a single point. */
    for(i=0;i<a->size;++i)
      for(j=0;j<b->size;++j)
        o[a->size>b->size?i:j] = distance_func(a1[i], a2[i], b1[j], b2[j]);
  else                         /* Both have the same length. */
    for(i=0;i<a->size;++i)     /* (all were originally from the same table) */
      o[i] = distance_func(a1[i], a2[i], b1[i], b2[i]);

  /* Clean up and put the output dataset onto the stack. */
  gal_list_data_free(a);
  gal_list_data_free(b);
  gal_list_data_add(stack, out);
}





static void
arithmetic_trig_hyper(struct tableparams *p, gal_data_t **stack,
                      int operator)
{
  size_t i;
  double *x, *y;
  gal_data_t *in=NULL, *in2=NULL;
  double pi=3.14159265358979323846264338327;

  /* Read the input columns as a 'double'. */
  in=arithmetic_stack_pop(stack, operator, NULL);
  in=gal_data_copy_to_new_type_free(in, GAL_TYPE_FLOAT64);
  if(in->comment) { free(in->comment); in->comment=NULL; }
  if(in->name)    { free(in->name);    in->name=NULL;    }
  if(in->unit)    { free(in->unit);    in->unit=NULL;    }
  x=in->array;

  /* Pop the second operand if necessary. */
  if(operator==ARITHMETIC_TABLE_OP_ATAN2)
    {
      in2=arithmetic_stack_pop(stack, operator, NULL);
      in2=gal_data_copy_to_new_type_free(in2, GAL_TYPE_FLOAT64);
      y=in2->array;
    }

  /* Parse the array and do the calculation in place. */
  switch(operator)
    {
      /* Single-operand operators. */
    case ARITHMETIC_TABLE_OP_SIN:
      for(i=0;i<in->size;++i) x[i]=sin( pi*x[i]/180.0f );
      gal_checkset_allocate_copy("sin", &in->name);
      gal_checkset_allocate_copy("ratio", &in->unit);
      gal_checkset_allocate_copy("Sine of an angle.",
                                 &in->comment);
      break;
    case ARITHMETIC_TABLE_OP_COS:
      for(i=0;i<in->size;++i) x[i]=cos( pi*x[i]/180.0f );
      gal_checkset_allocate_copy("cos", &in->name);
      gal_checkset_allocate_copy("ratio", &in->unit);
      gal_checkset_allocate_copy("Cosine of an angle.",
                                 &in->comment);
      break;
    case ARITHMETIC_TABLE_OP_TAN:
      for(i=0;i<in->size;++i) x[i]=tan( pi*x[i]/180.0f );
      gal_checkset_allocate_copy("tan", &in->name);
      gal_checkset_allocate_copy("ratio", &in->unit);
      gal_checkset_allocate_copy("Tangent of an angle.",
                                 &in->comment);
      break;
    case ARITHMETIC_TABLE_OP_ASIN:
      for(i=0;i<in->size;++i) x[i]=asin(x[i])*180.0f/pi;
      gal_checkset_allocate_copy("asin", &in->name);
      gal_checkset_allocate_copy("deg", &in->unit);
      gal_checkset_allocate_copy("Inverse sine of a value.",
                                 &in->comment);
      break;
    case ARITHMETIC_TABLE_OP_ACOS:
      for(i=0;i<in->size;++i) x[i]=acos(x[i])*180.0f/pi;
      gal_checkset_allocate_copy("acos", &in->name);
      gal_checkset_allocate_copy("deg", &in->unit);
      gal_checkset_allocate_copy("Inverse cosine of a value.",
                                 &in->comment);
      break;
    case ARITHMETIC_TABLE_OP_ATAN:
      for(i=0;i<in->size;++i) x[i]=atan(x[i])*180.0f/pi;
      gal_checkset_allocate_copy("atan", &in->name);
      gal_checkset_allocate_copy("deg", &in->unit);
      gal_checkset_allocate_copy("Inverse tangent of a value.",
                                 &in->comment);
      break;
    case ARITHMETIC_TABLE_OP_SINH:
      for(i=0;i<in->size;++i) x[i]=sinh(x[i]);
      gal_checkset_allocate_copy("sinh", &in->name);
      gal_checkset_allocate_copy("ratio", &in->unit);
      gal_checkset_allocate_copy("Hyperbolic sine of a value.",
                                 &in->comment);
      break;
    case ARITHMETIC_TABLE_OP_COSH:
      for(i=0;i<in->size;++i) x[i]=cosh(x[i]);
      gal_checkset_allocate_copy("cosh", &in->name);
      gal_checkset_allocate_copy("ratio", &in->unit);
      gal_checkset_allocate_copy("Hyperbolic cosine of a value.",
                                 &in->comment);
      break;
    case ARITHMETIC_TABLE_OP_TANH:
      for(i=0;i<in->size;++i) x[i]=tanh(x[i]);
      gal_checkset_allocate_copy("tanh", &in->name);
      gal_checkset_allocate_copy("ratio", &in->unit);
      gal_checkset_allocate_copy("Hyperbolic tangent of a value.",
                                 &in->comment);
      break;
    case ARITHMETIC_TABLE_OP_ASINH:
      for(i=0;i<in->size;++i) x[i]=asinh(x[i]);
      gal_checkset_allocate_copy("asinh", &in->name);
      gal_checkset_allocate_copy("ratio", &in->unit);
      gal_checkset_allocate_copy("Inverse hyperbolic sine of a value.",
                                 &in->comment);
      break;
    case ARITHMETIC_TABLE_OP_ACOSH:
      for(i=0;i<in->size;++i) x[i]=acosh(x[i]);
      gal_checkset_allocate_copy("acosh", &in->name);
      gal_checkset_allocate_copy("ratio", &in->unit);
      gal_checkset_allocate_copy("Inverse hyperbolic cossine of a value.",
                                 &in->comment);
      break;
    case ARITHMETIC_TABLE_OP_ATANH:
      for(i=0;i<in->size;++i) x[i]=atanh(x[i]);
      gal_checkset_allocate_copy("atanh", &in->name);
      gal_checkset_allocate_copy("ratio", &in->unit);
      gal_checkset_allocate_copy("Inverse hyperbolic tangent of a value.",
                                 &in->comment);
      break;

    /* Two operand operators. */
    case ARITHMETIC_TABLE_OP_ATAN2:
      for(i=0;i<in->size;++i) x[i]=atan2(y[i], x[i])*180.0f/pi;
      gal_checkset_allocate_copy("atan2", &in->name);
      gal_checkset_allocate_copy("deg", &in->unit);
      gal_checkset_allocate_copy("Inverse tangent of point (preserving "
                                 "the quadrant)", &in->comment);
      break;

    /* Not recognized (a bug). */
    default:
      error(EXIT_FAILURE, 0, "%s: a bug! please contact us at %s to fix "
            "the problem. The code %d is not recognized as an operator "
            "related to this function", __func__, PACKAGE_BUGREPORT,
            operator);
    }

  /* Clean up and put the resulting calculation back on the stack. */
  if(in2) gal_data_free(in2);
  gal_list_data_add(stack, in);
}




















/*********************************************************************/
/********************          Operations        *********************/
/*********************************************************************/
static void
arithmetic_placeholder_name(gal_data_t *col)
{
  static size_t counter=0;

  /* Increment counter next time this function is called. */
  ++counter;

  /* Free any possibly existing metadata. */
  if(col->name)    free(col->name);
  if(col->unit)    free(col->unit);
  if(col->comment) free(col->comment);

  /* Set the new meta-data. */
  errno=0;
  if( asprintf(&col->name, "ARITH_%zu", counter)==-1 )
    error(EXIT_FAILURE, errno, "%s: asprintf error for name", __func__);
  if( asprintf(&col->unit, "arith_unit_%zu", counter)==-1)
    error(EXIT_FAILURE, errno, "%s: asprintf error for unit", __func__);
  if( asprintf(&col->comment, "Column from arithmetic operation %zu",
               counter)==-1 )
    error(EXIT_FAILURE, errno, "%s: asprintf error for comment", __func__);
}





static void
arithmetic_operator_run(struct tableparams *p, gal_data_t **stack,
                        int operator, size_t num_operands)
{
  gal_data_t *d1=NULL, *d2=NULL, *d3=NULL;
  int flags = ( GAL_ARITHMETIC_INPLACE | GAL_ARITHMETIC_FREE
                | GAL_ARITHMETIC_NUMOK );

  /* When 'num_operands!=0', the operator is in the library. */
  if(num_operands)
    {
      /* Pop the necessary number of operators. Note that the
         operators are poped from a linked list (which is
         last-in-first-out). So for the operators which need a
         specific order, the first poped operand is actally the
         last (right most, in in-fix notation) input operand.*/
      switch(num_operands)
        {
        case 1:
          d1=arithmetic_stack_pop(stack, operator, NULL);
          break;

        case 2:
          d2=arithmetic_stack_pop(stack, operator, NULL);
          d1=arithmetic_stack_pop(stack, operator, NULL);
          break;

        case 3:
          d3=arithmetic_stack_pop(stack, operator, NULL);
          d2=arithmetic_stack_pop(stack, operator, NULL);
          d1=arithmetic_stack_pop(stack, operator, NULL);
          break;

        case -1:
          error(EXIT_FAILURE, 0, "operators with a variable number of "
                "operands are not yet implemented. Please contact us at "
                "%s to include them", PACKAGE_BUGREPORT);
          break;

        default:
          error(EXIT_FAILURE, 0, "%s: a bug! Please contact us at %s to fix "
                "the problem. '%zu' is not recognized as an operand "
                "counter (with '%s')", __func__, PACKAGE_BUGREPORT,
                num_operands, arithmetic_operator_name(operator));
        }

      /* Run the arithmetic operation. Note that 'gal_arithmetic' is a
         variable argument function (like printf). So the number of
         arguments it uses depend on the operator. In other words, when the
         operator doesn't need three operands, the extra arguments will be
         ignored. */
      gal_list_data_add(stack, gal_arithmetic(operator, p->cp.numthreads,
                                              flags, d1, d2, d3) );

      /* Reset the meta-data for the element that was just put on the
         stack. */
      arithmetic_placeholder_name(*stack);
    }

  /* This operator is specific to this program (Table). */
  else
    {
      switch(operator)
        {
        case ARITHMETIC_TABLE_OP_SIN:
        case ARITHMETIC_TABLE_OP_COS:
        case ARITHMETIC_TABLE_OP_TAN:
        case ARITHMETIC_TABLE_OP_ASIN:
        case ARITHMETIC_TABLE_OP_ACOS:
        case ARITHMETIC_TABLE_OP_ATAN:
        case ARITHMETIC_TABLE_OP_SINH:
        case ARITHMETIC_TABLE_OP_COSH:
        case ARITHMETIC_TABLE_OP_TANH:
        case ARITHMETIC_TABLE_OP_ASINH:
        case ARITHMETIC_TABLE_OP_ACOSH:
        case ARITHMETIC_TABLE_OP_ATANH:
        case ARITHMETIC_TABLE_OP_ATAN2:
          arithmetic_trig_hyper(p, stack, operator);
          break;

        case ARITHMETIC_TABLE_OP_WCSTOIMG:
        case ARITHMETIC_TABLE_OP_IMGTOWCS:
          arithmetic_wcs(p, stack, operator);
          break;

        case ARITHMETIC_TABLE_OP_DISTANCEFLAT:
        case ARITHMETIC_TABLE_OP_DISTANCEONSPHERE:
          arithmetic_distance(p, stack, operator);
          break;

        default:
          error(EXIT_FAILURE, 0, "%s: a bug! Please contact us at %s to "
                "fix the problem. The operator code %d is not recognized",
                __func__, PACKAGE_BUGREPORT, operator);
        }
    }
}





/* Apply reverse polish mechanism for this column. */
static void
arithmetic_reverse_polish(struct tableparams *p, struct column_pack *outpack)
{
  gal_data_t *single, *stack=NULL;
  struct arithmetic_token *token;

  /* Go through all the tokens given to this element. */
  for(token=outpack->tokens;token!=NULL;token=token->next)
    {
      /* We are on an operator. */
      if(token->operator!=GAL_ARITHMETIC_OP_INVALID)
        arithmetic_operator_run(p, &stack, token->operator,
                                token->num_operands);

      /* Constant number: just put it ontop of the stack. */
      else if(token->constant)
        {
          gal_list_data_add(&stack, token->constant);
          token->constant=NULL;
        }

      /* A column from the table. */
      else if(token->index!=GAL_BLANK_SIZE_T)
        gal_list_data_add(&stack, p->colarray[token->index]);

      /* Un-recognized situation. */
      else
        error(EXIT_FAILURE, 0, "%s: a bug! Please contact us at %s to "
              "fix the problem. The token can't be identified as an "
              "operator, constant or column", __func__, PACKAGE_BUGREPORT);
    }

  /* Put everything that remains in the stack (reversed) into the final
     table. Just note that 'gal_list_data_add' behaves differently for
     lists, so we'll add have to manually set the 'next' element to NULL
     before adding the column to the final table. */
  gal_list_data_reverse(&stack);
  while(stack!=NULL)
    {
      /* Keep the top element in 'single' and move 'stack' to the next
         element. */
      single=stack;
      stack=stack->next;

      /* A small sanity check. */
      if(single->size==1 && p->table && single->size!=p->table->size)
        error(EXIT_FAILURE, 0, "the arithmetic operation resulted in a "
              "single value, but other columns have also been requested "
              "which have more elements/rows");

      /* Set 'single->next' to NULL so it isn't treated as a list and
         remove all metadata */
      single->next=NULL;
      gal_list_data_add(&p->table, single);
    }
}




















/*********************************************************************/
/********************         High-level         *********************/
/*********************************************************************/
void
arithmetic_operate(struct tableparams *p)
{
  size_t i;
  struct column_pack *outpack;

  /* From now on, we will be looking for columns from the index in
     'colarray', so to keep things clean, we'll set all the 'next' elements
     to NULL. */
  for(i=0;i<p->numcolarray;++i) p->colarray[i]->next=NULL;

  /* We'll also reset the output table pointer, to fill it in as we
     progress. */
  p->table=NULL;

  /* Go over each package of columns. */
  for(outpack=p->outcols;outpack!=NULL;outpack=outpack->next)
    {
      if(outpack->tokens)
        arithmetic_reverse_polish(p, outpack);
      else
        {
          for(i=0;i<outpack->numsimple;++i)
            gal_list_data_add(&p->table, p->colarray[outpack->start+i]);
        }
    }

  /* Reverse the final output to be in the proper order. Note that all the
     column contents have either been moved into the new table, or have
     already been freed. */
  gal_list_data_reverse(&p->table);
}

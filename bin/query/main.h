/*********************************************************************
Query - Retreive data from a remote data server.
Query is part of GNU Astronomy Utilities (Gnuastro) package.

Original author:
     Mohammad akhlaghi <mohammad@akhlaghi.org>
Contributing author(s):
Copyright (C) 2020-2021, Free Software Foundation, Inc.

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
#ifndef MAIN_H
#define MAIN_H

/* Include necessary headers */
#include <gnuastro/data.h>

#include <gnuastro-internal/options.h>

/* Progarm names.  */
#define PROGRAM_NAME   "query"    /* Program full name.       */
#define PROGRAM_EXEC   "astquery" /* Program executable name. */
#define PROGRAM_STRING PROGRAM_NAME" (" PACKAGE_NAME ") " PACKAGE_VERSION







/* Main program parameters structure */
struct queryparams
{
  /* From command-line */
  struct gal_options_common_params cp; /* Common parameters.           */
  uint8_t      keeprawdownload;  /* Keep raw downloaded file.          */
  int                 database;  /* ID of database to use.             */
  char             *datasetstr;  /* ID of dataset in database to use.  */
  char            *overlapwith;  /* Image to use instead of center.    */
  gal_data_t           *center;  /* Center position of query.          */
  gal_data_t           *radius;  /* Radius around center.              */
  gal_data_t            *range;  /* Range of magnitudes to query.      */
  gal_data_t            *width;  /* Width of box around center.        */
  char                  *query;  /* Raw query string.                  */
  gal_list_str_t      *columns;  /* Columns to extract from database.  */

  /* Internal variables. */
  char            *databasestr;  /* Name of input database.            */
  char           *downloadname;  /* Temporary output name.             */
  size_t       outtableinfo[2];  /* To print in output.                */
  gal_list_str_t         *urls;  /* List of URLs to use.               */
  char                *ra_name;  /* Name of RA column.                 */
  char               *dec_name;  /* Name of Dec columns.               */

  /* Output: */
  time_t               rawtime;  /* Starting time of the program.      */
};

#endif

/*
This file is part of GOTCHA.  For copyright information see the COPYRIGHT
file in the top level directory, or at
https://github.com/LLNL/gotcha/blob/master/COPYRIGHT
This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License (as published by the Free
Software Foundation) version 2.1 dated February 1999.  This program is
distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the IMPLIED WARRANTY OF MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the terms and conditions of the GNU Lesser General Public License
for more details.  You should have received a copy of the GNU Lesser General
Public License along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/**
 * autotee -
 * Using gotcha, wrap the major IO writing routines with functions that 
 * "tee" any stdout output to another file.  Init by calling
 *    init_autotee(filename)
 * finish by calling:
 *    close_autotee()
 *
 * Note, this is a demonstration program for gotcha and does not handle
 * cases like stdout's file descriptor being dup'd or more esoteric 
 * IO routines.
 **/


#include "gotcha/gotcha_types.h"
#include "gotcha/gotcha.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <dlfcn.h>

static int tee_fd = -1;
static FILE *tee_FILE = NULL;

static int (*orig_printf)(const char *, ...);
static int (*orig_fprintf)(FILE *, const char *, ...);
static int (*orig_vfprintf)(FILE *, const char *, va_list);
static int (*orig_vprintf)(const char *, va_list);
void *(*orig_dlsym) (void * , const char *);

static int printf_wrapper(const char *format, ...);
static int fprintf_wrapper(FILE *stream, const char *format, ...);
static int vfprintf_wrapper(FILE *stream, const char *str, va_list args);
static int vprintf_wrapper(const char *str, va_list args);
void * dlsym_wrapper (void * handle , const char * name);

#define NUM_IOFUNCS 5
struct gotcha_binding_t iofuncs[] = {
   { "printf", printf_wrapper, &orig_printf },
   { "fprintf", fprintf_wrapper, &orig_fprintf },
   { "vfprintf", vfprintf_wrapper, &orig_vfprintf },
   { "vprintf", vprintf_wrapper, &orig_vprintf },
   { "dlsym", dlsym_wrapper, &orig_dlsym},
};

int init_autotee(const char *teefile)
{
   enum gotcha_error_t result;

   tee_FILE = fopen(teefile, "w");
   if (!tee_FILE) {
      perror("Failed to open tee file");
      return -1;
   }
   tee_fd = fileno(tee_FILE);

   result = gotcha_wrap(iofuncs, NUM_IOFUNCS, "autotee");
   if (result != GOTCHA_SUCCESS) {
      fprintf(stderr, "gotcha_wrap returned %d\n", (int) result);
      return -1;
   }

   return 0;
}

int close_autotee()
{
   if (tee_FILE) {
      fclose(tee_FILE);
      tee_fd = -1;
   }
   return 0;
}

static int printf_wrapper(const char *format, ...)
{
   int result;
   va_list args, args2;
   va_start(args, format);

   if (tee_FILE) {
      va_copy(args2, args);   
      orig_vfprintf(tee_FILE, format, args2);
      va_end(args2);
   }

   fprintf(stdout,"INSIDE the printf wrapper\n");
   result = orig_vprintf(format, args);   
   va_end(args);

   return result;
}

static int fprintf_wrapper(FILE *stream, const char *format, ...)
{
   int result;
   va_list args, args2;
   va_start(args, format);
   
   if (stream != stdout) {
      result = orig_vfprintf(stream, format, args);
   }
   else {
      if (tee_FILE) {
         va_copy(args2, args);
         orig_vfprintf(tee_FILE, format, args2);
         va_end(args2);
      }
      result = orig_vfprintf(stdout, format, args);
   }

   va_end(args);
   return result;
}

static int vfprintf_wrapper(FILE *stream, const char *str, va_list args)
{
   va_list args2;
   if (stream != stdout) {
      return orig_vfprintf(stream, str, args);
   }
   if (tee_FILE) {
      va_copy(args2, args);
      orig_vfprintf(tee_FILE, str, args2);
      va_end(args2);
   }
   return orig_vfprintf(stream, str, args);
}

static int vprintf_wrapper(const char *str, va_list args)
{
   va_list args2;
   if (tee_FILE) {
      va_copy(args2, args);
      orig_vfprintf(tee_FILE, str, args2);
      va_end(args2);
   }
   return orig_vprintf(str, args);
}


void * dlsym_wrapper (void * handle , const char * name){
   fprintf(stdout,"INSIDE the dlsym wrapper\n");
   return orig_dlsym(handle,name);
}

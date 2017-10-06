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

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

extern int init_autotee(char *filename);
extern int close_autotee();

typedef void* (*arbitrary)();

#define OUTPUT_FILE "tee.out"

int main()
{
   int result;

   printf("Every stdout print after this line should also appear in %s:\n", OUTPUT_FILE);

   result = init_autotee(OUTPUT_FILE);
   if (result != 0)
      return -1;

   printf("First line\n");
   printf("Second %s\n", "line");
   fprintf(stdout, "Third line\n");
   fprintf(stdout, "%s line\n", "Forth");
   close_autotee();
   printf("\n\nThis line is after close and should not appear in %s\n It is strange that this print goes to the wrapper even after close it\n", OUTPUT_FILE);
    arbitrary my_function;
    // Introduce already loaded functions to runtime linker's space
    void* handle = dlopen(0,RTLD_NOW|RTLD_GLOBAL);
    // Load the function to our pointer, which doesn't know how many arguments there sould be
    *(void**)(&my_function) = dlsym(handle,"fprintf");
    // Call something via my_function
    (void)  my_function(stdout,"Test dlsym main\n");

   return 0;
}

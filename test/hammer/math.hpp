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

#if !defined(MULT_HPP_)
#define MULT_HPP_

#include <map>
#include <utility>
#include <string>
#include <set>

#include "wrap.h"

#define X XSIZE
#define Y YSIZE


typedef signed long result_t ;

template <int A, int B>
class Mult {
public:
   static void init() {
      multinfo.addName(A, B, (void *) Mult<A, B>::math);
   }

   static result_t math() {
      return A * B;
   }
};

template <int A, int B>
class Add {
public:
   static void init() {
      addinfo.addName(A, B, (void *) Add<A, B>::math);
   }

   static result_t math() {
      return A + B;
   }
};

template <int A, int B>
class Neg {
public:
   static result_t (*mathfn_mult)(void);
   static result_t (*mathfn_add)(void);

   static void init() {
      multinfo.addNegPtr(A, B, (void *) Neg<A, B>::mult_math_wrapper, (void *) &mathfn_mult);
      addinfo.addNegPtr(A, B, (void *) Neg<A, B>::add_math_wrapper, (void *) &mathfn_add);
   }

   static result_t mult_math_wrapper() {
      return mathfn_mult() * -1;
   }

   static result_t add_math_wrapper() {
      return mathfn_add() * -1;
   }
};

#endif

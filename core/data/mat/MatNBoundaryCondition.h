/******************************************************************************\
|*                   Population library for C++ X.X.X                         *|
|*----------------------------------------------------------------------------*|
The Population License is similar to the MIT license in adding this clause:
for any writing public or private that has resulted from the use of the
software population, the reference of this book "Population library, 2012,
Vincent Tariel" shall be included in it.

So, the terms of the Population License are:

Copyright © 2012, Tariel Vincent

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software and for any writing
public or private that has resulted from the use of the software population,
the reference of this book "Population library, 2012, Vincent Tariel" shall
be included in it.

The Software is provided "as is", without warranty of any kind, express or
implied, including but not limited to the warranties of merchantability,
fitness for a particular purpose and noninfringement. In no event shall the
authors or copyright holders be liable for any claim, damages or other
liability, whether in an action of contract, tort or otherwise, arising
from, out of or in connection with the software or the use or other dealings
in the Software.
\***************************************************************************/
#ifndef FUNCTIONMatNBOUNDARYCONDITION_HPP
#define FUNCTIONMatNBOUNDARYCONDITION_HPP
#include"core/data/utility/Configuration.h"
namespace pop
{
enum BOUNDARY_CONDITION{
    Boundary_Condition_Bounded  = 0,
    Boundary_Condition_Periodic = 1,
    Boundary_Condition_Constant = 2,
    Boundary_Condition_Mirror   = 3
};

class POP_EXPORTS BoundaryCondition
        //class Boundary
{
public:
    template<typename Vec1,typename Vec2>
    static bool bounded(const Vec1 & domain,Vec2 & x)
    {
        if(x.allSuperiorEqual(0)&& x.allInferior(domain))
        {
            return true;
        }
        else
            return false;
    }
    template<typename Vec1,typename Vec2>
    static bool mirror(const Vec1 & domain,Vec2 & x)
    {
        for(int i=0;i<Vec1::DIM;i++)
        {

            if(x(i)<0){
                x(i)= -x(i)-1;
            }
            else if(x(i)>=domain(i))
                x(i)= 2*domain(i)-x(i)-1;
        }
        return true;
    }
        template<typename Vec1,typename Vec2>
    static bool periodic(const Vec1 & domain,Vec2 & x)
    {
        for(int i=0;i<Vec1::DIM;i++)
        {
            if(x(i)<0)
                x(i)= domain(i)+x(i);
            if(x(i)>=domain(i))
                x(i)= x(i)-domain(i);
        }
        return true;
    }
};

class POP_EXPORTS BoundaryConditionBounded
        //class Boundary
{
public:
    template<typename Vec1,typename Vec2>
    static bool VecNInsideDomain(const Vec1 & domain,Vec2 & x)
    {
        if(x.allSuperiorEqual(0)&&x.allInferior(domain))
        {
            return true;
        }
        else
            return false;
    }
};
class POP_EXPORTS BoundaryConditionMirror
        //class Boundary
{
public:
    template<typename Vec1,typename Vec2>
    static bool VecNInsideDomain(const Vec1 & domain,Vec2 & x)
    {
        for(int i=0;i<Vec1::DIM;i++)
        {

            if(x(i)<0){
                x(i)= -x(i)-1;
            }
            else if(x(i)>=domain(i))
                x(i)= 2*domain(i)-x(i)-1;
        }
        return true;
    }
};
class POP_EXPORTS BoundaryConditionPeriodic
        //class Boundary
{
public:
    template<typename Vec1,typename Vec2>
    static bool VecNInsideDomain(const Vec1 & domain,Vec2 & x)
    {
        for(int i=0;i<Vec1::DIM;i++)
        {
            if(x(i)<0)
                x(i)= domain(i)+x(i);
            if(x(i)>=domain(i))
                x(i)= x(i)-domain(i);
        }
        return true;
    }
};
}
#endif // FUNCTIONMatNBOUNDARYCONDITION_HPP

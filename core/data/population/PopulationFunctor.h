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

#ifndef FUNCTORPOP_HPP
#define FUNCTORPOP_HPP
#include<limits>
#include <stdio.h>
#include <stdlib.h>
#include<cmath>
#include"core/data/typeF/TypeTraitsF.h"
#include"core/data/functor/FunctorF.h"
#include"core/algorithm/FunctionProcedureFunctorF.h"
namespace pop
{
class POP_EXPORTS FunctorZero
{
public:
    I32 nbrLevel()
    {
        return 1;
    }
    template<typename  Label ,typename VecN>
    I32 operator()(Label , const VecN&)
            const
    {
        return 0;
    }
    template<typename VecN>
    I32 operator()( const VecN&)
            const
    {
        return 0;
    }
};
template<typename Function>
class POP_EXPORTS FunctorTopography
{
private:
    const Function &_f;
    typename Function::F _level;
public:
    FunctorTopography( const Function & f)
        :_f(f),_level(0)
    {}
    I32 nbrLevel()
    {
        if(NumericLimits<typename Function::F>::maximumRange()<=NumericLimits<UI16>::maximumRange())
            return NumericLimits<typename Function::F>::maximumRange()+1;
        else{
            typename Function::IteratorEDomain it(_f.getIteratorEDomain());
            FunctorF::FunctorAccumulatorMax<typename Function::F > func;
            typename Function::F value =  FunctionProcedureFunctorAccumulatorF(_f,func,it);
            return value+1;
        }
    }
    void setLevel(typename Function::F level)
    {
        _level=level;
    }
    template<typename  Label ,typename VecN>
    I32 operator()(Label , const VecN&x)
            const
    {
        return maximum(_f(x),_level);
    }
};

class POP_EXPORTS FunctorLabel
{
private:
    I32 _level;
public:
    FunctorLabel(I32 nbrlabel)
        :_level(nbrlabel)
    {}
    I32 nbrLevel()
    {
        return _level;
    }
    template<typename  Label ,typename VecN>
    I32 operator()(Label label, const VecN&)
            const
    {
        return label;
    }
};
class POP_EXPORTS FunctorSwitch
{
private:
    int _flipflop;
public:
    FunctorSwitch()
        :_flipflop(0)
    {
    }
    I32 nbrLevel()
    {
        return 2;
    }
    template<typename  Label ,typename VecN>
    I32 operator()(Label , const VecN&)
            const
    {
        return _flipflop;
    }
    template<typename VecN>
    I32 operator()( const VecN&)
            const
    {
        return _flipflop;
    }
    void switchFlipFlop()
    {
        _flipflop = (_flipflop+1)%2;
    }
    int getFlipFlop()
    {
        return _flipflop;
    }

};



template<typename FunctionTopo>
class POP_EXPORTS FunctorIntensityMax
{
private:

    const FunctionTopo &_topo;
    std::vector<std::pair<typename FunctionTopo::F,typename FunctionTopo::F> >  &_v_mean_sqrtvariance;
public:
    FunctorIntensityMax(const FunctionTopo & topo,std::vector<std::pair<typename FunctionTopo::F,typename FunctionTopo::F> > &v_mean_sqrtvariance)
        :_topo(topo),_v_mean_sqrtvariance(v_mean_sqrtvariance)
    {}
    I32 nbrLevel()
    {
        return NumericLimits<typename FunctionTopo::F>::maximumRange()+1;
    }
    template<typename  Label>
    I32 operator()(Label label, const typename FunctionTopo::E&x)
            const
    {
        return maximum(0.0,absolute(1.0*_topo(x)-_v_mean_sqrtvariance[label].first)-_v_mean_sqrtvariance[label].second);
    }
};


template<typename FunctionTopo>
class POP_EXPORTS FunctorMeanAdams
{
private:

    const FunctionTopo &_topo;
    std::vector<typename FunctionTopo::F> _v_mean;
public:
    FunctorMeanAdams(const FunctionTopo & topo, std::vector<typename FunctionTopo::F> & v_mean)
        :_topo(topo),_v_mean(v_mean){}
    I32 nbrLevel()
    {
        return NumericLimits<typename FunctionTopo::F>::maximumRange()+1;
    }
    template<typename  Label>
    I32 operator()(Label label, const typename FunctionTopo::E&x){
        return absolute((int)_topo(x)-(int)_v_mean[label]);
    }
};
}

#endif // FUNCTORPOP_HPP

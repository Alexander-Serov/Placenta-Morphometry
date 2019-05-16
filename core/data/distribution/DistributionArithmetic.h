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
#ifndef DISTRIBUTIONARITHMETIC_H
#define DISTRIBUTIONARITHMETIC_H
#include"core/data/distribution/Distribution.h"
#include"core/data/utility/Exception.h"
namespace pop
{
/// @cond DEV
class DistributionArithmetic : public Distribution
{
protected:
    Distribution  _fleft;
    Distribution  _fright;

    /*!
        \class pop::DistributionArithmetic
        \ingroup Distribution
        \brief Arithmetic distribution interface to allow arithmtic operations for the Distribution class
        \author Tariel Vincent

      \sa Distribution::operator +() Distribution::operator -() Distribution::operator *() Distribution::operator /()
    */

public:
    void setDistributionLeft(const Distribution & f_left);
    void setDistributionRight(const Distribution & f_right);
    F64 randomVariable()const throw(pexception);
    void setStep(F64 step)const;
    virtual void fromString(std::string str);
    virtual  std::string toString()const throw(pexception);

    Distribution & getDistributionLeft();
    Distribution & getDistributionRight();
    const Distribution & getDistributionLeft()const;
    const Distribution & getDistributionRight()const;
    DistributionArithmetic();
};


 class DistributionArithmeticAddition : public DistributionArithmetic
 {

 public:
     static const  std::string KEY;
    DistributionArithmeticAddition();
    DistributionArithmeticAddition(const DistributionArithmeticAddition & dist);
    virtual DistributionArithmeticAddition * clone()const throw(pexception);
    virtual F64 operator()(F64 value)const throw(pexception);
    ;
 };
 class DistributionArithmeticSubtraction : public DistributionArithmetic
 {

 public:
     static const  std::string KEY;
    DistributionArithmeticSubtraction();
    DistributionArithmeticSubtraction(const DistributionArithmeticSubtraction & dist);
    virtual DistributionArithmeticSubtraction * clone()const throw(pexception);
    virtual F64 operator()(F64 value)const throw(pexception);

 };
 class DistributionArithmeticMultiplication : public DistributionArithmetic
 {

 public:
     static const  std::string KEY;
    DistributionArithmeticMultiplication();
    DistributionArithmeticMultiplication(const DistributionArithmeticMultiplication & dist);
    virtual DistributionArithmeticMultiplication * clone()const throw(pexception);
    virtual F64 operator()(F64 value)const throw(pexception);

 };
 class DistributionArithmeticDivision : public DistributionArithmetic
 {

 public:
     static const  std::string KEY;
    DistributionArithmeticDivision();
    DistributionArithmeticDivision(const DistributionArithmeticDivision & dist);
    virtual DistributionArithmeticDivision * clone()const throw(pexception);
    virtual F64 operator()(F64 value)const throw(pexception);

 };
 class DistributionArithmeticComposition : public DistributionArithmetic
 {

 public:
     static const  std::string KEY;
    DistributionArithmeticComposition();
    DistributionArithmeticComposition(const DistributionArithmeticComposition & dist);
    virtual DistributionArithmeticComposition * clone()const throw(pexception);
    virtual F64 operator()(F64 value)const throw(pexception);

 };
 class DistributionArithmeticMax : public DistributionArithmetic
 {

 public:
     static const  std::string KEY;
    DistributionArithmeticMax();
    DistributionArithmeticMax(const DistributionArithmeticMax & dist);
    virtual DistributionArithmeticMax * clone()const throw(pexception);
    virtual F64 operator()(F64 value)const throw(pexception);

 };
 class DistributionArithmeticMin : public DistributionArithmetic
 {

 public:
     static const  std::string KEY;
    DistributionArithmeticMin();
    DistributionArithmeticMin(const DistributionArithmeticMin & dist);
    virtual DistributionArithmeticMin * clone()const throw(pexception);
    virtual F64 operator()(F64 value)const throw(pexception);

 };
/// @endcond
}
#endif // DISTRIBUTIONARITHMETIC_H

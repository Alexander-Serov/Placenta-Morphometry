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
#ifndef CLIBRARYDISTRIBUTIONMANUAL_HPP
#define CLIBRARYDISTRIBUTIONMANUAL_HPP
#include <cstring>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include"core/data/utility/Exception.h"
#include"core/data/distribution/Distribution.h"
#include"core/data/distribution/DistributionAnalytic.h"
#include"core/data/mat/MatN.h"
#include"core/data/mat/Mat2x.h"

#include"core/dependency/fparser.hh"


namespace pop
{

/// @cond DEV

class POP_EXPORTS DistributionRegularStep:public Distribution
{
private:
    /*!
        \class pop::DistributionRegularStep
        \ingroup Distribution
        \brief step function with regular spacing
        \author Tariel Vincent

    * The step function is a piecewise constant function having only finitely many regular discrete pieces:
    * \f[ f(x)=\sum_{i=0}^{n} a_i {\mathbf 1}_{x\in [xmin+i*spacing,xmin+(i+1)*spacing]} \f].
    *  To define it, we use an input matrix such that the first column contained the X-values with a regular spacing between successive
    *  value and the second colum Y-values.
    * \image html stepfunction.png
     *
    */
    F64 _xmin;
    F64 _xmax;
    F64 _spacing;
    DistributionUniformReal uni;
    std::vector<F64 >_table;
    std::vector<F64 >_repartition;
    void generateRepartition();
public:

    static const  std::string KEY;
    ~DistributionRegularStep();
    /*!
    * \fn DistributionRegularStep();
    *
    *   constructor the regular step distribution from a matrix such that the first column contained the X-values with a regular spacing between successive
    *  value and the second colum Y-values.
    */
    DistributionRegularStep();
    /*!
    * \fn DistributionRegularStep(const Mat2F64 & matrix);
    *
    *   constructor the regular step distribution from a matrix such that the first column contained the X-values with a regular spacing between successive
    *  value and the second colum Y-values.
    */
    DistributionRegularStep(const Mat2F64 & matrix);

    /*!
    \fn DistributionRegularStep(const DistributionRegularStep & dist);
    *
    *  copy constructor
    */
    DistributionRegularStep(const DistributionRegularStep & dist);



    virtual F64 operator ()(F64 value)const throw(pexception);
    virtual DistributionRegularStep * clone()const throw(pexception);
    virtual F64 randomVariable()const throw(pexception);


    /*!
    * \param matrix matrix 2d matrix  with two columns with x values and y values respectively
    * \param step set this value if the x-valus spacing is not constant
    *
    *  set the distribution from a matrix such that the first column contained the X-values with a regular spacing between successive
    *  values and the second colum Y-values.\n
    * However if the spacing between successive X-values is not regular but at least a monotonically increasing function, the step is set by default
    * as half of the minumum value of the difference of successive X-values.  X-values width you can define the sampling
    * of the step function with the argument step. Of course,you can define it!
    */
    void fromMatrix(const Mat2F64 & matrix,F64 step=0);

    /*!
    \fn Mat2F64 toMatrix()const;
    *
    *  return a two columns matrix where the first row contains the X-values with a regular spacing and the second column
    * the associated Y-values
    */
    Mat2F64 toMatrix()const;
     std::string toString()const throw(pexception);
    void fromString(std::string str);
    /*!
    \fn F64 fMinusOneForMonotonicallyIncreasing(F64 y)const;
    *
    *
    */
    F64 fMinusOneForMonotonicallyIncreasing(F64 y)const;

    void smoothGaussian(double sigma);
    virtual F64 getXmin()const;
    virtual F64 getXmax()const;
    virtual F64 getStep()const;
};

class POP_EXPORTS DistributionIntegerRegularStep:public DistributionDiscrete
{
private:
    /*!
        \class pop::DistributionIntegerRegularStep
        \ingroup Distribution
        \brief step function with regular spacing
        \author Tariel Vincent

    * The step function is a piecewise constant function having only finitely many regular discrete pieces:
    * \f[ f(x)=\sum_{i=0}^{n} a_i {\mathbf 1}_{x\in [xmin+i,xmin+(i+1)]} \f].
    *  To define it, we use an input matrix such that the first column contained the X-values with a regular spacing between successive
    *  value equal to 1 and the second colum Y-values.
    * \image html stepfunction.png
     *
    */
    F64 _xmin;
    F64 _xmax;
    DistributionUniformReal uni;
    std::vector<F64 >_table;
    std::vector<F64 >_repartition;
    void generateRepartition();
public:

    static const  std::string KEY;
    ~DistributionIntegerRegularStep();
    /*!
    * \fn DistributionIntegerRegularStep();
    *
    *   constructor the regular step distribution from a matrix such that the first column contained the X-values with a regular spacing between successive
    *  value equal to 1 and the second colum Y-values.
    */
    DistributionIntegerRegularStep();
    /*!
    * \fn DistributionIntegerRegularStep(const Mat2F64 & matrix);
    *
    *   constructor the regular step distribution from a matrix such that the first column contained the X-values with a regular spacing between successive
    *  value equal to 1  and the second colum Y-values.
    */
    DistributionIntegerRegularStep(const Mat2F64 & matrix);

    /*!
    \fn DistributionIntegerRegularStep(const DistributionIntegerRegularStep & dist);
    *
    *  copy constructor
    */
    DistributionIntegerRegularStep(const DistributionIntegerRegularStep & dist);



    virtual F64 operator ()(F64 value)const throw(pexception);
    virtual DistributionIntegerRegularStep * clone()const throw(pexception);
    virtual F64 randomVariable()const throw(pexception);


    /*!
    \fn void fromMatrix(const Mat2F64 & matrix)throw(pexception)
    *
    *  set the distribution from a matrix such that the first column contained the X-values with a regular spacing between successive
    *  values equal to 1 and the second colum Y-values.\n
    * However if the spacing between successive X-values is not regular but at least a monotonically increasing function, the step is set by default
    * as half of the minumum value of the difference of successive X-values.  X-values width you can define the sampling
    * of the step function with the argument step. Of course,you can define it!
    */
    void fromMatrix(const Mat2F64 & matrix)throw(pexception);

    /*!
    \fn Mat2F64 toMatrix()const;
    *
    *  return a two columns matrix where the first row contains the X-values with a regular spacing and the second column
    * the associated Y-values
    */
    Mat2F64 toMatrix()const;
     std::string toString()const throw(pexception);
    void fromString(std::string str);


    virtual F64 getXmin()const;
    virtual F64 getXmax()const;
};




class POP_EXPORTS DistributionExpression:public Distribution
{
private:
    /*!
        \class pop::DistributionExpression
        \ingroup Distribution
        \brief Parse a regular expression
        \author Tariel Vincent and Juha Nieminen

    * Parsing a regular expression with a single variable  x. For instance,
    \code
    std::string exp = "x*log(x)";
    DistributionExpression d(exp);
    Distribution derivate = Statistics::derivate(d,0.001,10);
    Distribution::multiDisplay(d,derivate,0.001,2.);
    \endcode
    *
    *
    */
     std::string func;
    mutable FunctionParser fparser;
public:
    static const  std::string KEY;
    ~DistributionExpression();
    DistributionExpression();
    DistributionExpression(std::string regularexpression)throw(pexception);
    DistributionExpression(const DistributionExpression & dist);
    virtual F64 operator ()(F64 value)const throw(pexception);
    F64 randomVariable()const throw(pexception);
    virtual DistributionExpression * clone()const throw(pexception);
    bool fromRegularExpression(std::string expression)throw(pexception);
     std::string toRegularExpression()const;
    void fromString(std::string str);
     std::string toString()const throw(pexception);
};
/// @endcond
}
#endif // CLIBRARYDISTRIBUTIONMANUAL_HPP

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

#ifndef COLLECTOREXECUTIONINFORMATION_H
#define COLLECTOREXECUTIONINFORMATION_H
#include<string>
#include"core/data/GP/Singleton.h"
#include"core/data/typeF/TypeF.h"
namespace pop
{

enum  CollectorExecutionMode{
    RATIO,
    INFO,
    NOINFO
};

//class POP_EXPORTS CollectorExecutionInformationImplementationInFile
//{
//protected:
//    std::string _function;
//    F64 _ratio;
//    CollectorExecutionMode _mode;

//public:
//    CollectorExecutionInformationImplementationInFile();
//    virtual ~CollectorExecutionInformationImplementationInFile();
//    virtual void startExecution(std::string function,CollectorExecutionMode mode);
//    virtual  void info(std::string msg);
//    virtual void endExecution(std::string function);
//    virtual void progression(F64 ratio,std::string msg="");
//};


class POP_EXPORTS CollectorExecutionInformationImplementation
{
protected:
    std::string _function;
    F64 _ratio;
    CollectorExecutionMode _mode;

public:
    CollectorExecutionInformationImplementation();
    virtual ~CollectorExecutionInformationImplementation();
    virtual void startExecution(std::string function,CollectorExecutionMode mode);
    virtual  void info(std::string msg);
    virtual void endExecution(std::string function);
    virtual void progression(F64 ratio,std::string msg="");
};

namespace Private{
class POP_EXPORTS CollectorExecutionInformation
{
protected:

    CollectorExecutionInformationImplementation* _impl;
        bool _activate;
public:
    CollectorExecutionInformation();
    void setCollector(CollectorExecutionInformationImplementation* collector);
    void startExecution(std::string function,CollectorExecutionMode mode = RATIO);
    void endExecution(std::string function);
    void info(std::string msg);
    void progression(F64 ratio,std::string msg="");
    void setActivate(bool activate);
};
}


typedef Singleton<Private::CollectorExecutionInformation> CollectorExecutionInformationSingleton;
}
#endif // COLLECTOREXECUTIONINFORMATION_H

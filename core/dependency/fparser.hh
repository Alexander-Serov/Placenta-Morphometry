/***************************************************************************\
|* Function Parser for C++ v3.1.4                                          *|
|*-------------------------------------------------------------------------*|
|* Copyright: Juha Nieminen                                                *|
\***************************************************************************/

#ifndef ONCE_FPARSER_H_
#define ONCE_FPARSER_H_

#include <string>
#include <vector>
#include"core/data/typeF/TypeF.h"
#include"core/data/utility/Configuration.h"
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
#include <iostream>
#endif

namespace FPoptimizer_CodeTree { class CodeTree; }

class POP_EXPORTS FunctionParser
{
public:
    enum ParseErrorType
    {
        SYNTAX_ERROR=0, MISM_PARENTH, MISSING_PARENTH, EMPTY_PARENTH,
        EXPECT_OPERATOR, OUT_OF_MEMORY, UNEXPECTED_ERROR, INVALID_VARS,
        ILL_PARAMS_AMOUNT, PREMATURE_EOS, EXPECT_PARENTH_FUNC,
        FP_NO_ERROR
    };

    pop::I32 Parse(const char* Function, const std::string& Vars,
              bool useDegrees = false);
    pop::I32 Parse(const std::string& Function, const std::string& Vars,
              bool useDegrees = false);

    void setDelimiterChar(char);

    const char* ErrorMsg() const;
    inline ParseErrorType GetParseErrorType() const { return parseErrorType; }

    pop::F64 Eval(const pop::F64* Vars);
    inline pop::I32 EvalError() const { return evalErrorType; }

    bool AddConstant(const std::string& name, pop::F64 value);
    bool AddUnit(const std::string& name, pop::F64 value);

    typedef pop::F64 (*FunctionPtr)(const pop::F64*);

    bool AddFunction(const std::string& name,
                     FunctionPtr, unsigned paramsAmount);
    bool AddFunction(const std::string& name, FunctionParser&);

    void Optimize();


    FunctionParser();
    ~FunctionParser();

    // Copy constructor and assignment operator (implemented using the
    // copy-on-write technique for efficiency):
    FunctionParser(const FunctionParser&);
    FunctionParser& operator=(const FunctionParser&);


    void ForceDeepCopy();


#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
    // For debugging purposes only:
    void PrintByteCode(std::ostream& dest) const;
#endif



//========================================================================
private:
//========================================================================

// Private data:
// ------------
    char delimiterChar;
    ParseErrorType parseErrorType;
    pop::I32 evalErrorType;

    friend class FPoptimizer_CodeTree::CodeTree;

    struct Data;
    Data* data;

    bool useDegreeConversion;
    unsigned evalRecursionLevel;
    unsigned StackPtr;
    const char* errorLocation;


// Private methods:
// ---------------
    void CopyOnWrite();
    bool CheckRecursiveLinking(const FunctionParser*) const;
    bool NameExists(const char*, unsigned);
    bool ParseVariables(const std::string&);
    pop::I32 ParseFunction(const char*, bool);
    const char* SetErrorType(ParseErrorType, const char*);

    void AddFunctionOpcode(unsigned);
    inline void incStackPtr();

    const char* CompileIf(const char*);
    const char* CompileFunctionParams(const char*, unsigned);
    const char* CompileElement(const char*);
    const char* CompilePossibleUnit(const char*);
    const char* CompilePow(const char*);
    const char* CompileUnaryMinus(const char*);
    const char* CompileMult(const char*);
    const char* CompileAddition(const char*);
    const char* CompileComparison(const char*);
    const char* CompileAnd(const char*);
    const char* CompileExpression(const char*);
};

#endif

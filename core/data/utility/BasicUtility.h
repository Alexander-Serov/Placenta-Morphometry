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

#ifndef BasicUtility_HPP
#define BasicUtility_HPP

#include <iostream>
#include <sstream>
#include<vector>
#include<cmath>
#include"core/data/typeF/TypeF.h"
#include"core/data/utility/Exception.h"
#include"core/data/utility/Configuration.h"

//namespace std
//{
//POP_EXPORTS std::istream& operator >> (std::istream& in, unsigned char & c);
//POP_EXPORTS std::ostream& operator << (std::ostream& out, unsigned char  c);
//}

namespace pop
{
/*! \ingroup Other
* \defgroup BasicUtility  BasicUtility
* \brief Some cross OS utilities

*/

const double PI = 3.141592;
const double EPSILON = 0.000001;
#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
template<typename T>
struct Identity
{
    typedef T Result;
};

template <typename T>
int POP_EXPORTS sgn(T val)
{
    return (val >= T(0)) - (val < T(0));
}

class POP_EXPORTS BasicUtility
{
public:
    /*!
        \class pop::BasicUtility
        \brief  String functionnality
        \author Tariel Vincent
        \ingroup BasicUtility
      *
    */

    /*!
    \brief Is the file_path a file ?
    \param filepath file path
    \return yes is file, false otherwise
    \brief check for a file's existence
    *
    * \code
    string path ="/home/vtariel/lena.pgm";
    if(BasicUtility::isFile(path)==true){
        //Process
    }
    * \endcode
    *
    */
    static bool isFile(std::string filepath);
    /*!
    \brief Is the file_path a directory ?
    \param dirpath directory path
    \return yes is directory, false otherwise
     \brief check for a directory's existence
    *
    * \code
    string path ="/home/vtariel/";
    if(BasicUtility::isDirectory(path)==true){
        //Process
    }
    * \endcode
    *
    */
    static bool isDirectory(std::string dirpath);
    /*!
    \brief make a directory
    \param dirpath directory path
    \return false if the directory already exists
    *
    * \code
    string path ="/home/vtariel/";
    if(BasicUtility::makeDirectory(path)==true){
        //Process
    }
    * \endcode
    *
    */
    static bool makeDirectory(std::string dirpath);

    /*!
    \brief return the base name
    \param file file name
    \return the base name
    *
    * \code
    string file ="/home/vtariel/toto.xml";
    cout<<BasicUtility::getBasefilename(file)<<endl;// toto
    * \endcode
    *
    */
    static  std::string getBasefilename(std::string file);

    /*!
    \brief return the directory path  n
    \param file file name
    \return the directory pat
    *
    * \code
    string file ="/home/vtariel/toto.xml";
    cout<<BasicUtility::getBasefilename(file)<<endl;// /home/vtariel/
    * \endcode
    *
    */
    static  std::string getPath(std::string file);
    static  std::string getExtension(std::string file);

    /*!
    \brief get string until the delimeter
    \param in input stream
    \param del delimeter
    \return string
    *
    * \code
    std::stringstream in("Data1$$Data2$$Data3$$Data4$$");
    while(!in.eof()){
        std::cout<<BasicUtility::getline(in,"$$")<<std::endl;
     }
    return 1;
    * \endcode
    *
    */
    static  std::string getline(std::istream& in,std::string del);
    /*!
    \brief get files in the given directory
    \param dir input stream
    \return file name in the directory
    *
    * \code
    string dir = "/home/vtariel/Bureau/DataBase/Acquisition/record/";
    std::vector<string> files = BasicUtility::getFilesInDirectory(dir);
    for(int i=0;i<files.size();i++){
        Mat2RGBUI8 img;
        string path = dir+files[i];
        img.load(path.c_str());
        //process
    }
    * \endcode
    *
    */
    static std::vector<std::string> getFilesInDirectory (std::string dir);
    /*!
    \brief convert a string to any type
    \param s input string
    \param Dest output object (The insertion operator (<<) should be surcharged for the template class)
    Outdated: \return true success, false otherwise
    Due to implementation issues, always return true
        *
    * \code
    std::string str = "2.7";
    double d;
    BasicUtility::String2Any(str,d);
    std::cout<<d<<std::endl;
    * \endcode
    *
    */
    template<typename T>
    static bool String2Any(std::string s,  T & Dest )
    {

        std::istringstream iss(s );
        iss >> Dest;

        return true;
    }

    /*!
    \brief convert any type to a string
    \param Value input object (The extraction operator (>>) should be surcharged for the template class)
    \param s output string
    \return true success, false otherwise
    *
    * \code
    std::string str ;
    double d=2.7;
    BasicUtility::Any2String(d,str);
    std::cout<<str<<std::endl;
    * \endcode
    *
    */
    template<typename T>
    static bool Any2String(T Value,  std::string & s)
    {
        s.clear();
        std::ostringstream oss;
        std::ostream& os =  oss;

        bool temp = os << Value;
        s= oss.str();
        return temp;
    }
    /*!
    \brief convert any type to a string
    \param Value input object (The extraction operator (>>) should be surcharged for the template class)
    \return output string
    *
    * \code
    double d=2.7;
    std::cout<<BasicUtility::Any2String(d,str)<<std::endl;
    * \endcode
    *
    */
    template<typename T>
    static  std::string Any2String(T Value)
    {
        std::string s;
        s.clear();
        std::ostringstream oss;
        //        std::ostream &os =  oss;
        oss << Value;
        s= oss.str();
        return s;
    }
    /*!
    \brief convert a possitive number to a string with a fixed digit number
    \param value unsigned integer value
    \param digitnumber number of digit
    \return output string
    *
    * \code
     std::cout<<BasicUtility::IntFixedDigit2String(35,4)<<std::endl;//0035
    * \endcode
    *
    */
    static  std::string IntFixedDigit2String(unsigned int value,int digitnumber);

    static  std::string replaceSlashByAntiSlash(std::string filepath);
    //edit-Distance
    static int editDistance(std::string s1,std::string s2);


    static void cryptOrDecryptFileXORKey(std::string inputfile,std::string outputfile, pop::UI32 key =0xAAF588BB);
};

static inline int power2 (unsigned int x)
{
    switch( x )
    {
    case  0:
        return 1;
    case  1:
        return 2;
    case  2:
        return 4;
    case  3:
        return 8;
    case  4:
        return 16;
    case  5:
        return 32;
    case  6:
        return 64;
    case  7:
        return 128;
    case  8:
        return 256;
    case  9:
        return 512;
    case  10:
        return 1024;
    case  11:
        return 2048;
    case  12:
        return  4096;
    case  13:
        return 8192;
    case  14:
        return 16384;
    case  15:
        return 32768;
    case  16:
        return  65536;
    case  17:
        return 131072;
    case  18:
        return  262144;
    case  19:
        return  524288;
    case  20:
        return  1048576;
    case  21:
        return  2097152;
    case  22:
        return  4194304;
    case  23:
        return   8388608;
    case  24:
        return  16777216;
    case  25:
        return 33554432;
    case  26:
        return   67108864;
    case  27:
        return  134217728;
    case  28:
        return  268435456;
    case  29:
        return  536870912;
    case  30:
        return   1073741824;
    default:
        return 1;
    }
}

template<typename T1>
T1 minimum(T1 v1,T1 v2){
    return std::min(v1,v2);
}
template<typename T1>
T1 maximum(T1 v1,T1 v2){
    return std::max(v1,v2);
}
template<typename T1>
T1 absolute(T1 v1){
    return std::abs(static_cast<double>(v1));
}
template<typename T1>
double normValue(const T1& v1,int =2){
    return std::abs(static_cast<double>(v1));
}
template<typename T1>
double normPowerValue(T1 v1,int p=2){
    if(p==0||p==1)
        return std::abs(static_cast<double>(v1));
    else if(p==2)
        return v1*v1;
    else
        return std::pow(std::abs(static_cast<double>(v1)),p);
}
template<typename T1>
double distance(const T1& v1,const T1& v2, int p =2){
    return normValue(v1-v2,p);
}


template <class T1, class T2>
double productInner(const T1&  x1,const T2&  x2)
{
    return static_cast<double>(x1)*static_cast<double>(x2);
}
template<typename T1>
T1 round(T1 v1){
    return std::floor(v1+0.5);
}
template <class T1>
T1  squareRoot(T1  x)
{
    return std::sqrt(x);
}
}
namespace std{

template<class _T1, class _T2>
std::ostream& operator << (std::ostream &out, const std::pair<_T1,_T2> &arg)
{

    out << arg.first << "<P>" << arg.second<<"<P>";
    return out;
}
template<class _T1, class _T2>
std::istream& operator >>(std::istream& in, std::pair<_T1,_T2> &arg)
{
    std::string str;
    str = pop::BasicUtility::getline( in, "<P>" );
    pop::BasicUtility::String2Any(str,arg.first );
    str = pop::BasicUtility::getline( in,"<P>" );
    pop::BasicUtility::String2Any(str,arg.second );
    return in;
}

template<class _T1>
std::ostream& operator << (std::ostream &out, const std::vector<_T1> &arg)
{
    out<<(int)arg.size()<<"<V>";
    for(int i=0;i<(int)arg.size();i++)
        out << arg[i] << "<V>";
    return out;
}
template<class _T1>
std::istream& operator >> (std::istream &in,std::vector<_T1> &arg)
{
    arg.clear();
    std::string str;
    str = pop::BasicUtility::getline( in, "<V>" );
    int size;
    pop::BasicUtility::String2Any(str,size );
    for(int i=0;i<size;i++){
        _T1 t;
        str = pop::BasicUtility::getline( in, "<V>" );
        pop::BasicUtility::String2Any(str,t );
        arg.push_back(t);
    }
    return in;
}


template <typename In1, typename In2, typename In3, typename Out, typename FUNC>
Out transform3(In1 first1, In1 last1, In2 first2, In3 first3, Out out, FUNC f) {
    while (first1 != last1) {
        *out++ = f(*first1++, *first2++, *first3++);
    }
    return out;
}
template <typename In1, typename In2, typename In3,typename In4, typename Out, typename FUNC>
Out transform4(In1 first1, In1 last1, In2 first2, In3 first3, In4 first4,Out out, FUNC f) {
    while (first1 != last1) {
        *out++ = f(*first1++, *first2++, *first3++,*first4++);
    }
    return out;
}







}
#endif // BasicUtility_HPP

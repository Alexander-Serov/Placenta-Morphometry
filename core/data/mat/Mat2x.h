#ifndef MATRIX2_2_H
#define MATRIX2_2_H
#include<vector>
#include<iostream>
#include"core/data/utility/Configuration.h"
#include"core/data/vec/Vec.h"
#include"core/data/mat/MatN.h"





/// @cond DEV
namespace pop
{
/*! \ingroup Matrix
* \defgroup Mat2x   Mat2x
* \brief template class for small matrices which fixed type and fixed size
*/
template<typename Type, int SIZEI, int SIZEJ>
class POP_EXPORTS Mat2x
{
public:
    /*!
    \class pop::Mat2x
    \ingroup Mat2x
    \brief template class for small matrices , which fixed type and fixed size
    \author Tariel Vincent
    \tparam SIZEI number of rows
    \tparam SIZEI number of cols
    \tparam Type Pixel type

    The class represents small matrices, which type and size are known at compile time. The elements of a matrix M are accessible using M(i,j) notation.
    The class defintion is similar to pop::MatN  and If you need to do some operation on Matx that is not implemented, it is easy to convert the matrix to MatN and backwards.
    */
    enum {DIM=2};
    Type _dat[SIZEI*SIZEJ];
    typedef Vec2I32 E;
    typedef Vec2I32 Domain;
    typedef Type F;
    typedef typename MatN<2,Type>::IteratorEDomain  IteratorEDomain;
    typedef typename MatN<2,Type>::IteratorENeighborhood IteratorENeighborhood;
    typedef MatN<1, F>  Hyperplane;
    Mat2x(Type value=Type());
    Mat2x(const Mat2x &m);
    Mat2x(const MatN<2,Type> &m);
//    Mat2x(const Type* v_value );
    unsigned int sizeI()const;
    unsigned int sizeJ()const;
    const Type & operator ()( const E &  i_j)const;
    Type & operator ()( const E &  i_j);
    const Type & operator ()( unsigned int  i,unsigned int j)const;
    Type & operator ()(unsigned int  i,unsigned int j);
    bool isValid(int  i,int j) const;
    Mat2x & operator =(const Mat2x &m);
    template<typename Type1>
    Mat2x & operator =(const MatN<2,Type1> &m);

    Mat2x & operator*=(const Mat2x &m);
    Mat2x<Type,SIZEI-1,SIZEJ> deleteRow(unsigned int i)const;
    Mat2x<Type,SIZEI,SIZEJ-1> deleteCol(unsigned int j)const;
    Vec<Type> getRow(unsigned int i)const;
    Vec<Type> getCol(unsigned int j)const;
    void setRow(unsigned int i,const Vec<Type>& v);
    void setCol(unsigned int j,const Vec<Type>& v);
    void swapRow(unsigned int i_0,unsigned int i_1);
    void swapCol(unsigned int j_0,unsigned int j_1);
    Type minorDet(unsigned int i,unsigned int j)const;
    Type cofactor(unsigned int i,unsigned int j)const;
    Mat2x cofactor()const;
    Mat2x<Type,SIZEJ,SIZEI> transpose()const;
    Type determinant()const;
    Type trace()const ;
    Mat2x inverse()const;
    Vec<Type>  operator*(const Vec<Type>& v)const ;
    VecN<2,Type>  operator*(const VecN<2,Type>& v)const ;
    template<int SIZEK>
    Mat2x<Type,SIZEI,SIZEK> operator*(const  Mat2x<Type,SIZEJ,SIZEK>& m)const ;
    Mat2x  mult(const  Mat2x& m)const ;
    void load(const char * file)throw(pexception);
    void save(const char * file)const throw(pexception);
    Domain getDomain()const;
    void display() const;


    Mat2x  operator+(const Mat2x& f)const{
        Mat2x h(*this);
        h +=f;
        return h;
    }
    Mat2x  operator-(const Mat2x& f)const{
        Mat2x h(*this);
        h -=f;
        return h;
    }
    Mat2x  operator-()const{
        Mat2x h(F(0));
        h -=*this;
        return h;
    }
    Mat2x&  operator+=(const Mat2x& f)
    {
        for(int i=0;i<SIZEI*SIZEJ;i++)
            this->_dat[i]+=f._dat[i];

        return *this;
    }
    Mat2x&  operator-=(const Mat2x& f)
    {
        for(int i=0;i<SIZEI*SIZEJ;i++)
            this->_dat[i]-=f._dat[i];
        return *this;
    }
    template<typename G>
    Mat2x&  operator=(const Mat2x<G,SIZEI,SIZEJ> & f)
    {
        for(int i=0;i<SIZEI*SIZEJ;i++)
            this->_dat[i]=f._dat[i];
        return *this;
    }
    template<typename G>
    Mat2x&  operator=(G value)
    {
        for(int i=0;i<SIZEI*SIZEJ;i++)
            this->_dat[i]=value;
        return *this;
    }


    Mat2x&  operator*=(Type value)
    {
        for(int i=0;i<SIZEI*SIZEJ;i++)
            this->_dat[i]*=value;
        return *this;
    }
    Mat2x operator*(Type value)const{
        Mat2x temp(*this);
        temp*=value;
        return temp;
    }
    Mat2x&  operator/=(Type value)
    {
        for(int i=0;i<SIZEI*SIZEJ;i++)
            this->_dat[i]/=value;
        return *this;
    }
    Mat2x operator/(Type value)const{
        Mat2x temp(*this);
        temp/=value;
        return temp;
    }

#ifdef WITHSWIG
    Type getValue(int i, int j)const{
        return  this->_dat[j+ (i<<1)];
    }
    void setValue(int i, int j , Type value){
        this->_dat[j+ (i<<1)]=value;
    }
#endif

};
template<typename Type, int SIZEI, int SIZEJ>
Mat2x<Type, SIZEI, SIZEJ>::Mat2x(Type value)
{
    for(int i=0;i<SIZEI*SIZEJ;i++)
        this->_dat[i]=value;
}
//template<typename Type, int SIZEI, int SIZEJ>
//Mat2x<Type, SIZEI, SIZEJ>::Mat2x(const Type* v_value ){
//    for(int i=0;i<SIZEI*SIZEJ;i++)
//        this->_dat[i]=v_value[i];
//}

template<typename Type, int SIZEI, int SIZEJ>
Mat2x<Type, SIZEI, SIZEJ>::Mat2x(const Mat2x &f)
{
    for(int i=0;i<SIZEI*SIZEJ;i++)
        this->_dat[i]=f._dat[i];

}
template<typename Type, int SIZEI, int SIZEJ>
Mat2x<Type, SIZEI, SIZEJ>::Mat2x(const MatN<2, Type> &f)
{
    POP_DbgAssert(f.sizeI()==SIZEI&&f.sizeJ()==SIZEJ);
    for(int i=0;i<SIZEI*SIZEJ;i++)
        this->_dat[i]=f._dat[i];

}

template<typename Type, int SIZEI, int SIZEJ>
unsigned int Mat2x<Type, SIZEI, SIZEJ>::sizeI()const {
    return SIZEI;
}
template<typename Type, int SIZEI, int SIZEJ>
unsigned int Mat2x<Type, SIZEI, SIZEJ>::sizeJ()const{
    return SIZEJ;
}

template<typename Type, int SIZEI, int SIZEJ>
bool Mat2x<Type, SIZEI, SIZEJ>::isValid(int  i,int j) const{
    if(i>=0&&i<SIZEI&&j>=0&&j<SIZEJ)
        return true;
    else
        return false;
}
template<typename Type, int SIZEI, int SIZEJ>
const Type & Mat2x<Type, SIZEI, SIZEJ>::operator ()(  const E &i_j)const
{
    POP_DbgAssert( i_j(0) >= 0&& i_j(1)>=0 && i_j(0)<sizeI()&&i_j(1)<sizeJ());
    return  this->_dat[i_j(1)+ i_j(0)*SIZEJ];
}
template<typename Type, int SIZEI, int SIZEJ>
Type & Mat2x<Type, SIZEI, SIZEJ>::operator ()( const E &i_j)
{
    POP_DbgAssert( i_j(0) >= 0&& i_j(1)>=0 && i_j(0)<sizeI()&&i_j(1)<sizeJ());
    return  this->_dat[i_j(1)+ i_j(0)*SIZEJ];
}
template<typename Type, int SIZEI, int SIZEJ>
const Type & Mat2x<Type, SIZEI, SIZEJ>::operator ()(unsigned int  i,unsigned int j)
const
{
    POP_DbgAssert(  i<sizeI()&&j<sizeJ());
    return  this->_dat[j+ i*SIZEJ];
}
template<typename Type, int SIZEI, int SIZEJ>
Type & Mat2x<Type, SIZEI, SIZEJ>::operator ()(unsigned int  i,unsigned int j)
{
    POP_DbgAssert(  i<sizeI()&&j<sizeJ());
    return  this->_dat[j+ i*SIZEJ];
}
template<typename Type, int SIZEI, int SIZEJ>
void Mat2x<Type, SIZEI, SIZEJ>::load(const char *file)throw(pexception)
{
    std::ifstream  in(file);
    if (in.fail())
    {
        throw(pexception("In Matrix::load, cannot open file: "+std::string(file)));
    }
    else
    {
        in>>*this;
    }
}
template<typename Type, int SIZEI, int SIZEJ>
void Mat2x<Type, SIZEI, SIZEJ>::save(const char * file)const throw(pexception){
    std::ofstream  out(file);
    if (out.fail())
    {
        throw(pexception("In Matrix::save, cannot open file: "+std::string(file)));
    }
    else
    {
        out<<*this;
    }
}
template<typename Type, int SIZEI, int SIZEJ>
Mat2x<Type, SIZEI-1, SIZEJ> Mat2x<Type, SIZEI, SIZEJ>::deleteRow(unsigned int i)const{
    POP_DbgAssert(i<sizeJ());
    Mat2x<Type, SIZEI-1, SIZEJ> temp;
    for( int i1=0;i1<SIZEI-1;i1++)
        for( int j=0;j<SIZEJ;j++)
        {
            if(i1<static_cast<int>(i))
                temp(i1,j)=this->operator ()(i1,j);
            else
                temp(i1,j)=this->operator ()(i1+1,j);
        }
    return temp;
}
template<typename Type, int SIZEI, int SIZEJ>
Mat2x<Type, SIZEI, SIZEJ-1> Mat2x<Type, SIZEI, SIZEJ>::deleteCol(unsigned int j)const{
    POP_DbgAssert(j<sizeJ());
    Mat2x<Type, SIZEI, SIZEJ-1> temp;
    for( int i=0;i<SIZEI;i++)
        for( int j1=0;j1<SIZEJ-1;j1++)
        {
            if(j1<static_cast<int>(j))
                temp(i,j1)=this->operator ()(i,j1);
            else
                temp(i,j1)=this->operator ()(i,j1+1);
        }
    return temp;

}

template<typename Type, int SIZEI, int SIZEJ>
Vec<Type> Mat2x<Type, SIZEI, SIZEJ>::getRow(unsigned int i)const{
    Vec<Type> v(SIZEJ);
    int add = i*SIZEJ;
    for(int j=0;j<SIZEJ;j++){
        v(j)= this->_dat[add+j];
    }
    return v;
}
template<typename Type, int SIZEI, int SIZEJ>
Vec<Type> Mat2x<Type, SIZEI, SIZEJ>::getCol(unsigned int j)const{
    Vec<Type> v(SIZEI);
    int add = j;
    for(int i=0;i<SIZEI;i++){
        v(j)= this->_dat[add];
        add +=SIZEJ;
    }
    return v;
}
template<typename Type, int SIZEI, int SIZEJ>
void Mat2x<Type, SIZEI, SIZEJ>::setRow(unsigned int i,const Vec<Type> &v){
    POP_DbgAssert(v.size()==SIZEJ);
    int add = i*SIZEJ;
    for(int j=0;j<SIZEJ;j++){
        this->_dat[add+j] = v(j);
    }
}
template<typename Type, int SIZEI, int SIZEJ>
void Mat2x<Type, SIZEI, SIZEJ>::setCol(unsigned int j,const Vec<Type>& v){
    POP_DbgAssert(v.size()==SIZEI);
    int add = j;
    for(int i=0;i<SIZEI;i++){
        this->_dat[add] = v(j);
        add +=SIZEJ;
    }
}
template<typename Type, int SIZEI, int SIZEJ>
void Mat2x<Type, SIZEI, SIZEJ>::swapRow(unsigned int i_0,unsigned int i_1){
    POP_DbgAssert(i_0<0||i_0>=SIZEI||i_1<0||i_1>=SIZEI);

    int addi_0 = i_0*SIZEJ;
    int addi_1 = i_1*SIZEJ;
    for(int j=0;j<SIZEJ;j++){
        std::swap(this->_dat[addi_0+j],this->_dat[addi_1+j]);
    }
}
template<typename Type, int SIZEI, int SIZEJ>
void Mat2x<Type, SIZEI, SIZEJ>::swapCol(unsigned int j_0,unsigned int j_1){
    POP_DbgAssert(j_0<SIZEJ&&j_1<SIZEJ);
    int addj_0 = j_0;
    int addj_1 = j_1;
    for(int i=0;i<SIZEI;i++){
        std::swap(this->_dat[addj_0],this->_dat[addj_1]) ;
        addj_0 +=SIZEJ;
        addj_1 +=SIZEJ;
    }
}
template<typename Type, int SIZEI, int SIZEJ>
Type Mat2x<Type, SIZEI, SIZEJ>::minorDet(unsigned int i,unsigned int j)const{
    POP_DbgAssert(  i<sizeI()&&j<sizeJ());
    return this->deleteRow(i).deleteCol(j).determinant();
}
template<typename Type, int SIZEI, int SIZEJ>
Type Mat2x<Type, SIZEI, SIZEJ>::cofactor(unsigned int i, unsigned int j)const{
    if( (i+j)%2==0)
        return this->minorDet(i,j);
    else
        return -this->minorDet(i,j);
}
template<typename Type, int SIZEI, int SIZEJ>
Mat2x<Type, SIZEI, SIZEJ> Mat2x<Type, SIZEI, SIZEJ>::cofactor()const{
    Mat2x<Type, SIZEI, SIZEJ> temp;
    for( int i=0;i<SIZEI;i++)
        for( int j=0;j<SIZEJ;j++)
        {
            temp.operator ()(i,j)=this->cofactor(i,j);
        }
    return temp;
}
template<typename Type, int SIZEI, int SIZEJ>
Mat2x<Type, SIZEJ, SIZEI> Mat2x<Type, SIZEI, SIZEJ>::transpose()const
{
    Mat2x<Type, SIZEJ, SIZEJ> temp;
    for( int i=0;i<SIZEI;i++)
        for( int j=0;j<SIZEJ;j++)
        {
            temp.operator ()(j,i)=this->operator()(i,j);
        }
    return temp;
}
template<typename Type, int SIZEI, int SIZEJ>
Type Mat2x<Type, SIZEI, SIZEJ>::determinant() const{
    F det=0;
    for(unsigned int i=0;i<this->sizeI();i++)
    {
        det +=(this->operator ()(i,0)*this->cofactor(i,0));
    }
    return det;
}
template<typename Type, int SIZEI, int SIZEJ>
Type Mat2x<Type, SIZEI, SIZEJ>::trace() const
{
    Type sum=0;
    for(unsigned int i=0;i<this->sizeI();i++)
        sum+=this->operator ()(i,i);
    return sum;
}
template<typename Type, int SIZEI, int SIZEJ>
Mat2x<Type, SIZEI, SIZEJ> Mat2x<Type, SIZEI, SIZEJ>::inverse()const{
    Mat2x<Type, SIZEI, SIZEJ> temp;
    Type det = this->determinant();
    temp = this->cofactor();
    temp = temp.transpose();
    temp/=det;
    return temp;
}
template<typename Type, int SIZEI, int SIZEJ>
Mat2x<Type, SIZEI, SIZEJ> &Mat2x<Type, SIZEI, SIZEJ>::operator =(const Mat2x& f)
{

    for(int i=0;i<SIZEI*SIZEJ;i++)
        this->_dat[i]=f._dat[i];
    return *this;
}

template<typename Type, int SIZEI, int SIZEJ>
template<typename Type1>
Mat2x<Type, SIZEI, SIZEJ> &Mat2x<Type, SIZEI, SIZEJ>::operator =(const MatN<2,Type1>& f)
{

    for(int i=0;i<SIZEI*SIZEJ;i++)
        this->_dat[i]=f.operator [](i);
    return *this;
}

template<typename Type, int SIZEI, int SIZEJ>
Mat2x<Type, SIZEI, SIZEJ> & Mat2x<Type, SIZEI, SIZEJ>::operator*=(const Mat2x<Type, SIZEI, SIZEJ> &m)
{   POP_DbgAssertMessage(SIZEI==SIZEJ,"Use operator* if SIZEI!=SIZEJ");
    Mat2x<Type, SIZEI, SIZEJ> temp(*this);
    for( int i=0;i<SIZEI;i++)
        for( int j=0;j<SIZEJ;j++)
        {
            Type sum=0;
            for(unsigned int k=0;k<SIZEJ;k++)
                sum+=temp(i,k)*m(k,j);
            this->operator ()(i,j)=sum;
        }
    return *this;
}
template<typename Type, int SIZEI, int SIZEJ>
Mat2x<Type, SIZEI, SIZEJ>  Mat2x<Type, SIZEI, SIZEJ>::mult(const Mat2x<Type, SIZEI, SIZEJ> &m)const
{
    return this->operator *(m);
}

template<typename Type, int SIZEI, int SIZEJ>
Vec<Type>  Mat2x<Type, SIZEI, SIZEJ>::operator*(const Vec<Type> & v)const
{
    Vec<Type> temp(SIZEI);
    for( int i=0;i<SIZEI;i++)
        for( int j=0;j<SIZEJ;j++)
        {
            temp(i)+=this->operator ()(i,j)*v(j);
        }
    return temp;
}
template<typename Type, int SIZEI, int SIZEJ>
template<int SIZEK>
Mat2x<Type, SIZEI, SIZEK>  Mat2x<Type, SIZEI, SIZEJ>::operator*(const Mat2x<Type, SIZEJ, SIZEK> &f)const {

    Mat2x<Type, SIZEI, SIZEK> temp;
    for( int i=0;i<SIZEI;i++)
        for( int j=0;j<SIZEK;j++)
        {
            Type sum=0;
            for(unsigned int k=0;k<SIZEJ;k++)
                sum+=this->operator ()(i,k)*f(k,j);
            temp(i,j)=sum;
        }
    return temp;

}
template<typename Type, int SIZEI, int SIZEJ>
void Mat2x<Type, SIZEI, SIZEJ>::display() const{
    std::cout<<*this;
    std::cout<<std::endl;
}

template<typename Type, int SIZEI, int SIZEJ>
typename Mat2x<Type, SIZEI, SIZEJ>::Domain Mat2x<Type, SIZEI, SIZEJ>::getDomain()const
{
    return Vec2I32(SIZEI,SIZEJ);
}

template<int Dim, typename Result>
template<int SIZEI, int SIZEJ>
MatN<Dim, Result>::MatN(const Mat2x<Result,SIZEI,SIZEJ> f)
    :std::vector<Result>(SIZEI*SIZEJ)
{

    _domain(0)=SIZEI;
    _domain(1)=SIZEJ;
    for(int i=0;i<SIZEI*SIZEJ;i++)
        this->_dat[i]=f._dat[i];
}

template<typename Type, int SIZEI, int SIZEJ>
void FunctionAssert(const Mat2x<Type, SIZEI, SIZEJ> & , const Mat2x<Type, SIZEI, SIZEJ> &  ,std::string )
{
}

template<typename F1,typename F2, int SIZEI, int SIZEJ>
struct FunctionTypeTraitsSubstituteF<Mat2x<F1,SIZEI,  SIZEJ>,F2 >
{
    typedef Mat2x<F2,SIZEI,  SIZEJ> Result;
};
template<typename Type, int SIZEI, int SIZEJ>
struct NumericLimits<Mat2x<Type, SIZEI, SIZEJ> >
{
    static F64 min() throw()
    { return -NumericLimits<Type>::maximumRange();}
    static F64 max() throw()
    { return NumericLimits<Type>::maximumRange();}
};
template<typename Type, int SIZEI, int SIZEJ>
struct isVectoriel<Mat2x<Type, SIZEI, SIZEJ>  >{
    enum { value =true};
};

template<typename Type, int SIZEI, int SIZEJ>
struct ArithmeticsSaturation< Mat2x<Type, SIZEI, SIZEJ>,Mat2x<Type, SIZEI, SIZEJ> >
{
    static Mat2x<Type, SIZEI, SIZEJ> Range(const Mat2x<Type, SIZEI, SIZEJ>& p)
    {
        return p;
    }
};

template<typename Type, int SIZEI, int SIZEJ>
std::ostream& operator << (std::ostream& out, const pop::Mat2x<Type, SIZEI, SIZEJ>& m)
{
    out<<'#'<<m.sizeI()<<" "<<m.sizeJ()<<std::endl;
    out.precision(NumericLimits<Type>::digits10);

    for(unsigned int i=0;i<m.sizeI();i++){
        for(unsigned int j=0;j<m.sizeJ();j++){
            out<<m(i,j);
            if(j!=m.sizeJ()-1)out<<"\t";

        }
        if(i!=m.sizeI()-1)out<<std::endl;

    }
    return out;
}
template<typename Type, int SIZEI, int SIZEJ>
std::istream& operator >> (std::istream& in, pop::Mat2x<Type, SIZEI, SIZEJ>& m)
{
    std::string str="";
    std::string sum_string;
    char c = in.get();
    while(c=='#'){
        if(str!="")
            sum_string+=str+'\n';
        getline ( in, str );
        c = in.get();
    }
    std::istringstream iss(str);
    int sizex;
    iss >> sizex;
    int sizey;
    iss >> sizey;
    in.unget();
    for(unsigned int i=0;i<m.sizeI();i++)
    {
        for(unsigned int j=0;j<m.sizeJ();j++)
        {
            in>>m(i,j);
        }
    }
    return in;
}
template<typename Type, int SIZEI, int SIZEJ>
pop::Mat2x<Type, SIZEI, SIZEJ>  maximum(const pop::Mat2x<Type, SIZEI, SIZEJ>& f,const pop::Mat2x<Type, SIZEI, SIZEJ> & g)throw(pop::pexception)
{
    pop::Mat2x<Type, SIZEI, SIZEJ> h;
    for( int i=0;i<SIZEI*SIZEJ;i++)
        h._dat[i]=maximum(f._dat[i],g._dat[i]);

    return h;
}
template<typename Type, int SIZEI, int SIZEJ>
pop::Mat2x<Type, SIZEI, SIZEJ>  minimum(const pop::Mat2x<Type, SIZEI, SIZEJ>& f,const pop::Mat2x<Type, SIZEI, SIZEJ> & g)throw(pop::pexception)
{
    pop::Mat2x<Type, SIZEI, SIZEJ> h;
    for( int i=0;i<SIZEI*SIZEJ;i++)
        h._dat[i]=minimum(f._dat[i],g._dat[i]);
    return h;
}
}

namespace pop
{
// it is the specialization of the class template<typename Type, int SIZEI int SIZEJ>
template<typename Type>
class POP_EXPORTS Mat2x<Type,2,2>
{
public:
    Type _dat[4];
    typedef Vec2I32 E;
    typedef Vec2I32 Domain;
    typedef Type F;
    typedef typename MatN<2,Type>::IteratorEDomain  IteratorEDomain;
    typedef typename MatN<2,Type>::IteratorENeighborhood IteratorENeighborhood;
    typedef MatN<1, F>  Hyperplane;
    Mat2x(Type value=Type());
    Mat2x(const Mat2x &m);
    Mat2x(const MatN<2,Type> &m);
//    Mat2x(const Type* v_value );
    unsigned int sizeI()const;
    unsigned int sizeJ()const;
    const Type & operator ()( const E &  i_j)const;
    Type & operator ()( const E &  i_j);
    const Type & operator ()( unsigned int  i,unsigned int j)const;
    Type & operator ()(unsigned int  i,unsigned int j);
    bool isValid(int  i,int j) const;
    Mat2x & operator =(const Mat2x &m);
    template<typename Type1>
    Mat2x & operator =(const MatN<2,Type1> &m);
    Mat2x & operator*=(const Mat2x &m);
    void deleteRow(unsigned int i);
    void deleteCol(unsigned int j);
    Vec<Type> getRow(unsigned int i)const;
    Vec<Type> getCol(unsigned int j)const;
    void setRow(unsigned int i,const Vec<Type>& v);
    void setCol(unsigned int j,const Vec<Type>& v);
    void swapRow(unsigned int i_0,unsigned int i_1);
    void swapCol(unsigned int j_0,unsigned int j_1);
    Type minorDet(unsigned int i,unsigned int j)const;
    Type cofactor(unsigned int i,unsigned int j)const;
    Mat2x cofactor()const;
    Mat2x transpose()const;
    Type determinant()const;
    Type trace()const ;
    Mat2x inverse()const;
    Vec<Type>  operator*(const Vec<Type>& v)const ;
    VecN<2,Type>  operator*(const VecN<2,Type>& v)const ;
    Mat2x  operator*(const  Mat2x& m)const ;
    Mat2x  mult(const  Mat2x& m)const ;
    void load(const char * file)throw(pexception);
    void save(const char * file)const throw(pexception);
    Domain getDomain()const;
    void display() const;


    Mat2x  operator+(const Mat2x& f)const{
        Mat2x h(*this);
        h +=f;
        return h;
    }
    Mat2x  operator-(const Mat2x& f)const{
        Mat2x h(*this);
        h -=f;
        return h;
    }
    Mat2x  operator-()const{
        Mat2x h(F(0));
        h -=*this;
        return h;
    }
    Mat2x&  operator+=(const Mat2x& f)
    {
        this->_dat[0]+=f._dat[0];
        this->_dat[1]+=f._dat[1];
        this->_dat[2]+=f._dat[2];
        this->_dat[3]+=f._dat[3];
        return *this;
    }
    Mat2x&  operator-=(const Mat2x& f)
    {
        this->_dat[0]-=f._dat[0];
        this->_dat[1]-=f._dat[1];
        this->_dat[2]-=f._dat[2];
        this->_dat[3]-=f._dat[3];
        return *this;
    }
    template<typename G>
    Mat2x&  operator=(const Mat2x<G,2,2> & M)
    {
        this->_dat[0]=M._dat[0];
        this->_dat[1]=M._dat[1];
        this->_dat[2]=M._dat[2];
        this->_dat[3]=M._dat[3];
        return *this;
    }
    template<typename G>
    Mat2x&  operator=(G value)
    {
        this->_dat[0]=value;
        this->_dat[1]=value;
        this->_dat[2]=value;
        this->_dat[3]=value;
        return *this;
    }


    Mat2x&  operator*=(Type value)
    {
        this->_dat[0]*=value;
        this->_dat[1]*=value;
        this->_dat[2]*=value;
        this->_dat[3]*=value;
        return *this;
    }
    Mat2x operator*(Type value)const{
        Mat2x temp(*this);
        temp._dat[0]*=value;
        temp._dat[1]*=value;
        temp._dat[2]*=value;
        temp._dat[3]*=value;
        return temp;
    }

#ifdef WITHSWIG
    Type getValue(int i, int j)const{
        return  this->_dat[j+ (i<<1)];
    }
    void setValue(int i, int j , Type value){
        this->_dat[j+ (i<<1)]=value;
    }
#endif

};


template<typename Type>
Mat2x<Type,2,2>::Mat2x(Type value)
{
    this->_dat[0]=value;
    this->_dat[1]=value;
    this->_dat[2]=value;
    this->_dat[3]=value;
}
//template<typename Type>
//Mat2x<Type,2,2>::Mat2x(const Type* v_value ){
//    this->_dat[0]=v_value[0];
//    this->_dat[1]=v_value[1];
//    this->_dat[2]=v_value[2];
//    this->_dat[3]=v_value[3];
//}

template<typename Type>
Mat2x<Type,2,2>::Mat2x(const Mat2x &m)
{
    this->_dat[0]=m._dat[0];
    this->_dat[1]=m._dat[1];
    this->_dat[2]=m._dat[2];
    this->_dat[3]=m._dat[3];

}
template<typename Type>
Mat2x<Type,2,2>::Mat2x(const MatN<2, Type> &m)
{
    POP_DbgAssert(m.sizeI()==2&&m.sizeJ()==2);
    this->_dat[0]=m.operator [](0);
    this->_dat[1]=m.operator [](1);
    this->_dat[2]=m.operator [](2);
    this->_dat[3]=m.operator [](3);

}

template<typename Type>
unsigned int Mat2x<Type,2,2>::sizeI()const {
    return 2;
}
template<typename Type>
unsigned int Mat2x<Type,2,2>::sizeJ()const{
    return 2;
}

template<typename Type>
bool Mat2x<Type,2,2>::isValid(int  i,int j) const{
    if(i>=0&&i<2&&j>=0&&j<2)
        return true;
    else
        return false;
}
template<typename Type>
const Type & Mat2x<Type,2,2>::operator ()(  const E &i_j)const
{
    POP_DbgAssert( i_j(0) >= 0&& i_j(1)>=0 && i_j(0)<sizeI()&&i_j(1)<sizeJ());
    return  this->_dat[i_j(1)+ (i_j(0)<<1)];
}
template<typename Type>
Type & Mat2x<Type,2,2>::operator ()( const E &i_j)
{
    POP_DbgAssert( i_j(0) >= 0&& i_j(1)>=0 && i_j(0)<sizeI()&&i_j(1)<sizeJ());
    return  this->_dat[i_j(1)+ (i_j(0)<<1)];
}
template<typename Type>
const Type & Mat2x<Type,2,2>::operator ()(unsigned int  i,unsigned int j)
const
{
    POP_DbgAssert(  i<sizeI()&&j<sizeJ());
    return  this->_dat[j+ (i<<1)];
}
template<typename Type>
Type & Mat2x<Type,2,2>::operator ()(unsigned int  i,unsigned int j)
{
    POP_DbgAssert(  i<sizeI()&&j<sizeJ());
    return  this->_dat[j+ (i<<1)];
}
template<typename Type>
void Mat2x<Type,2,2>::load(const char *file)throw(pexception)
{
    std::ifstream  in(file);
    if (in.fail())
    {
        throw(pexception("In Matrix::load, cannot open file: "+std::string(file)));
    }
    else
    {
        in>>*this;
    }
}
template<typename Type>
void Mat2x<Type,2,2>::save(const char * file)const throw(pexception){
    std::ofstream  out(file);
    if (out.fail())
    {
        throw(pexception("In Matrix::save, cannot open file: "+std::string(file)));
    }
    else
    {
        out<<*this;
    }
}
template<typename Type>
void Mat2x<Type,2,2>::deleteRow(unsigned int ){
    POP_DbgAssert(false);
}
template<typename Type>
void Mat2x<Type,2,2>::deleteCol(unsigned int ){
    POP_DbgAssert(false);

}

template<typename Type>
Vec<Type> Mat2x<Type,2,2>::getRow(unsigned int i)const{
    Vec<Type> v(2);
    if(i==0){
        v(0)=this->_dat[0];
        v(1)=this->_dat[1];
    }else{
        v(0)=this->_dat[2];
        v(1)=this->_dat[3];
    }
    return v;
}
template<typename Type>
Vec<Type> Mat2x<Type,2,2>::getCol(unsigned int j)const{
    Vec<Type> v(2);
    if(j==0){
        v(0)=this->_dat[0];
        v(1)=this->_dat[2];
    }else{
        v(0)=this->_dat[1];
        v(1)=this->_dat[3];
    }
    return v;
}
template<typename Type>
void Mat2x<Type,2,2>::setRow(unsigned int i,const Vec<Type> &v){
    POP_DbgAssert(v.size()==2);
    if(i==0){
        this->_dat[0] = v(0);
        this->_dat[1] = v(1);
    }else{
        this->_dat[2] = v(0);
        this->_dat[3] = v(1);
    }
}
template<typename Type>
void Mat2x<Type,2,2>::setCol(unsigned int j,const Vec<Type>& v){
    POP_DbgAssert(v.size()==2);
    if(j==0){
        this->_dat[0] = v(0);
        this->_dat[2] = v(1);
    }else{
        this->_dat[1] = v(0);
        this->_dat[3] = v(1);
    }
}
template<typename Type>
void Mat2x<Type,2,2>::swapRow(unsigned int i_0,unsigned int i_1){
    POP_DbgAssert(i_0<0||i_0>=2||i_1<0||i_1>=2);
    if(i_0!=i_1)
    {
        std::swap(this->_dat[0],this->_dat[2]);
        std::swap(this->_dat[1],this->_dat[3]);
    }
}
template<typename Type>
void Mat2x<Type,2,2>::swapCol(unsigned int j_0,unsigned int j_1){
    POP_DbgAssert(j_0<0||j_0>=2||j_1<0||j_1>=2);
    if(j_0!=j_1)
    {
        std::swap(this->_dat[0],this->_dat[1]);
        std::swap(this->_dat[2],this->_dat[3]);

    }
}
template<typename Type>
Type Mat2x<Type,2,2>::minorDet(unsigned int i,unsigned int j)const{
    POP_DbgAssert( i >= 0&& j>=0 && i<sizeI()&&j<sizeJ());
    return  this->_dat[j+ (i<<1)];;
}
template<typename Type>
Type Mat2x<Type,2,2>::cofactor(unsigned int i, unsigned int j)const{
    if( i==0&&j==0)
        return this->_dat[3];
    if( i==1&&j==0)
        return -this->_dat[1];
    if( i==0&&j==1)
        return -this->_dat[2];
    else
        return this->_dat[0];
}
template<typename Type>
Mat2x<Type,2,2> Mat2x<Type,2,2>::cofactor()const{
    Mat2x<Type,2,2> temp;
    temp._dat[0]= _dat[3];
    temp._dat[1]=-_dat[2];
    temp._dat[2]=-_dat[1];
    temp._dat[3]= _dat[0];
    return temp;

}
template<typename Type>
Mat2x<Type,2,2> Mat2x<Type,2,2>::transpose()const
{
    Mat2x<Type,2,2> temp;
    temp._dat[0]= _dat[0];
    temp._dat[1]=-_dat[2];
    temp._dat[2]=-_dat[1];
    temp._dat[3]= _dat[3];
    return temp;
}
template<typename Type>
Type Mat2x<Type,2,2>::determinant() const{
    return this->_dat[0] * this->_dat[3] - this->_dat[1] * this->_dat[2] ;

}
template<typename Type>
Type Mat2x<Type,2,2>::trace() const
{
    return this->_dat[0] + this->_dat[3] ;
}
template<typename Type>
Mat2x<Type,2,2> Mat2x<Type,2,2>::inverse()const{
    Mat2x<Type,2,2> temp;
    const Type det= Type(1)/ (this->_dat[0] * this->_dat[3] - this->_dat[1] * this->_dat[2]) ;
    temp._dat[1]=-this->_dat[1]*det;
    temp._dat[2]=-this->_dat[2]*det;
    temp._dat[0]=this->_dat[3]*det;
    temp._dat[3]=this->_dat[0]*det;
    return temp;
}
template<typename Type>
Mat2x<Type,2,2> &Mat2x<Type,2,2>::operator =(const Mat2x& m){
    this->_dat[0]=m._dat[0];
    this->_dat[1]=m._dat[1];
    this->_dat[2]=m._dat[2];
    this->_dat[3]=m._dat[3];
    return *this;
}

template<typename Type>
template<typename Type1>
Mat2x<Type,2,2> &Mat2x<Type,2,2>::operator =(const MatN<2,Type1>& img){
    this->_dat[0]=img.operator [](0);
    this->_dat[1]=img.operator [](1);
    this->_dat[2]=img.operator [](2);
    this->_dat[3]=img.operator [](3);
    return *this;
}

template<typename Type>
Mat2x<Type,2,2> & Mat2x<Type,2,2>::operator*=(const Mat2x<Type,2,2> &m){
    Mat2x<Type,2,2> temp(*this);
    this->_dat[0]=temp._dat[0]*m._dat[0]+temp._dat[1]*m._dat[2];
    this->_dat[1]=temp._dat[0]*m._dat[1]+temp._dat[1]*m._dat[3];
    this->_dat[2]=temp._dat[2]*m._dat[0]+temp._dat[3]*m._dat[2];
    this->_dat[3]=temp._dat[2]*m._dat[1]+temp._dat[3]*m._dat[3];
    return *this;
}
template<typename Type>
Mat2x<Type,2,2>  Mat2x<Type,2,2>::mult(const Mat2x<Type,2,2> &m)const{
    return this->operator *(m);
}

template<typename Type>
Vec<Type>  Mat2x<Type,2,2>::operator*(const Vec<Type> & v)const{
    Vec<Type> temp(2);
    temp(0)=  _dat[0]*v(0)+_dat[1]*v(1);
    temp(1)=  _dat[2]*v(0)+_dat[3]*v(1);
    return temp;
}
template<typename Type>
VecN<2,Type>  Mat2x<Type,2,2>::operator*(const VecN<2,Type> & x)const{
    return VecN<2,Type>(x(0)*_dat[0]+x(1)*_dat[1],x(0)*_dat[2]+x(1)*_dat[3]);
}

template<typename Type>
Mat2x<Type,2,2>  Mat2x<Type,2,2>::operator*(const Mat2x<Type,2,2> &m2)const {
    Mat2x<Type,2,2> m(*this);
    m *=m2;
    return m;
}
template<typename Type>
void Mat2x<Type,2,2>::display() const{
    std::cout<<*this;
    std::cout<<std::endl;
}

template<typename Type>
typename Mat2x<Type,2,2>::Domain Mat2x<Type,2,2>::getDomain()const{
    return Vec2I32(2,2);
}

template<int Dim, typename Result>
MatN<Dim, Result>::MatN(const Mat2x<Result,2,2> m)
    :std::vector<Result>(4)
{
    _domain(0)=2;
    _domain(1)=2;
    this->operator[](0)=m._dat[0];
    this->operator[](1)=m._dat[1];
    this->operator[](2)=m._dat[2];
    this->operator[](3)=m._dat[3];
}

template<typename Type>
void FunctionAssert(const Mat2x<Type,2,2> & , const Mat2x<Type,2,2> &  ,std::string )
{
}

typedef Mat2x<F64,2,2> Mat2x22F64;
typedef Mat2x<ComplexF64,2,2> Mat2x22ComplexF64;



template<typename Type>
struct NumericLimits<Mat2x<Type,2,2> >
{
    static F64 min() throw()
    { return -NumericLimits<Type>::maximumRange();}
    static F64 max() throw()
    { return NumericLimits<Type>::maximumRange();}
};
template<typename Type>
std::ostream& operator << (std::ostream& out, const pop::Mat2x<Type,2,2>& m)
{
    //    std::stringstream ss(m.getInformation());
    //    std::string item;
    //    while(std::getline(ss, item, '\n')) {
    //        out<<"#"<<item<<std::endl;
    //    }
    out<<'#'<<m.sizeI()<<" "<<m.sizeJ()<<std::endl;
    out.precision(NumericLimits<Type>::digits10);

    for(unsigned int i=0;i<m.sizeI();i++){
        for(unsigned int j=0;j<m.sizeJ();j++){
            out<<m(i,j);
            if(j!=m.sizeJ()-1)out<<"\t";

        }
        if(i!=m.sizeI()-1)out<<std::endl;

    }
    return out;
}
template<typename Type>
std::istream& operator >> (std::istream& in, pop::Mat2x<Type,2,2>& m)
{
    std::string str="";
    std::string sum_string;
    char c = in.get();
    while(c=='#'){
        if(str!="")
            sum_string+=str+'\n';
        getline ( in, str );
        c = in.get();
    }
    std::istringstream iss(str);
    int sizex;
    iss >> sizex;
    int sizey;
    iss >> sizey;
    in.unget();
    for(unsigned int i=0;i<m.sizeI();i++)
    {
        for(unsigned int j=0;j<m.sizeJ();j++)
        {
            in>>m(i,j);
        }
    }
    return in;
}
template<typename Type>
pop::Mat2x<Type,2,2>  maximum(const pop::Mat2x<Type,2,2>& f,const pop::Mat2x<Type,2,2> & g)throw(pop::pexception){
    pop::Mat2x<Type,2,2> h;
    h._dat[0]=maximum(f._dat[0],g._dat[0]);
    h._dat[1]=maximum(f._dat[1],g._dat[1]);
    h._dat[2]=maximum(f._dat[2],g._dat[2]);
    h._dat[3]=maximum(f._dat[3],g._dat[3]);
    return h;
}
template<typename Type>
pop::Mat2x<Type,2,2>  minimum(const pop::Mat2x<Type,2,2>& f,const pop::Mat2x<Type,2,2> & g)throw(pop::pexception){
    pop::Mat2x<Type,2,2> h;
    h._dat[0]=minimum(f._dat[0],g._dat[0]);
    h._dat[1]=minimum(f._dat[1],g._dat[1]);
    h._dat[2]=minimum(f._dat[2],g._dat[2]);
    h._dat[3]=minimum(f._dat[3],g._dat[3]);
    return h;
}
}

namespace pop
{
template<typename Type>
class Mat2x<Type,3,3>
{
public:
    Type _dat[9];
    typedef Vec2I32 E;
    typedef Vec2I32 Domain;
    typedef Type F;
    typedef typename MatN<2,Type>::IteratorEDomain  IteratorEDomain;
    typedef typename MatN<2,Type>::IteratorENeighborhood IteratorENeighborhood;
    typedef MatN<1, F>  Hyperplane;
    Mat2x(F value=F());
    Mat2x(const Mat2x &m);
    Mat2x(const MatN<2,Type> &m);
//    Mat2x(const Type* v_value );
    unsigned int sizeI()const;
    unsigned int sizeJ()const;
    const Type & operator ()( const E &  i_j)const;
    Type & operator ()( const E &  i_j);
    const Type & operator ()(unsigned int  i,unsigned int j)const;
    Type & operator ()(unsigned int  i,unsigned int j);
    bool isValid(int  i,int j) const;
    Mat2x & operator =(const Mat2x &m);
    template<typename Type1>
    Mat2x & operator =(const MatN<2,Type1> &m);
    Mat2x & operator*=(const Mat2x &m);
    Mat2x<Type,2,3> deleteRow(unsigned int i)const;
    Mat2x<Type,3,2> deleteCol(unsigned int j)const;
    Vec<Type> getRow(unsigned int i)const;
    Vec<Type> getCol(unsigned int j)const;
    void setRow(unsigned int i,const Vec<Type>& v);
    void setCol(unsigned int j,const Vec<Type>& v);
    void swapRow(unsigned int i_0,unsigned int i_1);
    void swapCol(unsigned int j_0,unsigned int j_1);
    Type minorDet(unsigned int i,unsigned int j)const;
    Type cofactor(unsigned int i,unsigned  int j)const;
    Mat2x cofactor()const;
    Mat2x transpose()const;
    Type determinant()const;
    Type trace()const ;
    Mat2x inverse()const;
//    Vec<Type>  operator*(const Vec<Type>& v)const ;
    VecN<3,Type>  operator*(const VecN<3,Type>& v)const ;
    Mat2x  operator*(const  Mat2x& m)const ;
    Mat2x  mult(const  Mat2x& m)const ;
    void load(const char * file)throw(pexception);
    void save(const char * file)const throw(pexception);
    Domain getDomain()const;
    void display() const;


    Mat2x  operator+(const Mat2x& f)const{
        Mat2x h(*this);
        h +=f;
        return h;
    }
    Mat2x  operator-(const Mat2x& f)const{
        Mat2x h(*this);
        h -=f;
        return h;
    }
    Mat2x  operator-()const{
        Mat2x h(F(0));
        h -=*this;
        return h;
    }
    Mat2x&  operator+=(const Mat2x& f)
    {
        this->_dat[0]+=f._dat[0];
        this->_dat[1]+=f._dat[1];
        this->_dat[2]+=f._dat[2];
        this->_dat[3]+=f._dat[3];
        this->_dat[4]+=f._dat[4];
        this->_dat[5]+=f._dat[5];
        this->_dat[6]+=f._dat[6];
        this->_dat[7]+=f._dat[7];
        this->_dat[8]+=f._dat[8];
        return *this;
    }
    Mat2x&  operator-=(const Mat2x& f)
    {
        this->_dat[0]-=f._dat[0];
        this->_dat[1]-=f._dat[1];
        this->_dat[2]-=f._dat[2];
        this->_dat[3]-=f._dat[3];
        this->_dat[4]-=f._dat[4];
        this->_dat[5]-=f._dat[5];
        this->_dat[6]-=f._dat[6];
        this->_dat[7]-=f._dat[7];
        this->_dat[8]-=f._dat[8];
        return *this;
    }
    template<typename G>
    Mat2x&  operator=(const Mat2x<G,3,3> & f)
    {
        this->_dat[0]=f._dat[0];
        this->_dat[1]=f._dat[1];
        this->_dat[2]=f._dat[2];
        this->_dat[3]=f._dat[3];
        this->_dat[4]=f._dat[4];
        this->_dat[5]=f._dat[5];
        this->_dat[6]=f._dat[6];
        this->_dat[7]=f._dat[7];
        this->_dat[8]=f._dat[8];
        return *this;
    }
    template<typename G>
    Mat2x&  operator=(G value)
    {
        this->_dat[0]=value;
        this->_dat[1]=value;
        this->_dat[2]=value;
        this->_dat[3]=value;
        this->_dat[4]=value;
        this->_dat[5]=value;
        this->_dat[6]=value;
        this->_dat[7]=value;
        this->_dat[8]=value;
        return *this;
    }


    Mat2x&  operator*=(Type value)
    {
        this->_dat[0]*=value;
        this->_dat[1]*=value;
        this->_dat[2]*=value;
        this->_dat[3]*=value;
        this->_dat[4]*=value;
        this->_dat[5]*=value;
        this->_dat[6]*=value;
        this->_dat[7]*=value;
        this->_dat[8]*=value;
        return *this;
    }
    Mat2x&  operator/=(Type value)
    {
        this->_dat[0]/=value;
        this->_dat[1]/=value;
        this->_dat[2]/=value;
        this->_dat[3]/=value;
        this->_dat[4]/=value;
        this->_dat[5]/=value;
        this->_dat[6]/=value;
        this->_dat[7]/=value;
        this->_dat[8]/=value;
        return *this;
    }
    Mat2x operator*(Type value)const{
        Mat2x temp(*this);
        temp._dat[0]*=value;
        temp._dat[1]*=value;
        temp._dat[2]*=value;
        temp._dat[3]*=value;
        temp._dat[4]*=value;
        temp._dat[5]*=value;
        temp._dat[6]*=value;
        temp._dat[7]*=value;
        temp._dat[8]*=value;
        return temp;
    }
    Mat2x operator/(Type value)const{
        Mat2x temp(*this);
        temp._dat[0]/=value;
        temp._dat[1]/=value;
        temp._dat[2]/=value;
        temp._dat[3]/=value;
        temp._dat[4]/=value;
        temp._dat[5]/=value;
        temp._dat[6]/=value;
        temp._dat[7]/=value;
        temp._dat[8]/=value;
        return temp;
    }

#ifdef WITHSWIG
    Type getValue(int i, int j)const{
        return this->_dat[j+ (i*3)];
    }
    void setValue(int i, int j , Type value){
        this->_dat[j+ (i*3)] =value;
    }
#endif
};

template<typename Type>
Mat2x<Type,3,3>::Mat2x(Type value)
{
    this->_dat[0]=value;
    this->_dat[1]=value;
    this->_dat[2]=value;
    this->_dat[3]=value;
    this->_dat[4]=value;
    this->_dat[5]=value;
    this->_dat[6]=value;
    this->_dat[7]=value;
    this->_dat[8]=value;
}

template<typename Type>
Mat2x<Type,3,3>::Mat2x(const Mat2x &m)
{
    this->_dat[0]=m._dat[0];
    this->_dat[1]=m._dat[1];
    this->_dat[2]=m._dat[2];
    this->_dat[3]=m._dat[3];
    this->_dat[4]=m._dat[4];
    this->_dat[5]=m._dat[5];
    this->_dat[6]=m._dat[6];
    this->_dat[7]=m._dat[7];
    this->_dat[8]=m._dat[8];
}
template<typename Type>
Mat2x<Type,3,3>::Mat2x(const MatN<2,Type> &m)
{
    POP_DbgAssert(m.sizeI()==3&&m.sizeJ()==3);
    this->_dat[0]=m.operator [](0);
    this->_dat[1]=m.operator [](1);
    this->_dat[2]=m.operator [](2);
    this->_dat[3]=m.operator [](3);
    this->_dat[4]=m.operator [](4);
    this->_dat[5]=m.operator [](5);
    this->_dat[6]=m.operator [](6);
    this->_dat[7]=m.operator [](7);
    this->_dat[8]=m.operator [](8);

}
//template<typename Type>
//Mat2x<Type,3,3>::Mat2x(const Type* v_value ){
//    this->_dat[0]=v_value[0];
//    this->_dat[1]=v_value[1];
//    this->_dat[2]=v_value[2];
//    this->_dat[3]=v_value[3];
//    this->_dat[4]=v_value[4];
//    this->_dat[5]=v_value[5];
//    this->_dat[6]=v_value[6];
//    this->_dat[7]=v_value[7];
//    this->_dat[8]=v_value[8];
//}
template<typename Type>
unsigned int Mat2x<Type,3,3>::sizeI()const {
    return 3;
}
template<typename Type>
unsigned int Mat2x<Type,3,3>::sizeJ()const{
    return 3;
}

template<typename Type>
bool Mat2x<Type,3,3>::isValid(int  i,int j) const{
    if(i>=0&&i<3&&j>=0&&j<3)
        return true;
    else
        return false;
}
template<typename Type>
const Type & Mat2x<Type,3,3>::operator ()(  const E &i_j)const
{
    POP_DbgAssert( i_j(0) >= 0&& i_j(1)>=0 && i_j(0)<sizeI()&&i_j(1)<sizeJ());
    return  this->_dat[i_j(1)+ (i_j(0)*3)];
}
template<typename Type>
Type & Mat2x<Type,3,3>::operator ()( const E &i_j)
{
    POP_DbgAssert( i_j(0) >= 0&& i_j(1)>=0 && i_j(0)<sizeI()&&i_j(1)<sizeJ());
    if(i_j(0)==0)
        return  this->_dat[i_j(1)];
    else if(i_j(0)==1)
        return  this->_dat[i_j(1)+3];
    else
        return  this->_dat[i_j(1)+6];
}
template<typename Type>
const Type & Mat2x<Type,3,3>::operator ()(unsigned int  i,unsigned int j)
const
{
    POP_DbgAssert(  i<sizeI()&&j<sizeJ());
    return  this->_dat[j+ (i*3)];
}
template<typename Type>
Type & Mat2x<Type,3,3>::operator ()(unsigned int  i,unsigned int j)
{
    POP_DbgAssert(  i<sizeI()&&j<sizeJ());
    return  this->_dat[j+ (i*3)];
}
template<typename Type>
void Mat2x<Type,3,3>::load(const char *file)throw(pexception)
{
    std::ifstream  in(file);
    if (in.fail())
    {
        throw(pexception("In Matrix::load, cannot open file: "+std::string(file)));
    }
    else
    {
        in>>*this;
    }
}
template<typename Type>
void Mat2x<Type,3,3>::save(const char * file)const throw(pexception){
    std::ofstream  out(file);
    if (out.fail())
    {
        throw(pexception("In Matrix::save, cannot open file: "+std::string(file)));
    }
    else
    {
        out<<*this;
    }
}
template<typename Type>
Mat2x<Type,2,3> Mat2x<Type,3,3>::deleteRow(unsigned int i)const{
    Mat2x<Type, 2, 3> temp;
    if(i==0){
        temp._dat[0]=this->_dat[3]; temp._dat[1]=this->_dat[4]; temp._dat[2]=this->_dat[5]; temp._dat[3]=this->_dat[6]; temp._dat[4]=this->_dat[7]; temp._dat[5]=this->_dat[8];
    }else if(i==1){
        temp._dat[0]=this->_dat[0]; temp._dat[1]=this->_dat[1]; temp._dat[2]=this->_dat[2]; temp._dat[3]=this->_dat[6]; temp._dat[4]=this->_dat[7]; temp._dat[5]=this->_dat[8];
    }else{
        temp._dat[0]=this->_dat[0]; temp._dat[1]=this->_dat[1]; temp._dat[2]=this->_dat[2]; temp._dat[3]=this->_dat[3]; temp._dat[4]=this->_dat[4]; temp._dat[5]=this->_dat[5];
    }
    return temp;
}
template<typename Type>
Mat2x<Type,3,2> Mat2x<Type,3,3>::deleteCol(unsigned int  j)const{
    Mat2x<Type, 3, 2> temp;
    if(j==0){
        temp._dat[0]=this->_dat[1];temp._dat[1]=this->_dat[2];temp._dat[2]=this->_dat[4];temp._dat[3]=this->_dat[5];temp._dat[4]=this->_dat[7];temp._dat[5]=this->_dat[8];
    }else if(j==1){
        temp._dat[0]=this->_dat[0];temp._dat[1]=this->_dat[2];temp._dat[2]=this->_dat[3];temp._dat[3]=this->_dat[5];temp._dat[4]=this->_dat[6];temp._dat[5]=this->_dat[8];
    }else{
        temp._dat[0]=this->_dat[0];temp._dat[1]=this->_dat[1];temp._dat[2]=this->_dat[3];temp._dat[3]=this->_dat[4];temp._dat[4]=this->_dat[6];temp._dat[5]=this->_dat[7];
    }
    return temp;

}

template<typename Type>
Vec<Type> Mat2x<Type,3,3>::getRow(unsigned int i)const{
    Vec<Type> v(3);
    if(i==0){
        v(0)=this->_dat[0];
        v(1)=this->_dat[1];
        v(2)=this->_dat[2];
    }else if(i==1){
        v(0)=this->_dat[3];
        v(1)=this->_dat[4];
        v(2)=this->_dat[5];
    }else{
        v(0)=this->_dat[6];
        v(1)=this->_dat[7];
        v(2)=this->_dat[8];
    }
    return v;
}
template<typename Type>
Vec<Type> Mat2x<Type,3,3>::getCol(unsigned int j)const{
    Vec<Type> v(3);
    if(j==0){
        v(0)=this->_dat[0];
        v(1)=this->_dat[3];
        v(2)=this->_dat[6];
    }else if(j==1){
        v(0)=this->_dat[1];
        v(1)=this->_dat[4];
        v(2)=this->_dat[7];
    }
    else{
        v(0)=this->_dat[2];
        v(1)=this->_dat[5];
        v(2)=this->_dat[8];
    }
    return v;
}
template<typename Type>
void Mat2x<Type,3,3>::setRow(unsigned int i,const Vec<Type> &v){
    POP_DbgAssert(v.size()==3);
    if(i==0){
        this->_dat[0] = v(0);
        this->_dat[1] = v(1);
        this->_dat[2] = v(2);
    }else if(i==1){
        this->_dat[3] = v(0);
        this->_dat[4] = v(1);
        this->_dat[5] = v(2);
    }else{
        this->_dat[6] = v(0);
        this->_dat[7] = v(1);
        this->_dat[8] = v(2);
    }
}
template<typename Type>
void Mat2x<Type,3,3>::setCol(unsigned int j,const Vec<Type>& v){
    POP_DbgAssert(v.size()==3);
    if(j==0){
        this->_dat[0] = v(0);
        this->_dat[3] = v(1);
        this->_dat[6] = v(2);
    }else if(j==1){
        this->_dat[1] = v(0);
        this->_dat[4] = v(1);
        this->_dat[7] = v(2);
    }else{
        this->_dat[2] = v(0);
        this->_dat[5] = v(1);
        this->_dat[8] = v(2);
    }
}
template<typename Type>
void Mat2x<Type,3,3>::swapRow(unsigned int i_0,unsigned int i_1){
    POP_DbgAssert(i_0<0||i_0>=3||i_1<0||i_1>=3);
    if(i_0!=i_1)
    {
        int k0=i_0*3;
        int k1=i_1*3;
        std::swap(this->_dat[k0],this->_dat[k1]);
        std::swap(this->_dat[k0+1],this->_dat[k1+1]);
        std::swap(this->_dat[k0+2],this->_dat[k1+2]);

    }
}
template<typename Type>
void Mat2x<Type,3,3>::swapCol(unsigned int j_0,unsigned int j_1){
    POP_DbgAssert(j_0<0||j_0>=3||j_1<0||j_1>=3);
    if(j_0!=j_1)
    {
        std::swap(this->_dat[j_0],this->_dat[j_1]);
        std::swap(this->_dat[j_0+3],this->_dat[j_1+3]);
        std::swap(this->_dat[j_0+6],this->_dat[j_1+6]);
    }
}
template<typename Type>
Type Mat2x<Type,3,3>::minorDet(unsigned int i,unsigned int j)const{
    POP_DbgAssert(  i<sizeI()&&j<sizeJ());

    if(i==0){
        if(j==0)
            return this->_dat[4]*this->_dat[8]-this->_dat[7]*this->_dat[5];
        else if(j==1)
            return this->_dat[3]*this->_dat[8]-this->_dat[6]*this->_dat[5];
        else
            return this->_dat[3]*this->_dat[7]-this->_dat[6]*this->_dat[4];

    }else if(i==1){
        if(j==0)
            return this->_dat[1]*this->_dat[8]-this->_dat[7]*this->_dat[2];
        else if(j==1)
            return this->_dat[0]*this->_dat[8]-this->_dat[6]*this->_dat[2];
        else
            return this->_dat[0]*this->_dat[7]-this->_dat[6]*this->_dat[1];
    }
    else{
        if(j==0)
            return this->_dat[1]*this->_dat[5]-this->_dat[4]*this->_dat[2];
        else if(j==1)
            return this->_dat[0]*this->_dat[5]-this->_dat[3]*this->_dat[2];
        else
            return this->_dat[0]*this->_dat[4]-this->_dat[3]*this->_dat[1];
    }
    return 0;
}
template<typename Type>
Type Mat2x<Type,3,3>::cofactor(unsigned int i,unsigned int j)const{
    if( (i+j)%2==0)
        return this->minorDet(i,j);
    else
        return -this->minorDet(i,j);
}
template<typename Type>
Mat2x<Type,3,3> Mat2x<Type,3,3>::cofactor()const{
    Mat2x<Type,3,3> temp;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
        {
            temp(i,j)=this->cofactor(i,j);
        }
    return temp;

}
template<typename Type>
Mat2x<Type,3,3> Mat2x<Type,3,3>::transpose()const
{
    Mat2x<Type,3,3> temp;
    temp._dat[0]=this->_dat[0];
    temp._dat[1]=this->_dat[3];
    temp._dat[2]=this->_dat[6];
    temp._dat[3]=this->_dat[1];
    temp._dat[4]=this->_dat[4];
    temp._dat[5]=this->_dat[7];
    temp._dat[6]=this->_dat[2];
    temp._dat[7]=this->_dat[5];
    temp._dat[8]=this->_dat[8];

    return temp;
}
template<typename Type>
Type Mat2x<Type,3,3>::determinant() const{
    return this->_dat[0] * (this->_dat[4]*this->_dat[8] - this->_dat[7] * this->_dat[5])-this->_dat[1] * (this->_dat[3]*this->_dat[8] - this->_dat[6] * this->_dat[5]) +this->_dat[2] * (this->_dat[3]*this->_dat[7] - this->_dat[4] * this->_dat[6]);

}
template<typename Type>
Type Mat2x<Type,3,3>::trace() const
{
    return this->_dat[0] + this->_dat[4] + this->_dat[8];
}
template<typename Type>
Mat2x<Type,3,3> Mat2x<Type,3,3>::inverse()const{
    const Type det= Type(1)/(this->_dat[0] * (this->_dat[4]*this->_dat[8] - this->_dat[7] * this->_dat[5])-this->_dat[1] * (this->_dat[3]*this->_dat[8] - this->_dat[6] * this->_dat[5]) +this->_dat[2] * (this->_dat[3]*this->_dat[7] - this->_dat[4] * this->_dat[6]));
    const Type t0=  this->_dat[4]*this->_dat[8]-this->_dat[7]*this->_dat[5];
    const Type t1=-(this->_dat[3]*this->_dat[8]-this->_dat[6]*this->_dat[5]);
    const Type t2=  this->_dat[3]*this->_dat[7]-this->_dat[6]*this->_dat[4];

    const Type t3=-(this->_dat[1]*this->_dat[8]-this->_dat[7]*this->_dat[2]);
    const Type t4= this->_dat[0]*this->_dat[8]-this->_dat[6]*this->_dat[2];
    const Type t5=-(this->_dat[0]*this->_dat[7]-this->_dat[6]*this->_dat[1]);
    const Type t6= this->_dat[1]*this->_dat[5]-this->_dat[4]*this->_dat[2];
    const Type t7=-(this->_dat[0]*this->_dat[5]-this->_dat[3]*this->_dat[2]);
    const Type t8= this->_dat[0]*this->_dat[4]-this->_dat[3]*this->_dat[1];
    Mat2x<Type,3,3> temp;
    temp._dat[0]=t0;
    temp._dat[1]=t1;
    temp._dat[2]=t2;
    temp._dat[3]=t3;
    temp._dat[4]=t4;
    temp._dat[5]=t5;
    temp._dat[6]=t6;
    temp._dat[7]=t7;
    temp._dat[8]=t8;
    std::swap(temp._dat[1],temp._dat[3]); std::swap(temp._dat[2],temp._dat[6]);std::swap(temp._dat[5],temp._dat[7]);
    temp._dat[0]*=det;
    temp._dat[1]*=det;
    temp._dat[2]*=det;
    temp._dat[3]*=det;
    temp._dat[4]*=det;
    temp._dat[5]*=det;
    temp._dat[6]*=det;
    temp._dat[7]*=det;
    temp._dat[8]*=det;
    return temp;
}
template<typename Type>
Mat2x<Type,3,3> &Mat2x<Type,3,3>::operator =(const Mat2x& m)
{

    this->_dat[0]=m._dat[0];
    this->_dat[1]=m._dat[1];
    this->_dat[2]=m._dat[2];
    this->_dat[3]=m._dat[3];
    this->_dat[4]=m._dat[4];
    this->_dat[5]=m._dat[5];
    this->_dat[6]=m._dat[6];
    this->_dat[7]=m._dat[7];
    this->_dat[8]=m._dat[8];
    return *this;
}
template<int Dim, typename Result>
MatN<Dim, Result>::MatN(const Mat2x<Result,3,3> m)
:std::vector<Result>(9)
{
    _domain(0)=3;
    _domain(1)=3;
    this->operator[](0)=m._dat[0];
    this->operator[](1)=m._dat[1];
    this->operator[](2)=m._dat[2];
    this->operator[](3)=m._dat[3];
    this->operator[](4)=m._dat[4];
    this->operator[](5)=m._dat[5];
    this->operator[](6)=m._dat[6];
    this->operator[](7)=m._dat[7];
    this->operator[](8)=m._dat[8];
}
template<typename Type>
template<typename Type1>
Mat2x<Type,3,3> &Mat2x<Type,3,3>::operator =(const MatN<2,Type1>& m)
{
    POP_DbgAssert( m.sizeI()==3&&m.sizeJ()==3);
    this->_dat[0]=m.operator [](0);
    this->_dat[1]=m.operator [](1);
    this->_dat[2]=m.operator [](2);
    this->_dat[3]=m.operator [](3);
    this->_dat[4]=m.operator [](4);
    this->_dat[5]=m.operator [](5);
    this->_dat[6]=m.operator [](6);
    this->_dat[7]=m.operator [](7);
    this->_dat[8]=m.operator [](8);
    return *this;
}

template<typename Type>
Mat2x<Type,3,3> & Mat2x<Type,3,3>::operator*=(const Mat2x<Type,3,3> &m)
{

    Mat2x<Type,3,3> temp(*this);
    this->_dat[0]=temp._dat[0]*m._dat[0]+temp._dat[1]*m._dat[3]+temp._dat[2]*m._dat[6];
    this->_dat[1]=temp._dat[0]*m._dat[1]+temp._dat[1]*m._dat[4]+temp._dat[2]*m._dat[7];
    this->_dat[2]=temp._dat[0]*m._dat[2]+temp._dat[1]*m._dat[5]+temp._dat[2]*m._dat[8];


    this->_dat[3]=temp._dat[3]*m._dat[0]+temp._dat[4]*m._dat[3]+temp._dat[5]*m._dat[6];
    this->_dat[4]=temp._dat[3]*m._dat[1]+temp._dat[4]*m._dat[4]+temp._dat[5]*m._dat[7];
    this->_dat[5]=temp._dat[3]*m._dat[2]+temp._dat[4]*m._dat[5]+temp._dat[5]*m._dat[8];

    this->_dat[6]=temp._dat[6]*m._dat[0]+temp._dat[7]*m._dat[3]+temp._dat[8]*m._dat[6];
    this->_dat[7]=temp._dat[6]*m._dat[1]+temp._dat[7]*m._dat[4]+temp._dat[8]*m._dat[7];
    this->_dat[8]=temp._dat[6]*m._dat[2]+temp._dat[7]*m._dat[5]+temp._dat[8]*m._dat[8];
    return *this;
}
template<typename Type>
Mat2x<Type,3,3>  Mat2x<Type,3,3>::mult(const Mat2x<Type,3,3> &m)const
{
    return this->operator *(m);
}

//template<typename Type>
//Vec<Type>  Mat2x<Type,3,3>::operator*(const Vec<Type> & v)const
//{
//    Vec<Type> temp(3);
//    temp(0)=  _dat[0]*v(0)+_dat[1]*v(1)+_dat[2]*v(2);
//    temp(1)=  _dat[3]*v(0)+_dat[4]*v(1)+_dat[5]*v(2);
//    temp(2)=  _dat[6]*v(0)+_dat[7]*v(1)+_dat[8]*v(2);
//    return temp;
//}
template<typename Type>
VecN<3,Type>  Mat2x<Type,3,3>::operator*(const VecN<3,Type> & v)const{
    return VecN<3,Type>(_dat[0]*v(0)+_dat[1]*v(1)+_dat[2]*v(2),_dat[3]*v(0)+_dat[4]*v(1)+_dat[5]*v(2),_dat[6]*v(0)+_dat[7]*v(1)+_dat[8]*v(2));
}

template<typename Type>
Mat2x<Type,3,3>  Mat2x<Type,3,3>::operator*(const Mat2x<Type,3,3> &m2)const {
    Mat2x<Type,3,3> m(*this);
    m *=m2;
    return m;

}
template<typename Type>
void Mat2x<Type,3,3>::display() const{
    std::cout<<*this;
    std::cout<<std::endl;
}

template<typename Type>
typename Mat2x<Type,3,3>::Domain Mat2x<Type,3,3>::getDomain()const
{
    return Vec2I32(2,2);
}
template<typename Type>
void FunctionAssert(const Mat2x<Type,3,3> & , const Mat2x<Type,3,3> &  ,std::string )
{
}

typedef Mat2x<F64,3,3> Mat2x33F64;
typedef Mat2x<ComplexF64,3,3> Mat2x33ComplexF64;




template<typename Type>
struct NumericLimits<Mat2x<Type,3,3> >
{
    static F64 min() throw()
    { return -NumericLimits<Type>::maximumRange();}
    static F64 max() throw()
    { return NumericLimits<Type>::maximumRange();}
};
template<typename Type>
std::ostream& operator << (std::ostream& out, const pop::Mat2x<Type,3,3>& m)
{
    //    std::stringstream ss(m.getInformation());
    //    std::string item;
    //    while(std::getline(ss, item, '\n')) {
    //        out<<"#"<<item<<std::endl;
    //    }
    out<<'#'<<m.sizeI()<<" "<<m.sizeJ()<<std::endl;
    out.precision(NumericLimits<Type>::digits10);

    for(unsigned int i=0;i<m.sizeI();i++){
        for(unsigned int j=0;j<m.sizeJ();j++){
            out<<m(i,j);
            if(j!=m.sizeJ()-1)out<<"\t";

        }
        if(i!=m.sizeI()-1)out<<std::endl;

    }
    return out;
}
template<typename Type>
std::istream& operator >> (std::istream& in, pop::Mat2x<Type,3,3>& m)
{
    std::string str="";
    std::string sum_string;
    char c = in.get();
    while(c=='#'){
        if(str!="")
            sum_string+=str+'\n';
        getline ( in, str );
        c = in.get();
    }
    std::istringstream iss(str);
    int sizex;
    iss >> sizex;
    int sizey;
    iss >> sizey;
    in.unget();
    for(unsigned int i=0;i<m.sizeI();i++)
    {
        for(unsigned int j=0;j<m.sizeJ();j++)
        {
            in>>m(i,j);
        }
    }
    return in;
}
template<typename Type>
pop::Mat2x<Type,3,3>  maximum(const pop::Mat2x<Type,3,3>& f,const pop::Mat2x<Type,3,3> & g)throw(pop::pexception)
{
    pop::Mat2x<Type,3,3> h;
    h._dat[0]=maximum(f._dat[0],g._dat[0]);
    h._dat[1]=maximum(f._dat[1],g._dat[1]);
    h._dat[2]=maximum(f._dat[2],g._dat[2]);
    h._dat[3]=maximum(f._dat[3],g._dat[3]);
    h._dat[4]=maximum(f._dat[4],g._dat[4]);
    h._dat[5]=maximum(f._dat[5],g._dat[5]);
    h._dat[6]=maximum(f._dat[6],g._dat[6]);
    h._dat[7]=maximum(f._dat[7],g._dat[7]);
    h._dat[8]=maximum(f._dat[8],g._dat[8]);
    return h;
}
template<typename Type>
pop::Mat2x<Type,3,3>  minimum(const pop::Mat2x<Type,3,3>& f,const pop::Mat2x<Type,3,3> & g)throw(pop::pexception)
{
    pop::Mat2x<Type,3,3> h;
    h._dat[0]=minimum(f._dat[0],g._dat[0]);
    h._dat[1]=minimum(f._dat[1],g._dat[1]);
    h._dat[2]=minimum(f._dat[2],g._dat[2]);
    h._dat[3]=minimum(f._dat[3],g._dat[3]);
    h._dat[4]=minimum(f._dat[4],g._dat[4]);
    h._dat[5]=minimum(f._dat[5],g._dat[5]);
    h._dat[6]=minimum(f._dat[6],g._dat[6]);
    h._dat[7]=minimum(f._dat[7],g._dat[7]);
    h._dat[8]=minimum(f._dat[8],g._dat[8]);
    return h;
}
}
/// @endcond
#endif // MATRIX2_2_H

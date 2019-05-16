#ifndef CONVERTOR_H
#define CONVERTOR_H

#include"core/data/functor/FunctorF.h"
#include"core/data/vec/VecN.h"
#include"core/data/GP/TypeManip.h"
namespace pop
{
/*!
\defgroup Convertor Convertor
\ingroup Algorithm
\brief Matrix In -> Matrix Out (toRGB, fromRGB, toYUV, fromRealImaginary)

Conversion: vectoriel image (vector field, color image,complex image) <-> list of scalar images
\code
Mat2RGBUI8 img;//2d grey-level image object
img.load("../Lena.bmp");//replace this path by those on your computer
Mat2UI8 r,g,b;
Convertor::toRGB(img,r,g,b);
r.save("../doc/image2/lenared.jpg");
g.save("../doc/image2/lenagreen.jpg");
b.save("../doc/image2/lenablue.jpg");
\endcode
\image html lenared.jpg "red channel"
\image html lenagreen.jpg "green channel"
\image html lenablue.jpg "blue channel"
*/
struct POP_EXPORTS Convertor
{

    /*!
        \class pop::Convertor
        \ingroup Convertor
        \brief convertor facilities
        \author Tariel Vincent
    */


    /*!
    \param fRGB rgb matrix
    \param r red channel output
    \param g green channel output
    \param b green channel output
    *
    * Extract the RGB channels of the rgb matrix
    */
    template<typename FunctionRGB, typename FunctionScalar>
    static void toRGB(const FunctionRGB & fRGB,  FunctionScalar &r,FunctionScalar &g,FunctionScalar &b)
    {
        r.resize(fRGB.getDomain());
        g.resize(fRGB.getDomain());
        b.resize(fRGB.getDomain());
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionRGB::F> fred(0);
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionRGB::F> fgreen(1);
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionRGB::F> fblue(2);
        std::transform(fRGB.begin(),fRGB.end(),r.begin(),fred);
        std::transform(fRGB.begin(),fRGB.end(),g.begin(),fgreen);
        std::transform(fRGB.begin(),fRGB.end(),b.begin(),fblue);
    }

    /*!
    \param r red channel input
    \param g green channel input
    \param b green channel input
    \param fRGB
    *
    * set the RGB channels of the rgb matrix
    */
    template<typename FunctionRGB, typename FunctionScalar>
    static void fromRGB(const FunctionScalar &r,const FunctionScalar &g,const FunctionScalar &b, FunctionRGB& fRGB)
    {

        POP_DbgAssertMessage(r.getDomain()==g.getDomain()&&g.getDomain()==b.getDomain(),"In MatN::fromRGB, R G B matrixs must have the same size");
        fRGB.resize(r.getDomain());
        FunctorF::FunctorFromMultiCoordinatesToVector<typename FunctionScalar::F,typename FunctionRGB::F> f;
        std::transform3(r.begin(),r.end(),g.begin(),b.begin(),fRGB.begin(),f);
    }

    /*!
    \param fRGB rgb input image
    \param y Y channel output
    \param u U channel output
    \param v V channel output
    *
    * Extract the YUV channels of the rgb matrix http://en.wikipedia.org/wiki/YUV
    */
    template<typename FunctionRGB, typename Function>
    static void toYUV(const FunctionRGB & fRGB, Function &y,Function &u,Function &v)
    {
        y.resize(fRGB.getDomain());
        u.resize(fRGB.getDomain());
        v.resize(fRGB.getDomain());
        typename std::vector<typename Function::F>::iterator  ity = y.begin();
        typename std::vector<typename Function::F>::iterator itu = u.begin();
        typename std::vector<typename Function::F>::iterator itv = v.begin();
        typename FunctionRGB::const_iterator it = fRGB.begin();
        typename FunctionRGB::const_iterator itend = fRGB.end();
        while (it != itend) {
            it->toYUV( *ity, *itu, *itv);
            ity++; itu++; itv++;it++;
        }
    }
    /*!
    \param y Y channel input
    \param u U channel input
    \param v V channel input
    \param fRGB rgb output image
    *
    * set the YUV channels of the rgb matrix http://en.wikipedia.org/wiki/YUV
    */
    template<typename FunctionRGB, typename FunctionScalar>
    static void fromYUV(const FunctionScalar &y,const FunctionScalar &u,const FunctionScalar &v, FunctionRGB& fRGB)
    {

        POP_DbgAssertMessage(y.getDomain()==u.getDomain()&&y.getDomain()==v.getDomain(),"In MatN::fromRGB, Y U B matrixs must have the same size");
        fRGB.resize(y.getDomain());
        typename FunctionScalar::const_iterator ity = y.begin();
        typename FunctionScalar::const_iterator ityend = y.end();
        typename FunctionScalar::const_iterator itu = u.begin();
        typename FunctionScalar::const_iterator itv = v.begin();
        typename FunctionRGB::iterator it = fRGB.begin();
        while (ity != ityend) {
            it->fromYUV(*ity,*itu,*itv);
            ity++; itu++; itv++;it++;
        }
    }
    /*!

    \param real real input
    \param img imaginary input
    \param f complex image output
    *
    * h(x) =real(x)+i*img(x)
    */
    template<typename FunctionComplex, typename FunctionScalar>
    static void fromRealImaginary(const FunctionScalar &real,const FunctionScalar &img,FunctionComplex & f)
    {
        POP_DbgAssertMessage(real.getDomain()==img.getDomain(),"In MatN::fromRealImaginary, real and img matrixs must have the same size");
        f.resize(real.getDomain());
        FunctorF::FunctorFromMultiCoordinatesToVector<typename FunctionScalar::F,typename FunctionComplex::F> func;
        std::transform(real.begin(),real.end(),img.begin(),f.begin(),func);
    }
    /*!
    \param real real input
    \param f complex image output
    *
    * h(x) =real(x) with 0 for imaginary part
        In this example, we apply a low pass filter:
        \code
        Mat2UI8 img;//2d grey-level image object
        img.load("/home/vincent/Dropbox/MyArticle/PhaseField/lena.pgm");//replace this path by those on your computer
        Mat2ComplexF64 imgcomplex;
        Convertor::fromRealImaginary(img,imgcomplex);
        Mat2ComplexF64 fft = Representation::FFT(imgcomplex);
        Mat2ComplexF64 filterlowpass(fft.getDomain());
        Vec2I32 x(0,0);
        Draw::disk(filterlowpass,x,10,NumericLimits<ComplexF64>::maximumRange(),1);
        fft = Processing::mask(fft,filterlowpass);
        imgcomplex = Representation::FFT(fft,-1);
        Mat2F64 imgd;
        Convertor::toRealImaginary(imgcomplex,imgd);
        img = Processing::greylevelRange(imgd,0,255);
        img.display();
        \endcode

    */
    template<typename FunctionComplex, typename FunctionScalar>
    static void fromRealImaginary(const FunctionScalar &real,FunctionComplex & f)
    {
        f.resize(real.getDomain());
        FunctorF::FunctorFromMultiCoordinatesToVector<typename FunctionScalar::F,typename FunctionComplex::F> func;
        std::transform(real.begin(),real.end(),f.begin(),func);
    }
    /*!
    \param fcomplex complex image input
    \param real real output
    \param imaginary imaginary output
    *
    * real(x)=Real(f(x)) and img(x)=Imaginary(f(x))
    */
    template<typename FunctionComplex, typename FunctionScalar>
    static void toRealImaginary(const FunctionComplex & fcomplex, FunctionScalar &real, FunctionScalar &imaginary)
    {
        real.resize(fcomplex.getDomain());
        imaginary.resize(fcomplex.getDomain());
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionComplex::F> freal(0);
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionComplex::F> fimg(1);
        std::transform(fcomplex.begin(),fcomplex.end(),real.begin(),freal);
        std::transform(fcomplex.begin(),fcomplex.end(),imaginary.begin(),fimg);
    }
    /*!
    \param fcomplex complex image input
    \param real real output
    *
    *  real(x)=Real(f(x))
    */
    template<typename FunctionComplex, typename FunctionScalar>
    static void toRealImaginary(const FunctionComplex & fcomplex, FunctionScalar &real)
    {
        real.resize(fcomplex.getDomain());
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionComplex::F> freal(0);
        std::transform(fcomplex.begin(),fcomplex.end(),real.begin(),freal);
    }
    /*!
    \param vecf vector of field
    \param f field of vector
    *
    *  vecf(0)(x)=f(x)(0)
    */

    template<typename FunctionVec, typename FunctionScalar>
    static void fromVecN( const VecN<FunctionVec::F::DIM,FunctionScalar >& vecf,FunctionVec& f){
        fromVecN(vecf,f,Loki::Int2Type<FunctionVec::F::DIM>());
    }
    /*!
     * \param vecf vector of field
     * \param f field of vector
     *
     *  vecf(0)(x)=f(x)(0)
     *
     * \code
     * Mat2UI8 img;//2d grey-level image object
     * img.load("../Lena.bmp");//replace this path by those on your computer
     * //gradient vector field
     * MatN<2,Vec2F64> gradient_vector_field = Processing::gradientVecGaussian(img,1);
     * //get each gradient in each direction in a
     * VecN<2,Mat2F64> fields_gradient;
     * pop::Convertor::toVecN(gradient_vector_field,fields_gradient);//gradient_vector_field(x)(coordinate)=fields_gradient(coordinate)(x)
     * fields_gradient(0).display("gradient",false);//gradient in  vertical direction
     * fields_gradient(1).display();//gradient in  horizontal direction
     * \endcode
     * \image html lenagrad0.jpg "gradient in vertical direction"
     * \image html lenagrad1.jpg "gradient in horizontal direction"
     */

    template<typename FunctionVec, typename FunctionScalar>
    static void toVecN( const FunctionVec& f, VecN<FunctionVec::F::DIM,FunctionScalar >& vecf){
        toVecN(f,vecf,Loki::Int2Type<FunctionVec::F::DIM>());
    }





private:
    template<typename FunctionVec, typename FunctionScalar>
    static void fromVecN( const VecN<1,FunctionScalar >& vecf,FunctionVec& f,Loki::Int2Type<1>)
    {
        const FunctionScalar & f0= vecf(0);
        f.resize(f0.getDomain());
        FunctorF::FunctorFromMultiCoordinatesToVector<typename FunctionScalar::F,typename FunctionVec::F> func;
        std::transform(f0.begin(),f0.end(),f.begin(),func);
    }
    template<typename FunctionVec, typename FunctionScalar>
    static void fromVecN( const VecN<2,FunctionScalar >& vecf,FunctionVec& f,Loki::Int2Type<2>)
    {
        const FunctionScalar & f0= vecf(0);
        const FunctionScalar & f1= vecf(1);
        f.resize(f0.getDomain());
        FunctorF::FunctorFromMultiCoordinatesToVector<typename FunctionScalar::F,typename FunctionVec::F> func;
        std::transform(f0.begin(),f0.end(),f1.begin(),f.begin(),func);
    }
    template<typename FunctionVec, typename FunctionScalar>
    static void fromVecN( const VecN<3,FunctionScalar >& vecf,FunctionVec& f,Loki::Int2Type<3>)
    {
        const FunctionScalar & f0= vecf(0);
        const FunctionScalar & f1= vecf(1);
        const FunctionScalar & f2= vecf(2);
        f.resize(f0.getDomain());
        FunctorF::FunctorFromMultiCoordinatesToVector<typename FunctionScalar::F,typename FunctionVec::F> func;
        std::transform3(f0.begin(),f0.end(),f1.begin(),f2.begin(),f.begin(),func);
    }
    template<typename FunctionVec, typename FunctionScalar>
    static void fromVecN( const VecN<4,FunctionScalar >& vecf,FunctionVec& f,Loki::Int2Type<4>)
    {
        const FunctionScalar & f0= vecf(0);
        const FunctionScalar & f1= vecf(1);
        const FunctionScalar & f2= vecf(2);
        const FunctionScalar & f3= vecf(3);
        f.resize(f0.getDomain());
        FunctorF::FunctorFromMultiCoordinatesToVector<typename FunctionScalar::F,typename FunctionVec::F> func;
        std::transform4(f0.begin(),f0.end(),f1.begin(),f2.begin(),f3.begin(),f.begin(),func);
    }
    template<typename FunctionVec, typename FunctionScalar>
    static void toVecN(const FunctionVec& f, VecN<1,FunctionScalar >& vecf,Loki::Int2Type<1>)
    {
        FunctionScalar & f0= vecf(0);
        f0.resize(f.getDomain());
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionVec::F> func0(0);
        std::transform(f.begin(),f.end(),f0.begin(),func0);
    }
    template<typename FunctionVec, typename FunctionScalar>
    static void toVecN( const FunctionVec& f, VecN<2,FunctionScalar >& vecf,Loki::Int2Type<2>)
    {
        FunctionScalar & f0= vecf(0);
        FunctionScalar & f1= vecf(1);
        f0.resize(f.getDomain());
        f1.resize(f.getDomain());
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionVec::F> func0(0);
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionVec::F> func1(1);
        std::transform(f.begin(),f.end(),f0.begin(),func0);
        std::transform(f.begin(),f.end(),f1.begin(),func1);

    }
    template<typename FunctionVec, typename FunctionScalar>
    static void toVecN( const FunctionVec& f, VecN<3,FunctionScalar >& vecf,Loki::Int2Type<3>)
    {
        FunctionScalar & f0= vecf(0);
        FunctionScalar & f1= vecf(1);
        FunctionScalar & f2= vecf(2);

        f0.resize(f.getDomain());
        f1.resize(f.getDomain());
        f2.resize(f.getDomain());
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionVec::F> func0(0);
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionVec::F> func1(1);
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionVec::F> func2(2);
        std::transform(f.begin(),f.end(),f0.begin(),func0);
        std::transform(f.begin(),f.end(),f1.begin(),func1);
        std::transform(f.begin(),f.end(),f2.begin(),func2);
    }
    template<typename FunctionVec, typename FunctionScalar>
    static void toVecN( const FunctionVec& f, VecN<4,FunctionScalar >& vecf,Loki::Int2Type<4>)
    {
        FunctionScalar & f0= vecf(0);
        FunctionScalar & f1= vecf(1);
        FunctionScalar & f2= vecf(2);
        FunctionScalar & f3= vecf(3);
        f0.resize(f.getDomain());
        f1.resize(f.getDomain());
        f2.resize(f.getDomain());
        f3.resize(f.getDomain());
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionVec::F> func0(0);
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionVec::F> func1(1);
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionVec::F> func2(2);
        FunctorF::FunctorFromVectorToScalarCoordinate<typename FunctionScalar::F,typename FunctionVec::F> func3(3);
        std::transform(f.begin(),f.end(),f0.begin(),func0);
        std::transform(f.begin(),f.end(),f1.begin(),func1);
        std::transform(f.begin(),f.end(),f2.begin(),func2);
        std::transform(f.begin(),f.end(),f3.begin(),func3);
    }
};
}
#endif // CONVERTOR_H

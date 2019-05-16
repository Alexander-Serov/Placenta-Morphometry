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
#ifndef FUNCTORPDE_H
#define FUNCTORPDE_H
#include<vector>
#include"core/data/typeF/TypeTraitsF.h"
#include"core/data/functor/FunctorF.h"
#include"core/data/mat/MatN.h"
#include"core/data/mat/Mat2x.h"
namespace pop
{
/// @cond DEV
/*! \ingroup Functor
* \defgroup FunctorPDE FunctorPDE
* \brief Collection of functors representing differential operator in finite difference scheme
*
* For instance the homogeneous diffusion equation in finite difference scheme
* \code
    Mat2UI8 img;
    img.load("../Lena.bmp");
    Mat2F64 img_timet(img);//operation on float
    Mat2F64 img_timet_plus_one(img);
    FunctorPDE::Laplacien<> laplacien;

    double D=0.25;//diffusion coefficient
    MatNDisplay disp;
    for(unsigned int index_time=0;index_time<300;index_time++){
        std::cout<<index_time<<std::endl;
        ForEachDomain(x,img_timet) {
            img_timet_plus_one(x) = img_timet(x) + D*laplacien(img_timet,x);
        }
        img_timet = img_timet_plus_one;//timet<-timet_plus_one
        disp.display(img_timet);//display the current image
    }
* \endcode
*
*
*/
namespace Private{

template<typename Function1,typename FunctorCausal, typename FunctorAntiCausal >
Function1 recursiveAllDirections(const Function1 & f, FunctorCausal & funccausal, FunctorAntiCausal & funcanticausal );
template<typename Function1>
Function1 smoothDeriche(const Function1 & f, F64 alpha);
template< typename Function1, typename IteratorEOrder>
Function1    recursive(const Function1 & f,IteratorEOrder & it, FunctorF::FunctorRecursiveOrder2 & func);



class GaussianKernelOnlyIterate
{
private:
    Vec<F64> v_value_one_dimension;
    double sigma;
    int radius_kernel;
public:
    inline GaussianKernelOnlyIterate(F64 sigma,int radius_kernel=1);
    template<typename Function, typename Iterator>
    Function iterate(const Function& f,Iterator & it);
};



template<int DIM>
class ConvolutionSeparableMirror
{
    template<int DIM1,typename Type2>
    struct mult_sum
    {
        typename MatN<DIM1,Type2>::const_iterator _it_matrix;
        int _incr;
        typedef typename FunctionTypeTraitsSubstituteF<Type2,F64>::Result TypeFloat;
        TypeFloat operator ()(TypeFloat res, F64 x)
        {
            res += FunctorF::FunctorMultiplicationF2<F64,Type2,TypeFloat>::op(x,(*_it_matrix));
            _it_matrix+=_incr;
            return res;

        }
    };
    VecN<DIM,Vec<F64> > _kernel;
public:
    void setSingleKernel(const Vec<F64>& kernel){
        for(unsigned int i=0;i<DIM;i++){
            _kernel[i]=kernel;
        }
    }
    void setDirectionnelKernel(const Vec<F64>& kernel_in_one_direction,Vec<F64>& kernel_in_other_direction,unsigned int coordinate){
        for(unsigned int i=0;i<DIM;i++){
            if(i == coordinate)
                _kernel[i]=kernel_in_one_direction;
            else
                _kernel[i]=kernel_in_other_direction;
        }
    }

    template<typename Function,typename Iterator>
    Function operator ()(Iterator & it, const Function& f,Loki::Type2Type<Iterator> );
    template<typename Type>
    MatN<DIM,Type> operator ()(typename MatN<DIM,Type>::IteratorEDomain & it,const MatN<DIM,Type>& f,Loki::Type2Type<typename MatN<DIM,Type>::IteratorEDomain>  );
    template<typename Type>
    MatN<DIM,Type> operator ()(typename MatN<DIM,Type>::IteratorERectangle & it,const MatN<DIM,Type>& f,Loki::Type2Type<typename MatN<DIM,Type>::IteratorERectangle>  );
    template<typename Type>
    MatN<DIM,Type> operator ()(int coordinate,const MatN<DIM,Type>& f,const VecN<DIM,int> &shif_begin ,const VecN<DIM,int> &shif_end );
};


}




class POP_EXPORTS FunctorPDE
{
public:
    /*!
     * \class pop::FunctorPDE
     * \ingroup FunctorPDE
     * \brief Collection of functors representing differential operator in finite difference scheme
     * \author Tariel Vincent
     *
     * \code
    Mat2UI8 img;
    img.load("../Lena.bmp");
    Mat2F64 img_timet(img);//operation on float
    Mat2F64 img_timet_plus_one(img);
    FunctorPDE::Laplacien<> laplacien;

    double D=0.25;//diffusion coefficient
    MatNDisplay disp;
    for(unsigned int index_time=0;index_time<300;index_time++){
        std::cout<<index_time<<std::endl;
        ForEachDomain(x,img_timet) {
            img_timet_plus_one(x) = img_timet(x) + D*laplacien(img_timet,x);
        }
        img_timet = img_timet_plus_one;//timet<-timet_plus_one
        disp.display(img_timet);//display the current image
    }
     * \endcode
     *
    */

    //-------------------------------------
    //
    //! \name Gaussian kernel
    //@{
    //-------------------------------------

    /*!
    \class pop::FunctorPDE::GaussianKernel
    \brief  Discrete convolution of the input field with a gaussian kernel : \f$ (f * g )(x)=\ \int_{x'\in\mathbf{R}^d} f(x')\, g(x - x')\, dx'\f$ with g a gaussian kernel
    *
    *
    *  Discrete convolution with a gaussian kernel with mirror boundary condition
    * \code
    Mat2RGBUI8 lena;
    lena.load("../Lena.bmp");
    FunctorPDE::GaussianKernel<Mat2RGBUI8 > kernel2(lena,6,30);
    Mat2RGBUI8::IteratorEDomain itlena(lena.getIteratorEDomain());
    lena = kernel2.iterate(lena,itlena);
    lena.display();

    Mat2F64 imgf(100,100);//calculation must be done on float values
    imgf=0;
    imgf(50,50)=100000;
    Mat2F64 img_smooth(imgf.getDomain());
    FunctorPDE::GaussianKernel<Mat2F64 > kernel(imgf,15,50);

    //use kernel as convolution on point
        Mat2UI8::IteratorEDomain it(imgf.getIteratorEDomain());
    while(it.next()){
        img_smooth(it.x()) = kernel(imgf,it.x());
    }
    Scene3d scene;
    Visualization::topography(scene,img_smooth);
    scene.display();
    Mat2RGBUI8 lena;
    lena.load("../Lena.bmp");
    FunctorPDE::GaussianKernel<Mat2RGBUI8 > kernel2(lena,6,30);
    Mat2RGBUI8::IteratorEDomain itlena(lena.getIteratorEDomain());
    //optimized (gaussian kernel is decompasable)
    lena = kernel2.iterate(lena,itlena);
    lena.display();
    * \endcode
    * \image html convolutiongaussian2.png
    * \image html convolutiongaussian.png
    */
    template<typename Function>
    class POP_EXPORTS GaussianKernel
    {
    public:
        typename Function::IteratorENeighborhoodMirror itn;
        Vec<F64> v_value;
        Vec<F64> v_value_one_dimension;
        double sigma;
        int radius_kernel;
    public:
        /*!
        * \param  f  field
        * \param  sigma standard deviation
        * \param radius_kernel kernel radius
        * \param only_iteration use only the method iterate (construct only the decomposable kernel)
        *
        * construct a gaussian kernel
        *
        */
        GaussianKernel(const Function& f,F64 sigma,int radius_kernel=1,bool only_iteration=false)
            :itn(f.getIteratorENeighborhood(radius_kernel,0)),sigma(sigma),radius_kernel(radius_kernel)
        {
            //initialisation multi dimension
            F64 sum=0;
            if(only_iteration==false){
                itn.init(0);
                while(itn.next()){
                    double dist = itn.xreal().normPower();
                    F64  value = std::exp(-0.5*dist/(sigma*sigma));
                    v_value.push_back(value);
                    sum+=value;
                }
                //normalisation
                for(int i=0;i<(int)v_value.size();i++){
                    v_value[i]/=sum;
                }
            }
            //initialisation one-dimension
            sum=0;
            for(int i=0;i<2*radius_kernel+1;i++){
                F64  value =std::exp(-0.5*(radius_kernel-i)*(radius_kernel-i)/(sigma*sigma));
                v_value_one_dimension.push_back(value);
                sum+=value;
            }
            //normalisation
            for(int i=0;i<(int)v_value_one_dimension.size();i++){
                v_value_one_dimension[i]/=sum;
                //                std::cout<<v_value_one_dimension[i]<<std::endl;
            }

        }
        /*!
        * \param  f input field
        * \param  it iteraror
        * \return  output field
        *
        * apply the convolution on each point of the iteration
        *
        */
        template<typename Function1, typename Iterator>
        Function1 iterate(const Function1& f,Iterator & it)
        {
            Private::ConvolutionSeparableMirror<Function::DIM> func;
            func.setSingleKernel(v_value_one_dimension);
            return func.operator ()(it,f,Loki::Type2Type<Iterator>());
        }

        /*!
        * \param  f input field
        * \param  x point
        * \return  output value
        *
        * apply the convolution on this point
        *
        */
        typename Function::F operator ()(const Function& f, const typename Function::E& x)
        {
            typedef typename FunctionTypeTraitsSubstituteF<typename Function::F,F64>::Result TypeFloat;
            typename FunctionTypeTraitsSubstituteF<typename Function::F,F64>::Result sum=0;
            FunctorF::FunctorMultiplicationF2<F64, TypeFloat,typename Function::F> op;
            itn.init( x);
            Vec<F64>::iterator it = v_value.begin();
            while(itn.next()){
                sum += op((*it),f(itn.x()));
                it ++;
            }
            return sum;
        }
    };

    /*!
    \class pop::FunctorPDE::GaussianDerivateKernel
    \brief  Discrete convolution of the input field with a derivate gaussian kernel : \f$ \frac{\partial(f * g )}{\partial x }= f* \frac{\partial( g )}{\partial x }\f$ with g a gaussian kernel
    *
    *
    *  Discrete convolution with a gaussian kernel with mirror boundary condition
    * \code
    Mat2RGBUI8 img;
    img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
    Mat2RGBF64 imgfloat1,imgfloat2(img.getDomain());//calculation must be done on float values
    imgfloat1 = img;
    FunctorPDE::GaussianDerivateKernel<Mat2RGBF64 > kernel(imgfloat1,5,10);
    Mat2UI8::IteratorEDomain it(img.getIteratorEDomain());
    int time1=time(NULL);
    while(it.next()){
        imgfloat2(it.x()) = kernel(imgfloat1,it.x());
    }
    int time2=time(NULL);
    std::cout<<"time "<<time2-time1<<std::endl;
    img = imgfloat2;
    img.display();
    time1=time(NULL);
    imgfloat2 = kernel(imgfloat1);
    time2=time(NULL);
    std::cout<<"time "<<time2-time1<<std::endl;
    img = imgfloat2;
    img.display();
    * \endcode
    * \image html convolutiongaussian.png
    */
    template<typename Function>
    class POP_EXPORTS GaussianDerivateKernel
    {
    private:
        typename Function::IteratorENeighborhoodMirror itn;
        Vec<F64> v_value;
        Vec<F64> v_value_one_dimension;
        Vec<F64> v_value_one_dimension_derivate;
        int derivate_coordinate;
        double sigma;
        int radius_kernel;
    public:
        /*!
        * \param  f input field
        * \param  derivate_coordinate  derivate coordinate
        * \param  sigma standard deviation
        * \param radius_kernel kernel radius
        * \param only_iteration use only the method iterate (construct only the decomposable kernel)
        *
        * construct a gaussian kernel
        *
        */
        template<typename Function1>
        GaussianDerivateKernel(const Function1& f,int derivate_coordinate,F64 sigma=1,int radius_kernel=2,bool only_iteration=false)
            :itn(f.getIteratorENeighborhood(radius_kernel,0)),derivate_coordinate(derivate_coordinate),sigma(sigma),radius_kernel(radius_kernel)
        {
            //initialisation multi dimension with this normalaziation condition absolute(integer(f))=2/sigma
            F64 sum=0;
            if(only_iteration==false){
                itn.init(0);
                while(itn.next()){
                    double dist = itn.xreal().normPower();
                    F64  value =  itn.xreal()(derivate_coordinate)*std::exp(-0.5*dist/(sigma*sigma));
                    v_value.push_back(value);
                    sum+=absolute(value);
                }

                for(int i=0;i<(int)v_value.size();i++){
                    v_value[i]*=(2/(sum*sigma));
                }
            }
            //initialisation one-dimension
            sum=0;
            double sum2=0;
            for(int i=0;i<2*radius_kernel+1;i++){
                F64  value = std::exp(-0.5*(radius_kernel-i)*(radius_kernel-i)/(sigma*sigma));
                v_value_one_dimension.push_back(value);
                v_value_one_dimension_derivate.push_back(-value*(radius_kernel-i));
                sum+=value;
                sum2+=absolute(value*(radius_kernel-i));
            }
            for(int i=0;i<(int)v_value_one_dimension.size();i++){
                v_value_one_dimension_derivate[i]*=(2/(sum2*sigma));
                v_value_one_dimension[i]/=sum;
            }

        }
        /*!
        * \param  f input field
        * \param  x point
        * \return  output value
        *
        * apply the convolution on this point
        *
        */
        template<typename Function1>
        typename Function1::F operator ()(const Function1& f, const typename Function1::E& x)
        {
            typedef typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64>::Result TypeFloat;
            typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64>::Result sum(0);
            FunctorF::FunctorMultiplicationF2< typename Function1::F,F64,TypeFloat> op;
            itn.init( x);
            Vec<F64>::iterator it = v_value.begin();
            while(itn.next()){
                sum += op((*it),f(itn.x()));
                it ++;
            }
            return sum;
        }
        /*!
        * \param  f input field
        * \param  it iteraror
        * \return  output field
        *
        * apply the convolution on each point of the iteration. Optimization trick: gaussian is decomposable
        *
        */
        template<typename Function1, typename Iterator>
        Function1 iterate(const Function1& f,Iterator & it)
        {
            Private::ConvolutionSeparableMirror<Function::DIM> func;
            func.setDirectionnelKernel(v_value_one_dimension_derivate,v_value_one_dimension,derivate_coordinate);
            return func.operator ()(it,f,Loki::Type2Type<Iterator>());
        }
    };
    //@}
    //-------------------------------------
    //
    //! \name partial derivate
    //@{
    //-------------------------------------

    /*!
    \class pop::FunctorPDE::PartialDerivateCentered
    \brief Partial derivative in centered difference finite  \f$\frac{\partial f}{\partial x_i}\f$ and \f$\frac{\partial \overrightarrow{f}}{\partial x_i}\f$
    \tparam div number in centered difference finite \f$(f(i+1,j)-f(i-1,j))/DIV\f$
    *
    * Functor to approximate the partial derivative in centered difference finite. For a VecN falling outside the field domain, the partial derivative is NULL (Neumann boundary condition).
    */
    class POP_EXPORTS PartialDerivateCentered
    {
    public:
        static const double DIV_INVERSE;
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate derivative direction
        * \return a scalar value
        *
        * For a scalar field, the functor returns the partial derivative \f$\frac{\partial f}{\partial x_i}\f$ in centered difference finite that is in 2d  equal \f$(f(i+1,j)-f(i-1,j))/DIV\f$ for coordinate=0 and x=(i,j).\n
        * For a vectoriel field, the functor returns the partial derivative \f$(\frac{\partial f_0}{\partial x_i},\frac{\partial f_1}{\partial x_i}),\ldots\f$ in centered difference finite.
        *
        *
        *
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        {
            x(coordinate)++;
            typename Function::F y;
            if( x(coordinate)<f.getDomain()(coordinate)){
                y= f(x);
                x(coordinate)-=2;
                if(x(coordinate)>=0)
                {
                    y-=f(x);
                    y*=0.5;
                    x(coordinate)++;
                    return y;
                }
                else{
                    x(coordinate)++;
                    return typename Function::F(0);
                }
            }else{
                x(coordinate)--;
                return typename Function::F(0);
            }

        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate);
            }
            return derivate;
        }
    };
    /*!
    \class pop::FunctorPDE::PartialDerivateForward
    \brief Partial derivative in forward difference finite  \f$\frac{\partial f}{\partial x_i}\f$ and \f$\frac{\partial  \overrightarrow{f}}{\partial x_i}\f$
    *
    * Functor to approximate the partial derivative in forward difference finite.
    * For a VecN falling outside the field domain, the partial derivative is NULL (Neumann boundary condition).
    */
    class POP_EXPORTS PartialDerivateForward
    {
    public:
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate derivative direction
        * \return a scalar value
        *
        * For a scalar field, the functor returns the partial derivative \f$\frac{\partial f}{\partial x_i}\f$ in centered difference finite that is in 2d  equal \f$f(i+1,j)-f(i,j)\f$ for coordinate=0 and x=(i,j).\n
        * For a vectoriel field, the functor returns the partial derivative \f$(\frac{\partial f_0}{\partial x_i},\frac{\partial f_1}{\partial x_i}),\ldots\f$.
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        {
            typename Function::F y=typename Function::F(0)-f(x);
            x(coordinate)++;
            if( x(coordinate)<f.getDomain()(coordinate)){
                y+=f(x);
                x(coordinate)--;
                return y;
            }else{
                x(coordinate)--;
                return typename Function::F(0);
            }

        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate);
            }
            return derivate;
        }

    };
    /*!
    \class pop::FunctorPDE::PartialDerivateBackward
    \brief Partial derivative in backward difference finite  \f$\frac{\partial f}{\partial x_i}\f$ and \f$\frac{\partial  \overrightarrow{f}}{\partial x_i}\f$
    *
    * Functor to approximate the partial derivative in backward difference finite
    * For a VecN falling outside the field domain, the partial derivative is NULL (Neumann boundary condition).
    */
    class POP_EXPORTS PartialDerivateBackward
    {
    public:
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate derivative direction
        * \return a scalar value
        *
        * For a scalar field, the functor returns the partial derivative \f$\frac{\partial f}{\partial x_i}\f$ in centered difference finite that is in 2d  equal \f$f(i,j)-f(i-1,j)\f$ for coordinate=0 and x=(i,j).\n
        * For a vectoriel field, the functor returns the partial derivative \f$(\frac{\partial f_0}{\partial x_i},\frac{\partial f_1}{\partial x_i}),\ldots\f$.
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        {
            typename Function::F y=f(x);
            x(coordinate)--;
            if( x(coordinate)>=0){
                y-=f(x);
                x(coordinate)++;
                return y;
            }else{
                x(coordinate)++;
                return typename Function::F(0);
            }

        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate);
            }
            return derivate;
        }
    };
    /*!
    \class pop::FunctorPDE::PartialDerivateSecondCentered
    \brief second partial derivative in centered difference finite  \f$\frac{\partial^2 f}{\partial x_i\partial x_j}\f$ and \f$\frac{\partial^2 \overrightarrow{f}}{\partial x_i\partial x_j}\f$
    *
    * Functor to approximate the second partial derivative with Neumann boundary condition
    *
    */
    class POP_EXPORTS PartialDerivateSecondCentered
    {
    private:
        PartialDerivateForward partiatforward;
        PartialDerivateBackward partiatbackward;
        PartialDerivateCentered partiatcentered;
    public:
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate_i,int coordinate_j )
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate_i first derivative direction
        * \param  coordinate_j second derivative direction
        * \return a scalar value
        *
        * For a scalar field, the functor returns the second partial derivative \f$\frac{\partial^2 f}{\partial x_j \partial x_i }\f$ in centered difference finite that is in 2d  equal
        *   - \f$f(i+1,j)+f(i-1,j)-2*f(i,j)\f$ for coordinate_i=coordinate_j=0
        *   - \f$(f(i+1,j+1)-f(i-1,j+1))-(f(i+1,j-1)-f(i-1,j-1))/4\f$ for coordinate_i=0 and coordinate_j=1.\n
        *
        * For a vectoriel field, the functor returns the vector partial derivative \f$(\frac{\partial f_0}{\partial x_i},\frac{\partial f_1}{\partial x_i}),\ldots\f$.
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate_i,int coordinate_j )
        {
            if(coordinate_i==coordinate_j){
                return partiatforward(f,x,coordinate_i)-partiatbackward(f,x,coordinate_i);
            }
            else{
                typename Function::F y(0);
                x(coordinate_j)++;
                if( x(coordinate_j)<f.getDomain()(coordinate_j)){
                    y=partiatcentered(f,x,coordinate_i);
                }
                x(coordinate_j)-=2;
                if( x(coordinate_j)>=0){
                    y-=partiatcentered(f,x,coordinate_i);
                }
                x(coordinate_j)++;
                return y*0.5;
            }
        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate_i first derivative direction
        * \param  coordinate_j second derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate_i,int coordinate_j){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate_i,coordinate_j);
            }
            return derivate;
        }

    };

    /*!
    \class pop::FunctorPDE::PartialDerivateCenteredInBulk
    \brief Partial derivative in centered difference finite  \f$\frac{\partial f}{\partial x_i}\f$ and \f$\frac{\partial \overrightarrow{f}}{\partial x_i}\f$
    *
    * Functor to approximate the partial derivative in centered difference finite for a scalar/vectoriel field. For a VecN falling outside the bulk \f$\{x:bulk(x)\neq 0\} \f$, the partial derivative is NULL (Neumann boundary condition).
    * \sa PartialDerivateCentered
    */
    template<typename FunctionBulk>
    class POP_EXPORTS PartialDerivateCenteredInBulk
    {
    private:
        const FunctionBulk * _bulk;
    public:
        void setBulk(const FunctionBulk & bulk){
            _bulk = & bulk;
        }
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate derivative direction
        * \return a scalar value
        *
        * The functor returns the partial derivative \f$\frac{\partial f}{\partial x_i}\f$ in centered difference finite that is in 2d  equal \f$(f(i+1,j)-f(i-1,j))/2\f$ for coordinate=0 and x=(i,j).
        *
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        {
            x(coordinate)++;
            typename Function::F y;
            if( x(coordinate)<f.getDomain()(coordinate)&&_bulk->operator()(x)!=0){
                y= f(x);
                x(coordinate)-=2;
                if(x(coordinate)>=0&&_bulk->operator()(x)!=0)
                {
                    y-=f(x);
                    y/=2;
                    x(coordinate)++;
                    return y;
                }
                else{
                    x(coordinate)++;
                    return typename Function::F(0);
                }
            }else{
                x(coordinate)--;
                return typename Function::F(0);
            }

        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate);
            }
            return derivate;
        }
    };
    /*!
    \class pop::FunctorPDE::PartialDerivateForwardInBulk
    \brief Partial derivative in forward difference finite  \f$\frac{\partial f}{\partial x_i}\f$ and \f$\frac{\partial \overrightarrow{f}}{\partial x_i}\f$
    *
    * Functor to approximate the partial derivative in forward difference finite. For a VecN falling outside the bulk \f$\{x:bulk(x)\neq 0\} \f$, the partial derivative is NULL (Neumann boundary condition).
    */
    template<typename FunctionBulk>
    class POP_EXPORTS PartialDerivateForwardInBulk
    {
    private:
        const FunctionBulk * _bulk;
    public:
        void setBulk(const FunctionBulk & bulk){
            _bulk = & bulk;
        }
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate derivative direction
        * \return a scalar value
        *
        * The functor returns the partial derivative \f$\frac{\partial f}{\partial x_i}\f$ in centered difference finite.\n
        * For a VecN falling outside the field domain, the partial derivative is NULL (Neumann boundary condition).
        * \sa PartialDerivateForward
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        {
            typename Function::F y=-f(x);
            x(coordinate)++;
            if( x(coordinate)<f.getDomain()(coordinate) &&_bulk->operator()(x)!=0){
                y+=f(x);
                x(coordinate)--;
                return y;
            }else{
                x(coordinate)--;
                return typename Function::F(0);
            }

        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate);
            }
            return derivate;
        }
    };
    /*!
    \class pop::FunctorPDE::PartialDerivateBackwardInBulk
    \brief Partial derivative in backward difference finite  \f$\frac{\partial f}{\partial x_i}\f$ and \f$\frac{\partial \overrightarrow{f}}{\partial x_i}\f$
    *
    * Functor to approximate the partial derivative in backward difference finite
    * For a VecN falling outside the bulk \f$\{x:bulk(x)\neq 0\} \f$, the partial derivative is NULL (Neumann boundary condition).
    */
    template<typename FunctionBulk>
    class POP_EXPORTS PartialDerivateBackwardInBulk
    {
    private:
        const FunctionBulk * _bulk;
    public:
        void setBulk(const FunctionBulk & bulk){
            _bulk = & bulk;
        }
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate derivative direction
        * \return a scalar value
        *
        * The functor returns the partial derivative \f$\frac{\partial f}{\partial x_i}\f$.
        * \sa PartialDerivateBackward
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        {
            typename Function::F y=f(x);
            x(coordinate)--;
            if( x(coordinate)>=0&&_bulk->operator()(x)!=0){
                y-=f(x);
                x(coordinate)++;
                return y;
            }else{
                x(coordinate)++;
                return typename Function::F(0);
            }

        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate);
            }
            return derivate;
        }
    };
    /*!
    \class pop::FunctorPDE::PartialDerivateSecondCenteredInBulk
    \brief second partial derivative in centered difference finite  \f$\frac{\partial^2 f}{\partial x_i\partial x_j}\f$ and \f$\frac{\partial^2 \overrightarrow{f}}{\partial x_i\partial x_j}\f$
    *
    * Functor to approximate the second partial derivative. For a VecN falling outside the bulk \f$\{x:bulk(x)\neq 0\} \f$, the partial derivative is NULL (Neumann boundary condition).
    *
    *
    */
    template<typename FunctionBulk>
    class POP_EXPORTS PartialDerivateSecondCenteredInBulk
    {
    private:
        const FunctionBulk * _bulk;
    public:
        void setBulk(const FunctionBulk & bulk){
            _bulk = & bulk;
            partiatforward.setBulk(bulk);
            partiatbackward.setBulk(bulk);
            partiatcentered.setBulk(bulk);
        }
        PartialDerivateForwardInBulk<FunctionBulk> partiatforward;
        PartialDerivateBackwardInBulk<FunctionBulk> partiatbackward;
        PartialDerivateCenteredInBulk<FunctionBulk> partiatcentered;
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate_i,int coordinate_j )
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate_i first derivative direction
        * \param  coordinate_j second derivative direction
        * \return a scalar value
        *
        * The functor returns the second partial derivative \f$\frac{\partial^2 f}{\partial x_j \partial x_i }\f$ in centered difference finite that is in 2d  equal
        *   - \f$f(i+1,j)+f(i-1,j)-2*f(i,j)\f$ for coordinate_i=coordinate_j=0
        *   - \f$(f(i+1,j+1)-f(i-1,j+1))-(f(i+1,j-1)-f(i-1,j-1))\f$ for coordinate_i=0 and coordinate_j=1
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate_i,int coordinate_j )
        {
            if(coordinate_i==coordinate_j){
                return partiatforward(f,x,coordinate_i)-partiatbackward(f,x,coordinate_i);
            }
            else{
                typename Function::F y(0);
                x(coordinate_j)++;
                if( x(coordinate_j)<f.getDomain()(coordinate_j)&&_bulk->operator()(x)!=0){
                    y=partiatcentered(f,x,coordinate_i);
                }
                x(coordinate_j)-=2;
                if( x(coordinate_j)>=0&&_bulk->operator()(x)!=0){
                    y-=partiatcentered(f,x,coordinate_i);
                }
                x(coordinate_j)++;
                return y;
            }
        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate);
            }
            return derivate;
        }
    };
    /*!
    \class pop::FunctorPDE::PartialDerivateCenteredMultiPhaseField
    \brief Partial derivative in centered difference finite  \f$\frac{\partial f}{\partial x_i}\f$ and \f$\frac{\partial \overrightarrow{f}}{\partial x_i}\f$
    *
    * Functor to approximate the partial derivative in centered difference finite for a scalar/vectoriel field in multi-phase field formalism.
    * \sa PartialDerivateCentered
    */
    template<typename FunctionLabelPhase>
    class POP_EXPORTS PartialDerivateCenteredMultiPhaseField
    {
    private:
        const FunctionLabelPhase * _label;
    public:
        void setLabelPhase(const FunctionLabelPhase & label){
            _label = & label;
        }
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate derivative direction
        * \return a scalar value
        *
        * The functor returns the partial derivative \f$\frac{\partial f}{\partial x_i}\f$ in centered difference finite that is in 2d  equal \f$(f(i+1,j)-f(i-1,j))/2\f$ for coordinate=0 and x=(i,j).
        *
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        {
            typename FunctionLabelPhase::F label = _label->operator()(x);
            x(coordinate)++;
            typename Function::F y;
            if( x(coordinate)<f.getDomain()(coordinate)){
                if(label==_label->operator()(x))
                    y= f(x);
                else
                    y=-f(x);
                x(coordinate)-=2;
                if(x(coordinate)>=0)
                {
                    if(label==_label->operator()(x))
                        y-=f(x);
                    else
                        y+=f(x);
                    y/=2;
                    x(coordinate)++;
                    return y;
                }
                else{
                    x(coordinate)++;
                    return typename Function::F(0);
                }
            }else{
                x(coordinate)--;
                return typename Function::F(0);
            }

        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate);
            }
            return derivate;
        }
    };
    /*!
    \class pop::FunctorPDE::PartialDerivateForwardMultiPhaseField
    \brief Partial derivative in forward difference finite  \f$\frac{\partial f}{\partial x_i}\f$ and \f$\frac{\partial \overrightarrow{f}}{\partial x_i}\f$
    *
    * Functor to approximate the partial derivative in forward difference finite.
    */
    template<typename FunctionLabelPhase>
    class POP_EXPORTS PartialDerivateForwardMultiPhaseField
    {
    private:
        const FunctionLabelPhase * _label;
    public:

        void setLabelPhase(const FunctionLabelPhase & label){
            _label = & label;
        }
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate derivative direction
        * \return a scalar value
        *
        * The functor returns the partial derivative \f$\frac{\partial f}{\partial x_i}\f$ in centered difference finite.\n
        * For a VecN falling outside the field domain, the partial derivative is NULL (Neumann boundary condition).
        * \sa PartialDerivateForward
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        {
            typename FunctionLabelPhase::F label = _label->operator()(x);
            typename Function::F y=-f(x);
            x(coordinate)++;
            if( x(coordinate)<f.getDomain()(coordinate)){
                if(label==_label->operator()(x))
                    y+=f(x);
                else
                    y-=f(x);
                x(coordinate)--;
                return y;
            }else{
                x(coordinate)--;
                return typename Function::F(0);
            }

        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate);
            }
            return derivate;
        }
    };
    /*!
    \class pop::FunctorPDE::PartialDerivateBackwardMultiPhaseField
    \brief Partial derivative in backward difference finite  \f$\frac{\partial f}{\partial x_i}\f$ and \f$\frac{\partial \overrightarrow{f}}{\partial x_i}\f$
    *
    * Functor to approximate the partial derivative in backward difference finite
    * For a VecN falling outside the bulk \f$\{x:bulk(x)\neq 0\} \f$, the partial derivative is NULL (Neumann boundary condition).
    */
    template<typename FunctionLabelPhase>
    class POP_EXPORTS PartialDerivateBackwardMultiPhaseField
    {
    private:
        const FunctionLabelPhase * _label;
    public:
        void setLabelPhase(const FunctionLabelPhase & label){
            _label = & label;
        }
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate derivative direction
        * \return a scalar value
        *
        * The functor returns the partial derivative \f$\frac{\partial f}{\partial x_i}\f$.
        * \sa PartialDerivateBackward
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        {
            typename FunctionLabelPhase::F label = _label->operator()(x);
            typename Function::F y=f(x);
            x(coordinate)--;
            if( x(coordinate)>=0){
                if(label==_label->operator()(x))
                    y-=f(x);
                else
                    y+=f(x);
                x(coordinate)++;
                return y;
            }else{
                x(coordinate)++;
                return typename Function::F(0);
            }

        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate);
            }
            return derivate;
        }
    };
    /*!
    \class pop::FunctorPDE::PartialDerivateSecondCenteredMultiPhaseField
    \brief second partial derivative in centered difference finite  \f$\frac{\partial^2 f}{\partial x_i\partial x_j}\f$ and \f$\frac{\partial^2 \overrightarrow{f}}{\partial x_i\partial x_j}\f$
    *
    * Functor to approximate the second partial derivative.
    *
    *
    */
    template<typename FunctionLabelPhase>
    class POP_EXPORTS PartialDerivateSecondCenteredMultiPhaseField
    {
    private:

        const FunctionLabelPhase * _label;
    public:
        void setLabelPhase(const FunctionLabelPhase & label){
            _label = & label;
            partiatforward.setLabelPhase(label);
            partiatbackward.setLabelPhase(label);
            partiatcentered.setLabelPhase(label);
        }
        PartialDerivateForwardMultiPhaseField<FunctionLabelPhase> partiatforward;
        PartialDerivateBackwardMultiPhaseField<FunctionLabelPhase> partiatbackward;
        PartialDerivateCenteredMultiPhaseField<FunctionLabelPhase> partiatcentered;
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate_i,int coordinate_j )
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate_i first derivative direction
        * \param  coordinate_j second derivative direction
        * \return a scalar value
        *
        * The functor returns the second partial derivative \f$\frac{\partial^2 f}{\partial x_j \partial x_i }\f$ in centered difference finite that is in 2d  equal
        *   - \f$f(i+1,j)+f(i-1,j)-2*f(i,j)\f$ for coordinate_i=coordinate_j=0
        *   - \f$(f(i+1,j+1)-f(i-1,j+1))-(f(i+1,j-1)-f(i-1,j-1))\f$ for coordinate_i=0 and coordinate_j=1
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate_i,int coordinate_j )
        {
            if(coordinate_i==coordinate_j){
                return partiatforward(f,x,coordinate_i)-partiatbackward(f,x,coordinate_i);
            }
            else{
                std::cout<<"PartialDerivateSecondCenteredMultiPhaseField is not yet implemented for cross derivation"<<std::endl;
                exit(0);
                //                typename Function::F y(0);
                //                x(coordinate_j)++;
                //                if( x(coordinate_j)<f.getDomain()(coordinate_j)&&_bulk->operator()(x)!=0){
                //                    y=partiatcentered(f,x,coordinate_i);
                //                }
                //                x(coordinate_j)-=2;
                //                if( x(coordinate_j)>=0&&_bulk->operator()(x)!=0){
                //                    y-=partiatcentered(f,x,coordinate_i);
                //                }
                //                x(coordinate_j)++;
                return 0;
            }
        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate);
            }
            return derivate;
        }
    };
    /*!
    \class pop::FunctorPDE::PartialDerivateCenteredInBulkMultiPhaseField
    \brief Partial derivative in centered difference finite  \f$\frac{\partial f}{\partial x_i}\f$ and \f$\frac{\partial \overrightarrow{f}}{\partial x_i}\f$
    *
    * Functor to approximate the partial derivative in centered difference finite for a scalar/vectoriel field in multi-phase field formalism. For a VecN falling outside the bulk \f$\{x:bulk(x)\neq 0\} \f$, the partial derivative is NULL (Neumann boundary condition).
    * \sa PartialDerivateCentered
    */
    template<typename FunctionBulk,typename FunctionLabelPhase>
    class POP_EXPORTS PartialDerivateCenteredInBulkMultiPhaseField
    {
    private:
        const FunctionBulk * _bulk;
        const FunctionLabelPhase * _label;
    public:
        void setBulk(const FunctionBulk & bulk){
            _bulk = & bulk;
        }
        void setLabelPhase(const FunctionLabelPhase & label){
            _label = & label;
        }
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate derivative direction
        * \return a scalar value
        *
        * The functor returns the partial derivative \f$\frac{\partial f}{\partial x_i}\f$ in centered difference finite that is in 2d  equal \f$(f(i+1,j)-f(i-1,j))/2\f$ for coordinate=0 and x=(i,j).
        *
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        {
            typename FunctionLabelPhase::F label = _label->operator()(x);
            x(coordinate)++;
            typename Function::F y;
            if( x(coordinate)<f.getDomain()(coordinate)&&_bulk->operator()(x)!=0){
                if(label==_label->operator()(x))
                    y= f(x);
                else
                    y=-f(x);
                x(coordinate)-=2;
                if(x(coordinate)>=0&&_bulk->operator()(x)!=0)
                {
                    if(label==_label->operator()(x))
                        y-=f(x);
                    else
                        y+=f(x);
                    y/=2;
                    x(coordinate)++;
                    return y;
                }
                else{
                    x(coordinate)++;
                    return typename Function::F(0);
                }
            }else{
                x(coordinate)--;
                return typename Function::F(0);
            }

        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate);
            }
            return derivate;
        }
    };
    /*!
    \class pop::FunctorPDE::PartialDerivateForwardInBulkMultiPhaseField
    \brief Partial derivative in forward difference finite  \f$\frac{\partial f}{\partial x_i}\f$ and \f$\frac{\partial \overrightarrow{f}}{\partial x_i}\f$
    *
    * Functor to approximate the partial derivative in forward difference finite. For a VecN falling outside the bulk \f$\{x:bulk(x)\neq 0\} \f$, the partial derivative is NULL (Neumann boundary condition).
    */
    template<typename FunctionBulk,typename FunctionLabelPhase>
    class POP_EXPORTS PartialDerivateForwardInBulkMultiPhaseField
    {
    private:
        const FunctionBulk * _bulk;
        const FunctionLabelPhase * _label;
    public:
        void setBulk(const FunctionBulk & bulk){
            _bulk = & bulk;
        }
        void setLabelPhase(const FunctionLabelPhase & label){
            _label = & label;
        }
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate derivative direction
        * \return a scalar value
        *
        * The functor returns the partial derivative \f$\frac{\partial f}{\partial x_i}\f$ in centered difference finite.\n
        * For a VecN falling outside the field domain, the partial derivative is NULL (Neumann boundary condition).
        * \sa PartialDerivateForward
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        {
            typename FunctionLabelPhase::F label = _label->operator()(x);
            typename Function::F y=-f(x);
            x(coordinate)++;
            if( x(coordinate)<f.getDomain()(coordinate) &&_bulk->operator()(x)!=0){
                if(label==_label->operator()(x))
                    y+=f(x);
                else
                    y-=f(x);
                x(coordinate)--;
                return y;
            }else{
                x(coordinate)--;
                return typename Function::F(0);
            }

        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate);
            }
            return derivate;
        }
    };
    /*!
    \class pop::FunctorPDE::PartialDerivateBackwardInBulkMultiPhaseField
    \brief Partial derivative in backward difference finite  \f$\frac{\partial f}{\partial x_i}\f$ and \f$\frac{\partial \overrightarrow{f}}{\partial x_i}\f$
    *
    * Functor to approximate the partial derivative in backward difference finite
    * For a VecN falling outside the bulk \f$\{x:bulk(x)\neq 0\} \f$, the partial derivative is NULL (Neumann boundary condition).
    */
    template<typename FunctionBulk,typename FunctionLabelPhase>
    class POP_EXPORTS PartialDerivateBackwardInBulkMultiPhaseField
    {
    private:
        const FunctionBulk * _bulk;
        const FunctionLabelPhase * _label;
    public:
        void setBulk(const FunctionBulk & bulk){
            _bulk = & bulk;
        }
        void setLabelPhase(const FunctionLabelPhase & label){
            _label = & label;
        }
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate derivative direction
        * \return a scalar value
        *
        * The functor returns the partial derivative \f$\frac{\partial f}{\partial x_i}\f$.
        * \sa PartialDerivateBackward
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate)
        {
            typename FunctionLabelPhase::F label = _label->operator()(x);
            typename Function::F y=f(x);
            x(coordinate)--;
            if( x(coordinate)>=0&&_bulk->operator()(x)!=0){
                if(label==_label->operator()(x))
                    y-=f(x);
                else
                    y+=f(x);
                x(coordinate)++;
                return y;
            }else{
                x(coordinate)++;
                return typename Function::F(0);
            }

        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate);
            }
            return derivate;
        }
    };
    /*!
    \class pop::FunctorPDE::PartialDerivateSecondCenteredInBulkMultiPhaseField
    \brief second partial derivative in centered difference finite  \f$\frac{\partial^2 f}{\partial x_i\partial x_j}\f$ and \f$\frac{\partial^2 \overrightarrow{f}}{\partial x_i\partial x_j}\f$
    *
    * Functor to approximate the second partial derivative. For a VecN falling outside the bulk \f$\{x:bulk(x)\neq 0\} \f$, the partial derivative is NULL (Neumann boundary condition).
    *
    *
    */
    template<typename FunctionBulk,typename FunctionLabelPhase>
    class POP_EXPORTS PartialDerivateSecondCenteredInBulkMultiPhaseField
    {
    private:
        const FunctionBulk * _bulk;
        const FunctionLabelPhase * _label;
    public:
        void setLabelPhase(const FunctionLabelPhase & label){
            _label = & label;
            partiatforward.setLabelPhase(label);
            partiatbackward.setLabelPhase(label);
            partiatcentered.setLabelPhase(label);
        }
        void setBulk(const FunctionBulk & bulk){
            _bulk = & bulk;
            partiatforward.setBulk(bulk);
            partiatbackward.setBulk(bulk);
            partiatcentered.setBulk(bulk);
        }
        PartialDerivateForwardInBulkMultiPhaseField<FunctionBulk,FunctionLabelPhase> partiatforward;
        PartialDerivateBackwardInBulkMultiPhaseField<FunctionBulk,FunctionLabelPhase> partiatbackward;
        PartialDerivateCenteredInBulkMultiPhaseField<FunctionBulk,FunctionLabelPhase> partiatcentered;
        /*!
        \fn  inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate_i,int coordinate_j )
        * \brief partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \param  coordinate_i first derivative direction
        * \param  coordinate_j second derivative direction
        * \return a scalar value
        *
        * The functor returns the second partial derivative \f$\frac{\partial^2 f}{\partial x_j \partial x_i }\f$ in centered difference finite that is in 2d  equal
        *   - \f$f(i+1,j)+f(i-1,j)-2*f(i,j)\f$ for coordinate_i=coordinate_j=0
        *   - \f$(f(i+1,j+1)-f(i-1,j+1))-(f(i+1,j-1)-f(i-1,j-1))\f$ for coordinate_i=0 and coordinate_j=1
        */
        template<typename Function>
        inline typename Function::F operator()(const Function &f, typename Function::E & x,int coordinate_i,int coordinate_j )
        {
            if(coordinate_i==coordinate_j){
                return partiatforward(f,x,coordinate_i)-partiatbackward(f,x,coordinate_i);
            }
            else{
                std::cout<<"PartialDerivateSecondCenteredInBulkMultiPhaseField is not yet implemented for cross derivation"<<std::endl;
                exit(0);
                //                typename Function::F y(0);
                //                x(coordinate_j)++;
                //                if( x(coordinate_j)<f.getDomain()(coordinate_j)&&_bulk->operator()(x)!=0){
                //                    y=partiatcentered(f,x,coordinate_i);
                //                }
                //                x(coordinate_j)-=2;
                //                if( x(coordinate_j)>=0&&_bulk->operator()(x)!=0){
                //                    y-=partiatcentered(f,x,coordinate_i);
                //                }
                //                x(coordinate_j)++;
                return 0;
            }
        }
        /*!
        \fn  Function iterate(const Function &f,Iterator &it,int coordinate)
        * \brief iterate the partial derivate for scalar/vectoriel field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \param  coordinate derivative direction
        * \return output derived scalar/vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it,int coordinate){

            Function derivate(f.getDomain());
            it.init();
            while(it.next()){
                derivate(it.x())=this->operator ()(f,it.x(),coordinate);
            }
            return derivate;
        }
    };


    //@}
    //-------------------------------------
    //
    //! \name y=F(f,x) with f a field, x a VecN and y a scalar/vectoriel value
    //@{
    //-------------------------------------

    /*!
    \class pop::FunctorPDE::Gradient
    \brief Gradient of a scalar/vectoriel field \f$\overrightarrow{\nabla} f\f$
    * \tparam PartialDerivate  partial derivate
    *
    */
    template<typename PartialDerivate=PartialDerivateCentered >
    class POP_EXPORTS Gradient
    {
    public:
        PartialDerivate partial;


        /*!
        \fn  Function iterate(const Function &f,Iterator &it)
        * \brief iterate the gradient for scalar field
        * \param  f input scalar/vectoriel field
        * \param  it iterator
        * \return output derived vectoriel field
        *
        */
        template<typename Function,typename Iterator>
        typename FunctionTypeTraitsSubstituteF<Function,VecN<Function::DIM, typename Function::F> >::Result iterate(const Function &f,Iterator &it){
            typename FunctionTypeTraitsSubstituteF<Function,VecN<Function::DIM, typename Function::F> >::Result grad(f.getDomain());
            it.init();
            while(it.next()){
                for(int i = 0;i<Function::DIM;i++)
                    grad(it.x())(i)=partial(f,it.x(),i);

            }
            return grad;
        }

        /*!
        \fn VecN<Function::DIM, typename Function::F> operator()(const Function & f,  typename Function::E & x)
        * \param  f input scalar field
        * \param  x input VecN
        * \return a vectoriel value
        *
        * The functor returns the vector \f$\overrightarrow{\nabla} f\f$ that is to \f$\begin{pmatrix}\frac{\partial f}{\partial x_0}\\\frac{\partial f}{\partial x_1}\\\vdots \end{pmatrix}\f$
        * with the partial derivate given by the template paramater
        */
        template<typename Function>
        inline VecN<Function::DIM, typename Function::F> operator()(const Function & f,  typename Function::E & x)
        {
            VecN<Function::DIM, typename Function::F> grad;
            for(int i = 0;i<Function::DIM;i++)
                grad(i)=partial(f,x,i);
            return grad;
        }

    };
    /*!
    \class pop::FunctorPDE::Divergence
    \brief  Divergence of the vertoriel field \f$\overrightarrow{\nabla}\cdot \overrightarrow{f}\f$
    *

    *
    */
    template<typename PartialDerivate=PartialDerivateCentered >
    class POP_EXPORTS Divergence
    {
    public:
        PartialDerivate partial;
        template<typename Function,typename Iterator>
        typename FunctionTypeTraitsSubstituteF<Function, typename Identity<typename Function::F>::Result::F >::Result iterate(const Function &f,Iterator &it){
            typename FunctionTypeTraitsSubstituteF<Function,typename Identity<typename Function::F>::Result::F >::Result div(f.getDomain());
            it.init();
            while(it.next()){
                typename Identity<typename Function::F>::Result::F divergence(0);
                for(int i = 0;i<Function::DIM;i++){
                    divergence+= partial(f,it.x(),i)(i);
                }
                div(it.x())=divergence;

            }
            return div;
        }
        /*!
        \fn typename Identity<typename Function::F>::Result::F operator()(const Function & f,  typename Function::E & x)
        * \param  f input vectoriel field
        * \param  x input VecN
        * \return a scalar value
        *
        * The functor returns the scalar value \f$\overrightarrow{\nabla}\cdot \overrightarrow{f}\f$ that is \f$\sum_{i=0}^{n-1} \frac{\partial f_i}{\partial x_i}\f$
        * with the partial derivate given by the template paramater
        *
        */
        template<typename Function>
        typename Identity<typename Function::F>::Result::F operator()(const Function & f,  typename Function::E & x)
        {
            typename Identity<typename Function::F>::Result::F divergence(0);
            for(int i = 0;i<Function::DIM;i++){
                typename Function::F div=partial(f,x,i);
                divergence+=div(i);
            }
            return divergence;
        }
    };
    /*!
    \class pop::FunctorPDE::Curl
    \brief  Curl of the 3d vectoriel field \f$\overrightarrow{\nabla}\times \overrightarrow{f}\f$
    *
    *
    */
    template<typename PartialDerivate=PartialDerivateCentered >
    class POP_EXPORTS Curl
    {
    public:
        PartialDerivate partial;
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it){
            Function curl(f.getDomain());
            it.init();
            while(it.next()){
                curl = this->operator ()(f,it.x());

            }
            return curl;
        }
        /*!
        \fn typename Function::F operator()(const Function & f,  typename Function::E & x)
        * \param  f input vectoriel field
        * \param  x input VecN
        * \return a vectoriel value
        *
        * The functor returns the scalar value \f$\overrightarrow{\nabla}\times \overrightarrow{f}\f$ that is \f$\begin{pmatrix}\frac{\partial f_2}{\partial x_1}-\frac{\partial f_1}{\partial x_2} \\\frac{\partial f_0}{\partial x_2}-\frac{\partial f_2}{\partial x_0} \\ \frac{\partial f_1}{\partial x_0}-\frac{\partial f_0}{\partial x_1}   \end{pmatrix}\f$
        * with the partial derivate given by the template paramater
        *
        */
        template<typename Function>
        typename Function::F operator()(const Function & f,  typename Function::E & x)
        {
            typename Function::F curl;
            typename Function::F divx = partial(f,x,0);
            typename Function::F divy = partial(f,x,1);
            typename Function::F divz = partial(f,x,2);
            curl(0) = divy(2)-divz(1);
            curl(1) = divz(0)-divx(2);
            curl(2) = divx(1)-divy(0);
            return curl;
        }
    };
    /*!
    \class pop::FunctorPDE::Laplacien
    \brief  Laplacien of a scalar/vectoriel field \f$\overrightarrow{\nabla} \cdot (\overrightarrow{\nabla} f)\f$ or\f$\overrightarrow{\nabla} \cdot (\overrightarrow{\nabla} \overrightarrow{f})\f$
    *
    *
    */
    template<typename PartialDerivateSecond=PartialDerivateSecondCentered>
    class POP_EXPORTS Laplacien
    {
    public:
        PartialDerivateSecond partialsecond;
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it){

            Vec<Function> vf;
            for(int i = 0;i<Function::DIM;i++)
                vf.push_back(partialsecond.iterate(f,it,i,i));

            Function laplacien(f.getDomain());
            it.init();
            while(it.next()){
                typename Function::F sum=0;
                for(int i = 0;i<Function::DIM;i++)
                    sum += vf[i](it.x());
                laplacien(it.x())=sum;
            }
            return laplacien;
        }




        /*!
        \fn typename Function::F operator()(const Function & f,  typename Function::E & x)
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \return a scalar/vectoriel value
        *
        * The functor returns the laplacien value \f$\overrightarrow{\nabla} \cdot (\overrightarrow{\nabla} f)=\sum_i \frac{\partial^2 f}{\partial x^2} \f$
        * with the partial second derivate given by the template paramater
        *
        */
        template<typename Function>
        typename Function::F operator()(const Function & f,  typename Function::E & x){
            typename Function::F laplacien(0);
            for(int i = 0;i<Function::DIM;i++){
                laplacien+=partialsecond(f,x,i,i);
            }
            return laplacien;
        }

    };
    /*!
    \class pop::FunctorPDE::HessianMatrix
    \brief  Hessian matrix of a scalar field
    *
\f$H(f) = \begin{bmatrix}
\dfrac{\partial^2 f}{\partial x_1^2} & \dfrac{\partial^2 f}{\partial x_1\,\partial x_2} & \cdots & \dfrac{\partial^2 f}{\partial x_1\,\partial x_n} \\[2.2ex]
\dfrac{\partial^2 f}{\partial x_2\,\partial x_1} & \dfrac{\partial^2 f}{\partial x_2^2} & \cdots & \dfrac{\partial^2 f}{\partial x_2\,\partial x_n} \\[2.2ex]
\vdots & \vdots & \ddots & \vdots \\[2.2ex]
\dfrac{\partial^2 f}{\partial x_n\,\partial x_1} & \dfrac{\partial^2 f}{\partial x_n\,\partial x_2} & \cdots & \dfrac{\partial^2 f}{\partial x_n^2}
\end{bmatrix}.\f$
    *
    *
    */
    template<typename PartialDerivateSecond=PartialDerivateSecondCentered>
    class POP_EXPORTS HessianMatrix
    {
    public:
        PartialDerivateSecond partialsecond;
        template<typename Function,typename Iterator>
        typename FunctionTypeTraitsSubstituteF<Function,Mat2x<F64,Function::DIM,Function::DIM> >::Result iterate(const Function &f,Iterator it){
            typedef Mat2x<F64,Function::DIM,Function::DIM> Mat2x22f;
            typename FunctionTypeTraitsSubstituteF<Function,Mat2x22f>::Result hessian(f.getDomain());
            it.init();
            while(it.next()){

                hessian(it.x())=operator ()(f,it.x());
            }
            return hessian;
        }
        /*!
        \fn typename Function::F operator()(const Function & f,  typename Function::E & x)
        * \param  f input scalar/vectoriel field
        * \param  x input VecN
        * \return a scalar/vectoriel value
        *
        * The functor returns the laplacien value \f$\overrightarrow{\nabla} \cdot (\overrightarrow{\nabla} f)=\sum_i \frac{\partial^2 f}{\partial x^2} \f$
        * with the partial second derivate given by the template paramater
        *
        */
        template<typename Function>
        inline Mat2x<F64,Function::DIM,Function::DIM> operator()(const Function & f,  typename Function::E & x){
            typedef Mat2x<F64,Function::DIM,Function::DIM> Mat2x22f;
            Mat2x22f m;
            for(int i = 0;i<Function::DIM;i++){
                for(int j = 0;j<Function::DIM;j++){
                    if(j<i)
                        m(i,j)=m(j,i);
                    else
                        m(i,j)= partialsecond.operator()(f,x,i,j);
                }
            }
            return m;
        }

    };
    //@}

    //@}
    //-------------------------------------
    //
    //! \name Composed functor
    //@{
    //-------------------------------------

    /*!
    \class pop::FunctorPDE::DiffusionMalikPeronaGaussian
    \brief  Malika Peronna \f$ D( \overrightarrow{\nabla} f )= c(\| \overrightarrow{\nabla} f *G_\sigma  \| )\overrightarrow{\nabla} f \f$ with \f$c(x)= \frac{1}{1 + \left(\frac{x}{K}\right)^2}  \f$
    * where \f$G_\sigma\f$ the centered gaussian(normal) kernel with \f$\sigma\f$ the standard deviation as scale paramete
    *
    *
    */
    class POP_EXPORTS DiffusionMalikPeronaGaussian
    {
    private:

        F64 kpower2;
        Private::GaussianKernelOnlyIterate gaussian;

        inline F64 fPower2(F64 v,const int dim) {
            return 0.5 /(1 + (v*80)/(kpower2))*1.0/dim;
        }
    public:

        /*!
        \fn DiffusionMalikPeronaGaussian(F64 K,double sigma=1,int kernel_size=2)
        * \param K the constant in the monotonically decreasing function c
        * \param sigma standard deviation of the gaussian kernel (scale parameter)
        * \param kernel_size radius of the gaussian kernel (kernel_size>sigma)
        *
        * Construct the functor
        *
        */
        DiffusionMalikPeronaGaussian(F64 K,double sigma=1,int kernel_size=2)
            :kpower2(K*K),gaussian(sigma,kernel_size)
        {
        }

        /*!
        \fn Function iterate(const Function &f,Iterator &it)
        * \param f input function
        * \param it iterator
        *
        * smooth the function on the domain defined by the iterator
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it){

            Function out = gaussian.iterate(f,it);
            it.init();
            while(it.next()){
                for(int i =0;i<Function::DIM;i++){
                    out(it.x())(i)=fPower2(productInner(out(it.x())(i),out(it.x())(i)),Function::DIM)*f(it.x())(i);
                }
            }
            return out;
        }
    };
    /*!
    \class pop::FunctorPDE::DiffusionMalikPeronaDeriche
    \brief  Malika Peronna \f$ D( \overrightarrow{\nabla} f )= c(\| \overrightarrow{\nabla} f *G_\alpha  \| )\overrightarrow{\nabla} f \f$ with \f$c(x)= \frac{1}{1 + \left(\frac{x}{K}\right)^2}  \f$
    * where \f$G_\alpha\f$ the smooth Deriche kernel
    *
    *
    */
    class POP_EXPORTS DiffusionMalikPeronaDeriche
    {
    private:

        inline F64 fPower2(F64 v,const int dim) {
            return 0.5 /(1 + (v*80)/(kpower2))*1.0/ dim;
        }
        F64 kpower2;
        F64 alpha;
    public:
        /*!
        \fn DiffusionMalikPeronaDeriche(F64 K,F64 alpha=1)
        * \param K the constant in the monotonically decreasing function c
        * \param alpha \f$\alpha\f$ a parameter (0.5= high smooth, 2 = low smooth)
        *
        * Construct the functor
        *
        */
        DiffusionMalikPeronaDeriche(F64 K,F64 alpha=1)
            :kpower2(K*K),alpha(alpha)
        {
        }

        /*!
        \fn Function iterate(const Function &f,Iterator &it)
        * \param f input function
        * \param it iterator
        *
        * smooth the function on the domain defined by the iterator
        *
        */
        template<typename Function,typename Iterator>
        Function iterate(const Function &f,Iterator &it){
            Function out = Private::smoothDeriche(f,alpha);
            it.init();
            while(it.next()){
                for(int i =0;i<Function::DIM;i++){
                    out(it.x())(i)=fPower2(normValue(out(it.x())(i))*normValue(out(it.x())(i)),Function::DIM)*f(it.x())(i);
                }
            }
            return out;
        }
    };

    /*!
    \class pop::FunctorPDE::DiffusionMalikPeronaDericheOneTime
    \brief  Malika Peronna \f$ D( \overrightarrow{\nabla} f )= c(\| \overrightarrow{\nabla} f *G_\alpha  \| )\overrightarrow{\nabla} f \f$ with \f$c(x)= \frac{1}{1 + \left(\frac{x}{K}\right)^2}  \f$
    * where \f$G_\alpha\f$ the smooth Deriche kernel
    *
    *
    */
    template<typename Function>
    class POP_EXPORTS DiffusionMalikPeronaDericheOneTime
    {
    private:

        inline F64 fPower2(F64 v,const int dim) {
            return 0.5 /(1 + (v*80)/(kpower2))*1.0/ dim;
        }
        F64 kpower2;
        F64 alpha;
        Function deriche;
        bool firsttime;
        Function outsmooth;
    public:
        /*!
        \fn DiffusionMalikPeronaDericheOneTime(F64 K,F64 alpha=1)
        * \param K the constant in the monotonically decreasing function c
        * \param alpha \f$\alpha\f$ a parameter (0.5= high smooth, 2 = low smooth)
        *
        * Construct the functor
        *
        */
        DiffusionMalikPeronaDericheOneTime(F64 K,F64 alpha=1)
            :kpower2(K*K),alpha(alpha),firsttime(false)
        {
        }

        /*!
        \fn Function iterate(const Function &f,Iterator &it)
        * \param f input function
        * \param it iterator
        *
        * smooth the function on the domain defined by the iterator
        *
        */
        template<typename Iterator>
        Function iterate(const Function &f,Iterator &it){
            if(firsttime==false){
                firsttime =true;
                outsmooth = Private::smoothDeriche(f,alpha);
            }
            Function out(f.getDomain());
            it.init();
            while(it.next()){
                for(int i =0;i<Function::DIM;i++){
                    out(it.x())(i)=fPower2(normValue(outsmooth(it.x())(i))*normValue(outsmooth(it.x())(i)),Function::DIM)*f(it.x())(i);
                }
            }
            return out;
        }
    };
    //@}

    /*!
    \class pop::FunctorPDE::FreeEnergy
    \brief  the opposite of the derivative of the double well potential \f$−\phi^2/2 + \phi^4/4\f$,
    *
    *
    */
    class POP_EXPORTS  FreeEnergy
    {
    private:
        //        Vec<F64> v;
        //        F64 step;
        //        F64 xmin;
        //        F64 xmax;
    public:
        //        FreeEnergy()
        //        {
        //            step=0.01;
        //            xmin = -2;
        //            xmax =  2;
        //            int x = ;
        //            for(int i)
        //        }
        /*!
        \fn Type operator()(Type x)
        * \param x input value
        * \return \f$\phi(1-\phi^2)\f$,
        *
        * Construct the functor
        *
        */
        template<typename Type>
        Type operator()(Type x){
            return x*(1-x*x);
        }
        void init(){}
    };
};
/// @endcond
template<typename Function1,typename FunctorCausal, typename FunctorAntiCausal>
Function1 Private::recursiveAllDirections(const Function1 & f, FunctorCausal & funccausal, FunctorAntiCausal & funcanticausal )
{
    typename Function1::IteratorEOrder itorder (f.getIteratorEOrder());
    typedef typename  FunctionTypeTraitsSubstituteF<typename Function1::F,F64>::Result TypeF64;
    typename FunctionTypeTraitsSubstituteF<Function1,TypeF64>::Result fprevious(f);
    typename FunctionTypeTraitsSubstituteF<Function1,TypeF64>::Result fcausal(f.getDomain());
    typename FunctionTypeTraitsSubstituteF<Function1,TypeF64>::Result fanticausal(f.getDomain());
    fprevious = f;
    for(I32 i=0;i <Function1::DIM;i++)
    {
        itorder.setLastLoop(i);
        typename Function1::E dir;
        dir = 1;
        itorder.setDirection(dir);
        itorder.init();
        fcausal =recursive(fprevious,itorder,funccausal);
        dir = -1;
        itorder.setDirection(dir);
        itorder.init();
        fanticausal= recursive(fprevious,itorder,funcanticausal);

        fprevious = fcausal + fanticausal;
    }
    Function1 h(fprevious);
    return h;
}



template<typename Function1>
Function1 Private::smoothDeriche(const Function1 & f, F64 alpha)
{
    F64 e_a = std::exp(- alpha);
    F64 e_2a = std::exp(- 2.0 * alpha);
    F64 k = (1.0 - e_a) * (1.0 - e_a) / (1.0 + (2 * alpha * e_a) - e_2a);

    F64 a0_c= k;
    F64 a1_c=  k * e_a * (alpha - 1.0);
    F64 a2_c=  0;
    F64 a0_ac= 0;
    F64 a1_ac=  k * e_a * (alpha + 1.0);
    F64 a2_ac=  - k * e_2a;

    F64 b1= 2 * e_a;
    F64 b2 = - e_2a;


    F64 a0_c_border0 = ((a0_c + a1_c) / (1.0 - b1 - b2));
    F64 a0_c_border1 = a0_c ;
    F64 a1_c_border1 = a1_c ;

    F64 a0_ac_border0 = ((a1_ac + a2_ac) / (1.0 - b1 - b2));
    F64 a0_ac_border1 = 0 ;
    F64 a1_ac_border1 = a1_ac + a2_ac ;

    F64 b1_border1 = b1 + b2 ;

    FunctorF::FunctorRecursiveOrder2 funccausal
            (a0_c,a1_c,a2_c,
             b1,b2,
             a0_c_border0,
             a0_c_border1,a1_c_border1,b1_border1) ;

    FunctorF::FunctorRecursiveOrder2 funcanticausal
            (a0_ac,a1_ac,a2_ac,
             b1,b2,
             a0_ac_border0,
             a0_ac_border1,a1_ac_border1,b1_border1) ;

    return recursiveAllDirections(f,funccausal,funcanticausal);
}
template< typename Function1, typename IteratorEOrder>
Function1    Private::recursive(const Function1 & f,IteratorEOrder & it, FunctorF::FunctorRecursiveOrder2 & func)
{
    Function1 h(f.getDomain());
    while(it.next()){
        h(it.x())=func(f,h,it.x(),it.getBordeLenghtLastLoop(),it.getIndexLastLoop(),it.getWayLastLoop());
    }
    return h;
}

Private::GaussianKernelOnlyIterate::GaussianKernelOnlyIterate(F64 sigma,int radius_kernel)
    :sigma(sigma),radius_kernel(radius_kernel)
{
    F64 sum=0;
    for(int i=0;i<2*radius_kernel+1;i++){
        F64  value =std::exp(-0.5*(radius_kernel-i)*(radius_kernel-i)/(sigma*sigma));
        v_value_one_dimension.push_back(value);
        sum+=value;
    }
    //normalisation
    for(int i=0;i<(int)v_value_one_dimension.size();i++){
        v_value_one_dimension[i]/=sum;
    }

}
template<typename Function, typename Iterator>
Function Private::GaussianKernelOnlyIterate::iterate(const Function& f,Iterator & it)
{
    Function h1(f);
    Function h2(f.getDomain());
    int d=0;
    for(;d<Function::DIM;d++){
        it.init();
        while(it.next()){
            typename Function::F sum(f(it.x()));
            sum=0;
            typename Function::E x = it.x();
            for(int r=-radius_kernel;r<=radius_kernel;r++){
                x[d]=it.x()[d]+r;
                if(x(d)<0)x(d)= -x(d)-1;
                if(x(d)>=f.getDomain()(d))x(d)= 2*f.getDomain()(d)-x(d)-1;
                if(d%2==0){
                    sum+=(h1(x)*v_value_one_dimension[radius_kernel+r]);
                }
                else{
                    sum+=(h2(x)*v_value_one_dimension[radius_kernel+r]);
                }
            }
            if(d%2==0){
                h2(it.x())=sum;
            }
            else{

                h1(it.x())=sum;
            }
        }
    }
    if(d%2==0)
        return h1;
    else
        return h2;
}
namespace Private
{


template<int DIM>
template<typename Function,typename Iterator>
Function ConvolutionSeparableMirror<DIM>::operator()(Iterator & it,const Function& f ,Loki::Type2Type<Iterator>)
{
    typedef typename FunctionTypeTraitsSubstituteF<typename Function::F,F64>::Result TypeFloat;
    typedef typename FunctionTypeTraitsSubstituteF<Function,TypeFloat>::Result FunctionFloat;
    FunctionFloat h1(f);
    FunctionFloat h2(f.getDomain());
    int d=0;
    int radius_kernel = (this->_kernel[0].size()-1)/2;
    for(;d<Function::DIM;d++){
        it.init();
        while(it.next()){

            TypeFloat sum(0.);

            typename Function::E x = it.x();
            for(int r=-radius_kernel;r<=radius_kernel;r++){
                x[d]=it.x()[d]+r;
                if(x(d)<=-f.getDomain()(d))
                    x(d)=-f.getDomain()(d);
                if(x(d)>=2*f.getDomain()(d)-1)
                    x(d)=2*f.getDomain()(d)-1;
                if(x(d)<0)x(d)= -x(d)-1;
                if(x(d)>=f.getDomain()(d))x(d)= 2*f.getDomain()(d)-x(d)-1;
                if(d%2==0){
                    sum+=h1(x)*this->_kernel[d][radius_kernel+r];
                }
                else{
                    sum+=h2(x)*this->_kernel[d][radius_kernel+r];
                }
            }
            if(d%2==0){
                h2(it.x())=sum;
            }
            else{
                h1(it.x())=sum;
            }

        }
    }
    if(d%2==0)
        return h1;
    else
        return h2;
}
template<int DIM>
template<typename Type>
MatN<DIM,Type> ConvolutionSeparableMirror<DIM >::operator()(typename MatN<DIM,Type>::IteratorEDomain & ,const MatN<DIM,Type>& f,Loki::Type2Type<typename MatN<DIM,Type>::IteratorEDomain>  )
{
    MatN<DIM,Type> fout(f);
    for(unsigned int i=0;i<DIM;i++)
        fout = this->operator ()(i,fout,VecN<DIM,int>(0),VecN<DIM,int>(0));
    return fout;
}
template<int DIM>
template<typename Type>
MatN<DIM,Type> ConvolutionSeparableMirror<DIM >::operator()(typename MatN<DIM,Type>::IteratorERectangle & it,const MatN<DIM,Type>& f,Loki::Type2Type<typename MatN<DIM,Type>::IteratorERectangle> )
{

    MatN<DIM,Type> fout(f);
    for(unsigned int i=0;i<DIM;i++)
        fout = this->operator ()(i,fout,it.xMin(),f.getDomain()-1-it.xMax());
    return fout;
}
namespace Private {

template<typename Function>
static void addBorder( Function & f,int widthborder, const typename Function::F& border_value=typename Function::F(0),int boundary_condition=Boundary_Condition_Constant)
{

    Function h(f.getDomain()+2*widthborder,border_value);
    typename Function::IteratorEDomain it (f.getIteratorEDomain());
    if(boundary_condition==Boundary_Condition_Constant){

        while(it.next())
            h(it.x()+widthborder)=f(it.x());
    }else if(boundary_condition==Boundary_Condition_Mirror){
        while(it.next()){
            h(it.x()+widthborder)=f(it.x());
            for(int i=0;i<Function::DIM;i++){
                if(it.x()(i)<widthborder){
                    typename Function::E x=it.x()+widthborder;
                    x(i)=widthborder-1-it.x()(i);
                    h(x)=f(it.x());
                }
                else if(f.getDomain()(i)-1-it.x()(i)<widthborder){
                    typename Function::E x=it.x()+widthborder;
                    x(i)=widthborder+2*f.getDomain()(i)-1-it.x()(i);
                    h(x)=f(it.x());
                }
            }
        }
    }
    f = h;
}
template<typename Type>
void addBorder( MatN<2,Type> & in,int width_border, const typename MatN<2,Type>::F& border_value=typename MatN<2,Type>::F(0),int boundary_condition=Boundary_Condition_Constant)
{
    MatN<2,Type> out(in.getDomain()+2*width_border,border_value);
    if(boundary_condition==Boundary_Condition_Constant){
        int sift_in_core =0;
        int sift_out_core=width_border*out.sizeJ()+width_border;
        for(unsigned int i=0;i<in.sizeI();i++){
            std::copy(in.begin()+sift_in_core,in.begin()+sift_in_core+in.sizeJ(),out.begin()+sift_out_core);
            sift_in_core +=in.sizeJ();
            sift_out_core+=out.sizeJ();
        }
    }
    else if(boundary_condition==Boundary_Condition_Mirror){
        int sift_in_core =0;
        int sift_out_core=width_border*out.sizeJ();
        for(unsigned int i=0;i<in.sizeI();i++){
            std::reverse_copy(in.begin()+sift_in_core,in.begin()+sift_in_core+width_border,out.begin()+sift_out_core);
            sift_out_core+=width_border;
            std::copy(in.begin()+sift_in_core,in.begin()+sift_in_core+in.sizeJ(),out.begin()+sift_out_core);
            sift_in_core +=in.sizeJ();
            sift_out_core+=in.sizeJ();
            std::reverse_copy(in.begin()+sift_in_core-width_border,in.begin()+sift_in_core,out.begin()+sift_out_core);
            sift_out_core+=width_border;
        }

        sift_in_core =(width_border-1)*(in.sizeJ());
        sift_out_core=width_border;
        for( int i=0;i<width_border;i++){
            std::copy(in.begin()+sift_in_core,in.begin()+sift_in_core+in.sizeJ(),out.begin()+sift_out_core);
            sift_in_core -=in.sizeJ();
            sift_out_core+=out.sizeJ();

        }
        sift_in_core =(in.sizeI()-1)*(in.sizeJ());
        sift_out_core=width_border+(out.sizeI()-width_border)*out.sizeJ();
        for( int i=0;i<width_border;i++){
            std::copy(in.begin()+sift_in_core,in.begin()+sift_in_core+in.sizeJ(),out.begin()+sift_out_core);
            sift_in_core -=in.sizeJ();
            sift_out_core+=out.sizeJ();

        }
    }
    in = out;
}
}
template<int DIM>
template<typename Type>
MatN<DIM,Type> ConvolutionSeparableMirror<DIM>::operator()(int index_coordinate,const MatN<DIM,Type>& in,const VecN<DIM,int> &shif_begin,const VecN<DIM,int>& shif_end)
{
    typedef typename FunctionTypeTraitsSubstituteF<Type,F64>::Result TypeFloat;
    unsigned int width = std::floor(this->_kernel[index_coordinate].size()/2.);
    MatN<DIM,Type> f1(in);
    Private::addBorder(f1,width,Type(0),Boundary_Condition_Mirror);
    MatN<DIM,Type> f2(f1.getDomain());
    mult_sum<DIM,Type> mult;
    int sift;
    if(index_coordinate==0){
        mult._incr = f1.sizeJ();
    }
    else if(index_coordinate==1){
        mult._incr = 1;
    }else{
        mult._incr = f1.sizeJ()*f1.sizeI();
    }
    sift= -width* mult._incr;
    if(DIM==2){
        for(unsigned int index_i=width+shif_begin(0);index_i<f1.sizeI()-width-shif_end(0);index_i++){
            for(unsigned int index_j=width+shif_begin(1);index_j<f1.sizeJ()-width-shif_end(1);index_j++){
                int global_sift = index_j  +  index_i*f1.getDomain()(1)+sift;
                mult._it_matrix = f1.begin()+ global_sift;
                f2(index_i,index_j)=std::accumulate(this->_kernel[index_coordinate].begin(), this->_kernel[index_coordinate].end(), TypeFloat(0.),mult);
            }
        }
    }else if(DIM==3){
        for(unsigned int index_i=width+shif_begin(0);index_i<f1.sizeI()-width-shif_end(0);index_i++){
            for(unsigned int index_j=width+shif_begin(1);index_j<f1.sizeJ()-width-shif_end(1);index_j++){
                for(unsigned int index_k=width+shif_begin(2);index_k<f1.sizeK()-width-shif_end(2);index_k++){
                    int global_sift = index_j  +  index_i*f1.getDomain()(1)+index_k*f1.getDomain()(1)*f1.getDomain()(0)+sift;
                    mult._it_matrix = f1.begin()+ global_sift;
                    f2(index_i,index_j,index_k)=std::accumulate(this->_kernel[index_coordinate].begin(), this->_kernel[index_coordinate].end(), TypeFloat(0.),mult);
                }
            }
        }

    }
    return f2( VecN<DIM,int>(width) ,VecN<DIM,int>(f1.getDomain()-VecN<DIM,int>(width)));
}
}
}
#endif // FUNCTOTPDE_H

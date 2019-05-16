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
#ifndef PROCESSINGADVANCED_H
#define PROCESSINGADVANCED_H

#include"core/data/typeF/TypeTraitsF.h"
#include"core/data/population/PopulationData.h"
#include"core/data/distribution/DistributionFromDataStructure.h"
#include"core/data/utility/BasicUtility.h"
#include"core/data/functor/FunctorF.h"
#include"core/data/population/PopulationFunctor.h"
#include"core/data/utility/CollectorExecutionInformation.h"
#include"core/data/functor/FunctorPDE.h"
#include"core/algorithm/Convertor.h"
#include"core/algorithm/FunctionProcedureFunctorF.h"
#include"core/algorithm/AnalysisAdvanced.h"
#include"core/algorithm/Statistics.h"
namespace pop
{
namespace Private{
template<typename Result >
struct FunctorConst{const Result & _v;FunctorConst(const Result & v ):_v(v){}Result operator()(){return _v;}};
template<typename Result, typename Type,bool isVectoriel, template<typename,typename> class PolicyClassOverFlow=ArithmeticsSaturation >
class FunctorDistribution: public PolicyClassOverFlow<Result,F64 >
{
private:
    Distribution & _f;
public:
    FunctorDistribution(Distribution & f )
        :_f(f)
    {}
    Result operator()(Type  x)
    {
        return PolicyClassOverFlow<Result,F64 >::Range(_f(x));
    }
};
template<typename Result, typename Type>
class FunctorDistribution<Result,  Type,true,ArithmeticsSaturation>: public ArithmeticsSaturation<Result,F64 >
{
private:
    Distribution & _f;
public:
    FunctorDistribution(Distribution & f )
        :_f(f)
    {}
    Result operator()(Type  x)
    {
        Result r;
        for(int i=0;i<Result::DIM;i++)
            r(i)=ArithmeticsSaturation<typename Result::F,F64 >::Range(_f(x(i)));
        return r;
    }
};
}
struct ProcessingAdvanced
{
    template<typename Function,int DIM>
    static Function  integral(const Function & f,Loki::Int2Type<DIM>);
    template<typename Function>
    static Function  integral(const Function & f,Loki::Int2Type<2>)
    {
        Function s (f.getDomain());
        Function out(f.getDomain());
        for(int i=0;i<f.getDomain()(0);i++){

            for(int j=0;j<f.getDomain()(1);j++){
                if(j==0)
                {
                    s(i,j)=f(i,j);
                }
                else
                {
                    s(i,j)=f(i,j)+s(i,j-1);
                }
                if(i==0)
                {
                    out(i,j)=s(i,j);
                }
                else
                {
                    out(i,j)=s(i,j)+out(i-1,j);
                }
            }
        }
        return out;
    }
    template<typename Function>
    static Function  integral(const Function & f,Loki::Int2Type<3>)
    {
        Function s (f.getDomain());
        Function integral2d (f.getDomain());
        Function out(f.getDomain());
        for(int i=0;i<f.getDomain()(0);i++){

            for(int j=0;j<f.getDomain()(1);j++){

                for(int k=0;k<f.getDomain()(2);k++){
                    if(k==0)
                    {
                        s(i,j,k)=f(i,j,k);
                    }
                    else
                    {
                        s(i,j,k)=f(i,j,k)+s(i,j,k-1);
                    }
                    if(j==0)
                    {
                        integral2d(i,j,k)=s(i,j,k);
                    }
                    else
                    {
                        integral2d(i,j,k)=s(i,j,k)+integral2d(i,j-1,k);
                    }
                    if(i==0)
                    {
                        out(i,j,k)=integral2d(i,j,k);
                    }
                    else
                    {
                        out(i,j,k)=integral2d(i,j,k)+out(i-1,j,k);
                    }
                }
            }
        }
        return out;
    }


    template<typename Function>
    static Function  nonMaximumSuppression(const Function & img,double sigma, typename FunctionTypeTraitsSubstituteF<Function,F64>::Result &gradnorm)
    {

        typedef typename FunctionTypeTraitsSubstituteF<Function,VecN<Function::DIM,F64> >::Result FunctionVecFloat;
        typedef typename FunctionTypeTraitsSubstituteF<Function,F64>::Result FunctionFloat;
        typedef typename FunctionTypeTraitsSubstituteF<Function,UI8 >::Result FunctionBinary;
        gradnorm= ProcessingAdvanced::gradNormGaussian(FunctionFloat(img),sigma,sigma*3,img.getIteratorEDomain());
        FunctionVecFloat grad= ProcessingAdvanced::gradientVecGaussian(FunctionFloat(img),sigma);

        std::vector<double> vtan;
        vtan.push_back(std::tan(-3*PI/8));
        vtan.push_back(std::tan(-PI/8));
        vtan.push_back(std::tan( PI/8));
        vtan.push_back(std::tan(3*PI/8));
        FunctionBinary edge(img.getDomain());

        typename Function::IteratorERectangle it = img.getIteratorERectangle(1,img.getDomain()-2);
        while(it.next())
        {
            Mat2UI8::E x = it.x();
            double slop=-grad(x)(1)/grad(x)(0);
            int direction = (int)(std::lower_bound (vtan.begin(), vtan.end(), slop)-vtan.begin());
            if(direction==2){
                if(gradnorm(x(0),x(1))>=gradnorm(x(0)-1,x(1))&&gradnorm(x(0),x(1))>=gradnorm(x(0)+1,x(1))){
                    edge(x)=255;
                }
            }else if(direction==3){
                if(gradnorm(x(0),x(1))>=gradnorm(x(0)-1,x(1)+1)&&gradnorm(x(0),x(1))>=gradnorm(x(0)+1,x(1)-1)){
                    edge(x)=255;
                }
            }
            else if(direction==0||direction==4){
                if(gradnorm(x(0),x(1))>=gradnorm(x(0),x(1)-1)&&gradnorm(x(0),x(1))>=gradnorm(x(0),x(1)+1)){
                    edge(x)=255;
                }
            }
            else{
                if(gradnorm(x(0),x(1))>=gradnorm(x(0)-1,x(1)-1)&&gradnorm(x(0),x(1))>=gradnorm(x(0)+1,x(1)+1)){
                    edge(x)=255;
                }
            }
        };
        return edge;
    }

    template< typename Function, typename IteratorEDomain, typename IteratorENeighborhood >
    static typename FunctionTypeTraitsSubstituteF<Function,UI8 >::Result minimaLocalMap(const Function & f,IteratorEDomain& itd,IteratorENeighborhood&itn){
        typename FunctionTypeTraitsSubstituteF<Function,UI8 >::Result map(f.getDomain());
        while(itd.next()){
            typename Function::F value = f(itd.x());
            bool minima=true;
            itn.init(itd.x());
            while(itn.next()&&minima==true){
                if(f(itn.x())<value){
                    minima = false;
                }
            }
            if(minima==true)
                map(itd.x())=255;
        }
        return map;
    }

    template< typename Function, typename IteratorEDomain, typename IteratorENeighborhood >
    static Vec<typename Function::E> minimaLocal(const Function & f,IteratorEDomain& itd,IteratorENeighborhood&itn){
        Vec<typename Function::E> v_minima;
        while(itd.next()){
            typename Function::F value = f(itd.x());
            bool minima=true;
            itn.init(itd.x());
            while(itn.next()&&minima==true){
                if(f(itn.x())<value){
                    minima = false;
                }
            }
            if(minima==true)
                v_minima.push_back(itd.x());
        }
        return v_minima;
    }
    template< typename Function, typename IteratorEDomain, typename IteratorENeighborhood >
    static std::vector<typename Function::E> maximaLocal(const Function & f,IteratorEDomain& itd,IteratorENeighborhood&itn){
        Vec<typename Function::E> v_maxima;
        while(itd.next()){
            typename Function::F value = f(itd.x());
            bool maxima=true;
            itn.init(itd.x());
            while(itn.next()&&maxima==true){
                if(f(itn.x())>value){
                    maxima = false;
                }
            }
            if(maxima==true)
                v_maxima.push_back(itd.x());
        }
        return v_maxima;
    }
    template< typename Function, typename IteratorEDomain, typename IteratorENeighborhood >
    static typename FunctionTypeTraitsSubstituteF<Function,UI8 >::Result maximaLocalMap(const Function & f,IteratorEDomain& itd,IteratorENeighborhood&itn){
        typename FunctionTypeTraitsSubstituteF<Function,UI8 >::Result map(f.getDomain());
        while(itd.next()){
            typename Function::F value = f(itd.x());
            bool maxima=true;
            itn.init(itd.x());
            while(itn.next()&&maxima==true){
                if(f(itn.x())>value){
                    maxima = false;
                }
            }
            if(maxima==true)
                map(itd.x())=255;
        }
        return map;
    }
    template< typename Function, typename IteratorEDomain, typename IteratorENeighborhood >
    static Vec<typename Function::E> extremaLocal(const Function & f,IteratorEDomain& itd,IteratorENeighborhood&itn){
        Vec<typename Function::E> v_maxima;
        while(itd.next()){
            typename Function::F value = f(itd.x());
            bool maxima=true;
            bool minima=true;
            itn.init(itd.x());
            while(itn.next()&&(maxima==true||minima==true)){
                if(f(itn.x())>value){
                    maxima = false;
                }
                if(f(itn.x())<value){
                    minima = false;
                }
            }
            if(maxima==true||minima==true)
                v_maxima.push_back(itd.x());
        }
        return v_maxima;
    }
    template< typename Function, typename IteratorEDomain, typename IteratorENeighborhood >
    static typename FunctionTypeTraitsSubstituteF<Function,UI8 >::Result extremaLocalMap(const Function & f,IteratorEDomain& itd,IteratorENeighborhood&itn){
        typename FunctionTypeTraitsSubstituteF<Function,UI8 >::Result map(f.getDomain());
        while(itd.next()){
            typename Function::F value = f(itd.x());
            bool maxima=true;
            bool minima=true;
            itn.init(itd.x());
            while(itn.next()&&(maxima==true||minima==true)){
                if(f(itn.x())>value){
                    maxima = false;
                }
                if(f(itn.x())<value){
                    minima = false;
                }
            }
            if(maxima==true||minima==true)
                map(itd.x())=255;
        }
        return map;
    }

    template<typename Function,typename Iterator>
    static Function  randomField(const typename Function::Domain & domain, Distribution &d,Iterator &it){
        Function out (domain);
        while(it.next()){
            out(it.x())=d.randomVariable();
        }
        return out;
    }


    template<typename Function,typename Iterator>
    static Function  fill(const Function & in,const typename Function::F & y,Iterator &it){
        Function out (in.getDomain());
        Private::FunctorConst<typename Function::F> func(y);
        FunctionProcedureGenerator(func,it,out);
        return out;
    }



    template<typename Function,typename Iterator>
    static Function  fofx(const Function& in,Distribution &d,Iterator &it){
        Function out (in.getDomain());

        Private::FunctorDistribution<typename Function::F,typename Function::F,isVectoriel<typename Function::F>::value > func(d);
        FunctionProcedureFunctorUnaryF(in,func,it,out);
        return out;
    }
    template<typename Function,typename Iterator>
    static typename FunctionTypeTraitsSubstituteF<Function,UI8>::Result  threshold(const Function& in,typename Function::F ymin,typename Function::F ymax,Iterator &it){

        typename FunctionTypeTraitsSubstituteF<Function,UI8>::Result out(in.getDomain());
        FunctorF::FunctorThreshold<unsigned char,typename Function::F,typename  Function::F> func(ymin,ymax);
        FunctionProcedureFunctorUnaryF(in,func,it,out);
        return out;
    }
    template<typename Function,typename Iterator>
    static Function greylevelScaleContrast(const Function & f,F64 scale, Iterator & it)throw(pexception){

        Function out(f.getDomain());
        typedef typename FunctionTypeTraitsSubstituteF<typename Function::F,F64>::Result FloatF;
        FunctorF::FunctorAccumulatorMean<typename Function::F> func;
        it.init();
        FloatF mean = FunctionProcedureFunctorAccumulatorF(f,func,it);
        it.init();
        while(it.next()){
            FloatF value(f(it.x()));
            value = (value-mean)*scale+mean;
            out(it.x())= ArithmeticsSaturation<typename Function::F,FloatF>::Range(value);
        }
        return out;
    }
    template<typename Function,typename Iterator,int isVec>
    static Function greylevelRange(const Function & f,Iterator & it,typename Function::F min, typename Function::F max,Loki::Int2Type<isVec>)throw(pexception);

    template<typename Function,typename Iterator>
    static Function greylevelRange(const Function & f,Iterator & it,typename Function::F min, typename Function::F max,Loki::Int2Type<true>)throw(pexception)
    {
        typedef typename TypeTraitsTypeScalar<typename Function::F>::Result  FScalar;
        typedef typename FunctionTypeTraitsSubstituteF<Function,FScalar>::Result  FunctionScalar;
        VecN< Function::F::DIM,FunctionScalar > v ;
        pop::Convertor::toVecN(f,v);
        for(int i=0;i<Function::F::DIM;i++){
            it.init();
            v[i] = ProcessingAdvanced::greylevelRange(v[i],it,min(i),max(i),Loki::Int2Type< isVectoriel<FScalar>::value>());
        }
        Function dilat;
        Convertor::fromVecN(v,dilat);
        return dilat;
    }

    template<typename Function,typename Iterator>
    static Function greylevelRange(const Function & f,Iterator & it,typename Function::F min, typename Function::F max,Loki::Int2Type<false>)throw(pexception)
    {
        Function h(f.getDomain());
        FunctorF::FunctorAccumulatorMin<typename Function::F > funcmini;
        typename Function::F mini = FunctionProcedureFunctorAccumulatorF(f,funcmini,it);
        it.init();
        FunctorF::FunctorAccumulatorMax<typename Function::F > funcmaxi;
        typename Function::F maxi = FunctionProcedureFunctorAccumulatorF(f,funcmaxi,it);
        typename FunctionTypeTraitsSubstituteF<typename Function::F,F64>::Result ratio;
        if(maxi!=mini)
            ratio= 1.0*(max-min)/(maxi-mini);
        else
            ratio =1;

        std::string exp =BasicUtility::Any2String(ratio)+"*(x-"+BasicUtility::Any2String(mini)+")+"+BasicUtility::Any2String(min);
        DistributionExpression dist(exp);
        Private::FunctorDistribution<typename Function::F,typename Function::F,isVectoriel<typename Function::F>::value > func(dist);
        it.init();
        FunctionProcedureFunctorUnaryF(f,func,it,h);
        return h;
    }
    template<typename Function,typename Iterator>
    static Function greylevelRange(const Function & f,Iterator & it,typename Function::F min=NumericLimits<typename Function::F>::minimumRange(), typename Function::F max=NumericLimits<typename Function::F>::maximumRange())throw(pexception)
    {
        Function h(f.getDomain());
        FunctorF::FunctorAccumulatorMin<typename Function::F > funcmini;
        typename Function::F mini = FunctionProcedureFunctorAccumulatorF(f,funcmini,it);
        it.init();
        FunctorF::FunctorAccumulatorMax<typename Function::F > funcmaxi;
        typename Function::F maxi = FunctionProcedureFunctorAccumulatorF(f,funcmaxi,it);
        typename FunctionTypeTraitsSubstituteF<typename Function::F,F64>::Result ratio= 1.0*(max-min)/(maxi-mini);

        std::string exp =BasicUtility::Any2String(ratio)+"*(x-"+BasicUtility::Any2String(mini)+")+"+BasicUtility::Any2String(min);
        DistributionExpression dist(exp);
        Private::FunctorDistribution<typename Function::F,typename Function::F,isVectoriel<typename Function::F>::value > func(dist);
        it.init();
        FunctionProcedureFunctorUnaryF(f,func,it,h);
        return h;
    }

    template<typename Function,typename Iterator>
    static Function greylevelRemoveEmptyValue(const Function & f,  Iterator & it)throw(pexception)
    {
        Function h(f.getDomain());
        std::vector<typename Function::F> valueinf;
        while(it.next())
        {
            typename Function::F i= f(it.x());
            if( find(valueinf.begin(),valueinf.end(),i)==valueinf.end() )valueinf.push_back(i);
        }
        std::sort (valueinf.begin(), valueinf.end());

        typename std::vector<typename Function::F>::iterator  itvalue;
        it.init();
        while(it.next())
        {
            typename Function::F i= f(it.x());
            itvalue=find(valueinf.begin(),valueinf.end(),i);
            i=static_cast<I32>(itvalue-valueinf.begin());
            h(it.x())=i;
        }
        return h;
    }
    template<typename Function,typename Type>
    static Function greylevelTranslateMeanValueCast(const Function& f, typename Function::F mean,Loki::Type2Type<Type> )throw(pexception)
    {
        return  greylevelTranslateMeanValueScalar(f,mean);
    }
    template<typename Function>
    static Function greylevelTranslateMeanValueCast(const Function& f, typename Function::F mean,Loki::Type2Type<RGBUI8> )throw(pexception)
    {
        typename FunctionTypeTraitsSubstituteF<Function,typename TypeTraitsTypeScalar<RGBUI8>::Result>::Result r,g,b;
        Convertor::toRGB(f,r,g,b);
        typename TypeTraitsTypeScalar<RGBUI8>::Result rmean,gmean,bmean;
        rmean=mean.r();
        gmean=mean.g();
        bmean=mean.b();
        r = greylevelTranslateMeanValueScalar(r,rmean);
        g = greylevelTranslateMeanValueScalar(g,gmean);
        b = greylevelTranslateMeanValueScalar(b,bmean);
        Function h(f.getDomain());
        Convertor::fromRGB(r,g,b,h);
        return h;
    }
    template<typename Function>
    static Function greylevelTranslateMeanValueCast(const Function& f, typename Function::F mean,Loki::Type2Type<RGBF64> )throw(pexception)
    {
        typename FunctionTypeTraitsSubstituteF<Function,typename TypeTraitsTypeScalar<RGBF64>::Result>::Result r,g,b;
        Convertor::toRGB(f,r,g,b);
        typename TypeTraitsTypeScalar<RGBF64>::Result rmean,gmean,bmean;
        rmean=mean.r();
        gmean=mean.g();
        bmean=mean.b();
        r = greylevelTranslateMeanValueScalar(r,rmean);
        g = greylevelTranslateMeanValueScalar(g,gmean);
        b = greylevelTranslateMeanValueScalar(b,bmean);
        Function h(f.getDomain());
        Convertor::fromRGB(r,g,b,h);
        return h;
    }
    template<typename Function>
    static Function greylevelTranslateMeanValueScalar(const Function& f, typename Function::F mean )throw(pexception)
    {
        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        Mat2F64 m = AnalysisAdvanced::histogram(f,it);
        it.init();
        F64 pow_min=0;
        F64 pow_max=1000;
        F64 pow_current=1;
        F64 error_max=0.1;
        F64 error_current=1;
        int number=0;
        bool test=false;
        while(test==false){
            number++;
            DistributionExpression exp;
            std::string str = "(x/256)^("+BasicUtility::Any2String(pow_current)+")*256";
            exp.fromRegularExpression(str);
            F64 meantemp=0;
            for(unsigned int i=0;i<m.sizeI();i++){
                meantemp  +=  exp.operator ()(m(i,0))*m(i,1);
            }
            error_current = absolute(meantemp-mean);
            if(error_current<error_max)
                test =true;
            else{
                if(meantemp<mean)
                    pow_max = pow_current;
                else
                    pow_min  = pow_current;
                pow_current =(pow_max-pow_min)/2+pow_min;
                if(number>2000){
                    test =true;
                }
            }
        }
        it.init();
        //small covolution
        typedef typename FunctionTypeTraitsSubstituteF<Function,F64>::Result FunctionF64;
        FunctionF64 fd(f.getDomain());

        typename Function::E x=3;
        FunctionF64 kernel(x);
        for(int i = 0;i<FunctionF64::DIM;i++){
            x=1;
            x(i)=0;
            kernel(x)=0.05;
            x(i)=2;
            kernel(x)=0.05;
        }
        x=1;
        kernel(x)=1-0.05*FunctionF64::DIM*2;
        it.init();
        fd = ProcessingAdvanced::convolution(f,kernel,it,1);

        Function outcast(f.getDomain());
        DistributionExpression exp;
        std::string str = "(x/256)^("+BasicUtility::Any2String(pow_current)+")*256";
        exp.fromRegularExpression(str);
        Private::FunctorDistribution<typename Function::F,F64,false> func(exp);
        it.init();
        FunctionProcedureFunctorUnaryF(fd,func,it,outcast);
        return outcast;
    }
    template<typename Function,typename FunctionMask, typename Iterator>
    static Function mask(const Function & f,const FunctionMask & mask,  Iterator & it)throw(pexception)
    {
        Function h(f.getDomain());
        it.init();
        while(it.next())
        {
            if(mask(it.x())!=0)
                h(it.x())=f(it.x());
            else
                h(it.x())=0;
        }
        return h;
    }

    /*! \fn Function erosion(const Function & f,IteratorGlobal & itglobal, IteratorLocal & itlocal)throw(pexception)
     *  \brief Erosion of the input matrix
     * \param f input function
     * \param itglobal Global IteratorE
     * \param itlocal Local  IteratorE
     * \return h output function
     *
     *  Erosion of the input matrix:\n
     * \f$\forall x \in E':\quad h(x) =\min_{\forall x'\in N(x) }f(x) \f$ where the iteration trough  \f$\forall x \in E'\f$ is done by the global IteratorE
     * and the iteration trough  \f$\forall x'\in N(x)\f$  is done by the local  IteratorE
    */
    template< typename Function,typename IteratorGlobal, typename IteratorLocal >
    static Function erosion(const Function & f,IteratorGlobal & itglobal, IteratorLocal & itlocal)throw(pexception)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("Erosion",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Slow algorithm for large structural element");
        Function h(f.getDomain());
        typedef FunctorF::FunctorAccumulatorMin<typename Function::F > FunctorAccumulator;
        FunctorAccumulator funcAccumulator;
        FunctionProcedureLocal(f,itglobal,itlocal,funcAccumulator, h);
        CollectorExecutionInformationSingleton::getInstance()->endExecution("Erosion");
        return h;
    }
    /*! \fn Function dilation(const Function & f,IteratorGlobal & itglobal, IteratorLocal & itlocal )throw(pexception)
     *  \brief Dilation of the input matrix
     * \param f input function
     * \param itglobal Global IteratorE
     * \param itlocal Local  IteratorE
     * \return h output function
     *
     *  Dilation of the input matrix:\n
     * \f$\forall x \in E':\quad h(x) =\max_{\forall x'\in N(x) }f(x) \f$ where the iteration trough \f$\forall x \in E'\f$ is done by the global IteratorE
     * and the iteration trough  \f$\forall x'\in N(x)\f$  is done by the local  IteratorE
    */
    template< typename Function, typename IteratorGlobal,typename IteratorLocal>
    static Function dilation(const Function & f,IteratorGlobal & itglobal, IteratorLocal & itlocal )throw(pexception)
    {
        Function h(f.getDomain());
        CollectorExecutionInformationSingleton::getInstance()->startExecution("Dilation",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Slow algorithm for large structural element");
        typedef FunctorF::FunctorAccumulatorMax<typename Function::F > FunctorAccumulator;
        FunctorAccumulator funcAccumulator;
        FunctionProcedureLocal(f,itglobal,itlocal,funcAccumulator, h);
        CollectorExecutionInformationSingleton::getInstance()->endExecution("Dilation");
        return h;
    }
    /*! \fn Function closing(const Function & f,IteratorGlobal & itglobal, IteratorLocal & itlocal )throw(pexception)
     *  \brief Closing of the input matrix
     * \param f input function
     * \param itglobal Global IteratorE
     * \param itlocal Local  IteratorE
     * \param h output function
     *
     *  Closing of the input matrix:\n
     * \f$\forall x \in E':\quad h(x) =Erosion(Dilation(f,E',N),E',N) \f$ where the iteration trough \f$E'\f$ is done by the global IteratorE
     * and the iteration trough \f$N(x)\f$  is done by the local  IteratorE
    */
    template<typename Function,typename IteratorGlobal,typename IteratorLocal>
    static Function closing(const Function & f,IteratorGlobal & itglobal, IteratorLocal & itlocal )throw(pexception)
    {
        Function h(f.getDomain());
        CollectorExecutionInformationSingleton::getInstance()->startExecution("Closing",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Slow algorithm for large structural element");
        Function temp =ProcessingAdvanced::dilation(f,itglobal,itlocal);
        itglobal.init();
        h = ProcessingAdvanced::erosion(temp,itglobal,itlocal);
        CollectorExecutionInformationSingleton::getInstance()->endExecution("Closing");
        return h;
    }

    /*! \fn Function opening(const Function & f,IteratorGlobal & itglobal, IteratorLocal & itlocal)throw(pexception)
     *  \brief Opening of the input matrix
     * \param f input function
     * \param itglobal Global IteratorE
     * \param itlocal Local  IteratorE
     * \return h output function
     *
     *  Opening of the input matrix:\n
     * \f$\forall x \in E':\quad h(x) =Dilation(Erosion(f,E',N),E',N) \f$ where the iteration trough \f$E'\f$ is done by the global IteratorE
     * and the iteration trough \f$N(x)\f$  is done by the local  IteratorE
    */
    template<typename Function, typename IteratorGlobal,typename IteratorLocal >
    static Function opening(const Function & f,IteratorGlobal & itglobal, IteratorLocal & itlocal)throw(pexception)
    {
        Function h(f.getDomain());
        CollectorExecutionInformationSingleton::getInstance()->startExecution("Opening",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Slow algorithm for large structural element");
        Function temp =ProcessingAdvanced::erosion(f,itglobal,itlocal);
        itglobal.init();
        h = ProcessingAdvanced::dilation(temp,itglobal,itlocal);
        CollectorExecutionInformationSingleton::getInstance()->endExecution("Opening");
        return h;
    }

    /*! \fn Function median(const Function & f,IteratorGlobal & itglobal, IteratorLocal & itlocal)throw(pexception)
     *  \brief Median filter of the input matrix
     * \param f input function
     * \param itglobal Global IteratorE
     * \param itlocal Local  IteratorE
     * \return h output function
     *
     *  Median filter of the input matrix:\n
     * \f$\forall x \in E':\quad h(x) =\mbox{median}_{\forall x'\in N(x) }f(x) \f$ where the operator median returns the median value of the list of input values
     * , the iteration trough  \f$\forall x \in E'\f$ is done by the global IteratorE
     * and the iteration trough  \f$\forall x'\in N(x)\f$  is done by the local  IteratorE
    */
    template<typename Function,typename IteratorGlobal,typename IteratorLocal>
    static Function median(const Function & f,IteratorGlobal & itglobal, IteratorLocal & itlocal)throw(pexception)
    {
        Function h(f.getDomain());
        CollectorExecutionInformationSingleton::getInstance()->startExecution("Median",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Slow algorithm for large structural element");
        typedef FunctorF::FunctorAccumulatorMedian<typename Function::F> FunctorAccumulator;
        FunctorAccumulator funcAccumulator;
        FunctionProcedureLocal(f,itglobal,itlocal,funcAccumulator, h);
        CollectorExecutionInformationSingleton::getInstance()->endExecution("Median");
        return h;
    }

    /*! \fn Function mean(const Function & f,IteratorGlobal & itglobal, IteratorLocal & itlocal)throw(pexception)
     *  \brief Median filter of the input matrix
     * \param f input function
     * \param itglobal Global IteratorE
     * \param itlocal Local  IteratorE
     * \return h output function
     *
     *  Median filter of the input matrix:\n
     * \f$\forall x \in E':\quad h(x) =\mbox{mean}_{\forall x'\in N(x) }f(x) \f$ where the operator mean returns the mean value of the list of input values
     * , the iteration trough  \f$\forall x \in E'\f$ is done by the global IteratorE
     * and the iteration trough  \f$\forall x'\in N(x)\f$  is done by the local  IteratorE
    */
    template<typename Function,typename IteratorGlobal,typename IteratorLocal>
    static Function mean(const Function & f,IteratorGlobal & itglobal, IteratorLocal & itlocal)throw(pexception)
    {
        Function h(f.getDomain());
        CollectorExecutionInformationSingleton::getInstance()->startExecution("Mean",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Slow algorithm for large structural element");
        typedef FunctorF::FunctorAccumulatorMean<typename Function::F> FunctorAccumulator;
        FunctorAccumulator funcAccumulator;
        FunctionProcedureLocal(f,itglobal,itlocal,funcAccumulator, h);
        CollectorExecutionInformationSingleton::getInstance()->endExecution("Mean");
        return h;
    }


    /*! \fn Function alternateSequentialCOStructuralElement(const Function & f,IteratorGlobal & itglobal,IteratorLocal & itlocal, int maxradius)throw(pexception)
     *  \brief Sequential Alternate filter of the input matrix
     * \param f input function
     * \param itglobal Global IteratorE
     * \param itlocal initial structural element
     * \param maxradius max radius
     * \return h output function
     *
     *  Successive application of Closing and opening by increasing the scale factor of structure element until max radius
    */
    template<typename Function,typename IteratorGlobal,typename IteratorLocal >
    static Function alternateSequentialCO(const Function & f,IteratorGlobal & itglobal,IteratorLocal & itlocal, int maxradius)throw(pexception)
    {
        Function h(f.getDomain());
        CollectorExecutionInformationSingleton::getInstance()->startExecution("AlternateSequentialCOStructuralElement",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Slow algorithm for large structural element");

        IteratorLocal  itlocalsuccessive(itlocal);
        Function temp(f);
        h=f;
        for(int radius=1;radius<=maxradius;radius++){
            CollectorExecutionInformationSingleton::getInstance()->info("Radius : "+BasicUtility::Any2String(radius));
            itglobal.init();
            temp = ProcessingAdvanced::closing(h,itglobal,itlocalsuccessive);
            itglobal.init();
            h = ProcessingAdvanced::opening(temp,itglobal,itlocalsuccessive);
            itlocalsuccessive.dilate(itlocal);
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("AlternateSequentialCOStructuralElement");
        return h;

    }
    /*! \fn Function alternateSequentialOCStructuralElement(const Function & f,IteratorGlobal & itglobal,IteratorLocal & itlocal, int maxradius)throw(pexception)
     *  \brief Sequential Alternate filter of the input matrix
     * \param f input function
     * \param itglobal Global IteratorE
      * \param itlocal initial structural element
     * \param maxradius max radius
     * \return h output function
     *
     *  Successive application of opening and closing by increasing the scale factor of structure element until max radius
    */
    template< typename Function,typename IteratorGlobal,typename IteratorLocal>
    static Function alternateSequentialOC(const Function & f,IteratorGlobal & itglobal,IteratorLocal & itlocal, int maxradius)throw(pexception)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("AlternateSequentialOCStructuralElement",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Slow algorithm for large structural element");
        Function h(f.getDomain());

        IteratorLocal  itlocalsuccessive(itlocal);
        Function temp(f);
        h=f;
        for(int radius=1;radius<=maxradius;radius++){
            itglobal.init();
            temp = ProcessingAdvanced::opening(h,itglobal,itlocalsuccessive);
            itglobal.init();
            h = ProcessingAdvanced::closing(temp,itglobal,itlocalsuccessive);
            itlocalsuccessive.dilate(itlocal);
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("AlternateSequentialOCStructuralElement");
        return h;

    }

    /*! \fn static Function hitOrMiss(const Function & f,IteratorGlobal & itglobal,IteratorLocal & itC, IteratorLocal & itD)throw(pexception)
     *  \brief Hit or miss filter
     * \param f input function
     * \param itglobal Global IteratorE
     * \param itC  local iterator for the iteration through the set C
     * \param itD local iterator for the iteration through the set D
     * \return h output function
     *
     *  \f$ H = (X\ominus C)\cap (X^c\ominus D) \f$ with \f$\ominus\f$ the erosion, \f$X=\{x:f(x)\neq 0 \} \f$ and \f$H=\{x:h(x)\neq 0 \}\f$.\n
     * For instance a direct implementation of the thinning algorithm http://en.wikipedia.org/wiki/Hit-or-miss_transform is
     * \code
    Mat2UI8 img;
    img.load("/home/vincent/Desktop/Population/doc/image/outil.bmp");
    Processing processing;
    Mat2UI8 C_1(3,3),D_1(3,3);
    Mat2UI8 C_2(3,3),D_2(3,3);
    C_1(1,1)=255;C_1(0,0)=255;C_1(1,0)=255;C_1(2,0)=255;
    D_1(0,2)=255;D_1(1,2)=255;D_1(2,2)=255;
    C_2(0,1)=255;C_2(1,1)=255;C_2(0,0)=255;C_2(1,0)=255;
    D_2(1,2)=255;D_2(2,2)=255;D_2(2,1)=255;
    LinearAlgebra algebra;

    Mat2UI8 temp(img);
    Mat2UI8 temp2;
    int nbr_equal=0;
    while(nbr_equal<8){
        temp2 = processing.hitOrMiss(temp,C_1,D_1);
        temp = temp -temp2;
        C_1.rotate(algebra.generate2DRotation(PI/2),Vec2I32(1,1) );
        C_1 =processing.threshold(C_1,125);//due to the interpolation with the rotation, the value can fluctuate to remove these fluctuation, we apply a threshold
        D_1.rotate(algebra.generate2DRotation(PI/2),Vec2I32(1,1) );
        D_1 =processing.threshold(D_1,125);

        temp2 = processing.hitOrMiss(temp,C_2,D_2);
        temp = temp -temp2;
        C_2.rotate(algebra.generate2DRotation(PI/2),Vec2I32(1,1) );
        C_2 =processing.threshold(C_2,125);

        D_2.rotate(algebra.generate2DRotation(PI/2),Vec2I32(1,1) );
        D_2 =processing.threshold(D_2,125);
        if(temp==img){
            nbr_equal++;
        }else{
            nbr_equal=0;
        }
        img =temp;
    }
    img.display();
    \endcode
    */

    template< typename Function, typename IteratorGlobal,typename IteratorLocal >
    static Function hitOrMiss(const Function & f,IteratorGlobal & itglobal,IteratorLocal & itC, IteratorLocal & itD)throw(pexception)
    {
        Function h(f.getDomain());
        CollectorExecutionInformationSingleton::getInstance()->startExecution("HitOrMiss",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Slow algorithm for large structural element");

        Function erosion1(f.getDomain());
        itglobal.init();
        //        std::cout<<f<<std::endl;
        erosion1 =ProcessingAdvanced::erosion(f,itglobal,itC);
        //        std::cout<<erosion1<<std::endl;
        //        getchar();
        Function temp(f.getDomain(),NumericLimits<typename Function::F>::maximumRange());
        temp =   temp-f;
        //        std::cout<<"TEMP"<<std::endl;
        //        std::cout<<temp<<std::endl;
        Function erosion2(f.getDomain());
        itglobal.init();
        erosion2 = ProcessingAdvanced::erosion(temp,itglobal,itD);
        //        std::cout<<erosion2<<std::endl;
        //        getchar();
        h=minimum(erosion1,erosion2);
        //        std::cout<<h<<std::endl;
        //        getchar();
        CollectorExecutionInformationSingleton::getInstance()->endExecution("HitOrMiss");
        return h;
    }
    /*! \fn Function1 convolution(const Function1 & f, const Function2 & kernel,Iterator itglobal,int boundarycondition)
     *  \brief convolution of f with the kernel
     * \param f input function
     * \param kenel input kernel
     * \param itglobal Global IteratorE
     * \param boundarycondition  0=bounded, 1, mirror
     * \return h output function
     *
     *
     * \code
     *
     *  \endcode
    */
    template<typename Function1,typename Function2,typename Iterator>
    static Function1 convolution(const Function1 & f, const Function2 & kernel,Iterator itglobal,int boundarycondition)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("convolution",NOINFO);
        Function1 h(f.getDomain());
        typedef typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64>::Result Type_F64;
        typename Function1::IteratorEDomain itlocal(kernel.getIteratorEDomain());
        typename Function1::E center = (kernel.getDomain()-1)/2;
        while(itglobal.next())
        {
            itlocal.init();
            Type_F64 value(0);
            while(itlocal.next())
            {
                typename Function1::E x = itglobal.x()-itlocal.x()+center;
                if( (boundarycondition==0&& BoundaryCondition::bounded(f.getDomain(),x)) || (boundarycondition==1&&BoundaryCondition::mirror(f.getDomain(),x))  ){
                    Type_F64 v(f(x));
                    Type_F64 v2(kernel(itlocal.x()));
                    value+=v*v2;
                }
            }
            h(itglobal.x())=ArithmeticsSaturation<typename Function1::F,Type_F64>::Range (value);
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("convolution");
        return h;
    }

    template <I32 Begin, I32 End> struct LoopproductTensoriel
    {
        template<class Function>
        static  typename FunctionTypeTraitsSubstituteDIM<Function,End-Begin+1>::Result CreateDecomposableDirectionnelKernel(I32 d, const Function & smooth,const Function & der)
        {
            if(Begin==d)return productTensoriel(der,LoopproductTensoriel<Begin+1,End>::CreateDecomposableDirectionnelKernel(d,smooth,der));
            else return productTensoriel(smooth,LoopproductTensoriel<Begin+1,End>::CreateDecomposableDirectionnelKernel(d,smooth,der));
        }
        template<class Function>
        static  typename FunctionTypeTraitsSubstituteDIM<Function,End-Begin+1>::Result CreateDecomposableIsotropeKernel( const Function & unary)
        {
            return productTensoriel(unary,LoopproductTensoriel<Begin+1,End>::CreateDecomposableDirectionnelKernel(unary));

        }
    };
    template <I32 N> struct LoopproductTensoriel<N,N>
    {

        template<class Function>
        static  typename FunctionTypeTraitsSubstituteDIM<Function,1>::Result CreateDecomposableDirectionnelKernel(I32 d, const Function & smooth,const Function & der)
        {
            if(N==d)return der;
            else return smooth;
        }
        template<class Function>
        static  typename FunctionTypeTraitsSubstituteDIM<Function,1>::Result CreateDecomposableIsotropeKernel(const Function & unary)
        {
            return unary;
        }
    };

    template<class Function1,typename Iterator>
    static Function1 gradSobel(const Function1 & f, I32 direction, Iterator it)
    {
        typedef typename FunctionTypeTraitsSubstituteF<Function1,F64>::Result FunctionKernel;
        typedef typename FunctionTypeTraitsSubstituteDIM<FunctionKernel,1>::Result FunctionUnary;
        typename FunctionUnary::E x; x(0)=3;
        FunctionUnary der (x),smooth(x);
        x(0)=0;der(x)=-1;smooth(x)=0.5;
        x(0)=1;der(x)=0; smooth(x)=1;
        x(0)=2;der(x)=1; smooth(x)=0.5;//der=(-1,0,1) and smooth=(0.5,1,0.5)
        FunctionKernel kernel = LoopproductTensoriel<0,Function1::DIM-1>::CreateDecomposableDirectionnelKernel(direction,smooth,der);
        return convolution(f,kernel,it,1);
    }
    template<class Function1,typename Iterator>
    static Function1 gradNormSobel(const Function1 & f, Iterator it)
    {
        typedef typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64>::Result Type_F64;
        typename FunctionTypeTraitsSubstituteF<Function1,Type_F64>::Result ffloat(f);
        typename FunctionTypeTraitsSubstituteF<Function1,Type_F64>::Result fdir(f.getDomain());
        typename FunctionTypeTraitsSubstituteF<Function1,Type_F64>::Result fsum(f.getDomain());

        for(I32 i=0;i <Function1::DIM;i++)
        {
            it.init();
            fdir = ProcessingAdvanced::gradSobel(ffloat,i,it);
            fsum+= fdir.multTermByTerm(fdir);
        }
        Function1 grad(f.getDomain());
        it.init();
        while(it.next())
        {
            Type_F64 value = std::sqrt(static_cast<double>(normValue(fsum(it.x()))));
            grad(it.x())=ArithmeticsSaturation<typename Function1::F,Type_F64>::Range(value);
        }
        return grad;
    }
    template<class Function1,typename Iterator>
    static Function1 gradGaussian(const Function1 & f, I32 direction,F64 sigma, int size_kernel, Iterator it)
    {
        FunctorPDE::GaussianDerivateKernel<Function1> d(f,direction,sigma,size_kernel,true);
        return d.iterate(f,it);
    }
    template<class Function1,typename Iterator>
    static Function1 smoothGaussian(const Function1 & f, F64 sigma, int size_kernel,Iterator it)
    {
        FunctorPDE::GaussianKernel<Function1> d(f,sigma,size_kernel,true);
        return d.iterate(f,it);
    }
    template<class Function1,typename Iterator>
    static  Function1 gradNormGaussian(const Function1 & f, F64 sigma,int size_kernel,Iterator it)
    {
        typedef typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64>::Result Type_F64;
        typename FunctionTypeTraitsSubstituteF<Function1,Type_F64>::Result ffloat(f);
        typename FunctionTypeTraitsSubstituteF<Function1,Type_F64>::Result fdir(f.getDomain());
        typename FunctionTypeTraitsSubstituteF<Function1,Type_F64>::Result fsum(f.getDomain());

        for(I32 i=0;i <Function1::DIM;i++)
        {
            it.init();
            fdir = ProcessingAdvanced::gradGaussian(ffloat,i,sigma,size_kernel,it);
            fsum+= fdir.multTermByTerm(fdir);
        }
        Function1 g(f.getDomain());
        it.init();
        while(it.next())
        {
            g(it.x())=ArithmeticsSaturation<typename Function1::F,Type_F64>::Range(squareRoot(fsum(it.x())));
        }
        return g;
    }
    template<class Function1>
    static typename FunctionTypeTraitsSubstituteF<Function1,VecN<Function1::DIM,F64> >::Result gradientVecGaussian(const Function1  & f,double sigma=1)
    {
        typedef typename FunctionTypeTraitsSubstituteF<Function1,F64 >::Result  FunctionFloat;
        VecN<Function1::DIM,FunctionFloat> v_der;
        for(int i =0;i<Function1::DIM;i++){
            v_der[i]= ProcessingAdvanced::gradGaussian(f,i,sigma,3*sigma,f.getIteratorEDomain());
        }
        typename FunctionTypeTraitsSubstituteF<Function1,VecN<Function1::DIM,F64> >::Result f_grad(f.getDomain());
        Convertor::fromVecN(v_der,f_grad);
        return f_grad;
    }
    /*! \fn Function1    recursive(const Function1 & f,IteratorEOrder & it, FunctorRecursive & func)
     *  \brief recursive filter
     * \param f input function
     * \param it order iterator
     * \param func recursive functor func
     * \return h output function
     *
     *
     *  \code
     * Mat2UI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
     *
     * //define the alpha-functor
     * double alpha =0.01;
     * F64 a0= alpha;
     * F64 a1=  0;
     * F64 b1= (1-alpha);
     * F64 a0_border0 = 1;
     * FunctorF::FunctorRecursiveOrder1 func(a0,a1,b1,a0_border0);


     * //define the iterator in x-direction by increasing the index
     * Mat2RGBUI8::IteratorEOrder itorder (img.getIteratorEOrder());
     * itorder.setLastLoop(0);
     * itorder.setDirection(1);

     * //convert the input matrix to float pixel
     * Mat2F64 img_float(img);
     * ProcessingAdvanced processing;
     * img = processing.recursive(img_float,itorder,func);
     * img.display();
     *  \endcode
    */
    template< typename Function1, typename IteratorEOrder, typename FunctorRecursive>
    static Function1    recursive(const Function1 & f,IteratorEOrder & it, FunctorRecursive & func)
    {
        Function1 h(f.getDomain());
        while(it.next()){
            h(it.x())=func(f,h,it.x(),it.getBordeLenghtLastLoop(),it.getIndexLastLoop(),it.getWayLastLoop());
        }
        return h;
    }
    /*! \fn static Function labelMerge(const Function & label1, const Function &label2, Iterator & it)throw(pexception)
     *  \brief merge of the labelled matrix
     * \param label1 labelled matrix1
     * \param label1 labelled matrix2
     * \param it order iterator
     * \return h output labelled function
     *
     * Operate the merge of the two input labelled matrixs that can contain multi-labels
     *  \code
     * Mat2UI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/iex.pgm");
     * //filtering
     * img = pop::Processing::median(img,4);
     * //seed localisation
     * Mat2UI8 seed1 = pop::Processing::threshold(img,0,100);//seed in the grains
     * Mat2UI8 seed2 = pop::Processing::threshold(img,160);//seed in the background
     * Mat2UI8 seeds = pop::Processing::labelMerge(seed1,seed2);//merge of the seeds
     * //for a good segmentation, each seed should be include in its associated object and touch component of its associated object
     * //Test of the condition with a visal checking
     * Mat2RGBUI8 RGB = pop::Visualization::labelForeground(seed1,img);
     * RGB.display();
     *  \endcode
    */
    template<typename Function,typename Iterator>
    static Function labelMerge(const Function & label1, const Function & label2, Iterator & it)throw(pexception)
    {
        FunctionAssert(label1,label2,"pop::Processing::labelMerge");
        Function h(label1.getDomain());
        std::vector<typename Function::F> vf;
        std::vector<typename Function::F> vg;
        while(it.next())
        {
            if(label1(it.x())!=0)
            {
                typename Function::F valuef= label1(it.x());
                if( find(vf.begin(),vf.end(),valuef)==vf.end() )vf.push_back(valuef);
            }
            if(label2(it.x())!=0)
            {
                typename Function::F valueg= label2(it.x());
                if( find(vg.begin(),vg.end(),valueg)==vg.end() )vg.push_back(valueg);
            }
        }
        std::sort (vf.begin(), vf.end());
        std::sort (vg.begin(), vg.end());
        if(vf.size()+vg.size()>=NumericLimits<typename Function::F>::maximumRange())
        {
            throw(pexception("In pop::Processing::labelMerge, we have more labels than the grey-level range. Convert your labels images with the type Mat2UI32 before to call this algorithm (Mat2UI32 label=your_image_label"));
        }
        typename std::vector<typename Function::F>::iterator  itvalue;
        it.init();
        while(it.next())
        {
            if(label1(it.x())!=0)
            {
                typename Function::F value= label1(it.x());
                itvalue=find(vf.begin(),vf.end(),value);
                value=static_cast<I32>(itvalue-vf.begin())+1;
                h(it.x())=value;
            } else if(label2(it.x())!=0)
            {
                typename Function::F value= label2(it.x());
                itvalue=find(vg.begin(),vg.end(),value);
                value=static_cast<I32>(itvalue-vg.begin())+1+static_cast<I32>(vf.size());
                h(it.x())=value;
            }
            else h(it.x())=0;
        }
        return h;
    }
    /*! \fn static Function2 labelFromSingleSeed(const Function1 & label,const Function2& seed, Iterator & it)throw(pexception)
     *  \brief extract the label including the binary seed
     * \param label multi-labelled matrix
     * \param seed binary seed
     * \param it order iterator
     * \return h output binary matrix function
     *
     * From the multi-labelled matrix, we extract the label including the seed
     *  \code
     * Mat2UI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/iex.pgm");
     * //filtering
     * img = pop::Processing::median(img,4);
     * //seed localisation
     * Mat2UI8 seed1 = pop::Processing::threshold(img,0,100);//seed in the grains
     * Mat2UI8 seed2 = pop::Processing::threshold(img,160);//seed in the background
     * Mat2UI8 seeds = pop::Processing::labelMerge(seed1,seed2);//merge of the seeds
     * //for a good segmentation, each seed should be include in its associated object (here the grains and the background) and touch each component of its associated object
     * //Test of the condition with a visal checking
     * Mat2RGBUI8 RGB = pop::Visualization::labelForeground(seed1,img);
     * RGB.display();

     *  //topographic surface is the magnitude gradient of the input matrix
     * Mat2UI8 topo = pop::Processing::gradientMagnitudeDeriche(img,0.5);
     * //watershed as region growing on the topographic surface with seeds
     * Mat2UI8 regions = pop::Processing::watershed(seeds,topo);


     * //test the agreement between visual segmentation and numerical one
     * RGB = pop::Visualization::labelForeground(regions,img);
     * RGB.display();

     * //Extract the grain label
     * Mat2UI8 grain = pop::Processing::labelFromSingleSeed(regions,seeds);
     * grain.display();
     *  \endcode
    */

    template<typename Function1,typename Iterator,typename Function2>
    static Function2 labelFromSingleSeed(const Function1 & label,const Function2& seed, Iterator & it)throw(pexception)
    {
        FunctionAssert(label,seed,"pop::Processing::labelFromSingleSeed");
        Function2 h(seed.getDomain());
        it.init();
        typename Function2::F value(0);
        while(it.next())
        {
            if(seed(it.x())!=0) value = label(it.x());
        }
        it.init();
        while(it.next())
        {
            if(label(it.x())==value) h(it.x()) = NumericLimits<typename Function2::F>::maximumRange();
            else h(it.x())=0;
        }
        return h;
    }

    /*! \fn FunctionBinary holeFilling( const FunctionBinary& bin,typename FunctionBinary::IteratorENeighborhood itneigh)
     * \param bin input binary matrix
     * \param itneigh domain of the neighborhood iterator
     * \return hole output matrix
     *
     *  hole filling of the input binary matrix
    */

    template<typename FunctionBinary>
    static FunctionBinary holeFilling( const FunctionBinary& bin,typename FunctionBinary::IteratorENeighborhood itneigh)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("HoleFilling",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        FunctorZero f;
        Population<FunctionBinary,FunctorZero> pop(bin.getDomain(),f,itneigh);
        typename FunctionBinary::IteratorEDomain it(bin.getIteratorEDomain());
        it.init();
        while(it.next()){
            for(int i = 0; i<FunctionBinary::DIM;i++){
                if(it.x()(i)==0||it.x()(i)==bin.getDomain()(i)-1){
                    if(bin(it.x())==0)
                        pop.growth(0,it.x());
                }
            }
            if(bin(it.x())!=0){
                pop.setRegion(1,it.x());
            }
        }
        while(pop.next()){
            pop.growth(pop.x().first,pop.x().second);
        }
        FunctionBinary hole(bin.getDomain());
        it.init();
        while(it.next()){
            if(pop.getRegion()(it.x())==0)
                hole(it.x())=0;
            else
                hole(it.x())=255;
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("HoleFilling");
        return hole;
    }
    /*! \fn FunctionLabel regionGrowingAdamsBischof(FunctionLabel & seed ,typename FunctionLabel::IteratorENeighborhood itneigh,Functor & func )
     * \param seed seeds
     * \param itneigh domain of the neighborhood iterator
     * \param func ordering attribute function
     * \return the regions
     *
     *  Classical algorithm  of Adams and Bischof such that the ordering attribute function is the input functor
     * \sa regionGrowingAdamsBischofMeanOverStandardDeviation regionGrowingAdamsBischofMean
    */

    template<typename FunctionLabel, typename Functor>
    static FunctionLabel regionGrowingAdamsBischof(const FunctionLabel & seed ,typename FunctionLabel::IteratorENeighborhood itneigh,Functor & func )
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("regionGrowing",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        Population<FunctionLabel,Functor, RestrictedSetWithoutALL,SQFIFONextSmallestLevel,Growth> pop(seed.getDomain(),func,itneigh);
        typename FunctionLabel::IteratorEDomain it(seed.getDomain());

        while(it.next()){
            if(seed(it.x())!=0){
                pop.growth(seed(it.x()),it.x());
            }
        }
        while(pop.next()){
            pop.growth(pop.x().first,pop.x().second);
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("regionGrowing");
        return pop.getRegion();
    }
    /*! \fn FunctionLabel regionGrowingAdamsBischofMeanOverStandardDeviation(const FunctionLabel & seed,const FunctionTopo & topo, typename FunctionTopo::IteratorENeighborhood  itneigh )
     * \param seed input seeds
     * \param topo topographic surface
     * \param itneigh neighborhood IteratorE domain
     * \return  regions
     *
     *  Classical region growing algorithm  of Adams and Bischof such that the ordering attribute function is:\n
     *  \f$ \delta(x,i) = \frac{|f(x)- \mu_i|}{\sigma_i}\f$   where
     *  with f the topograhic surface, X_i the region, \f$\mu_i\f$ the mean value inside the seed and \f$\sigma_i\f$ is the standard deviation \f$\sqrt[]{\frac{\sum_{y\in X_i}(f(y)-\mu_i)^2}{\sum_{x\in X_i}1}} \f$
     *
     *     * \code
     * Mat2UI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/iex.pgm");
     * //filtering
     * img = pop::Processing::median(img,3);
     * //seed localisation
     * Mat2UI8 seed1 = pop::Processing::threshold(img,0,100);//seed in the grains
     * Mat2UI8 seed2 = pop::Processing::threshold(img,160);//seed in the background
     * Mat2UI8 seeds = pop::Processing::labelMerge(seed1,seed2);//merge of the seeds
     * //for a good segmentation, each seed should be include in its associated object (here the grains and the background) and touch each component of its associated object
     * //Test of the condition with a visal checking
     * Mat2RGBUI8 RGB = pop::Visualization::labelForeground(seed1,img);
     * RGB.display();
     * //region growing on the topographic surface with seeds
     * Mat2UI8 regions = pop::Processing::regionGrowingAdamsBischofMeanOverStandardDeviation(seeds,img);
     * //test the agreement between visual segmentation and numerical one
     * RGB = pop::Visualization::labelForeground(regions,img);
     * RGB.display();
     * \endcode
     *
    */
    template<typename FunctionTopo,typename FunctionLabel >
    static FunctionLabel regionGrowingAdamsBischofMeanOverStandardDeviation(const FunctionLabel & seed,const FunctionTopo & topo, typename FunctionTopo::IteratorENeighborhood  itneigh )
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("AdamsBischof",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        std::vector<FunctorStatistic<typename FunctionTopo::F> > vstattopo;
        typename FunctionTopo::IteratorEDomain it(topo.getIteratorEDomain());
        while(it.next()){
            if(seed(it.x())!=0){
                if(seed(it.x())>=(typename FunctionLabel::F)vstattopo.size())vstattopo.resize(seed(it.x())+1);
                vstattopo[seed(it.x())](topo,it.x());
            }
        }
        std::vector<std::pair<typename FunctionTopo::F,typename FunctionTopo::F> > vtopomeanvariance;
        for(I32 i =0;i<(I32)vstattopo.size();i++){
            vtopomeanvariance.push_back(std::make_pair(static_cast<typename FunctionTopo::F>(vstattopo[i].mean()),std::sqrt(static_cast<double>(vstattopo[i].variance()))));
        }
        FunctorIntensityMax<FunctionTopo> functopo(topo,vtopomeanvariance);
        FunctionLabel region =regionGrowingAdamsBischof(seed,itneigh,functopo);
        CollectorExecutionInformationSingleton::getInstance()->endExecution("AdamsBischof");
        return region;
    }

    /*! \fn FunctionLabel regionGrowingAdamsBischofMean(const FunctionLabel & seed, const FunctionTopo & topo, typename FunctionTopo::IteratorENeighborhood  itneigh )

     * \param seed input seeds
     * \param topo topographic surface
     * \param itneigh neighborhood IteratorE domain
      * \return  regions
     *
     *  Classical region growing algorithm  of Adams and Bischof such that the ordering attribute function is:\n
     *  \f$ \delta(x,i) = |f(x)- \mu_i|\f$\n
     *  with f the topograhic surface, X_i the region and \f$\mu_i\f$ the mean value inside the seed
     *
     * \code
     * Mat2UI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/iex.pgm");
     * //filtering
     * img = pop::Processing::median(img,3);
     * //seed localisation
     * Mat2UI8 seed1 = pop::Processing::threshold(img,0,100);//seed in the grains
     * Mat2UI8 seed2 = pop::Processing::threshold(img,160);//seed in the background
     * Mat2UI8 seeds = pop::Processing::labelMerge(seed1,seed2);//merge of the seeds
     * //for a good segmentation, each seed should be include in its associated object (here the grains and the background) and touch each component of its associated object
     * //Test of the condition with a visal checking
     * Mat2RGBUI8 RGB = pop::Visualization::labelForeground(seed1,img);
     * RGB.display();
     * //region growing on the topographic surface with seeds
     * Mat2UI8 regions = pop::Processing::regionGrowingAdamsBischofMean(seeds,img);
     * //test the agreement between visual segmentation and numerical one
     * RGB = pop::Visualization::labelForeground(regions,img);
     * RGB.display();
     * \endcode
    */
    template< typename FunctionTopo,typename FunctionLabel>
    static FunctionLabel regionGrowingAdamsBischofMean(const FunctionLabel & seed, const FunctionTopo & topo, typename FunctionTopo::IteratorENeighborhood  itneigh )
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("AdamsBischofMean",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        std::vector<FunctorStatistic<typename FunctionTopo::F> > vstattopo;
        typename FunctionTopo::IteratorEDomain it(topo.getIteratorEDomain());
        while(it.next()){
            if(seed(it.x())!=0){
                if(seed(it.x())>=(typename FunctionLabel::F)vstattopo.size())
                    vstattopo.resize(seed(it.x())+1);
                vstattopo[seed(it.x())](topo,it.x());
            }
        }
        std::vector<typename FunctionTopo::F > vtopomean;
        for(I32 i =0;i<(I32)vstattopo.size();i++){
            vtopomean.push_back(vstattopo[i].mean());
        }
        FunctorMeanAdams<FunctionTopo> functopo(topo,vtopomean);
        FunctionLabel region = regionGrowingAdamsBischof(seed,itneigh,functopo);
        CollectorExecutionInformationSingleton::getInstance()->endExecution("AdamsBischofMean");
        return region;
    }
    /*! 

     * \param cluster input binary matrix
     * \param itneigh neighborhood IteratorE 
	 * \param itneigh domain IteratorE 
     * \return  label output label matrix
     *
     *  Each cluster of the input binary matrix has a specific label in the output label matrix
     *  Mat2UI8 img;
     *  img.load("/home/vincent/Desktop/Population/doc/matrix/outil.bmp");
     *  img.display();
     *  Mat2UI32 label = ProcessingAdvanced::clusterToLabel(img,img.getIteratorENeighborhood(),img.getIteratorEDomain());
     *  Mat2RGBUI8 RGB = pop::Visualization::label2RGB(label);
     *  RGB.display();
    */
    template<typename FunctionBinary,typename IteratorE>
    static typename FunctionTypeTraitsSubstituteF<FunctionBinary,UI32>::Result clusterToLabel(const FunctionBinary & cluster, typename FunctionBinary::IteratorENeighborhood  itneigh,IteratorE it)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("clusterToLabel",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        typedef typename FunctionTypeTraitsSubstituteF<FunctionBinary,UI32>::Result FunctionLabel;
        FunctorZero f;
        Population<FunctionLabel,FunctorZero> pop(cluster.getDomain(),f,itneigh);
        it.init();
        while(it.next()){
            if(cluster(it.x())==0)
                pop.setRegion(0,it.x());
        }
        typename FunctionLabel::F i=0;
        pop.setLevel(0);

        it.init();
        while(it.next()){
            if(pop.getRegion()(it.x())==pop.getLabelNoRegion()){
                i++;

                pop.growth(i,it.x());
                while(pop.next()){
                    pop.growth(i,pop.x().second);
                }
            }
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("clusterToLabel");
        return pop.getRegion();
    }
    /*! \fn FunctionBinary clusterMax(const FunctionBinary & bin, typename FunctionBinary::IteratorENeighborhood  itneigh)
     * \param bin input binary matrix
     * \param itneigh neighborhood IteratorE domain
      *\return  max cluster
     *
     *  The ouput matrix is the max cluster of the input binary matrix
    */
    template<typename FunctionBinary>
    static FunctionBinary clusterMax(const FunctionBinary & bin, typename FunctionBinary::IteratorENeighborhood  itneigh)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("ClusterMax",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        typename FunctionTypeTraitsSubstituteF<FunctionBinary,UI32 >::Result label(bin.getDomain());
        label = ProcessingAdvanced::clusterToLabel(bin,itneigh,bin.getIteratorEDomain());
        typename FunctionBinary::IteratorEDomain it(bin.getIteratorEDomain());
        std::vector<UI32> occurence;
        while(it.next())
        {

            if(label(it.x())>=(UI32)occurence.size())occurence.resize(label(it.x())+1,0);
            if(label(it.x())!=0)
                occurence[label(it.x())]++;
        }

        UI32 maxoccurence=0;
        UI32 maxlabel=-1;
        for(I32 i=1;i<(I32)occurence.size();i++)
        {
            if(maxoccurence<occurence[i])
            {
                maxlabel=i;
                maxoccurence=occurence[i];
            }
        }
        FunctionBinary clustermax(bin.getDomain());
        it.init();
        while(it.next())
        {
            if(label(it.x())==maxlabel)
                clustermax(it.x())=NumericLimits<UI8>::maximumRange();
            else
                clustermax(it.x())=0;
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("ClusterMax");
        return clustermax;
    }

    /*! \fn void FunctionProcedureMinimaRegional(const FunctionTopo & topo, typename FunctionTopo::IteratorENeighborhood  itneigh , FunctionLabel & minima)
     * \param topo input topographic surface
     * \param itneigh neighborhood IteratorE domain
      *\param  minima labelled minima
     *
     *  The labelled minima is the minima of the input binary matrix such that each minumum has a specific label
    */

    template< typename FunctionTopo>
    static typename FunctionTypeTraitsSubstituteF<FunctionTopo,UI32>::Result minimaRegional(const FunctionTopo & topo, typename FunctionTopo::IteratorENeighborhood  itneigh)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("MinimaRegional",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");

        FunctorZero functortopo;
        typedef typename FunctionTypeTraitsSubstituteF<FunctionTopo,UI32>::Result FunctionLabel;
        Population<FunctionLabel,FunctorZero, RestrictedSetWithMySelf> pop(topo.getDomain(),functortopo,itneigh);
        typename FunctionLabel::F labelminima=0;
        std::vector<bool> globalminima(1,true);//the label 0 is for the blank region
        pop.setLevel(0);
        typename FunctionTopo::IteratorEDomain it(topo.getIteratorEDomain());
        while(it.next()){
            if(pop.getRegion()(it.x())==pop.getLabelNoRegion()){
                labelminima++;
                pop.growth(labelminima,it.x());
                globalminima.push_back(true);
                typename FunctionTopo::F value = topo(it.x());
                while(pop.next()){
                    typename FunctionTopo::F temp = topo(pop.x().second);
                    if(temp==value)
                        pop.growth(pop.x().first,pop.x().second);
                    else
                    {
                        pop.pop();
                        if(temp<value)
                            *(globalminima.rbegin())=false;
                    }
                }
            }
        }
        it.init();
        while(it.next()){
            if(globalminima[pop.getRegion()(it.x())]==false  )
                pop.setRegion(0,it.x());//set blank region
        }

        CollectorExecutionInformationSingleton::getInstance()->endExecution("MinimaRegional");
        it.init();
        return ProcessingAdvanced::greylevelRemoveEmptyValue(pop.getRegion(),  it);
    }

    /*! \fn FunctionLabel watershed(const FunctionLabel & seed, const FunctionTopo & topo, typename FunctionTopo::IteratorENeighborhood itneigh )
      * \param seed input seed
     * \param topo input topographic surface
     * \param itneigh neighborhood IteratorE domain
      *\return  basins of the watershed transformation
     *
     *  Watershed transformation on the topographic surface initialiased by the seeds withoutboundary
    */

    template< typename FunctionLabel,typename FunctionTopo>
    static FunctionLabel watershed(const FunctionLabel & seed, const FunctionTopo & topo, typename FunctionTopo::IteratorENeighborhood itneigh )
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("Watershed",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        FunctorTopography<FunctionTopo  > functortopo(topo);
        Population<FunctionLabel,FunctorTopography<FunctionTopo>, RestrictedSetWithoutALL,SQFIFO,Growth> pop(topo.getDomain(),functortopo,itneigh);
        typename FunctionTopo::IteratorEDomain it(topo.getDomain());
        while(it.next()){
            if(seed(it.x())!=0){
                pop.setRegion(seed(it.x()),it.x());
            }
        }
        it.init();
        while(it.next()){
            if(seed(it.x())!=0){
                pop.growth(seed(it.x()),it.x());
            }
        }
        for(I32 i=0;i<functortopo.nbrLevel();i++)
        {
            pop.setLevel(i);
            functortopo.setLevel(i);
            while(pop.next())
            {
                pop.growth(pop.x().first,pop.x().second);
            }
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("Watershed");
        return pop.getRegion();
    }
    /*! \fn FunctionLabel watershed(const FunctionLabel & seed, const FunctionTopo & topo, const FunctionMask & mask , typename FunctionTopo::IteratorENeighborhood itneigh )

      * \param seed input seed matrix
     * \param topo input topographic surface
     * \param mask mask restricted the region growing
     * \param itneigh neighborhood IteratorE domain
      *\return  basins of the watershed transformation
     *
     *  Watershed transformation on the topographic surface initialiased by the seeds restricted by the mask
    */


    template<
            typename FunctionLabel,
            typename FunctionTopo,
            typename FunctionMask
            >
    static FunctionLabel watershed(const FunctionLabel & seed, const FunctionTopo & topo, const FunctionMask & mask , typename FunctionTopo::IteratorENeighborhood itneigh )
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("Watershed",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        FunctorTopography<FunctionTopo  > functortopo(topo);
        Population<FunctionLabel,FunctorTopography<FunctionTopo>, RestrictedSetWithoutALL,SQFIFO,Growth> pop(topo.getDomain(),functortopo,itneigh);
        typename FunctionTopo::IteratorEDomain it(topo.getDomain());
        while(it.next()){
            if(mask(it.x())==0){
                pop.setRegion(0,it.x());
            }
            else if(seed(it.x())!=0){
                pop.setRegion(seed(it.x()),it.x());
            }
        }
        it.init();
        while(it.next()){
            if(seed(it.x())!=0&&mask(it.x())!=0){
                pop.growth(seed(it.x()),it.x());
            }
        }
        for(I32 i=0;i<functortopo.nbrLevel();i++)
        {
            pop.setLevel(i);
            functortopo.setLevel(i);
            while(pop.next())
            {
                pop.growth(pop.x().first,pop.x().second);
            }
        }
        it.init();
        while(it.next()){
            if(pop.getRegion()(it.x())==pop.getLabelNoRegion()){
                pop.setRegion(0,it.x());
            }
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("Watershed");
        return pop.getRegion();
    }
    /*! \fn FunctionLabel watershedBoundary(const FunctionLabel & seed, const FunctionTopo & topo, typename FunctionTopo::IteratorENeighborhood itneigh )
      * \param seed input seed
     * \param topo input topographic surface
     * \param itneigh neighborhood IteratorE domain
      *\return  basins of the watershed transformation
     *
     *  Watershed transformation on the topographic surface initialiased by the seeds with a boundary region to separate the basins
    */

    template<
            typename FunctionTopo,
            typename FunctionLabel
            >
    static FunctionLabel watershedBoundary(const FunctionLabel & seed, const FunctionTopo & topo, typename FunctionTopo::IteratorENeighborhood itneigh )
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("WatershedBoundary",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        FunctorTopography<FunctionTopo  > functortopo(topo);
        Population<FunctionLabel,FunctorTopography<FunctionTopo>, RestrictedSetWithoutSuperiorLabel> pop(topo.getDomain(),functortopo,itneigh);

        typename FunctionTopo::IteratorEDomain it(topo.getDomain());
        while(it.next()){
            if(seed(it.x())!=0){
                pop.setRegion(seed(it.x()),it.x());
            }
        }
        it.init();
        while(it.next()){
            if(seed(it.x())!=0){
                pop.growth(seed(it.x()),it.x());
            }
        }

        for(I32 i=0;i<functortopo.nbrLevel();i++)
        {
            pop.setLevel(i);
            functortopo.setLevel(i);
            while(pop.next())
            {
                if(pop.getRegion()(pop.x().second)==pop.getLabelNoRegion())
                {
                    pop.growth(pop.x().first,pop.x().second);
                }
                else{
                    pop.setRegion(0,pop.x().second);
                }
            }
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("WatershedBoundary");
        return pop.getRegion();
    }

    /*! \fn FunctionLabel watershedBoundary(const FunctionLabel & seed,const FunctionTopo & topo,const FunctionMask & mask, typename FunctionTopo::IteratorENeighborhood itneigh )
      * \param seed input seed matrix
     * \param topo input topographic surface
     * \param mask mask restricted the region growing
     * \param itneigh neighborhood IteratorE domain
      *\return  basins of the watershed transformation
     *
     *  Watershed transformation on the topographic surface initialiased by the seeds restricted by the mask with a boundary region to separate the basins
    */

    template<
            typename FunctionTopo,
            typename FunctionLabel,
            typename FunctionMask
            >
    static FunctionLabel watershedBoundary(const FunctionLabel & seed,const FunctionTopo & topo,const FunctionMask & mask, typename FunctionTopo::IteratorENeighborhood itneigh )
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("WatershedBoundary",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        FunctorTopography<FunctionTopo  > functortopo(topo);
        Population<FunctionLabel,FunctorTopography<FunctionTopo>, RestrictedSetWithoutSuperiorLabel> pop(topo.getDomain(),functortopo,itneigh);
        typename FunctionTopo::IteratorEDomain it(topo.getIteratorEDomain());
        while(it.next()){
            if(mask(it.x())==0){
                pop.setRegion(0,it.x());
            }
            else if(seed(it.x())!=0){
                pop.setRegion(seed(it.x())+1,it.x());
            }
        }
        it.init();
        while(it.next()){
            if(seed(it.x())!=0&&mask(it.x())!=0){
                pop.growth(seed(it.x())+1,it.x());
            }
        }

        for(I32 i=0;i<functortopo.nbrLevel();i++)
        {
            pop.setLevel(i);
            functortopo.setLevel(i);
            while(pop.next())
            {
                if(pop.getRegion()(pop.x().second)==pop.getLabelNoRegion())
                    pop.growth(pop.x().first,pop.x().second);
                else
                    pop.setRegion(1,pop.x().second);
            }
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("WatershedBoundary");
        return pop.getRegion();
    }
    /*! \fn Function1 geodesicReconstruction(const Function1 & f,const Function2 & g, typename Function1::IteratorENeighborhood  itneigh)
      * \param f input matrix
     * \param g input matrix
     * \param itneigh neighborhood IteratorE domain
      *\return  the geodesic reconstruction
     *
     *  The geodesic reconstruction is the infinitely iterated geodesic erosion \f$E_g^\infty(f)\f$ such as \f$E_g^{t+1}(f)=\sup (E_g^{t}(f)\ominus N,g)\f$ with \f$E_g^{0}(f)=f\f$
     * \sa dynamic
    */

    template<typename Function1,typename Function2>
    static Function1 geodesicReconstruction(const Function1 & f,const Function2 & g, typename Function1::IteratorENeighborhood  itneigh)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("GeodesicReconstruction",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        FunctorZero zero;
        Population<Function1,FunctorZero,RestrictedSetWithoutSuperiorLabel,SQFIFO> pop(f.getDomain(),zero,itneigh) ;
        typename Function1::IteratorEDomain it(f.getIteratorEDomain());

        while(it.next()){

            pop.setRegion(f(it.x()),it.x());
        }
        it.init();
        while(it.next()){
            pop.growth(f(it.x()),it.x());

        }
        pop.setLevel(0);
        while(pop.next())
        {
            if( g(pop.x().second)<pop.x().first)
                pop.growth(pop.x().first,pop.x().second);
            else
                pop.pop();
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("GeodesicReconstruction");
        return pop.getRegion();
    }
    template<typename Function1,typename Function2>
    static Function1 dynamicNoRegionGrowing(const Function1 & f,const Function2 & g, typename Function1::IteratorENeighborhood  itneigh)
    {
        Function1 fi;
        Function1 fiplusun(f);
        do{
            fi = fiplusun;
            typename Function1::IteratorEDomain it(fi.getIteratorEDomain());
            fiplusun =maximum(erosion(fi,it,itneigh),g);
        }while(!(fi==fiplusun));
        return fiplusun;
    }

    /*! \fn static Function1 dynamic(const Function1 & f, typename Function1::F value, typename Function1::IteratorENeighborhood  itneigh)
      * \param f input matrix
     * \param value dynamic value
      * \param itneigh neighborhood IteratorE domain
      *\return  the geodesic reconstruction
     *
     *  Geodesic reconstruction with f = f+value and g=f
     * \sa FunctionProcedureGeodesicReconstruction
    */

    template<typename Function1>
    static Function1 dynamic(const Function1 & f, typename Function1::F value, typename Function1::IteratorENeighborhood  itneigh)
    {
        Function1 h(f);
        h+=value;
        return geodesicReconstruction(h, f,itneigh);
    }
    /*! \fn std::pair<FunctionRegion,typename FunctionTypeTraitsSubstituteF<FunctionRegion,plabel>::Result > voronoiTesselation(const FunctionRegion & seed, typename FunctionRegion::IteratorENeighborhood  itneigh)

      * \param seed input seed
      * \param itneigh neighborhood IteratorE domain
      * \return the first element of the pair contain the voronoi tesselation and the second the mapping
      *
      *  Voronoi tesselation based on the seeds \f$ region_i(x) = \{y :  d(y ,s_i) \leq d(y , s_j), j\neq i\}\f$ (work only for 1-norm and \f$\infty-norm\f$)
    */
    template<typename FunctionRegion>
    static std::pair<FunctionRegion,typename FunctionTypeTraitsSubstituteF<FunctionRegion,UI16>::Result > voronoiTesselation(const FunctionRegion & seed, typename FunctionRegion::IteratorENeighborhood  itneigh)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("VoronoiTesselation",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        FunctorSwitch f;
        Population<FunctionRegion,FunctorSwitch> pop(seed.getDomain(),f,itneigh);
        typename FunctionTypeTraitsSubstituteF<FunctionRegion,UI16>::Result dist(seed.getDomain());
        typename FunctionRegion::IteratorEDomain it(seed.getIteratorEDomain());

        while(it.next()){
            if(seed(it.x())!=0){
                dist(it.x())=0;
                pop.setRegion(seed(it.x()),it.x());
            }
        }
        it.init();
        while(it.next()){
            if(seed(it.x())!=0){
                pop.growth(seed(it.x()),it.x());
            }
        }
        UI16 distancevalue=0;
        bool atleastonegrowth=false;
        do
        {
            pop.setLevel(f.getFlipFlop());
            f.switchFlipFlop();
            distancevalue++;
            atleastonegrowth=false;
            while(pop.next()){
                atleastonegrowth=true;
                pop.growth(pop.x().first,pop.x().second);
                dist(pop.x().second)=distancevalue;
            }
        }while(atleastonegrowth==true);

        CollectorExecutionInformationSingleton::getInstance()->endExecution("VoronoiTesselation");
        return std::make_pair<FunctionRegion,typename FunctionTypeTraitsSubstituteF<FunctionRegion,UI16>::Result >(pop.getRegion(),dist);
    }
    template<typename FunctionRegion,typename FunctionMask>
    static FunctionRegion voronoiTesselationWithoutDistanceFunction(const FunctionRegion & seed, const FunctionMask & mask, typename FunctionRegion::IteratorENeighborhood  itneigh)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("VoronoiTesselation",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        FunctorZero f;
        Population<FunctionRegion,FunctorZero> pop(seed.getDomain(),f,itneigh);
        typename FunctionRegion::IteratorEDomain it(seed.getIteratorEDomain());
        while(it.next()){
            if(mask(it.x())==0){
                pop.setRegion(0,it.x());
            }
            else   if(seed(it.x())!=0){
                pop.setRegion(seed(it.x()),it.x());

            }
        }
        it.init();
        while(it.next()){
            if(mask(it.x())!=0 && seed(it.x())!=0){
                pop.growth(seed(it.x()),it.x());
            }
        }
        while(pop.next()){
            pop.growth(pop.x().first,pop.x().second);
        }
        it.init();
        while(it.next()){
            if(pop.getRegion()(it.x())==pop.getLabelNoRegion())
                pop.getRegion()(it.x())=0;
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("VoronoiTesselation");
        return pop.getRegion();
    }
    template<typename FunctionRegion>
    static FunctionRegion voronoiTesselationWithoutDistanceFunction(const FunctionRegion & seed,  typename FunctionRegion::IteratorENeighborhood  itneigh)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("VoronoiTesselation",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        FunctorZero f;
        Population<FunctionRegion,FunctorZero> pop(seed.getDomain(),f,itneigh);
        typename FunctionRegion::IteratorEDomain it(seed.getIteratorEDomain());

        while(it.next()){
            if(seed(it.x())!=0)
                pop.setRegion(seed(it.x()),it.x());
        }
        it.init();
        while(it.next()){

            if(seed(it.x())!=0)
                pop.growth(seed(it.x()),it.x());
        }
        while(pop.next()){
            pop.growth(pop.x().first,pop.x().second);
        }


        CollectorExecutionInformationSingleton::getInstance()->endExecution("VoronoiTesselation");
        return pop.getRegion();
    }
    /*! \fn     std::pair<FunctionRegion,typename FunctionTypeTraitsSubstituteF<FunctionRegion,plabel>::Result > voronoiTesselation(const FunctionRegion & seed, const FunctionMask & mask, typename FunctionRegion::IteratorENeighborhood  itneigh)
      * \param seed input seed
      * \param mask mask restricted the region growing
      * \param itneigh neighborhood IteratorE domain
      * \return the first element of the pair contain the voronoi tesselation and the second the mapping
      *
      *  Voronoi tesselation based on the seeds\f$ region_i(x) = \{y :  d(y ,s_i) \leq d(y , s_j), j\neq i\}\f$ such that the distunce function
      * is restricted by the mask (work only for 1-norm and \f$\infty-norm\f$)
    */
    template<typename FunctionRegion,typename FunctionMask>
    static std::pair<FunctionRegion,typename FunctionTypeTraitsSubstituteF<FunctionRegion,UI16>::Result > voronoiTesselation(const FunctionRegion & seed, const FunctionMask & mask, typename FunctionRegion::IteratorENeighborhood  itneigh)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("VoronoiTesselation",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        FunctorSwitch f;
        Population<FunctionRegion,FunctorSwitch> pop(seed.getDomain(),f,itneigh);
        typename FunctionTypeTraitsSubstituteF<FunctionRegion,UI16>::Result dist(seed.getDomain());
        typename FunctionRegion::IteratorEDomain it(seed.getIteratorEDomain());

        while(it.next()){
            if(mask(it.x())==0){
                pop.setRegion(0,it.x());
            }
            else
            {
                if(seed(it.x())!=0){
                    dist(it.x())=0;
                    pop.setRegion(seed(it.x()),it.x());
                }
            }
        }

        it.init();
        while(it.next()){
            if(mask(it.x())!=0 && seed(it.x())!=0){
                pop.growth(seed(it.x()),it.x());
            }
        }
        int distancevalue=0;
        bool atleastonegrowth=false;
        do
        {
            pop.setLevel(f.getFlipFlop());
            f.switchFlipFlop();
            distancevalue++;
            atleastonegrowth=false;
            while(pop.next()){
                atleastonegrowth=true;
                pop.growth(pop.x().first,pop.x().second);
                dist(pop.x().second)=distancevalue;
            }
        }while(atleastonegrowth==true);
        CollectorExecutionInformationSingleton::getInstance()->endExecution("VoronoiTesselation");
        return std::make_pair<FunctionRegion,typename FunctionTypeTraitsSubstituteF<FunctionRegion,UI16>::Result >(pop.getRegion(),dist);
    }
    /*! \fn std::pair<FunctionRegion,typename FunctionTypeTraitsSubstituteF<FunctionRegion,F64>::Result >  voronoiTesselationEuclidean(const FunctionRegion & seed)
      * \param seed input seed
      * \param region ouput region
      * \param dist distunce function
      * \return the first element of the pair contain the voronoi tesselation and the second the mapping
      *
      *  Quasi-voronoi tesselation based on the seeds \f$ region_i(x) = \{y :  d(y ,s_i) \leq d(y , s_j), j\neq i\}\f$ calculated with the euclidean norm
    */

    template<typename FunctionRegion>
    static std::pair<FunctionRegion,typename FunctionTypeTraitsSubstituteF<FunctionRegion,F64>::Result >  voronoiTesselationEuclidean(const FunctionRegion & seed)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("VoronoiTesselationEuclidean",INFO);
        typename FunctionRegion::IteratorENeighborhood itn(seed.getIteratorENeighborhood(1,0));
        FunctorZero f;
        typedef typename FunctionTypeTraitsSubstituteF<FunctionRegion ,UI32 >::Result FunctionLabel;
        Population<FunctionLabel,FunctorZero,RestrictedSetWithMySelf> pop(seed.getDomain(),f,seed.getIteratorENeighborhood(1,0));


        FunctionRegion region(seed.getDomain());
        typedef typename FunctionTypeTraitsSubstituteF<FunctionRegion,F64>::Result FunctionDistance;
        FunctionDistance dist(seed.getDomain());
        std::vector<typename FunctionRegion::E > vrand;
        typename FunctionRegion::IteratorEDomain it(seed.getIteratorEDomain());

        FunctionRegion  seedinner(seed.getDomain());
        seedinner = ProcessingAdvanced::erosion(seed,it,itn);
        seedinner = seed-seedinner;

        it.init();
        while(it.next()){
            if(seed(it.x())==0){
                dist(it.x())=NumericLimits<typename FunctionDistance::F>::maximumRange();
            }else{
                dist(it.x())=0;
            }
            if(seedinner(it.x())!=0){
                vrand.push_back(it.x());
            }
            region(it.x())=seed(it.x());
        }
        /* initialize random seed: */
        srand ( time(NULL) );
        pop.setLevel(0);
        int index =0;
        int display_step=1;
        while(vrand.empty()==false)
        {
            if(index>=display_step-1){
                display_step*=2;
                CollectorExecutionInformationSingleton::getInstance()->info("Random seed number="+BasicUtility::Any2String(index));
            }
            I32 i = rand()%((I32)vrand.size());
            typename FunctionRegion::E x = vrand[i];
            vrand[i]= *(vrand.rbegin());
            vrand.pop_back();
            int label = seed(x);
            pop.growth(index,x);
            region(it.x())=label;
            index++;
            while(pop.next())
            {
                typename FunctionRegion::E diff = pop.x().second-x;
                F64 disttemp = diff.normPower();
                if(disttemp<= dist(pop.x().second))
                {
                    pop.growth(pop.x().first,pop.x().second);
                    region(pop.x().second)=label;
                    dist(pop.x().second)=disttemp;
                }
                else
                    pop.pop();
            }
        }
        it.init();
        while(it.next()){
            if(dist(it.x())==NumericLimits<typename FunctionDistance::F>::maximumRange())
                dist(it.x()) =0;
            else
                dist(it.x()) =std::sqrt(static_cast<double>(dist(it.x())));
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("VoronoiTesselationEuclidean");
        return std::make_pair<FunctionRegion,typename FunctionTypeTraitsSubstituteF<FunctionRegion,F64>::Result >(region,dist);
    }

    /*! \fn static Function  erosionRegionGrowing(const Function & f,F64 radius, int norm=1)
          * \param bin input binary matrix
          * \param radius radius
          * \param norm norm
          * \return erosion ouput matrix
          *
          *  erosion(x) =  \min_{x'\in B(x,r,n)}f(x') \f$, where \f$B(x,norm)=\{x':|x'-x|_n\leq r\}\f$ the ball centered in 0 of radius r and the norm n
        */
    template<typename Function>
    static Function  erosionRegionGrowing(const Function & f,F64 radius, int norm=1)
    {
        return erosionRegionGrowing(f,radius,  norm, Loki::Int2Type<isVectoriel<typename Function::F>::value > ());
    }
    template<typename Function>
    static Function  erosionRegionGrowing(const Function & f,F64 radius, int norm,Loki::Int2Type<true>)
    {
        typedef typename Identity<typename Function::F>::Result::F TypeScalar;
        VecN<Function::F::DIM,typename FunctionTypeTraitsSubstituteF<Function,TypeScalar>::Result > V;
        Convertor::toVecN(f,V);
        for(int i=0;i<Function::DIM;i++){
            V(i) = erosionRegionGrowing(V(i),radius,norm);
        }
        Function exit;
        Convertor::fromVecN(V,exit);
        return exit;
    }

    template<typename Function>
    static Function  erosionRegionGrowing(const Function & f,F64 radius, int norm,Loki::Int2Type<false>)
    {
        if(norm<=1){
            CollectorExecutionInformationSingleton::getInstance()->startExecution("erosionRegionGrowing",NOINFO);
            CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
            FunctorSwitch func;
            Population<Function,FunctorSwitch,RestrictedSetWithoutSuperiorLabel> pop(f.getDomain(),func,f.getIteratorENeighborhood(1,norm));

            typename Function::IteratorEDomain it(f.getIteratorEDomain());

            while(it.next()){
                pop.setRegion(f(it.x()),it.x());
            }
            it.init();
            while(it.next()){
                pop.growth(f(it.x()),it.x());
            }
            int distancevalue=0;
            bool atleastonegrowth=true;
            while(atleastonegrowth==true&&distancevalue<radius)
            {
                pop.setLevel(func.getFlipFlop());
                func.switchFlipFlop();
                distancevalue++;
                atleastonegrowth=false;
                while(pop.next()){
                    atleastonegrowth=true;
                    pop.growth(pop.x().first,pop.x().second);
                }
            }
            CollectorExecutionInformationSingleton::getInstance()->endExecution("erosionRegionGrowing");
            return pop.getRegion();
        }else{
            CollectorExecutionInformationSingleton::getInstance()->startExecution("erosionRegionGrowing",NOINFO);
            CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
            FunctorSwitch func;
            PopulationInformation<Function,FunctorSwitch,typename Function::E,RestrictedSetWithoutSuperiorLabel> pop(f.getDomain(),func,f.getIteratorENeighborhood(1,0));

            typename Function::IteratorEDomain it(f.getIteratorEDomain());

            while(it.next()){
                pop.setRegion(f(it.x()),it.x());
            }
            it.init();
            while(it.next()){
                pop.growth(f(it.x()),it.x(),it.x());
            }
            double radiuspower2= radius*radius;
            int distancevalue=0;
            bool atleastonegrowth=true;
            while(atleastonegrowth==true&&distancevalue<radius)
            {
                pop.setLevel(func.getFlipFlop());
                func.switchFlipFlop();
                distancevalue++;
                atleastonegrowth=false;
                while(pop.next()){
                    typename Function::E diff = pop.x().first.second-pop.x().second;
                    F64 disttemp = diff.normPower();
                    if(disttemp<= radiuspower2)
                    {
                        atleastonegrowth=true;
                        pop.growth(pop.x().first.first,pop.x().first.second,pop.x().second);
                    }
                    else{
                        pop.pop();
                    }
                }

            }
            CollectorExecutionInformationSingleton::getInstance()->endExecution("erosionRegionGrowing");
            return pop.getRegion();
        }
    }
    /*! \fn static Function  dilationRegionGrowing(const Function & f,F64 radius, int norm=1)
          * \param bin input binary matrix
          * \param radius radius
          * \param norm norm
          * \return dilation ouput matrix
          *
          *  dilation(x) =  \max_{x'\in B(x,r,n)}f(x') \f$, where \f$B(x,norm)=\{x':|x'-x|_n\leq r\}\f$ the ball centered in 0 of radius r and the norm n
        */
    template<typename Function>
    static Function  dilationRegionGrowing(const Function & f,F64 radius, int norm=1)
    {
        return dilationRegionGrowing(f,radius,  norm, Loki::Int2Type<isVectoriel<typename Function::F>::value > ());
    }
    template<typename Function>
    static Function  dilationRegionGrowing(const Function & f,F64 radius, int norm,Loki::Int2Type<true>)
    {
        typedef typename Identity<typename Function::F>::Result::F TypeScalar;
        VecN<Function::F::DIM,typename FunctionTypeTraitsSubstituteF<Function,TypeScalar>::Result > V;
        Convertor::toVecN(f,V);
        for(int i=0;i<Function::DIM;i++){
            V(i) = dilationRegionGrowing(V(i),radius,norm);
        }
        Function exit;
        Convertor::fromVecN(V,exit);
        return exit;
    }

    template<typename Function>
    static Function  dilationRegionGrowing(const Function & f,F64 radius, int norm,Loki::Int2Type<false>)
    {
        if(norm<=1){
            CollectorExecutionInformationSingleton::getInstance()->startExecution("dilationRegionGrowing",NOINFO);
            CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
            FunctorSwitch func;
            Population<Function,FunctorSwitch,RestrictedSetWithoutInferiorLabel> pop(f.getDomain(),func,f.getIteratorENeighborhood(1,norm));

            typename Function::IteratorEDomain it(f.getIteratorEDomain());

            while(it.next()){
                pop.setRegion(f(it.x()),it.x());
            }
            it.init();
            while(it.next()){
                pop.growth(f(it.x()),it.x());
            }
            int distancevalue=0;
            bool atleastonegrowth=true;
            while(atleastonegrowth==true&&distancevalue<radius)
            {
                pop.setLevel(func.getFlipFlop());
                func.switchFlipFlop();
                distancevalue++;
                atleastonegrowth=false;
                while(pop.next()){
                    atleastonegrowth=true;
                    pop.growth(pop.x().first,pop.x().second);
                }
            }
            CollectorExecutionInformationSingleton::getInstance()->endExecution("dilationRegionGrowing");
            return pop.getRegion();
        }else{
            CollectorExecutionInformationSingleton::getInstance()->startExecution("dilationRegionGrowing",NOINFO);
            CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
            FunctorSwitch func;
            PopulationInformation<Function,FunctorSwitch,typename Function::E,RestrictedSetWithoutInferiorLabel> pop(f.getDomain(),func,f.getIteratorENeighborhood(1,0));

            typename Function::IteratorEDomain it(f.getIteratorEDomain());

            while(it.next()){
                pop.setRegion(f(it.x()),it.x());
            }
            it.init();
            while(it.next()){
                pop.growth(f(it.x()),it.x(),it.x());
            }
            double radiuspower2= radius*radius;
            int distancevalue=0;
            bool atleastonegrowth=true;
            while(atleastonegrowth==true&&distancevalue<radius)
            {
                pop.setLevel(func.getFlipFlop());
                func.switchFlipFlop();
                distancevalue++;
                atleastonegrowth=false;
                while(pop.next()){
                    typename Function::E diff = pop.x().first.second-pop.x().second;
                    F64 disttemp = diff.normPower();
                    if(disttemp<= radiuspower2)
                    {
                        atleastonegrowth=true;
                        pop.growth(pop.x().first.first,pop.x().first.second,pop.x().second);
                    }
                    else{
                        pop.pop();
                    }
                }
            }
            CollectorExecutionInformationSingleton::getInstance()->endExecution("dilationRegionGrowing");
            return pop.getRegion();

        }
    }
    template<typename Function>
    static Function  erosionRegionGrowing(const Function & f,const typename Function::IteratorENeighborhood & itneigh,double radius)
    {

        CollectorExecutionInformationSingleton::getInstance()->startExecution("erosionRegionGrowing",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");
        FunctorSwitch func;
        Population<Function,FunctorSwitch,RestrictedSetWithoutSuperiorLabel> pop(f.getDomain(),func,itneigh);

        typename Function::IteratorEDomain it(f.getIteratorEDomain());

        while(it.next()){
            pop.setRegion(f(it.x()),it.x());
        }
        it.init();
        while(it.next()){
            pop.growth(f(it.x()),it.x());
        }
        int distancevalue=0;
        bool atleastonegrowth=true;
        while(atleastonegrowth==true&&distancevalue<radius)
        {
            pop.setLevel(func.getFlipFlop());
            func.switchFlipFlop();
            distancevalue++;
            atleastonegrowth=false;
            while(pop.next()){
                atleastonegrowth=true;
                pop.growth(pop.x().first,pop.x().second);
            }
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("erosionRegionGrowing");
        return pop.getRegion();
    }
    template<typename Function>
    static Function  dilationRegionGrowing(const Function & f,const typename Function::IteratorENeighborhood & itneigh,double radius)
    {
        FunctorSwitch func;
        Population<Function,FunctorSwitch,RestrictedSetWithoutInferiorLabel> pop(f.getDomain(),func,itneigh);

        typename Function::IteratorEDomain it(f.getIteratorEDomain());

        while(it.next()){
            pop.setRegion(f(it.x()),it.x());
        }
        it.init();
        while(it.next()){
            pop.growth(f(it.x()),it.x());
        }
        int distancevalue=0;
        bool atleastonegrowth=true;
        while(atleastonegrowth==true&&distancevalue<radius)
        {
            pop.setLevel(func.getFlipFlop());
            func.switchFlipFlop();
            distancevalue++;
            atleastonegrowth=false;
            while(pop.next()){
                atleastonegrowth=true;
                pop.growth(pop.x().first,pop.x().second);
            }
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("dilationRegionGrowing");
        return pop.getRegion();
    }
    template<typename Function>
    static Function  closingRegionGrowing(const Function & f,F64 radius, int norm=1)
    {
        Function temp(f.getDomain());
        temp = dilationRegionGrowing(f,radius,norm);
        return erosionRegionGrowing(temp,radius,norm);
    }
    template<typename Function>
    static Function  openingRegionGrowing(const Function & f,F64 radius, int norm=1)
    {
        Function temp(f.getDomain());
        temp = erosionRegionGrowing(f,radius,norm);
        return dilationRegionGrowing(temp,radius,norm);
    }

    /*! \fn static Function1    recursive(const Function1 & f,FunctorRecursive & func,int direction=0, int way=1)
     * \brief recursive filter
     * \param f input function
     * \param func recursive functor as FunctorF::FunctorRecursiveOrder1 or FunctorF::FunctorRecursiveOrder2
     * \param direction recursive direction 0=x-direction, 1=y-direction
     * \param way  way 1=by increasing the position, -1=by deceasing the position
     * \return h output function
     *
     * A recursive algorithm uses one or more of its outputs as an input at each step.
     * For a 2d grid matrix, we select the way and the direction to scan the grid: left to right along each row or right to left along each row
     * or up to down along each column or down to up along each column. Then, we apply a recursive formula as the \f$\alpha\f$-recursive filter:
     * \f$g(n)=\alpha f(n)+ (1-\alpha)g(n-1)\f$.\n The code of this filter is
     *  \code
#include"core/data/utility/CollectorExecutionInformation.h"
#include"core/data/mat/MatN.h"
#include"core/algorithm/Processing.h"
using namespace pop;
        int main(){
    CollectorExecutionInformationSingleton::getInstance()->setActivate(true);
    try{
        Mat2RGBUI8 img;
        img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");

        //define the alpha-functor
        double alpha =0.05;
        F64 a0= alpha;
        F64 a1=  0;
        F64 b1= (1-alpha);
        F64 a0_border0 = 1;
        FunctorF::FunctorRecursiveOrder1 func(a0,a1,b1,a0_border0);

        //convert the input matrix to float pixel
        Mat2RGBF64 img_float(img);
        Mat2RGBUI8 rain;
        rain =  Processing::recursive(img_float,func,0,-1);
        rain.display();
    }
    catch(const pexception& e){
        std::cerr<<e.what()<<std::endl;
    }
    return 0;
}
     *  \endcode
     * \sa FunctorF::FunctorRecursiveOrder1 FunctorF::FunctorRecursiveOrder2
    */
    template< typename Function1, typename FunctorRecursive>
    static Function1  recursive(const Function1 & f,FunctorRecursive & func,int direction=0, int way=1)
    {
        typename Function1::IteratorEOrder itorder (f.getIteratorEOrder());
        itorder.setLastLoop(direction);
        itorder.setDirection(way);
        return pop::ProcessingAdvanced::recursive(f,itorder,func);
    }
    /*! \fn static Function1 recursiveAllDirections(const Function1 & f, FunctorCausal & funccausal, FunctorAntiCausal & funcanticausal )
     * \brief recursive filter \f$f_i = causal(f_{i-1})+ anticausal(f_{i-1})\f$ with i through the directions
     * \param f input function
     * \param funccausal causal functor as FunctorF::FunctorRecursiveOrder1 or FunctorF::FunctorRecursiveOrder2
     * \param funcanticausal anticausal functor as FunctorF::FunctorRecursiveOrder1 or FunctorF::FunctorRecursiveOrder2
     * \return h output function
     *
     * We apply successively the recursive though all directions such that \f$f_i = causal(f_{i-1})+ anticausal(f_{i-1})\f$. F
     * \sa FunctorF::FunctorRecursiveOrder1 FunctorF::FunctorRecursiveOrder2
    */


    template<typename Function1,typename FunctorCausal, typename FunctorAntiCausal>
    static Function1 recursiveAllDirections(const Function1 & f, FunctorCausal & funccausal, FunctorAntiCausal & funcanticausal )
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
            fcausal =pop::ProcessingAdvanced::recursive(fprevious,itorder,funccausal);
            dir = -1;
            itorder.setDirection(dir);
            itorder.init();
            fanticausal= pop::ProcessingAdvanced::recursive(fprevious,itorder,funcanticausal);

            fprevious = fcausal + fanticausal;
        }
        Function1 h(fprevious);
        return h;
    }
    /*! \fn Function1 recursiveAllDirections(const Function1 & f, I32 direction,FunctorCausal & funccausal, FunctorAntiCausal & funcanticausal ,FunctorCausalDirection & funccausaldirection, FunctorAntiCausalDirection & funcanticausaldirection)
     * \brief recursive filter \f$f_i = causal(f_{i-1})+ anticausal(f_{i-1})\f$ with i through the directions exception direction, \f$f_i = causaldirection(f_{i-1})+ anticausaldirection(f_{i-1})\f$ otherwise
     * \param f input function
     * \param direction apply the direction functors in this direction
     * \param funccausal causal functor in all directions exception direction
     * \param funcanticausal anticausal functor as FunctorF::FunctorRecursiveOrder1 or FunctorF::FunctorRecursiveOrder2
     * \param funccausaldirection causal functor in the input direction
     * \param funcanticausaldirection anticausal functor in the input direction
     * \return h output function
     *
     * We apply successively the recursive though all directions such that \f$f_i = causal(f_{i-1})+ anticausal(f_{i-1})\f$ for \f$i\neq\f$direction,  \f$f_i = causaldirection(f_{i-1})+ anticausaldirection(f_{i-1})\f$ otherwise. For instance for Deriche's gradient filter,
     * the code is:
     *  \code
#include"core/data/utility/CollectorExecutionInformation.h"
#include"core/data/mat/MatN.h"
#include"core/algorithm/Processing.h"
using namespace pop;
        int main(){
    CollectorExecutionInformationSingleton::getInstance()->setActivate(true);
    try{
        Mat2RGBUI8 img;
        img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");

        double alpha = 1;
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


        F64 kp = - (1.0 - e_a) * (1.0 - e_a) / e_a;
        F64 a0_c_d= 0;
        F64 a1_c_d=  kp * e_a;
        F64 a2_c_d=  0;
        F64 a0_ac_d= 0;
        F64 a1_ac_d= - kp * e_a;
        F64 a2_ac_d= 0;

        F64 a0_c_border0_d = ((a0_c_d + a1_c_d) / (1.0 - b1 - b2));
        F64 a0_c_border1_d = a0_c_d ;
        F64 a1_c_border1_d = a1_c_d ;

        F64 a0_ac_border0_d = ((a1_ac_d + a2_ac_d) / (1.0 - b1 - b2));
        F64 a0_ac_border1_d = 0 ;
        F64 a1_ac_border1_d = a1_ac_d + a2_ac_d ;

        FunctorF::FunctorRecursiveOrder2 funccausalsmooth
                (a0_c,a1_c,a2_c,
                 b1,b2,
                 a0_c_border0,
                 a0_c_border1,a1_c_border1,b1_border1) ;

        FunctorF::FunctorRecursiveOrder2 funcanticausalsmooth
                   (a0_ac,a1_ac,a2_ac,
                 b1,b2,
                 a0_ac_border0,
                 a0_ac_border1,a1_ac_border1,b1_border1) ;

        FunctorF::FunctorRecursiveOrder2 funccausalgrad
                   (a0_c_d,a1_c_d,a2_c_d,
                 b1,b2,
                 a0_c_border0_d,
                 a0_c_border1_d,a1_c_border1_d,b1_border1) ;

        FunctorF::FunctorRecursiveOrder2 funcanticausalgrad
                (a0_ac_d,a1_ac_d,a2_ac_d,
                 b1,b2,
                 a0_ac_border0_d,
                 a0_ac_border1_d,a1_ac_border1_d,b1_border1) ;

        MatN<2,RGB<F64> > derivate_x,derivate_y;
        derivate_x = Processing::recursiveAllDirections(img,0,funccausalsmooth,funcanticausalsmooth,funccausalgrad,funcanticausalgrad);
        derivate_y = Processing::recursiveAllDirections(img,1,funccausalsmooth,funcanticausalsmooth,funccausalgrad,funcanticausalgrad);
        MatN<2,Mat2F64 >::IteratorEDomain it(img.getIteratorEDomain());//global iteration
        while(it.next()){
            RGB<F64> value =2* std::sqrt(normValue(derivate_x(it.x())*derivate_x(it.x()) +derivate_y(it.x())*derivate_y(it.x())));//gradient magnitude
            img(it.x())= ArithmeticsSaturation<RGB<UI8>,RGB<F64> >::Range(value); //ArithmeticsSaturation to avoid the arithmic overflow artifact
        }
        img.display();

    }
    catch(const pexception& e){
        std::cerr<<e.what()<<std::endl;
    }
    return 0;
}
     *  \endcode
     * \sa FunctorF::FunctorRecursiveOrder1 FunctorF::FunctorRecursiveOrder2
    */
    template<typename Function1,typename FunctorCausal, typename FunctorAntiCausal,typename FunctorCausalDirection, typename FunctorAntiCausalDirection >
    static Function1 recursiveAllDirections(const Function1 & f, I32 direction,FunctorCausal & funccausal, FunctorAntiCausal & funcanticausal ,FunctorCausalDirection & funccausaldirection, FunctorAntiCausalDirection & funcanticausaldirection)
    {
        typename Function1::IteratorEOrder itorder (f.getIteratorEOrder());
        typedef typename  FunctionTypeTraitsSubstituteF<typename Function1::F,F64>::Result TypeF64;
        typename FunctionTypeTraitsSubstituteF<Function1,TypeF64>::Result fprevious(f);
        typename FunctionTypeTraitsSubstituteF<Function1,TypeF64>::Result fcausal(f.getDomain());
        typename FunctionTypeTraitsSubstituteF<Function1,TypeF64>::Result fanticausal(f.getDomain());
        for(I32 i=0;i <Function1::DIM;i++)
        {
            itorder.setLastLoop(i);
            typename Function1::E dir;
            dir = 1;
            itorder.setDirection(dir);
            itorder.init();
            if(i==direction)
                fcausal =pop::ProcessingAdvanced::recursive(fprevious,itorder,funccausaldirection);
            else
                fcausal =pop::ProcessingAdvanced::recursive(fprevious,itorder,funccausal);
            dir = -1;
            itorder.setDirection(dir);
            itorder.init();
            if(i==direction)
                fanticausal =pop::ProcessingAdvanced::recursive(fprevious,itorder,funcanticausaldirection);
            else
                fanticausal =pop::ProcessingAdvanced::recursive(fprevious,itorder,funcanticausal);
            fprevious = (fcausal  + fanticausal);
        }
        Function1 h(fprevious);
        return h;
    }

};
}
#endif // PROCESSINGADVANCED_H

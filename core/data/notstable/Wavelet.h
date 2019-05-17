#ifndef WAVELET_H
#define WAVELET_H
#include"core/data/vec/VecN.h"
#include"core/algorithm/Processing.h"
namespace pop{


/*! \ingroup Data
* \defgroup Wavelet Wavelet
* @{
*/
//Pure interface
template<typename Function>
class POP_EXPORTS Wavelet
{
public:

    /*!
        \class pop::Wavelet
        \brief  Interface for the multi-dimensional wavelet classes
        \author Tariel Vincent

        The concrete classes must implement the copy constructor

    */
    enum{DIM=Function::DIM};
    typedef typename Function::E Feature;
    typedef typename FunctionTypeTraitsSubstituteF<typename Function::E,double>::Result VecNDouble;


    /*!
     \brief scale the wavelet by the  \f$ \psi_\lambda\leftarrow (\psi,\lambda)  \f$
     \param lambda scale vector
     *
     */
    virtual void scale(const VecNDouble & lambda)=0;

    /*!
     \brief iterative translation of the wavelet\f$ \psi_x\leftarrow (\psi,x)  \f$
     \param x translate vector

     * if you call it two time, the trranslation is the sum of the two vector
     */
    virtual void translate(const VecNDouble & x)=0;
    /*!
     \brief translate the wavelet \f$ \psi_x\leftarrow (\psi,x)  \f$
     \param x translate vector
     *
     * if you call it two times, the translation is the vector of the last call
     */
    virtual void translateTemp(const VecNDouble & x)=0;

    /*!
     \brief wavelet coefficient as inner product between the wavelet and the input function \f$a(\psi_{\lambda,x},f) =\langle \psi_{\lambda,x} , f \rangle=\int_{R^n} \psi_{\lambda,x}(x')f(x')\,dx'.\f$
     \param x input VecN
     \return wavelet coefficient
     */
    virtual double operator ()(const typename  Function::E & x)=0;



    /*!
    \brief create a collection of wavelet from a single wavelet by scaling it
    \param wavelet_elementary elementatary wavelet
    \param minscale minimum scale parameter
    \param maxscale maximum scale parameter
    \param step incrementation step
    \return scaling collection of wavelets

    the collection of wavelet is the result of the scaling of the elementary wavelet between the range [minscale,maxscale] with a given step
    */
    template<typename WaveletConcrete>
    static std::vector<WaveletConcrete > scaleElementaryWavelet(const WaveletConcrete & wavelet_elementary,const VecNDouble& minscale,const VecNDouble& maxscale,const VecNDouble& scalefactor){
        std::vector<WaveletConcrete > v_base;
        if(DIM==2){
            VecNDouble scale;
            for(scale(0)=minscale(0);scale(0)<=maxscale(0);scale(0)*=scalefactor(0)){
                for(scale(1)=minscale(1);scale(1)<=maxscale(1);scale(1)*=scalefactor(1)){
                    WaveletConcrete wavelet(wavelet_elementary);
                    wavelet.scale(scale);
                    v_base.push_back(wavelet);
                }
            }
            return v_base;
        }else if(DIM==3){
            VecNDouble scale;
            for(scale(0)=minscale(0);scale(0)<=maxscale(0);scale(0)*=scalefactor(0)){
                for(scale(1)=minscale(1);scale(1)<=maxscale(1);scale(1)*=scalefactor(1)){
                    for(scale(2)=minscale(2);scale(2)<=maxscale(2);scale(2)*=scalefactor(2)){
                        WaveletConcrete wavelet(wavelet_elementary);
                        wavelet.scale(scale);
                        v_base.push_back(wavelet);
                    }
                }
            }
            return v_base;
        }
    }
    // the window size should be power of 2  work only in 2d
    template<typename WaveletConcrete>
    static std::vector<WaveletConcrete > scanWindow(const WaveletConcrete& wavelet_elementary,const VecNDouble& radius=16,const VecNDouble& scalefactor =2 ,const VecNDouble& step =2, const VecNDouble& border=2){
        std::vector<WaveletConcrete > v_wavelet;
        VecNDouble minscale (1);
        VecNDouble maxscale (radius);
        std::vector<WaveletConcrete > v_wavelet_scale = scaleElementaryWavelet(wavelet_elementary,minscale,maxscale,scalefactor);
        VecNDouble x = -radius+border;
        for(x(0)=-radius(0)+border(0);x(0)<=radius(0)-border(0);x(0)+=step(0)){
            for(x(1)=-radius(1)+border(1);x(1)<=radius(1)-border(1);x(1)+=step(1)){
                typename std::vector<WaveletConcrete >::iterator it;
                for(it=v_wavelet_scale.begin();it!=v_wavelet_scale.end();it++){
                    WaveletConcrete wavelet=*it;
                    wavelet.translate(x);
                    if(wavelet.isValid(-radius,radius))
                        v_wavelet.push_back(wavelet);
                }

            }
        }
        return v_wavelet;
    }
};



template<typename Function>
class POP_EXPORTS WaveletHaar :public Wavelet<Function>
{
    /*!
        \class pop::WaveletHaar
        \brief  pseudo Haar wavelet with integral matrixs to speed-up the calculation
        \author Tariel Vincent


        as explained in Viola and Jone article, we normalize the calculation by the standart deviation. The wavelet coefficient is \f$a(\phi,f) =\frac{<f>_{\sqcap_+}- <f>_{\sqcap_-}}{(<f^2>_\sqcap-<f>^2_\sqcap)}  \f$ such that \f$< >\f$ is the mean symbol,
        \f$\sqcap_- \f$ is the negative rectangular windows,  \f$\sqcap_+ \f$ is the possitive rectangular windows and \f$\sqcap =\sqcap_+ \cup \sqcap-   \f$ is the windows
    */





public:
    typedef typename FunctionTypeTraitsSubstituteF<typename Function::E,double>::Result VecNDouble;
    enum{DIM=Function::DIM};
    typedef typename Function::E Feature;
    /*!
      \brief constructor
    */
    WaveletHaar()
    {}
    /*!
      \brief copy constructor
    */
    WaveletHaar(const WaveletHaar & haar)
        :_x1_windows(haar._x1_windows),
          _x2_windows(haar._x2_windows),
          _x3_windows(haar._x3_windows),
          _x4_windows(haar._x4_windows),
          _x1_windows_negative(haar._x1_windows_negative),
          _x2_windows_negative(haar._x2_windows_negative),
          _x3_windows_negative(haar._x3_windows_negative),
          _x4_windows_negative(haar._x4_windows_negative)

    {

    }
    /*!
      \brief constructor by defining the rectangular windows \f$\sqcap\f$=[windows_min,windows_max] and the negative rectangular windows\f$\sqcap_-\f$=[windows_negative_min,windows_negative_max]
    such that the possitive rectangular windows is equal to \f$\sqcap\setminus\sqcap_- \f$
    *
    */
    WaveletHaar(const VecNDouble & windows_min,const VecNDouble & windows_max,const VecNDouble & windows_negative_min,const VecNDouble & windows_negative_max )
    {
        _x1_windows = windows_min;
        _x4_windows = windows_max;
        _x2_windows(0)=_x4_windows(0);_x2_windows(1)=_x1_windows(1);
        _x3_windows(0)=_x1_windows(0);_x3_windows(1)=_x4_windows(1);


        _x1_windows_negative = windows_negative_min;
        _x4_windows_negative = windows_negative_max;
        _x2_windows_negative(0)=_x4_windows_negative(0);_x2_windows_negative(1)=_x1_windows_negative(1);
        _x3_windows_negative(0)=_x1_windows_negative(0);_x3_windows_negative(1)=_x4_windows_negative(1);
    }
    void scale(const VecNDouble & scalefactor){
        _x1_windows=_x1_windows*scalefactor;
        _x2_windows=_x2_windows*scalefactor;
        _x3_windows=_x3_windows*scalefactor;
        _x4_windows=_x4_windows*scalefactor;
        _x1_windows_negative=_x1_windows_negative*scalefactor;
        _x2_windows_negative=_x2_windows_negative*scalefactor;
        _x3_windows_negative=_x3_windows_negative*scalefactor;
        _x4_windows_negative=_x4_windows_negative*scalefactor;
    }

    void translateTemp(const VecNDouble & x){

        _x1_windows_translate = x + _x1_windows;
        _x2_windows_translate = x + _x2_windows;
        _x3_windows_translate = x + _x3_windows;
        _x4_windows_translate = x + _x4_windows;


        _x1_windows_negative_translate = x + _x1_windows_negative;
        _x2_windows_negative_translate = x + _x2_windows_negative;
        _x3_windows_negative_translate = x + _x3_windows_negative;
        _x4_windows_negative_translate = x + _x4_windows_negative;
    }
    void translate(const VecNDouble & x){

        _x1_windows = x + _x1_windows;
        _x2_windows = x + _x2_windows;
        _x3_windows = x + _x3_windows;
        _x4_windows = x + _x4_windows;


        _x1_windows_negative = x + _x1_windows_negative;
        _x2_windows_negative = x + _x2_windows_negative;
        _x3_windows_negative = x + _x3_windows_negative;
        _x4_windows_negative = x + _x4_windows_negative;
    }
    double operator ()(const typename  Function::E & x){
        //        setIntegralFunction(f);
        translateTemp(x);
        F64 area_windows = (_x4_windows_translate-_x1_windows_translate).multCoordinate();
        F64 area_windows_negative = (_x4_windows_negative_translate-_x1_windows_negative_translate).multCoordinate();
        F64 area_windows_possitive = area_windows - area_windows_negative;


        F64 sumwindowspower2;
        sumwindowspower2 = _funtion_integral_power2(_x1_windows_translate)+_funtion_integral_power2(_x4_windows_translate)-(_funtion_integral_power2(_x2_windows_translate)+_funtion_integral_power2(_x3_windows_translate));

        F64 sumwindows;
        sumwindows =      _funtion_integral(_x1_windows_translate)+_funtion_integral(_x4_windows_translate)-(_funtion_integral(_x2_windows_translate)+_funtion_integral(_x3_windows_translate));
        F64 mean_windows      = sumwindows/area_windows;
        F64 mean_deviation_windows =sumwindowspower2/area_windows-mean_windows*mean_windows;
        if(mean_deviation_windows>0)
            mean_deviation_windows = std::sqrt( mean_deviation_windows);
        else
            mean_deviation_windows =1;

        F64 sumwindows_negative;

        sumwindows_negative =        _funtion_integral(_x1_windows_negative_translate)+_funtion_integral(_x4_windows_negative_translate)-(_funtion_integral(_x2_windows_negative_translate)+_funtion_integral(_x3_windows_negative_translate));
        F64 diff_mean_negative = sumwindows_negative/area_windows_negative-mean_windows;
        // use this relation xox = xxx -  x
        F64 sumwindows_possitive = sumwindows-sumwindows_negative;
        F64 diff_mean_possitive = sumwindows_possitive/area_windows_possitive-mean_windows;
        F64 result = (diff_mean_possitive -  diff_mean_negative)/  mean_deviation_windows;
        if(result>5)
            return 5;
        else if(result<-5)
            return -5;
        else
            return result;

    }

    /*!
     \brief nice elementary base of Haar wavelet with these properties area(positive)=area(negative) and  area(positive)+area(negative)=2
     \return wavelet base
     */
    static std::vector<WaveletHaar<Function> > baseWaveletHaar(){

        if(DIM==2){
            std::vector<WaveletHaar<Function> > v_haar_feature;

            VecNDouble windows_full_negative_corner,windows_full_positive_corner,windows_negative_negative_corner,windows_negative_positive_corner;

            //A
            // xo;
            windows_full_negative_corner(0)=-1;windows_full_negative_corner(1)=-1;
            windows_full_positive_corner(0)= 1;windows_full_positive_corner(1)= 1;
            windows_negative_negative_corner(0)= 0;  windows_negative_negative_corner(1)=-1;
            windows_negative_positive_corner(0)= 1;windows_negative_positive_corner(1)= 1;
            v_haar_feature.push_back(WaveletHaar<Function>(windows_full_negative_corner,windows_full_positive_corner,windows_negative_negative_corner,windows_negative_positive_corner));

            //B
            //  x
            //  o
            windows_full_negative_corner(0)=-1;windows_full_negative_corner(1)=-1;
            windows_full_positive_corner(0)= 1;windows_full_positive_corner(1)= 1;
            windows_negative_negative_corner(0)=-1;windows_negative_negative_corner(1)=0;
            windows_negative_positive_corner(0)= 1;windows_negative_positive_corner(1)=1;
            v_haar_feature.push_back(WaveletHaar<Function>(windows_full_negative_corner,windows_full_positive_corner,windows_negative_negative_corner,windows_negative_positive_corner));

            //C
            //  xox = xxx - 2*o
            windows_full_negative_corner(0)=-1;windows_full_negative_corner(1)=-1;
            windows_full_positive_corner(0)= 1;windows_full_positive_corner(1)= 1;
            windows_negative_negative_corner(0)=-0.5;windows_negative_negative_corner(1)=-1;
            windows_negative_positive_corner(0)= 0.5;windows_negative_positive_corner(1)= 1;
            v_haar_feature.push_back(WaveletHaar<Function>(windows_full_negative_corner,windows_full_positive_corner,windows_negative_negative_corner,windows_negative_positive_corner));

            //D
            //  x
            //  o
            //  x
            windows_full_negative_corner(0)=-1;windows_full_negative_corner(1)=-1;
            windows_full_positive_corner(0)= 1;windows_full_positive_corner(1)= 1;
            windows_negative_negative_corner(0)=-1;windows_negative_negative_corner(1)=-0.5;
            windows_negative_positive_corner(0)= 1;windows_negative_positive_corner(1)= 0.5;
            v_haar_feature.push_back(WaveletHaar<Function>(windows_full_negative_corner,windows_full_positive_corner,windows_negative_negative_corner,windows_negative_positive_corner));

            //E
            // xxx
            // x0x
            // xxx
            windows_full_negative_corner(0)=-1;windows_full_negative_corner(1)=-1;
            windows_full_positive_corner(0)= 1;windows_full_positive_corner(1)= 1;
            windows_negative_negative_corner(0)=-0.707106781;windows_negative_negative_corner(1)=-0.707106781;
            windows_negative_positive_corner(0)= 0.707106781;windows_negative_positive_corner(1)= 0.707106781;
            v_haar_feature.push_back(WaveletHaar<Function>(windows_full_negative_corner,windows_full_positive_corner,windows_negative_negative_corner,windows_negative_positive_corner));
            return v_haar_feature;
        }

    }
    bool isValid(const VecNDouble& xmin,const VecNDouble& xmax ){
        if(_x1_windows>=xmin && _x4_windows<=xmax)
            return true;
        else
            return false;
    }



    void setIntegralFunction(const Function & f){


        _funtion = &f;
        Function temp = Processing::greylevelRange(f,0,1);
        _funtion_integral = Processing::integral(temp);
        _funtion_integral_power2= Processing::integralPower2(temp);


    }
    // we use this formula xox = xxx - o where xox is the Harr wavelet, xxx is the windows and  x is the negative part in the the Harr wavelet

    void display(){
        Mat2RGBUI8 out(_funtion->getDomain());
        Mat2RGBUI8::IteratorERectangle itrec(out.getIteratorERectangle(_x1_windows_translate,_x4_windows_translate));
        while(itrec.next()){
            if(itrec.x()>=_x1_windows_negative_translate && itrec.x()<=_x4_windows_negative_translate){
                out(itrec.x())=RGBUI8( (*_funtion)(itrec.x()),0,0);
            }else
                out(itrec.x())=RGBUI8(0,0,(*_funtion)(itrec.x()));
        }
        out.display();

    }

    void save(std::ostream& out)const{
        out<<_x1_windows<<"<Haar>";
        out<<_x2_windows<<"<Haar>";
        out<<_x3_windows<<"<Haar>";
        out<<_x4_windows<<"<Haar>";
        out<<_x1_windows_negative<<"<Haar>";
        out<<_x2_windows_negative<<"<Haar>";
        out<<_x3_windows_negative<<"<Haar>";
        out<<_x4_windows_negative<<"<Haar>";
    }
    void load(std::istream& in){
        std::string str;
        str = pop::BasicUtility::getline(in,"<Haar>");
        pop::BasicUtility::String2Any(str,_x1_windows);
        str = pop::BasicUtility::getline(in,"<Haar>");
        pop::BasicUtility::String2Any(str,_x2_windows );
        str = pop::BasicUtility::getline(in,"<Haar>");
        pop::BasicUtility::String2Any(str,_x3_windows );
        str = pop::BasicUtility::getline(in,"<Haar>");
        pop::BasicUtility::String2Any(str,_x4_windows );
        str = pop::BasicUtility::getline(in,"<Haar>");
        pop::BasicUtility::String2Any(str,_x1_windows_negative);
        str = pop::BasicUtility::getline(in,"<Haar>");
        pop::BasicUtility::String2Any(str,_x2_windows_negative );
        str = pop::BasicUtility::getline(in,"<Haar>");
        pop::BasicUtility::String2Any(str,_x3_windows_negative );
        str = pop::BasicUtility::getline(in,"<Haar>");
        pop::BasicUtility::String2Any(str,_x4_windows_negative );
    }

private:

    VecNDouble _x1_windows;
    VecNDouble _x2_windows;
    VecNDouble _x3_windows;
    VecNDouble _x4_windows;

    VecNDouble _x1_windows_negative;
    VecNDouble _x2_windows_negative;
    VecNDouble _x3_windows_negative;
    VecNDouble _x4_windows_negative;

    VecNDouble _x1_windows_translate;
    VecNDouble _x2_windows_translate;
    VecNDouble _x3_windows_translate;
    VecNDouble _x4_windows_translate;

    VecNDouble _x1_windows_negative_translate;
    VecNDouble _x2_windows_negative_translate;
    VecNDouble _x3_windows_negative_translate;
    VecNDouble _x4_windows_negative_translate;
    static const Function * _funtion;
    static  Function _funtion_integral;
    static  Function _funtion_integral_power2;
};

/*!
@}
*/

template<typename Function>
const Function *  WaveletHaar<Function>::_funtion=NULL;
template<typename Function>
Function   WaveletHaar<Function>::_funtion_integral;
template<typename Function>
Function   WaveletHaar<Function>::_funtion_integral_power2;


template<typename Function>
std::istream& operator >> (std::istream& in,  pop::WaveletHaar<Function>& m)
{
    m.load(in);
    return in;
}
template<typename Function>
std::ostream& operator << (std::ostream& out, const pop::WaveletHaar<Function>& m)
{
    m.save(out);
    return out;
}



}


#endif // WAVELET_H

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
#ifndef PROCESSING_H
#define PROCESSING_H

#include"core/algorithm/GeometricalTransformation.h"
#include"core/algorithm/ProcessingAdvanced.h"
#include"core/algorithm/Analysis.h"
namespace pop
{
/*!  * \defgroup Processing Processing
     * \ingroup Algorithm
     * \brief Matrix In -> Matrix Out (threshold operators, morphological operators, gradient, edge detectors, region growing,...)
     *
     *
     *  Processing refers to a data-process where input and output are matrixs. We classify the algorithms in how the operator act. Concerning
     * the application of these algorithms, you can read the segmentation tutorials.
     *
     * \section Generator Generator
     *
     * The output matrix h is defined as follows, h(x) = H(), where g is the generating function. For H() = c, we fill the input matrix with a constant value and for
     * H()=X with X independent and identically distributed random variables following the probability distribution \f$\mathcal{P}\f$, we generate an homgeneous random field.

     * \section Point Point

     * In Point algorithm, we iterate globally a process on the Points of the matrixs, e.g. the threshold operator. The complexity is linear with the number of pixels/voxels.

     * \section Neighborhood Neighborhood

     * In neightborhood algorithm, the iteration is twofold: globally and locally (see \ref  DOCNeighborhood). In each Point of the global iteration, we iterate a local iteration.
     * The computational time is proportionnel to the radius power the dimension multiply by the number of pixels/voxels. The radius is the characteristic size of the local iteration.

     * \image html Erosion.gif "iterative Point process"

     * \section Convolution  Convolution

     * In convolution algorithm as neightborhood algorithm, the iteration is twofold: globally and locally (see \ref  DOCConvolution) except that we keep the local position
     * in the local iteration.

     * \section Recursive  Recursive

     * A recursive algorithm uses one or more of its outputs as an input at each step (see \ref DOCRecursive).

     * \section Region Region Growing

     * Many fields in computer science, stereovision mathematical morphology, use algorithm which principle is Region Growing.
     * This method consists in initializing each region with a seed and then iterate pixel by pixel growth controlling by a metric until convergence.
     * A large literature is dedicated to the definition of the metric divided into two distinct categories: the object feature like the tint and the object boundary discontinuity.
     * The implementation of each algorithm can be daunting since many data-structures should interact in the algorithm.
     * To overcome that, I introduced a conceptual framework with a modern architecture design for the localization and the organization of the growing process.
     * The following video shows the growing process for the watershed algorithm on the topographic surface defined by the gradient magnitude of the original matrix :
     * \image html Lenawater.gif

*/
struct POP_EXPORTS Processing
{

    /*!
        \class pop::Processing
        \ingroup Processing
        \brief process on  Matrices
        \author Tariel Vincent


    */


    //-------------------------------------
    //
    //! \name Generator
    //@{
    //-------------------------------------
    /*! \fn static void  randomField(const typename Function::Domain & domain ,Distribution &d,Function& h)
     *  \brief generate a uniform random field following the probability distribution d (in cameleon randomFieldMatN)
     * \param domain domain of defintion of the output function
     * \param d input distribution
     * \param h output function
     *
     *
        \code
                Mat2UI8 img(255,255);//2d grey-level matrix object
                Distribution d( DistributionUniformInt(0,255));
                Processing::randomField(img.getDomain(),d,img);
                img.display();
        \endcode
        \sa Distribution Analysis
    */
    template<typename Function>
    static void  randomField(const typename Function::Domain & domain ,Distribution &d,Function& h){
        h.resize(domain) ;
        typename Function::IteratorEDomain it(h.getIteratorEDomain());
        h= pop::ProcessingAdvanced::randomField<Function>(domain,d,it);
    }

    /*! \fn static Function  fill(const Function & f, const typename Function::F & value)
     *  \brief fill the function with a constant value
     * \param f input matrix
     * \param value constant value
     * \return h output function
     *
     *
        \code
                Mat2UI8 img(255,255);//2d grey-level matrix object
                img = Processing::fill(img,30);
                img.display();
                //For a fast implementation, you have alse:
                img=30;
        \endcode
        \sa Distribution Analysis
    */
    template<typename Function>
    static Function  fill(const Function & f, const typename Function::F & value){
        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        return pop::ProcessingAdvanced::fill(f,value,it);
    }
    //@}
    //-------------------------------------
    //
    //! \name Point
    //@{
    //-------------------------------------

    /*! \fn typename FunctionTypeTraitsSubstituteF<Function,UI8>::Result  threshold(const Function& f,double ymin,double ymax=255)
     *  \brief threshold
     * \param f input function
     * \param ymin min value
     * \param ymax  max value
     * \return output function noted h
     *
     *  Given that each object of the matrix  has a specific tint,  the threshold operation uses this information to extract the phases.
     *   The threshold operation requires the selection of a range of grey-levels.
     *   The label '255' is assigned to each pixel/voxel of the matrix when the grey-level belongs to this range, and  the label '0' otherwise.
     *  The grey-level range selection is usually based on the information contained in the grey level histogram of the matrix.
     *   The automatic or manual  grey-level range is selected in order to best separate the mode in the histogram. One requirement is that the mode is populated almost always by the same phase.
     *
     *  The function h with pixel coded in 1Byte (puint6) is defined as:
        \f$
         \forall x \in E:\quad h(x)= \left\{\begin{array}{ll}255 & \mbox{for} \quad f(x)\geq min\quad  \mbox{and}\quad f(x)<max\\
          0& \mbox{otherwise.}
          \end{array}\right.
        \f$
        * For instance this code
        \code
        Mat2RGBUI8 img;
        img.load("/home/vincent/Desktop/Population/doc/matrix/Lena.bmp");
        Processing op;
        Mat2UI8 threshold = op.threshold(img,130);
        threshold.save("lenathreshold130.bmp");
        \endcode
        produce this matrix
        \image html lenathreshold130.png
        For a manual selection of the theshold parameters with a feed back loop, you can use the MatNDisplay facility. For instance, this code:
        \code
            Mat2UI8 img;
            img.load("/home/vincent/Desktop/Population/doc/image/iex.pgm");
            img = PDE::nonLinearAnisotropicDiffusionGaussian(img);
            int high_value=255;
            int low_value=125;
            MatNDisplay window;
            do{
                if(window.is_keyARROWUP())
                    low_value++;
                if(window.is_keyARROWDOWN())
                    low_value--;
                if(window.is_keyARROWRIGHT())
                    high_value++;
                if(window.is_keyARROWLEFT())
                    high_value--;
                Mat2UI8 threshold = Processing::threshold(img,low_value,high_value);
                Mat2RGBUI8 visu = pop::Visualization::labelForeground(threshold,img);
                 std::string title = "threshold min = "+BasicUtility::Any2String(low_value)+ "\t theshold max = "+BasicUtility::Any2String(high_value);
                window.set_title(title.c_str());
                window.display(visu);
                if(window.is_key()==true)
                    window.waitTime(50);
           }while(!window.is_closed());
      \endcode
       will help you to find the suitable parameter
    */
    template<typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,UI8>::Result  threshold(const Function& f,typename Function::F ymin,typename Function::F ymax=NumericLimits<typename Function::F>::maximumRange() ){
        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        return pop::ProcessingAdvanced::threshold(f,ymin,ymax,it);
    }

    /*!
     *  \brief threshold in color range
     * \param img input matrix in color
     * \param lowvalue min RGB value
     * \param highvalue  max RGB value
     * \return output function noted h
     *
    */
    template<int DIM>
    static MatN<DIM,UI8>  thresholdColorInRange(const MatN<DIM,RGBUI8> & img,RGBUI8 lowvalue,RGBUI8 highvalue=RGBUI8(255,255,255) ){
        MatN<DIM,UI8> threshold(img.getDomain());
        typename MatN<DIM,UI8>::IteratorEDomain it = threshold.getIteratorEDomain();
        while(it.next())
        {
            typename MatN<DIM,UI8>::E x = it.x();
            if(img(x).r()>=lowvalue.r()&&img(x).r()<=highvalue.r()
                    &&img(x).g()>=lowvalue.g()&&img(x).g()<=highvalue.g()&&
                    img(x).b()>=lowvalue.b()&&img(x).b()<=highvalue.b()
                    )
                threshold(x)=255;
            else
                threshold(x)=0;

        }
        return threshold;
    }

    /*!
     *  \brief automatic threshold following K-mean variation http://homepages.inf.ed.ac.uk/rbf/CVonline/LOCAL_COPIES/MORSE/threshold.pdf
     * \param f input function
     * \return output function noted h

        \code
        Mat2UI8 img;//2d grey-level matrix object
        img.load("/home/vincent/Desktop/Population/iex.pgm");//replace this path by those on your computer
        img = PDE::nonLinearAnisotropicDiffusionDericheFast(img);//filtering
        Mat2UI8 threshold = Processing::thresholdKMeansVariation(img);//threshold segmentation
        threshold.save("/home/vincent/Desktop/Population/iexthreshold.pgm");
        Mat2RGBUI8 RGB = Visualization::labelForeground(threshold,img);//Visual validation
        RGB.display();
      \endcode
      * \image html iex.png
      * \image html iexseg.png
    */
    template<typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,UI8>::Result  thresholdKMeansVariation(const Function& f){

        Mat2F64 m = Analysis::histogram(f);
        int value_threshold_step_i    = 0;
        int value_threshold_step_i_plus1=m(m.sizeI()/2,0);
        while(value_threshold_step_i_plus1!=value_threshold_step_i){
            value_threshold_step_i = value_threshold_step_i_plus1;
            double mean_min=0;
            double mean_max=0;
            double sum_min=0;
            double sum_max=0;
            for(unsigned int i=0;i<m.sizeI();i++){
                if(m(i,0)<value_threshold_step_i){
                    mean_min += m(i,0)*m(i,1);
                    sum_min  +=m(i,1);
                }
                else{
                    mean_max += m(i,0)*m(i,1);
                    sum_max  +=m(i,1);
                }
            }
            mean_min/=sum_min;
            mean_max/=sum_max;
            value_threshold_step_i_plus1 = (mean_max+mean_min)/2;
        }
        return Processing::threshold(f,value_threshold_step_i);
    }
    /*!
     *  \brief automatic threshold following Otsu's method http://homepages.inf.ed.ac.uk/rbf/CVonline/LOCAL_COPIES/MORSE/threshold.pdf
     * \param f input function
     * \param thresholdvalue founded tresholded value
     * \return output function noted h

        \code
        Mat2UI8 img;//2d grey-level matrix object
        img.load("/home/vincent/Desktop/Population/iex.pgm");//replace this path by those on your computer
        img = PDE::nonLinearAnisotropicDiffusionDericheFast(img);//filtering
        Mat2UI8 threshold = Processing::thresholdOtsuMethod(img);//threshold segmentation
        threshold.save("/home/vincent/Desktop/Population/iexthreshold.pgm");
        Mat2RGBUI8 RGB = Visualization::labelForeground(threshold,img);//Visual validation
        RGB.display();
      \endcode
    */
    template<typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,UI8>::Result  thresholdOtsuMethod(const Function& f,double & thresholdvalue){

        Mat2F64 m = Analysis::histogram(f);
        double variane_between_class_max=0;
        int threshold_max =0;
        double mean = 0;
        for (unsigned int i=0 ; i<m.sizeI() ; i++)
            mean += m(i,0) * m(i,1);
        double sumB = 0;
        double wB = 0;
        double wF = 0;
        for(unsigned int i=0;i<m.sizeI();i++){
            wB += m(i,1);               // Weight Background
            if (wB == 0) continue;

            wF = 1 - wB;                 // Weight Foreground
            if (wF == 0) break;

            sumB += m(i,0) * m(i,1);

            double meanB = sumB / wB;            // Mean Background
            double meanF = (mean - sumB) / wF;    // Mean Foreground

            // Calculate Between Class Variance
            double variane_between_class = wB * wF * (meanB - meanF) * (meanB - meanF);

            // Check if new maximum found
            if (variane_between_class > variane_between_class_max) {
                variane_between_class_max = variane_between_class;
                threshold_max = i+1;
            }
        }
        thresholdvalue = threshold_max;
        return Processing::threshold(f,threshold_max);
    }
    /*!
     *  \brief automatic threshold following the morphological ToggleMapping http://cmm.ensmp.fr/~marcoteg/cv/publi_pdf/jonathan/fabrizio_marcotegui_cord_icip09.pdf
     * \param f input function
     * \param radius structural element size
     * \param norm structural element form (0=square, 1=losange, 2=sphere)
     * \return output function noted h
     *
     * \f$\forall x : s(x) =\arg\min_{i\in\{1,2\}} |f(x)-h_i(x)| \mbox{with } h_1(x)=erosion(f(x),B_n(r)) \mbox{ and } h_2(x)=dilation(f(x),B_n(r)) \f$
     * \code
     * std::string path_linux ="../../../vitrine.jpg";
     * Mat2UI8 img(path_linux.c_str());
     * img = Processing::thresholdToggleMappingMorphological(img,5);
     * img.save("../doc/image2/vitrinethresholdTMMS1.jpg");
     * \endcode
     * \image html vitrine.jpg
     * \image html vitrinethresholdTMMS1.jpg
    */

    template<typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,UI8>::Result thresholdToggleMappingMorphological(const Function& f,int radius=2,double norm=0){
        Function erosion  = Processing::erosionRegionGrowing(f,radius,norm);
        Function dilation = Processing::dilationRegionGrowing(f,radius,norm);
        typename FunctionTypeTraitsSubstituteF<Function,UI8>::Result s(f.getDomain());
        ForEachDomain(x,f){
            if(pop::normValue(f(x)-erosion(x))<pop::normValue(dilation(x)-f(x)))
                s(x)= 255;
            else
                s(x)=0;
        }
        return s;
    }
    /*!
     *  \brief automatic threshold following the morphological ToggleMapping http://cmm.ensmp.fr/~marcoteg/cv/publi_pdf/jonathan/fabrizio_marcotegui_cord_icip09.pdf
     * \param f input function
     * \param c cut-off of undetermined pixels
     * \param p thickness
     * \param radius structural element size
     * \param norm structural element form (0=square, 1=losange, 2=sphere)
     * \return output function noted h

     * \code
    std::string path_linux ="../../../vitrine.jpg";
    Mat2UI8 img(path_linux.c_str());
    img = Processing::thresholdToggleMappingMorphologicalFabrizio(img,50,0.6,5);
    img.save("../doc/image2/vitrinethresholdTMMS2.jpg");
     * \endcode
     * \image html vitrine.jpg
     * \image html vitrinethresholdTMMS2.jpg
    */
    template<typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,UI8>::Result thresholdToggleMappingMorphologicalFabrizio2(const Function& f,double c=16,double p=0.8,int radius=2,double norm=1){
    Function erosion  = Processing::erosionRegionGrowing(f,radius,norm);
    Function dilation = Processing::dilationRegionGrowing(f,radius,norm);

    typename FunctionTypeTraitsSubstituteF<Function,UI8>::Result label(erosion.getDomain());
    ForEachDomain(x,f){
        double v_d = pop::normValue(dilation(x));
        double v_e = pop::normValue(erosion(x));
        double v = pop::normValue(f(x));

        if(std::abs(v_d-v_e)<c){
            label(x)=255;
            dilation(x)= 0;
        }
        else if(std::abs(v_d-v)<=p*std::abs(v_e-v)){
            label(x)=0;
            dilation(x)=1;
        }
        else{
            label(x)=0;
            dilation(x)=2;
        }
    }
    typename FunctionTypeTraitsSubstituteF<Function,UI32>::Result labelled = Processing::clusterToLabel(label,0);
    labelled = Processing::dilation(labelled,1,1);
    UI32 max = Analysis::maxValue(labelled);
    Vec<int> label1(max+1);
    Vec<int> label2(max+1);
    typedef typename Function::F PixelType;
    ForEachDomain(x,f){
        if(dilation(x)==PixelType(1))
            label1(labelled(x))++;
        else if(dilation(x)==PixelType(2))
            label2(labelled(x))++;
    }
    ForEachDomain(x,f){
        if(dilation(x)==PixelType(0)){
            if(label1(labelled(x))>label2(labelled(x)))
                label(x)=255;
            else
                label(x)=0;
        }else if(dilation(x)==PixelType(1))
            label(x)=255;
        else
            label(x)=0;

    }
    return label;
    }
    /*!
     *  \brief Niblack threshold (1986), An introduction to Digital Image Processing, Prentice-Hall
     * \param f input function
     * \param k multiplicative factor of the standard deviation
     * \param radius neighbordhood radius
     * \param offset_value offset value
     * \return output function noted h
     *
     * pixel = ( pixel >  mean + k * standard_deviation - offset_value) ? object : background
     * \code
    std::string path_linux ="../../../vitrine.jpg";
    Mat2UI8 img(path_linux.c_str());
    img = Processing::thresholdNiblackMethod(img,0.6);
    img.display();
    img.save("../doc/image2/vitrinethresholdNiblack.jpg");
     * \endcode
     * \image html vitrine.jpg
     * \image html vitrinethresholdNiblack.jpg
    */
    template<typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,UI8>::Result  thresholdNiblackMethod(const Function & f,double k=0.2,int radius=5,double offset_value=0  ){

        Function fborder(f);
        Draw::addBorder(fborder,radius,typename Function::F(0),Boundary_Condition_Mirror);

        typedef typename FunctionTypeTraitsSubstituteF<Function,F64>::Result FunctionF64;
        FunctionF64 f_double(fborder);
        FunctionF64 integral = Processing::integral(f_double);
        FunctionF64 integralpower2 = Processing::integralPower2(f_double);

        typename FunctionF64::IteratorERectangle it(fborder.getIteratorERectangle(Vec2I32(radius),f_double.getDomain()-1-Vec2I32(radius)));
        int area = (2*radius+1)*(2*radius+1);
        while(it.next()){
            Vec2I32 xadd1=it.x()+Vec2I32(radius);
            Vec2I32 xadd2=it.x()+Vec2I32(-radius);
            Vec2I32 xsub1=it.x()-Vec2I32(radius,-radius);
            Vec2I32 xsub2=it.x()-Vec2I32(-radius,radius);
            F64 mean = integral(xadd1)+integral(xadd2)-integral(xsub1)-integral(xsub2);
            mean/=area;

            F64 standartdeviation =integralpower2(xadd1)+integralpower2(xadd2)-integralpower2(xsub1)-integralpower2(xsub2);
            standartdeviation/=area;
            standartdeviation =standartdeviation-mean*mean;

            if(standartdeviation>0)
                standartdeviation = std::sqrt( standartdeviation);
            else
                standartdeviation =1;
            if(f(it.x()-radius)>ArithmeticsSaturation<typename Function::F,F64>::Range( mean+k*standartdeviation)-offset_value)
                fborder(it.x())=255;
            else
                fborder(it.x())=0;
        }
        return fborder( Vec2I32(radius) , fborder.getDomain()-Vec2I32(radius));
    }
    /*!
     *  \brief automatic multi-threshold with the ranges defined by the valleys of the histogram
     * \param f input function
     * \param dynamic filter the valleys with depth inferior to dynamic
     *
     * \return output function noted h

        \code
    Mat2UI8 img2;
    img2.load("/home/vincent/Downloads/OpenCV-2.4.3/samples/cpp/lena.jpg");
    Mat2UI8 label2 = Processing::thresholdMultiValley(img2);
    Visualization::labelAverageRGB(label2,img2).display();
      \endcode
    */
    template<typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,typename FunctionTypeTraitsSubstituteF<typename Function::F,UI8>::Result    >::Result  thresholdMultiValley(const Function& f,double dynamic=0.001){


        typedef typename FunctionTypeTraitsSubstituteF<Function,typename FunctionTypeTraitsSubstituteF<typename Function::F,UI8>::Result    >::Result FunctionReturn;
        if(typeid(typename Function::F)!=typeid(typename FunctionTypeTraitsSubstituteF<typename Function::F,UI8>::Result))
            return thresholdMultiValley(FunctionReturn(Processing::greylevelRange(f,typename Function::F(0),typename Function::F(255))),dynamic);
        typename FunctionTypeTraitsSubstituteF<Function,UI8   >::Result ff(f);


        Mat2F64 mm =Analysis::histogram(ff);
        Mat2F64 mmm(mm);
        mmm.deleteCol(0);
        mmm = Processing::dynamicNoRegionGrowing(mmm,dynamic);
        mm.setCol(1,mmm.getCol(0));
        DistributionRegularStep step(mm);
        std::vector<F64> vmin=Statistics::argMinLocal(step,0,255,1);
        std::vector<F64> vmax=Statistics::argMaxLocal(step,0,255,1);
        for(unsigned int i=0;i<vmin.size();i++){
            if(vmin[i]<*vmax.begin()|| vmin[i]>*vmax.rbegin()){
                vmin.erase(vmin.begin()+i);
                i--;
            }
        }
        ff=0;
        for( int i =(static_cast<int>(vmin.size())-1);i>=0;i--){
            typename Function::IteratorEDomain it(f.getIteratorEDomain());
            if(i==(static_cast<int>(vmin.size())-1)){
                FunctorF::FunctorThreshold<UI8,double,typename  Function::F> func(vmin[i],255,255);
                while(it.next())
                    if(ff(it.x())==0)
                        ff(it.x())=func( f(it.x()));
            }else{
                FunctorF::FunctorThreshold<UI8,double,typename  Function::F> func(vmin[i],vmin[i+1],255-  255/(1.0*vmin.size())*(vmin.size()-1-i));
                while(it.next())
                    if(ff(it.x())==0)
                        ff(it.x())=func( f(it.x()));
            }
        }
        return ff;

    }

    /*! \fn Function  fofx(const Function& f,Distribution &d)
     *  \brief apply at pixel value, f(x), the distribution : h(x)=d(f(x))
     * \param f input function
     * \param d input distribution
     * \return output function noted h
     *
     *  We apply a transformation at each pixel defined by a function with a symbolic link, h(x) = d(f(x)) for instance,
     *  this code applies this transformation f(x) = (x/255)^3*255
        \code
                Mat2RGBUI8 img;
                img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
                Distribution d("(x/255)^3*255");
                Mat2RGBUI8 power2 =Processing::fofx(img,d);
                power2.display();
        \endcode
        \image html dilatgrey.png
        \sa Distribution Analysis
    */
    template<typename Function>
    static Function  fofx(const Function& f,Distribution &d){

        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        return pop::ProcessingAdvanced::fofx(f,d,it);
    }



    /*! \fn static Function  greylevelScaleContrast(const Function& f,double scale)
     *  \brief apply at pixel value, f(x), the distribution : h(x)=d(f(x))
     * \param f input function
     * \param scale scale factor
     * \return output function noted h
     *
     *  Let sigma the Root Mean Square Contrast RMSC  http://en.wikipedia.org/wiki/Contrast_%28vision%29  of the input function with scalar pixel/voxel type,
        the RMSC of the ouput function  is equal to sigma*scale
        \code
                Mat2RGBUI8 img;
                img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
                Mat2UI8 r,g,b;
                Convertor::toRGB(img,r,g,b);
                Processing processing;
                r= processing.greylevelScaleContrast(r,2);
                g= processing.greylevelScaleContrast(g,0.5);
                b= processing.greylevelScaleContrast(b,0.5);
                Convertor::fromRGB(r,g,b,img);
                img.display();
        \endcode
    */

    template<typename Function>
    static Function  greylevelScaleContrast(const Function& f,double scale){

        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        return pop::ProcessingAdvanced::greylevelScaleContrast(f,scale,it);
    }

    /*! \fn  Function greylevelRange(const Function & f,typename Function::F min=NumericLimits<typename Function::F>::minimumRange(), typename Function::F max=NumericLimits<typename Function::F>::maximumRange())throw(pexception)
     *  \brief h(x)=(f(x)-min(f))*(max-min)/(max(f)-min(f))+min
     * \param f input function
     * \param min min value of the output function
     * \param max max value of the output function
     * \return output function noted h
     *
     * Scale the pixel/voxel value range using this formula h(x)=(f(x)-min(f))*(max-min)/(max(f)-min(f))+min
        \code
                Mat2RGBUI8 img;
                img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
                img = Processing::greylevelRange(img);
                img.display();
        \endcode
    */

    template<typename Function>
    static Function greylevelRange(const Function & f,typename Function::F min=NumericLimits<typename Function::F>::minimumRange(), typename Function::F max=NumericLimits<typename Function::F>::maximumRange())throw(pexception)
    {
        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        return pop::ProcessingAdvanced::greylevelRange(f,it,min,max,Loki::Int2Type<isVectoriel<typename Function::F>::value >());
    }


    /*! \fn  Function greylevelTranslateMeanValue(const Function& f, F64 mean )throw(pexception)
     *  \brief  h(x)=(f(x)/255)^d*255 with d such that mean(h)=mean
     * \param f input function
     * \param mean mean value of the output function
     * \return output function noted h
     *  Translate the mean value of the input function using this transformation h(x)=(f(x)/max(F))^d*max(F) with max(F)=255 for 1Byte pixel/voxel type. for instance, this code:
        \code
                Mat2RGBUI8 img;
                img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
                img = Processing::greylevelTranslateMeanValue(img,RGBUI8(150,150,150));
                img.display();
        \endcode
        produces the RGB red (200,10,10) if you look far away.
    */
    template<typename Function>
    static Function greylevelTranslateMeanValue(const Function& f, typename Function::F mean )throw(pexception)
    {
        return ProcessingAdvanced::greylevelTranslateMeanValueCast(f,mean,Loki::Type2Type<typename Function::F >() );
    }
    /*! \fn  Function greylevelRemoveEmptyValue(const Function & f)throw(pexception)
     *  \brief  Remove the grey-level values not populated by pixesl/voxels
     * \param f input function
     * \return output function noted h
     *  Remove the grey-level values not populated by pixesl/voxels that can be usefull to analyse a segmented matrix. For instance, this code:
        \code
                Mat2RGBUI8 img;
                img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
                Mat2UI8 bin =Processing::threshold(img,100);
                bin =Processing::greylevelRemoveEmptyValue(bin);
                Mat2F64 m = Analysis::histogram(bin);
                m.display();
        \endcode
        produces this output :\n
#2 2\n
0	0.32\n
1	0.68\n
without  the application of greylevelRemoveEmptyValue, all grey-level excepted 0 and 255 will be not populated by pixels. So the result will be\n
#2 2\n
0	0.32\n
1   0\n
2   0\n
...\n
255	0.68\n
    */
    template<typename Function>
    static Function greylevelRemoveEmptyValue(const Function & f)throw(pexception)
    {
        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        return pop::ProcessingAdvanced::greylevelRemoveEmptyValue(f,it);
    }

    /*! \fn  Function integral(const Function & f)throw(pexception)
     *  \brief  integral of the matrix http://research.microsoft.com/~viola/Pubs/Detect/violaJones_IJCV.pdf
     * \param f input function
     * \return output function noted h
     *
     *  \f$ h(x)=\int_{0\leq x'\leq x} f(x') dx' \f$
        \code
    Mat2UI8 img;
    img.load("/home/vincent/Desktop/test.pgm");

    Mat2UI32 imgl;
    imgl=img;
    imgl = Processing::integral(imgl);
    img = Processing::greylevelRange(imgl,0,255);
    img.display();
        \endcode

    */

    template<typename Function>
    static Function integral(const Function & f)throw(pexception)
    {
        return pop::ProcessingAdvanced::integral(f,Loki::Int2Type<Function::DIM>());
    }
    /*! \fn  Function integralPower2(const Function & f)throw(pexception)
     *  \brief  integral of the matrix http://research.microsoft.com/~viola/Pubs/Detect/violaJones_IJCV.pdf
     * \param f input function
     * \return output function noted h
     *
     *  \f$ h(x)=\int_{0\leq x'\leq x} f(x')^2 dx'\f$
        \code
    Mat2UI8 img;
    img.load("/home/vincent/Desktop/test.pgm");

    Mat2UI32 imgl;
    imgl=img;
    imgl = Processing::integralPower2(imgl);
    img = Processing::greylevelRange(imgl,0,255);
    img.display();
        \endcode

    */

    template<typename Function>
    static Function integralPower2(const Function & f)throw(pexception)
    {
        return pop::ProcessingAdvanced::integral(f.multTermByTerm(f),Loki::Int2Type<Function::DIM>());
    }

    /*! \fn static Function mask(const Function & f,const FunctionMask & mask)throw(pexception)
     *  \brief  set to 0 the value outside the mask
     * \param f input function
     * \param mask mask function
     \return output function
     *
     *  h(x)= f(x) for mask(x) neq 0, 0 otherwise
    */

    template<typename Function,typename FunctionMask>
    static Function mask(const Function & f,const FunctionMask & mask)throw(pexception)
    {
        FunctionAssert(f,mask,"In processing::mask");
        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        return pop::ProcessingAdvanced::mask(f,mask,it);
    }

    //@}
    //-------------------------------------
    //
    //! \name Neighborhood
    //@{
    //-------------------------------------


    /*! \fn Vec<typename Function::E>  minimaLocal(const Function & f,double radius=1,double norm=0)throw(pexception)
     *  \brief local minima of the input matrix
     * \param f input function
     * \param radius ball radius
     * \param norm ball norm (norm=2 for disk)
     * \return vector of minima
     *
     *  local minima of the input matrix
     * \code
    Mat3UI8 m(6,6,6);
    DistributionUniformInt d(0,255);
    Processing::randomField(m.getDomain(),d,m);
    Mat3UI8::IteratorERectangle itd = m.getIteratorERectangle(1,m.getDomain()-2);
    Mat3UI8::IteratorENeighborhood itn = m.getIteratorENeighborhood(1,0);
    Vec<Mat3UI8::E> v_x =  ProcessingAdvanced::minimaLocal(m,itd,itn);
    cout<<m<<endl;
    for(int i =0;i<v_x.size();i++)
        cout<<v_x[i]<<endl;
     *  \endcode
     */
    template< typename Function>
    static Vec<typename Function::E> minimaLocal(const Function & f,double radius=1,double norm=0)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(radius,norm));
        return pop::ProcessingAdvanced::minimaLocal(f,itg,itn);
    }
    /*!
     *  \brief local minima of the input matrix
     * \param f input minima
     * \param radius ball radius
     * \param norm ball norm (norm=2 for disk)
     * \return map of minima
     *
     *  local minima of the input matrix
     * \code
    Mat2UI8 lena("../Lena.bmp");
    Mat2F64 grad = Processing::gradientMagnitudeDeriche(Mat2F64(lena),0.5);
    Mat2UI8 minima= Processing::minimaLocalMap(grad);//minima
    Mat2UI32 label  = Processing::clusterToLabel(minima);//each minima gets a label
    Mat2UI32 water = Processing::watershed(label,Mat2UI8(grad));//watershed transformation
    Visualization::labelToRandomRGB(water).display("watershed transformation",true,false);//well-over segmentation of watershed transformation
     *  \endcode
     */
    template< typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,UI8 >::Result minimaLocalMap(const Function & f,double radius=1,double norm=0)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(radius,norm));
        return pop::ProcessingAdvanced::minimaLocalMap(f,itg,itn);
    }

    /*! \fn Vec<typename Function::E>  maximaLocal(const Function & f,double radius=1,double norm=0)throw(pexception)
     *  \brief local maxima of the input matrix
     * \param f input function
     * \param radius ball radius
     * \param norm ball norm (norm=2 for disk)
     * \return vector of minima
     *
     *  local maxima of the input matrix
     * \code
    Mat3UI8 m(6,6,6);
    DistributionUniformInt d(0,255);
    Processing::randomField(m.getDomain(),d,m);
    Mat3UI8::IteratorERectangle itd = m.getIteratorERectangle(1,m.getDomain()-2);
    Mat3UI8::IteratorENeighborhood itn = m.getIteratorENeighborhood(1,0);
    Vec<Mat3UI8::E> v_x =  ProcessingAdvanced::maximaLocal(m,itd,itn);
    cout<<m<<endl;
    for(int i =0;i<v_x.size();i++)
        cout<<v_x[i]<<endl;
     *  \endcode
     */
    template< typename Function>
    static Vec<typename Function::E> maximaLocal(const Function & f,double radius=1,double norm=0)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(radius,norm));
        return pop::ProcessingAdvanced::maximaLocal(f,itg,itn);
    }
    /*!
     *  \brief local maxima of the input matrix
     * \param f input function
     * \param radius ball radius
     * \param norm ball norm (norm=2 for disk)
     * \return maxima map
     *
     *  local maxima of the input matrix
     * \code
        Mat2UI8 img("/home/vincent/Desktop/500x.bmp");
        Mat2F64 grad = Processing::gradientMagnitudeDeriche(Mat2F64(img),1);
        Processing::maximaLocalMap(grad).display();
     *  \endcode
     */
    template< typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,UI8 >::Result maximaLocalMap(const Function & f,double radius=1,double norm=0)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(radius,norm));
        return pop::ProcessingAdvanced::maximaLocalMap(f,itg,itn);
    }
    /*!
     *  \brief local minima of the input matrix
     * \param f input function
     * \param radius ball radius
     * \param norm ball norm (norm=2 for disk)
     * \return vector of minima
     *
     *  local extrema of the input matrix
     * \code
    Mat3UI8 m(6,6,6);
    DistributionUniformInt d(0,255);
    Processing::randomField(m.getDomain(),d,m);
    Mat3UI8::IteratorERectangle itd = m.getIteratorERectangle(1,m.getDomain()-2);
    Mat3UI8::IteratorENeighborhood itn = m.getIteratorENeighborhood(1,0);
    Vec<Mat3UI8::E> v_x =  ProcessingAdvanced::extremaLocal(m,itd,itn);
    cout<<m<<endl;
    for(int i =0;i<v_x.size();i++)
        cout<<v_x[i]<<endl;
     *  \endcode
     */
    template< typename Function>
    static Vec<typename Function::E> extremaLocal(const Function & f,double radius=1,double norm=0)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(radius,norm));
        return pop::ProcessingAdvanced::extremaLocal(f,itg,itn);
    }
    /*!
     *  \brief local extrama of the input matrix
     * \param f input function
     * \param radius ball radius
     * \param norm ball norm (norm=2 for disk)
     * \return maxima map
     *
     *  local maxima of the input matrix
     * \code
        Mat2UI8 img("/home/vincent/Desktop/500x.bmp");
        Mat2F64 grad = Processing::gradientMagnitudeDeriche(Mat2F64(img),1);
        Processing::extremaLocalMap(grad).display();
     *  \endcode
     */
    template< typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,UI8 >::Result extremaLocalMap(const Function & f,double radius=1,double norm=0)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(radius,norm));
        return pop::ProcessingAdvanced::extremaLocalMap(f,itg,itn);
    }


    /*! \fn Function erosion(const Function & f,double radius,double norm=2)throw(pexception)
     *  \brief erosion of the input matrix
     * \param f input function
     * \param radius ball radius
     * \param norm ball norm (norm=2 for disk)
     * \return h output function
     *
     *  erosion of the input matrix:\n
     * \f$\forall x \in E:\quad h(x) =\min_{\forall x'\in N(x) }f(x') \f$ where \f$N(x)=\{x': \|x'-x\|_n<=r\} \f$. For instance,
     * \code
     * Mat2RGBUI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
     * Processing processing;
     * Mat2RGBUI8 erosion =processing.erosion(img,3,2);
     * erosion.display();
     *  \endcode
     */
    template< typename Function>
    static Function erosion(const Function & f,double radius,double norm=2)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(radius,norm));
        return pop::ProcessingAdvanced::erosion(f,itg,itn);
    }
    /*! \fn Function erosionStructuralElement(const Function & f,const FunctionStructuralElement & struct,int dilate=1)throw(pexception)
     *  \brief erosion of the input matrix
     * \param f input function
     * \param struct input structural element
     * \param dilate number of dilation of the structural element
     * \return h output function
     *
     *  erosion of the input matrix:\n
     * \f$\forall x \in E:\quad h(x) =\min_{\forall x'\in N(x) }f(x') \f$ where the neighborhood is defined using the iterative stuctural element, \f$S^n\f$,\n
     * The initial stuctural element \f$ S= \{x :\mbox{struct}(x-center)\neq 0\}\f$ with center the center domain of the matrix.\n
     * For instance, with struct=\f$\begin{pmatrix} 0 & 0 & 0\\0 & 255 & 0\\0 & 0 & 255\end{pmatrix}\f$, we have \f$ S=\{(0,0),(1,1)\}\f$.\n \n\n
     * The iterative stuctural element, \f$S^n\f$ is n times the dilation by itselt : \f$ S\oplus S\ldots \oplus S\f$ n times
     * \code
        Mat2RGBUI8 img;
        img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
        Mat2UI8 elt(3,3);
        elt(1,1)=1;
        elt(2,2)=1;
        Processing processing;
        Mat2RGBUI8 erosion =processing.erosionStructuralElement(img,elt,10);
        erosion.display();
     \endcode
    */
    template< typename Function,typename FunctionStructuralElement>
    static Function erosionStructuralElement(const Function & f,const FunctionStructuralElement & structural_element,int dilate=1)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(structural_element));

        if(dilate>1){
            typename Function::IteratorENeighborhood itinit (f.getIteratorENeighborhood(structural_element));
            for(int j=1;j<dilate;j++)
                itn.dilate(itinit);
        }
        return pop::ProcessingAdvanced::erosion(f,itg,itn);
    }


    /*! \fn Function dilation(const Function & f,double radius,double norm=2)throw(pexception)
     *  \brief dilation of the input matrix
     * \param f input function
     * \param radius ball radius
     * \param norm ball norm (norm=2 for disk)
     * \return h output function
     *
     *  dilation of the input matrix:\n
     * \f$\forall x \in E:\quad h(x) =\max_{\forall x'\in N(x) }f(x') \f$ where \f$N(x)=\{x': \|x'-x\|_n<=r\} \f$. For instance,
     * \code
     * Mat2RGBUI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
     * Processing processing;
     * Mat2RGBUI8 dilation =processing.dilation(img,3,2);
     * dilation.display();
     *  \endcode
     */
    template< typename Function>
    static Function dilation(const Function & f,double radius,double norm=2)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(radius,norm));
        return pop::ProcessingAdvanced::dilation(f,itg,itn);
    }
    /*! \fn Function dilationStructuralElement(const Function & f,const FunctionStructuralElement & struct,int dilate=1)throw(pexception)
     *  \brief dilation of the input matrix
     * \param f input function
     * \param struct input structural element
     * \param dilate number of dilation of the structural element
     * \return h output function
     *
     *  dilation of the input matrix:\n
     * \f$\forall x \in E:\quad h(x) =\max_{\forall x'\in N(x) }f(x') \f$ where the neighborhood is defined using the iterative stuctural element, \f$S^n\f$,\n
     * The initial stuctural element \f$ S= \{x :\mbox{struct}(x-center)\neq 0\}\f$ with center the center domain of the matrix.\n
     * For instance, with struct=\f$\begin{pmatrix} 0 & 0 & 0\\0 & 255 & 0\\0 & 0 & 255\end{pmatrix}\f$, we have \f$ S=\{(0,0),(1,1)\}\f$.\n \n\n
     * The iterative stuctural element, \f$S^n\f$ is n times the dilation by itselt : \f$ S\oplus S\ldots \oplus S\f$ n times
     * \code
                Mat2RGBUI8 img;
                img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
                Mat2UI8 elt(3,3);
                elt(1,1)=1;
                elt(2,2)=1;
                Processing processing;
                Mat2RGBUI8 erosion =processing.dilationStructuralElement(img,elt,10);
                erosion.display();
     \endcode
    */
    template< typename Function,typename FunctionStructuralElement>
    static Function dilationStructuralElement(const Function & f,const FunctionStructuralElement & structural_element,int dilate=1)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(structural_element));

        if(dilate>1){
            typename Function::IteratorENeighborhood itinit (f.getIteratorENeighborhood(structural_element));
            for(int j=1;j<dilate;j++)
                itn.dilate(itinit);
        }
        return pop::ProcessingAdvanced::dilation(f,itg,itn);
    }
    /*! \fn Function median(const Function & f,double radius,double norm=2)throw(pexception)
     *  \brief median of the input matrix
     * \param f input function
     * \param radius ball radius
     * \param norm ball norm (norm=2 for disk)
     * \return h output function
     *
     *  median of the input matrix:\n
     * \f$\forall x \in E:\quad h(x) =\mbox{median}_{\forall x'\in N(x) }f(x') \f$ where the operator median returns the median value of the list of input values and
     * \f$N(x)=\{x': \|x'-x\|_n<=r\} \f$. For instance,
     * \code
     * Mat2RGBUI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
     * Processing processing;
     * Mat2RGBUI8 median =processing.median(img,3,2);
     * median.display();
     *  \endcode
     */
    template< typename Function>
    static Function median(const Function & f,double radius,double norm=2)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(radius,norm));
        return pop::ProcessingAdvanced::median(f,itg,itn);
    }
    /*! \fn Function medianStructuralElement(const Function & f,const FunctionStructuralElement & struct,int dilate=1)throw(pexception)
     *  \brief median of the input matrix
     * \param f input function
     * \param struct input structural element
     * \param dilate number of dilation of the structural element
     * \return h output function
     *
     *  median of the input matrix:\n
     * \f$\forall x \in E:\quad h(x) =\mbox{median}_{\forall x'\in N(x) }f(x') \f$ where the operator median returns the median value of the list of input values and
     * the neighborhood is defined using the iterative stuctural element, \f$S^n\f$,\n
     * The initial stuctural element \f$ S= \{x :\mbox{struct}(x-center)\neq 0\}\f$ with center the center domain of the matrix.\n
     * For instance, with struct=\f$\begin{pmatrix} 0 & 0 & 0\\0 & 255 & 0\\0 & 0 & 255\end{pmatrix}\f$, we have \f$ S=\{(0,0),(1,1)\}\f$.\n \n\n
     * The iterative stuctural element, \f$S^n\f$ is n times the median by itselt : \f$ S\oplus S\ldots \oplus S\f$ n times
     * \code
     * Mat2RGBUI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
     * Mat2RGBUI8 elt(3,3);
     * elt(1,1)=1;
     * elt(2,2)=1;
     * Processing processing;
     * Mat2RGBUI8 median =processing.medianStructuralElement(img,elt,10);
     * median.display();
     \endcode
    */
    template< typename Function,typename FunctionStructuralElement>
    static Function medianStructuralElement(const Function & f,const FunctionStructuralElement & structural_element,int dilate=1)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(structural_element));

        if(dilate>1){
            typename Function::IteratorENeighborhood itinit (f.getIteratorENeighborhood(structural_element));
            for(int j=1;j<dilate;j++)
                itn.dilate(itinit);
        }
        return pop::ProcessingAdvanced::median(f,itg,itn);
    }
    /*! \fn Function mean(const Function & f,double radius,double norm=2)throw(pexception)
     *  \brief mean of the input matrix
     * \param f input function
     * \param radius ball radius
     * \param norm ball norm (norm=2 for disk)
     * \return h output function
     *
     *  mean of the input matrix:\n
     * \f$\forall x \in E:\quad h(x) =\mbox{mean}_{\forall x'\in N(x) }f(x') \f$ where \f$N(x)=\{x': \|x'-x\|_n<=r\} \f$. For instance,
     * \code
     * Mat2RGBUI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
     * Processing processing;
     * Mat2RGBUI8 mean =processing.mean(img,3,2);
     * mean.display();
     *  \endcode
     */
    template< typename Function>
    static Function mean(const Function & f,double radius,double norm=2)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(radius,norm));
        return pop::ProcessingAdvanced::mean(f,itg,itn);
    }
    /*! \fn Function meanStructuralElement(const Function & f,const FunctionStructuralElement & struct,int dilate=1)throw(pexception)
     *  \brief mean of the input matrix
     * \param f input function
     * \param struct input structural element
     * \param dilate number of dilation of the structural element
     * \return h output function
     *
     *  mean of the input matrix:\n
     * \f$\forall x \in E:\quad h(x) =\mbox{mean}_{\forall x'\in N(x) }f(x') \f$ where the neighborhood is defined using the iterative stuctural element, \f$S^n\f$,\n
     * The initial stuctural element \f$ S= \{x :\mbox{struct}(x-center)\neq 0\}\f$ with center the center domain of the matrix.\n
     * For instance, with struct=\f$\begin{pmatrix} 0 & 0 & 0\\0 & 255 & 0\\0 & 0 & 255\end{pmatrix}\f$, we have \f$ S=\{(0,0),(1,1)\}\f$.\n \n\n
     * The iterative stuctural element, \f$S^n\f$ is n times the mean by itselt : \f$ S\oplus S\ldots \oplus S\f$ n times
     * \code
     * Mat2RGBUI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
     * Mat2RGBUI8 elt(3,3);
     * elt(1,1)=1;
     * elt(2,2)=1;
     * Processing processing;
     * Mat2RGBUI8 mean =processing.meanStructuralElement(img,elt,10);
     * mean.display();
     \endcode
    */
    template< typename Function,typename FunctionStructuralElement>
    static Function meanStructuralElement(const Function & f,const FunctionStructuralElement & structural_element,int dilate=1)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(structural_element));

        if(dilate>1){
            typename Function::IteratorENeighborhood itinit (f.getIteratorENeighborhood(structural_element));
            for(int j=1;j<dilate;j++)
                itn.dilate(itinit);
        }
        return pop::ProcessingAdvanced::mean(f,itg,itn);
    }
    /*! \fn Function closing(const Function & f,double radius,double norm=2)throw(pexception)
     *  \brief closing of the input matrix
     * \param f input function
     * \param radius ball radius
     * \param norm ball norm (norm=2 for disk)
     * \return h output function
     *
     *  closing of the input matrix:\n
     *  h =eorion(dilation(f,radius,norm),radius,norm)
     * \code
     * Mat2RGBUI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
     * Processing processing;
     * Mat2RGBUI8 closing =processing.closing(img,3,2);
     * closing.display();
     *  \endcode
     * \sa dilation erosion
     */
    template< typename Function>
    static Function closing(const Function & f,double radius,double norm=2)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(radius,norm));
        return pop::ProcessingAdvanced::closing(f,itg,itn);
    }
    /*! \fn Function closingStructuralElement(const Function & f,const FunctionStructuralElement & struct,int dilate=1)throw(pexception)
     *  \brief closing of the input matrix
     * \param f input function
     * \param struct input structural element
     * \param dilate number of dilation of the structural element
     * \return h output function
     *
     *  closing of the input matrix:\n
     *  h =erosion(dilation(f,struct,dilate),struct,dilate)

     * \code
                Mat2RGBUI8 img;
                img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
                Mat2UI8 elt(3,3);
                elt(1,1)=1;
                elt(2,2)=1;
                elt(0,0)=1;
                Processing processing;
                Mat2RGBUI8 closing =processing.closingStructuralElement(img,elt,10);
                closing.display();
     * \endcode
     * \sa dilation erosion
    */
    template< typename Function,typename FunctionStructuralElement>
    static Function closingStructuralElement(const Function & f,const FunctionStructuralElement & structural_element,int dilate=1)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(structural_element));

        if(dilate>1){
            typename Function::IteratorENeighborhood itinit (f.getIteratorENeighborhood(structural_element));
            for(int j=1;j<dilate;j++)
                itn.dilate(itinit);
        }
        return pop::ProcessingAdvanced::closing(f,itg,itn);
    }

    /*! \fn Function opening(const Function & f,double radius,double norm=2)throw(pexception)
     *  \brief opening of the input matrix
     * \param f input function
     * \param radius ball radius
     * \param norm ball norm (norm=2 for disk)
     * \return h output function
     *
     *  opening of the input matrix:\n
     *  h =eorion(dilation(f,radius,norm),radius,norm)
     * \code
     * Mat2RGBUI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
     * Processing processing;
     * Mat2RGBUI8 opening =processing.opening(img,3,2);
     * opening.display();
     *  \endcode
     * \sa dilation erosion
     */
    template< typename Function>
    static Function opening(const Function & f,double radius,double norm=2)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(radius,norm));
        return pop::ProcessingAdvanced::opening(f,itg,itn);
    }
    /*! \fn Function openingStructuralElement(const Function & f,const FunctionStructuralElement & struct,int dilate=1)throw(pexception)
     *  \brief opening of the input matrix
     * \param f input function
     * \param struct input structural element
     * \param dilate number of opening of the structural element
     * \return h output function
     *
     *  opening of the input matrix:\n
     *  h =erosion(dilation(f,struct,dilate),struct,dilate)

     * \code
                Mat2RGBUI8 img;
                img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
                Mat2UI8 elt(3,3);
                elt(1,1)=1;
                elt(2,2)=1;
                elt(0,0)=1;
                Processing processing;
                Mat2RGBUI8 closing =processing.openingStructuralElement(img,elt,10);
                closing.display();
     * \endcode
     * \sa dilation erosion
    */
    template< typename Function,typename FunctionStructuralElement>
    static Function openingStructuralElement(const Function & f,const FunctionStructuralElement & structural_element,int dilate=1)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(structural_element));

        if(dilate>1){
            typename Function::IteratorENeighborhood itinit (f.getIteratorENeighborhood(structural_element));
            for(int j=1;j<dilate;j++)
                itn.dilate(itinit);
        }
        return pop::ProcessingAdvanced::opening(f,itg,itn);
    }



    /*! \fn Function alternateSequentialCO(const Function1 & f,int maxradius,int norm=2)throw(pexception)
     *  \brief Sequential Alternate filter of the input matrix
     * \param f input function
     * \param maxradius max radius
     * \param norm norm of the ball
     * \return h output function
     *
     *  Successive application of closing and opening by increasing the radius of the ball until max radius
     * \code
        Mat2UI8 iex;
        iex.load("../iex.png");
        iex = Processing::alternateSequentialCO(iex,5,2);
        iex.save("../doc/image2/iexalternateCO.jpg");
     * \endcode
     * \image html iexalternateCO.jpg
     *
    */
    template<typename Function>
    static Function alternateSequentialCO(const Function & f,int maxradius, int norm=2)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(1,norm));
        return pop::ProcessingAdvanced::alternateSequentialCO(f,itg,itn,maxradius);
    }
    /*! \fn Function alternateSequentialCOStructuralElement(const Function1 & f,int maxradius, const FunctionStructuralElement & structural_element)throw(pexception)
     *  \brief Sequential Alternate filter of the input matrix
     * \param f input function
     * \param maxradius max radius
     * \param structural_element structural element
     * \return h output function
     *
     *  Successive application of closing and opening by increasing the radius of the initial structural element until max radius.


    */
    template<typename Function,typename FunctionStructuralElement>
    static Function alternateSequentialCOStructuralElement(const Function & f,int maxradius, const FunctionStructuralElement & structural_element)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(structural_element));
        return pop::ProcessingAdvanced::alternateSequentialCO(f,itg,itn,maxradius);
    }

    /*! \fn Function alternateSequentialOC(const Function1 & f,int maxradius,int norm=2)throw(pexception)
     *  \brief Sequential Alternate filter of the input matrix
     * \param f input function
     * \param maxradius max radius
     * \param norm norm of the ball
     * \return h output function
     *
     *  Successive application of opening and opening  by increasing the radius of the ball until max radius.

     * \code
                Mat2UI8 iex;
                iex.load("/home/vincent/Desktop/Population/doc/image/iex.pgm");
                iex = Processing::alternateSequentialOC(iex,5);
                iex.display();
     * \endcode
    */
    template<typename Function>
    static Function alternateSequentialOC(const Function & f,int maxradius, int norm=2)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(1,norm));
        return pop::ProcessingAdvanced::alternateSequentialOC(f,itg,itn,maxradius);
    }
    /*! \fn Function alternateSequentialOCStructuralElement(const Function1 & f,int maxradius, const FunctionStructuralElement & structural_element)throw(pexception)
     *  \brief Sequential Alternate filter of the input matrix
     * \param f input function
     * \param maxradius max radius
     * \param structural_element structural element
     * \return h output function
     *
     *  Successive application of opening and opening by increasing the radius of the initial structural element until max radius
    */
    template<typename Function,typename FunctionStructuralElement>
    static Function alternateSequentialOCStructuralElement(const Function & f,int maxradius, const FunctionStructuralElement & structural_element)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn (f.getIteratorENeighborhood(structural_element));
        return pop::ProcessingAdvanced::alternateSequentialOC(f,itg,itn,maxradius);
    }
    /*! \fn static Function hitOrMiss(const Function & f,const FunctionStructuralElement & C,const  FunctionStructuralElement & D)throw(pexception)
     *  \brief Hit or miss filter
     * \param f input function
     * \param C   structuring element C
     * \param D  structuring element D
     * \return h output function
     *
     *  \f$ H = (X\ominus C)\cap (X^c\ominus D) \f$ with \f$\ominus\f$ the erosion, \f$X=\{x:f(x)\neq 0 \} \f$ and \f$H=\{x:h(x)\neq 0 \}\f$
     * For instance a direct implementation of the thinning algorithm http://en.wikipedia.org/wiki/Hit-or-miss_transform is
     * \code
        Mat2UI8 img;

        img.load("/home/vincent/Desktop/Population/doc/image/outil.bmp");
        img = Processing::threshold(img,150);
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
        MatNDisplay d(temp);
        while(nbr_equal<8){
            temp2 = Processing::hitOrMiss(temp,C_1,D_1);
            temp = temp -temp2;
            C_1 = GeometricalTransformation::rotate(C_1,algebra.generate2DRotation(PI/2),Vec2I32(1,1) );
            C_1 =Processing::threshold(C_1,125);//due to the interpolation with the rotation, the value can fluctuate to remove these fluctuation, we apply a threshold
            D_1 = GeometricalTransformation::rotate(D_1,algebra.generate2DRotation(PI/2),Vec2I32(1,1) );
            D_1 =Processing::threshold(D_1,125);

            temp2 = Processing::hitOrMiss(temp,C_2,D_2);
            temp = temp -temp2;
            C_2 = GeometricalTransformation::rotate(C_2,algebra.generate2DRotation(PI/2),Vec2I32(1,1) );
            C_2 =Processing::threshold(C_2,125);

            D_2 = GeometricalTransformation::rotate(D_2,algebra.generate2DRotation(PI/2),Vec2I32(1,1) );
            D_2 =Processing::threshold(D_2,125);

            d.display(temp);
            if(temp==img){
                nbr_equal++;
            }else{
                nbr_equal=0;
            }
            img =temp;
        }
    \endcode
    \image html outil.png
    \image html thinning.png
    */

    template< typename Function, typename FunctionStructuralElement >
    static Function hitOrMiss(const Function & f,const FunctionStructuralElement & C,const  FunctionStructuralElement & D)throw(pexception)
    {
        typename Function::IteratorEDomain itg (f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itC (f.getIteratorENeighborhood(C));
        typename Function::IteratorENeighborhood itD (f.getIteratorENeighborhood(D));
        return pop::ProcessingAdvanced::hitOrMiss(f,itg,itC,itD);
    }

    /*!
     *  \brief mean shift filter http://xphilipp.developpez.com/articles/meanshift/
     * \param in input function
     * \param radius_E  spatial windows size
     * \param radius_F  feature windows size
     * \param nbr_loop_max maximum number of  displacements in mean shift algorithm
     * \return h output function
     *
     * \code
    Mat2UI8 img;
    img.load("lena.jpg");
    Processing::meanShiftFilter(img).display();
    \endcode
    \todo implementation for RGB matrix
    */
    template< typename Function>
    static Function meanShiftFilter(const Function & in, double radius_E=10,double radius_F=20,int nbr_loop_max=5){
        typename Function::IteratorEDomain itd(in.getIteratorEDomain());

        typename Function::IteratorENeighborhood itn(in.getIteratorENeighborhood(radius_E,2));
        Function out(in);

        while(itd.next()){
            std::vector<double> caracteristic(normValue(NumericLimits<typename Function::F>::maximumRange())+1);
            itn.init(itd.x());
            while(itn.next()){
                caracteristic[normValue(in(itn.x()))]++;
            }
            ;
            int current_index=0;
            double move=0;
            do{
                double value = normValue(out(itd.x()));
                double mean=0;
                double sum=0;
                for(int v=value-radius_F;v<=value+radius_F;v++){
                    if(v<0||v>=(int)caracteristic.size()){
                        continue;
                    }
                    sum+=caracteristic[v];
                    mean+=(caracteristic[v]*v);

                }
                mean/=sum;
                move=absolute(mean-value);
                out(itd.x()) =round( mean);
                current_index++;
            }while(current_index<nbr_loop_max&&move>0.1);
        }
        return out;
    }
    /*!
     *  \brief toggle mapping enhancement with erosion and dilation
     * \param in input function
     * \param radius neighborhood radius
     * \param norm  norm of ball
     * \return h output function
     *
     * \code
    std::string path_linux ="../../../windsurf.jpg";
    Mat2RGBUI8 img(path_linux.c_str());
    img = Processing::enhanceContrastToggleMapping(img,15);
    img.display();
     * \endcode
     * \image html windsurf.jpg
     * \image html windsurf_enhance.jpg
     */
    template<typename Function>
    static Function enhanceContrastToggleMapping(const Function& f,int radius=2,double norm=2){
        Function erosion  = Processing::erosion(f,radius,norm);
        Function dilation = Processing::dilation(f,radius,norm);
        ForEachDomain(x,f){
            if(pop::normValue(f(x)-erosion(x))<pop::normValue(dilation(x)-f(x)))
                dilation(x)= erosion(x);
        }
        return dilation;
    }

    //@}
    //-------------------------------------
    //
    //! \name Convolution
    //@{
    //-------------------------------------
    /*! \fn Function1 convolution(const Function1 & f, const Function2 & kernel,int boundarycondition=0)
     *  \brief convolution of f with the kernel
     * \param f input function
     * \param kernel input kernel
     * \param boundarycondition  0=bounded, 1, mirror
     * \return h output function
     *
     * The convolution of the functions \f$f\f$ and \f$k\f$ is defined as the integral:\n
     *   \f$(f\ast k) (x)= \int_{R^n} f(x-x') \cdot k(x') \, dx'\f$\n
     * where the function \f$f\f$ is the input matrix and the function \f$k\f$ is the kernel.\n
     * In order to compute the convolution in discrete space, the basic idea is the truncation of the infinite support of the filtering function by a finite support,a window of some finite size and shape.
     * Then this windows is scanned across the matrix. The output pixel value is the weighted sum of the input pixels within the window where the weights are the values of the filter assigned
     * to every pixel of the window itself.\n
     * For instance, the convolution with a home-made kernel:
     \code
    double d[]=
    {
        0.5, 1, 0.5,
        1  , 2,   1,
        0.5, 1, 0.5
    };
    Mat2F64 kernel(Vec2I32(3,3),d);
    kernel = kernel/normValue(kernel);
    Mat2RGBUI8 lena;
    lena.load("/home/vtariel/Bureau/Population/bin/lena.jpg");
    for(int i =0;i<10;i++)
       lena = Processing::convolution(lena,kernel);
    lena.display();
         \endcode
     * For instance, the code for the sobel filter for 2d grey matrix is as follows
     * \code
     *     Mat2UI8 img;
     *     img.load("/home/vincent/Desktop/Population/doc/image/Lena.pgm");
     *     Mat2F64 kernel_sobel_derivate_x(3,3);//create the sobel kernel
     *     kernel_sobel_derivate_x(0,0)=-1;kernel_sobel_derivate_x(1,0)=0;kernel_sobel_derivate_x(2,0)=1;
     *     kernel_sobel_derivate_x(0,1)=-2;kernel_sobel_derivate_x(1,1)=0;kernel_sobel_derivate_x(2,1)=2;
     *     kernel_sobel_derivate_x(0,2)=-1;kernel_sobel_derivate_x(1,2)=0;kernel_sobel_derivate_x(2,2)=1;
     *     Mat2F64 imgf(img);//The ouput value can be negative, so we work on float pixel type for each rgb channel
     *     Processing processing;
     *     LinearAlgebra algebra;
     *     Mat2F64 derivate_x =  processing.convolution(imgf,kernel_sobel_derivate_x,1);//derivate in x-direction with convolution having mirror condition
     *     kernel_sobel_derivate_x.rotate(algebra.generate2DRotation(PI/2),Vec2I32(1,1) );//Rotate the kernel by an angle PI/2 with the center = image center
     *     Mat2F64 derivate_y =  processing.convolution(imgf,kernel_sobel_derivate_x,1);//derivate in x-direction with convolution having mirror condition
     *     MatN<2,Mat2F64 >::IteratorEDomain it(img.getIteratorEDomain());//global iteration
     *     while(it.next()){
     *        F64 value = std::sqrt(derivate_x(it.x())*derivate_x(it.x()) +derivate_y(it.x())*derivate_y(it.x()) );//gradient magnitude
     *        img(it.x())= ArithmeticsSaturation<UI8,F64 >::Range(value); //ArithmeticsSaturation to avoid the arithmic overflow artifact
     *     }
     *     img.display();
     *  \endcode
    */
    template<typename Function1,typename Function2>
    static Function1 convolution(const Function1 & f, const Function2 & kernel,int boundarycondition=0)
    {
        typename Function1::IteratorEDomain itg (f.getIteratorEDomain());
        return pop::ProcessingAdvanced::convolution(f, kernel, itg, boundarycondition);
    }
    /*! \fn Function1 gradientMagnitudeSobel(const Function1 & f)
     *  \brief Magnitude of the Sobel's gradient
     * \param f input function
     * \return h output function
     *
     *  Magnitude of the Sobel's gradient http://en.wikipedia.org/wiki/Sobel_operator
     * \code
        Mat2RGBUI8 img;
         img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
         Processing processing;
         img = processing.gradientMagnitudeSobel(img);
         img.display();
     *  \endcode
    */
    template<class Function>
    static Function gradientMagnitudeSobel(const Function & f)
    {
        return pop::ProcessingAdvanced::gradNormSobel(f, f.getIteratorEDomain());
    }
    /*!
     *  \brief Sobel's gradient in the given direction
     * \param f input function
     * \param direction coordinate direction
     * \return h output function
     *
     *  Sobel's gradient http://en.wikipedia.org/wiki/Sobel_operator
     * \code
        Mat2UI8 img;
        img.load("/home/vtariel/Bureau/Lena.png");
        Mat2F64 gradx = Processing::gradientSobel(img,0);
        gradx.display();
     *  \endcode
    */
    template<class Function1>
    static typename FunctionTypeTraitsSubstituteF<Function1,typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64 >::Result >::Result gradientSobel(const Function1 & f,int direction)
    {
        typename Function1::IteratorEDomain itg (f.getIteratorEDomain());
        if(NumericLimits<typename Function1::F>::is_integer==false){
            return pop::ProcessingAdvanced::gradSobel(f,direction, itg);
        }
        else
        {
            typedef typename FunctionTypeTraitsSubstituteF<Function1,typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64 >::Result >::Result FunctionFloat;
            FunctionFloat ffloat(f);
            return pop::ProcessingAdvanced::gradSobel(ffloat,direction, itg);
        }
    }
    /*!
     *  \brief Vector field of Sobel's gradient
     * \param f input function
     * \return h output function
     *
     *  Sobel's gradient http://en.wikipedia.org/wiki/Sobel_operator
     * \code
    Mat2UI8 lena("../Lena.bmp");
    lena = Processing::smoothDeriche(lena,0.3);
    Mat2Vec2F64 grad = Processing::gradientVecSobel(lena);
    Visualization::vectorField2DToArrows(grad,RGBUI8(0,0,255),RGBUI8(255,0,0),16).display();
     *  \endcode
    */
    template<class Function1>
    static typename FunctionTypeTraitsSubstituteF<Function1,VecN<Function1::DIM,F64> >::Result gradientVecSobel(const Function1  & f)
    {
        typedef typename FunctionTypeTraitsSubstituteF<Function1,F64 >::Result  FunctionFloat;
        VecN<Function1::DIM,FunctionFloat> v_der;
        for(int i =0;i<Function1::DIM;i++){
            v_der[i]=  gradientSobel(f,i);

        }
        typename FunctionTypeTraitsSubstituteF<Function1,VecN<Function1::DIM,F64> >::Result f_grad(f.getDomain());
        Convertor::fromVecN(v_der,f_grad);
        return f_grad;
    }
    /*! \fn static Function1 gradientMagnitudeGaussian(const Function1 & f,F64 sigma=1)
     *  \brief Magnitude of the gaussian gradient
     * \param f input function
     * \param sigma standard deviation
     * \return h output function
     *
     *  Magnitude of the gaussian
     * \code
        Mat2RGBUI8 img;
        img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
        Mat2UI8 grad;
        grad = Processing::gradientMagnitudeGaussian(img,2);
        grad.display();
     *  \endcode
    */
    template<class Function1>
    static Function1 gradientMagnitudeGaussian(const Function1 & f,F64 sigma=1)
    {
        typename Function1::IteratorEDomain itg (f.getIteratorEDomain());
        return pop::ProcessingAdvanced::gradNormGaussian(f, sigma,sigma*3,itg);
    }
    /*! \fn static Function1 gradientGaussian(const Function1 & f, int direction,F64 sigma=1)
     *  \brief Gaussian gradient
     * \param f input function
     * \param direction derived coordinate
     * \param sigma standard deviation
     * \return h output function
     *
     *  Gaussian gaussian in the follownig direction
    \code
        Mat2RGBUI8 img;
        img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
        Mat2RGBF64 imgf;
        imgf=img;
        imgf = Processing::gradientGaussian(imgf,0,2);
        img = Processing::greylevelRange(imgf,0,255);
        img.display();
    \endcode
     */
    template<class Function1>
    static typename FunctionTypeTraitsSubstituteF<Function1,typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64 >::Result >::Result gradientGaussian(const Function1 & f, int direction,F64 sigma=1)
    {
        typename Function1::IteratorEDomain itg (f.getIteratorEDomain());
        if(NumericLimits<typename Function1::F>::is_integer==false){
            return pop::ProcessingAdvanced::gradGaussian(f,direction, sigma,sigma*3,itg);
        }
        else
        {
            typedef typename FunctionTypeTraitsSubstituteF<Function1,typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64 >::Result >::Result FunctionFloat;
            FunctionFloat ffloat(f);
            return pop::ProcessingAdvanced::gradGaussian(ffloat,direction, sigma,sigma*3,itg);
        }
    }
    /*!
     *  \brief Vector field of a gradient smoothed by a gaussian
     * \param f input function
     * \param sigma  scale parameter
     * \return h output function
     *
     *  Gaussian gradient
     * \code
    Mat2UI8 lena("../Lena.bmp");
    Mat2Vec2F64 grad = Processing::gradientVecGaussian(Mat2F64(lena),8);
    Visualization::vectorField2DToArrows(grad,RGBUI8(0,0,255),RGBUI8(255,0,0),16).display();
     *  \endcode
    */
    template<class Function1>
    static typename FunctionTypeTraitsSubstituteF<Function1,VecN<Function1::DIM,F64> >::Result gradientVecGaussian(const Function1  & f,double sigma=1)
    {
        typename Function1::IteratorEDomain itg (f.getIteratorEDomain());
        if(NumericLimits<typename Function1::F>::is_integer==false){
            return ProcessingAdvanced::gradientVecGaussian(f,sigma);
        }
        else
        {
            typedef typename FunctionTypeTraitsSubstituteF<Function1,typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64 >::Result >::Result FunctionFloat;
            FunctionFloat ffloat(f);
            return pop::Processing::gradientVecGaussian(ffloat,sigma);
        }
    }

    /*! \fn Function1 smoothGaussian(const Function1 & f, F64 sigma)
     *  \brief Gaussian smooth
     * \param f input function
     * \param sigma standard deviation
     * \return h output function
     *
     *  Gaussian smooth
    \code
        Mat2RGBUI8 img;
        img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
        img = Processing::smoothGaussian(img,3,10);
        img.display();
    \endcode
     */
    template<typename Function1>
    static Function1 smoothGaussian(const Function1 & f, F64 sigma=1)
    {
        typename Function1::IteratorEDomain itg (f.getIteratorEDomain());

        //        typename FunctionTypeTraitsSubstituteF< Function1,typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64>::Result >::Result ffloat(f);
        //        Function1 ff;
        //        ff=

        return pop::ProcessingAdvanced::smoothGaussian(f, sigma,sigma*2.5,itg);
    }


//    template<typename Function1>
//    static Function1 smoothGaussian(const Function1 & f, F64 sigma_i=1,F64 sigma_j=1)
//    {
//        typename Function1::IteratorEDomain itg (f.getIteratorEDomain());

//        //        typename FunctionTypeTraitsSubstituteF< Function1,typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64>::Result >::Result ffloat(f);
//        //        Function1 ff;
//        //        ff=

//        return pop::ProcessingAdvanced::smoothGaussian(f, sigma,sigma*3,itg);
//    }


    //@}
    //-------------------------------------
    //
    //! \name Recursive
    //@{
    //-------------------------------------





    /*! \fn Function1 smoothDeriche(const Function1 & f, F64 alpha=1)
     * \brief Deriche's smooth filter
     * \param f input matrix
     * \param alpha inverse scale parameter
     * \return h output function
     *
     * Smooth the input matrix with the inverse scale parameter (alpha=2=low, alpha=0.5=high)
     * \code
     * Mat2RGBUI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
     * img = Processing::smoothDeriche(img,0.5);
     * img.display();
     * \endcode
     */
    template<typename Function1>
    static Function1 smoothDeriche(const Function1 & f, F64 alpha=1)
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

        return ProcessingAdvanced::recursiveAllDirections(f,funccausal,funcanticausal);
    }

    /*! \fn Function1 gradientDeriche(const Function1 & f, F64 alpha,I32 direction)
     * \brief Deriche's smooth filter
     * \param f input matrix used float type as pixel/voxel type
     * \param alpha inverse scale parameter
     * \param direction derivate in the following direction
     * \return h output function
     *
     * Derivate the input matrix in the following direction with the inverse scale parameter (alpha=2=low, alpha=0.5=high)
     * \code
        Mat2RGBUI8 img;
        img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
        Mat2RGBF64 gradx(img);
        gradx = Processing::gradientDeriche(gradx,0,1);//Calculate the gradient in the direction 0
        img = Processing::greylevelRange(gradx,0,255);//to display the matrix with a float type, the
        img.display();
     * \endcode
     * \image html LenaGradDeriche.jpg
     */
    template<typename Function1>
    static typename FunctionTypeTraitsSubstituteF<Function1,typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64 >::Result >::Result  gradientDeriche(const Function1 & f, I32 direction, F64 alpha=1)
    {
        if(NumericLimits<typename Function1::F>::is_integer==true){
            typedef typename FunctionTypeTraitsSubstituteF<Function1,typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64 >::Result >::Result FunctionFloat;
            FunctionFloat ffloat(f);
            return pop::Processing::gradientDeriche(ffloat, direction,alpha);
        }
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

        return ProcessingAdvanced::recursiveAllDirections(f,direction,funccausalsmooth,funcanticausalsmooth,funccausalgrad,funcanticausalgrad);
    }
    /*!
     *  \brief Vector field of Deriche's gradient
     * \param f input function
     * \param alpha inverse scale parameter
     * \return h output function
     *
     *  Deriche's gradient
     * \code
    Mat2UI8 img;
    img.load("/usr/share/doc/opencv-doc/examples/c/lena.jpg");
    Mat2Vec2F64 gradx = Processing::gradientVecDeriche(img);
    Visualization::vectorField2DToArrows(gradx,RGBUI8(0,0,255),RGBUI8(255,0,0),8).display();
     *  \endcode
    */
    template<class Function1>
    static typename FunctionTypeTraitsSubstituteF<Function1,VecN<Function1::DIM,F64> >::Result gradientVecDeriche(const Function1  & f,double alpha=1)
    {
        typedef typename FunctionTypeTraitsSubstituteF<Function1,F64 >::Result  FunctionFloat;
        VecN<Function1::DIM,FunctionFloat> v_der;
        for(int i =0;i<Function1::DIM;i++){
            v_der[i]= gradientDeriche(f,i,alpha);

        }
        typename FunctionTypeTraitsSubstituteF<Function1,VecN<Function1::DIM,F64> >::Result f_grad(f.getDomain());
        Convertor::fromVecN(v_der,f_grad);
        return f_grad;
    }
    /*! \fn Function1 gradientMagnitudeDeriche(const Function1 & f, F64 alpha=1)
     * \brief Deriche's smooth filter
     * \param f input matrix
     * \param alpha inverse scale parameter
     * \return h output function
     *
     * caculate the gradient magnitude following the Deriche's gradient (0.5=smooth gradient, 2=sharp gradient)
     * \code
        Mat2RGBUI8 img;
        img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
        img = Processing::gradientMagnitudeDeriche(img,1);//Calculate the magnitude of Deriche gradient
        img.display();
     * \endcode
     */
    template<typename Function1>
    static Function1 gradientMagnitudeDeriche(const Function1 & f, F64 alpha=1)throw(pexception)
    {

        typedef typename  FunctionTypeTraitsSubstituteF<typename Function1::F,F64>::Result TypeF64;

        typename FunctionTypeTraitsSubstituteF<Function1,TypeF64>::Result fdir(f.getDomain());
        typename FunctionTypeTraitsSubstituteF<Function1,TypeF64>::Result fsum(f.getDomain());
        for(I32 i=0;i <Function1::DIM;i++){
            fdir=f;
            fdir = Processing::gradientDeriche(fdir,i,alpha);
            fsum+= fdir.multTermByTerm(fdir);
        }
        Function1 g(fdir.getDomain());
        typename Function1::IteratorEDomain it (f.getIteratorEDomain());
        while(it.next())
        {
            g(it.x())=ArithmeticsSaturation<typename Function1::F,TypeF64>::Range(squareRoot(fsum(it.x())));
        }
        return g;
    }


    //@}
    //-------------------------------------
    //
    //! \name Seeds
    //@{
    //-------------------------------------

    /*! \fn static Function labelMerge(const Function & label1, const Function &label2)throw(pexception)
     *  \brief merge of the labelled matrix
     * \param label1 labelled matrix1
     * \param label2 labelled matrix2
     * \return h output labelled function
     *
     * Operate the merge of the two input labelled matrixs that can contain multi-labels
     *  \code
        Mat2UI8 img;
        img.load("/home/vincent/Desktop/Population/doc/image/iex.pgm");
        //filtering
        img = Processing::median(img,4);
        //seed localisation
        Mat2UI8 seed1 = Processing::threshold(img,0,100);//seed in the grains
        Mat2UI8 seed2 = Processing::threshold(img,160);//seed in the background
        Mat2UI8 seeds = Processing::labelMerge(seed1,seed2);//merge of the seeds
        //for a good segmentation, each seed should be included in its associated object and touch each component of its associated object
        //Test of the condition with a visal checking
        Mat2RGBUI8 RGB = pop::Visualization::labelForeground(seeds,img);
        RGB.display();
     *  \endcode
    */
    template<typename Function>
    static Function labelMerge(const Function & label1, const Function & label2)throw(pexception)
    {
        typename Function::IteratorEDomain it(label1.getIteratorEDomain());
        return pop::ProcessingAdvanced::labelMerge(label1,label2,it);
    }
    /*! \fn static Function2 labelFromSingleSeed(const Function1 & label,const Function2& seed)throw(pexception)
     *  \brief extract the label including the binary seed
     * \param label multi-labelled matrix
     * \param seed binary seed
     * \return h output binary matrix function
     *
     * From the multi-labelled matrix, we extract the label including the seed
     *  \code
        Mat2UI8 img;
        img.load("/home/vincent/Desktop/Population/doc/image/iex.pgm");
        //filtering
        img = Processing::median(img,4);
        //seed localisation
        Mat2UI8 seed1 = Processing::threshold(img,0,100);//seed in the grains
        Mat2UI8 seed2 = Processing::threshold(img,160);//seed in the background
        Mat2UI8 seeds = Processing::labelMerge(seed1,seed2);//merge of the seeds
        //for a good segmentation, each seed should be include in its associated object (here the grains and the background) and touch each component of its associated object
        //Test of the condition with a visal checking
//        Mat2RGBUI8 RGB = pop::Visualization::labelForeground(seeds,img);
//        RGB.display();

         //topographic surface is the magnitude gradient of the input matrix
        Mat2UI8 topo = Processing::gradientMagnitudeDeriche(img,0.5);
        //watershed as region growing on the topographic surface with seeds
        Mat2UI8 regions = Processing::watershed(seeds,topo);


        //test the agreement between visual segmentation and numerical one
        RGB = pop::Visualization::labelForeground(regions,img);
        RGB.display();

        //Extract the grain label
        Mat2UI8 grain = Processing::labelFromSingleSeed(regions,seeds);
        grain.display();
    * \endcode
    */
    template<typename Function1,typename Function2>
    static Function2 labelFromSingleSeed(const Function1 & label,const Function2& seed)throw(pexception)
    {
        typename Function1::IteratorEDomain it(label.getIteratorEDomain());
        return pop::ProcessingAdvanced::labelFromSingleSeed(label,seed,it);
    }


    //@}
    //-------------------------------------
    //
    //! \name Region growing
    //@{
    //-------------------------------------


    /*!
     * \brief classical edge detector algorithm
     * \param f input matrix
     * \param sigma_gaussian  gaussian factor
     * \param low low histogram threshold hysterisis
     * \param high high histogram threshold hysterisis
     * \return edge detector
     *
     * \code
        Mat2UI8 lena("../Lena.bmp");
        Processing::edgeDetectorCanny(lena).display("Cany's edge detector",true,false);
     * \endcode
    */
    template<typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,UI8    >::Result  edgeDetectorCanny(const Function& f,double sigma_gaussian=1,double low =2,double high=20){
        typedef typename FunctionTypeTraitsSubstituteF<Function,F64>::Result FunctionFloat;
        FunctionFloat grad;
        typename FunctionTypeTraitsSubstituteF<Function,UI8    >::Result maxima = ProcessingAdvanced::nonMaximumSuppression(f,sigma_gaussian,grad);
        typename FunctionTypeTraitsSubstituteF<Function,UI8    >::Result seed = minimum(maxima,Processing::threshold(grad,high));
        typename FunctionTypeTraitsSubstituteF<Function,UI8    >::Result propagation =minimum(maxima,Processing::threshold(grad,low));
        typename FunctionTypeTraitsSubstituteF<Function,UI8    >::Result thinning = Processing::voronoiTesselation(seed,propagation,1);
        return thinning;
    }

    /*! \fn FunctionBinary holeFilling( const FunctionBinary& bin,int norm=1)
     * \brief fill with white label the connected components surrounded by white label
     * \param bin input binary matrix
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
     * \return hole output matrix
     *
     *  hole filling of the input binary matrix
     *
     * \code
     * Mat2UI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/outil.bmp");
     * Mat2UI8 bin = Processing::holeFilling(img);
     * bin.display("hole filling",true,false);
     * \endcode
    */
    template<typename FunctionBinary>
    static FunctionBinary holeFilling( const FunctionBinary& bin,int norm=1)
    {
        return pop::ProcessingAdvanced::holeFilling(bin, bin.getIteratorENeighborhood(1,norm));
    }

    /*!
     * \brief classical Adams Bischof segmentation algorithm contolled by the tint
     * \param seed input seeds
     * \param topo topographic surface
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
     * \return  regions
     *
     *
     * Classical region growing algorithm  of Adams and Bischof such that the ordering attribute function is:
     * \f$ \delta(x,i) = \frac{|f(x)- \mu_i|}{\sigma_i}\f$   where  \f$\sigma_i = \sqrt[]{\frac{\sum_{y\in X_i}(f(y)-\mu_i)^2}{\sum_{x\in X_i}1}} \f$
     * with f the topograhic surface and X_i the region
     * \code
        Mat2UI8 img;
        img.load("/home/vincent/Desktop/Population/doc/image/iex.pgm");
        //filtering
        img = Processing::median(img,4);
        //seed localisation
        Mat2UI8 seed1 = Processing::threshold(img,0,100);//seed in the grains
        Mat2UI8 seed2 = Processing::threshold(img,160);//seed in the background
        Mat2UI8 seeds = Processing::labelMerge(seed1,seed2);//merge of the seeds
        //for a good segmentation, each seed should be include in its associated object (here the grains and the background) and touch each component of its associated object
        //Test of the condition with a visal checking
        Mat2RGBUI8 RGB = pop::Visualization::labelForeground(seeds,img);
//        RGB.display();

        //watershed as region growing on the topographic surface with seeds
        Mat2UI8 regions = Processing::regionGrowingAdamsBischofMeanOverStandardDeviation(seeds,img);


        //test the agreement between visual segmentation and numerical one
        RGB = pop::Visualization::labelForeground(regions,img);
        RGB.display();

        //Extract the grain label
        Mat2UI8 grain = Processing::labelFromSingleSeed(regions,seeds);
        grain.display();
     * \endcode

    */
    template<typename FunctionLabel,typename FunctionTopo>
    static FunctionLabel regionGrowingAdamsBischofMeanOverStandardDeviation(const FunctionLabel & seed,const FunctionTopo & topo, int norm=1 )
    {
        return pop::ProcessingAdvanced::regionGrowingAdamsBischofMeanOverStandardDeviation(seed,topo, seed.getIteratorENeighborhood(1,norm));
    }

    /*! \fn static FunctionLabel regionGrowingAdamsBischofMean(const FunctionLabel & seed, const FunctionTopo & topo,int norm=1 )
     * \brief classical Adams Bischof segmentation algorithm contolled by the tint
     * \param seed input seeds
     * \param topo topographic surface
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
     * \return  regions
     *
     *
     * Classical region growing algorithm  of Adams and Bischof such that the ordering attribute function is: \f$ \delta(x,i) = |f(x)- \mu_i|\f$ with f the topograhic surface and X_i the region
     * \code
        Mat2UI8 img;
        img.load("/home/vincent/Desktop/Population/doc/image/iex.pgm");
        //filtering
        img = Processing::median(img,4);
        //seed localisation
        Mat2UI8 seed1 = Processing::threshold(img,0,100);//seed in the grains
        Mat2UI8 seed2 = Processing::threshold(img,160);//seed in the background
        Mat2UI8 seeds = Processing::labelMerge(seed1,seed2);//merge of the seeds
        //for a good segmentation, each seed should be include in its associated object (here the grains and the background) and touch each component of its associated object
        //Test of the condition with a visal checking
        Mat2RGBUI8 RGB = pop::Visualization::labelForeground(seeds,img);
//        RGB.display();

        //watershed as region growing on the topographic surface with seeds
        Mat2UI8 regions = Processing::regionGrowingAdamsBischofMean(seeds,img);


        //test the agreement between visual segmentation and numerical one
        RGB = pop::Visualization::labelForeground(regions,img);
        RGB.display();

        //Extract the grain label
        Mat2UI8 grain = Processing::labelFromSingleSeed(regions,seeds);
        grain.display();
      * \endcode
      */
    template<typename FunctionLabel, typename FunctionTopo>
    static FunctionLabel regionGrowingAdamsBischofMean(const FunctionLabel & seed, const FunctionTopo & topo,int norm=1 )
    {
        return pop::ProcessingAdvanced::regionGrowingAdamsBischofMean(seed,topo, seed.getIteratorENeighborhood(1,norm));
    }
    /*! \fn static typename FunctionTypeTraitsSubstituteF<FunctionBinary,plabel>::Result clusterToLabel(const FunctionBinary & cluster,int norm=1)
     * \brief set a label to each connected component
     * \param cluster input binary matrix
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
     * \return label output labelling matrix
     *
     * Each cluster of the input binary matrix has a specific label in the output label matrix
     *
     * \code
        Mat2UI8 img;
        img.load("/home/vincent/Desktop/Population/doc/image/outil.bmp");
        Mat2UI32 label = Processing::clusterToLabel(img);
        Mat2RGBUI8 RGB = pop::Visualization::labelToRandomRGB(label);
        RGB.display();
     * \endcode
     *
     *
    */

    template<typename FunctionBinary>
    static typename FunctionTypeTraitsSubstituteF<FunctionBinary,UI32>::Result  clusterToLabel(const FunctionBinary & cluster,int norm=1)
    {
        return pop::ProcessingAdvanced::clusterToLabel(cluster, cluster.getIteratorENeighborhood(1,norm),cluster.getIteratorEDomain());
    }
    /*! \fn FunctionBinary clusterMax(const FunctionBinary & cluster,int norm=1)
     * \brief extract the connected component with size
     * \param cluster input binary matrix
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
      *\return  max cluster

     *
     * The output matrix is the max cluster of the input binary matrix
     * \code
        Mat2UI8 img;
        img.load("/home/vincent/Desktop/Population/doc/image/outil.bmp");
        img = Processing::clusterMax(img);
        img.display();
     * \endcode
    */
    template<typename FunctionBinary>
    static FunctionBinary clusterMax(const FunctionBinary & cluster,int norm=1)
    {
        return pop::ProcessingAdvanced::clusterMax(cluster, cluster.getIteratorENeighborhood(1,norm));
    }

    /*!
     * \brief extract the global minima
     * \param topo input topographic surface
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
     * \return  minima as labelled matrix
     *
     * The output labelled matrix is the minima of the input binary matrix such that each minumum has a specific label. This code is the swamping segmentation
     * \code
     * Mat2RGBUI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
     * Mat2UI8 grad;
     * grad = Processing::gradientMagnitudeDeriche(img,1);
     * grad = Processing::dynamic(grad,5);//Vertical filter
     * Mat2UI32 minima  = Processing::minimaRegional(grad);
     * Mat2UI32 water  = Processing::watershed(minima,grad);
     * img = pop::Visualization::labelAverageRGB(water,img);
     * img.display();
     * \endcode
     */

    template< typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,UI32>::Result minimaRegional(const Function & topo,int norm=1 )
    {
        return pop::ProcessingAdvanced::minimaRegional(topo, topo.getIteratorENeighborhood(1,norm));
    }
    /*! \fn FunctionLabel watershed(const FunctionLabel & seed, const FunctionTopo & topo, int norm=1 )
     * \brief watershed transformation by flooding
     * \param seed input seeds
     * \param topo input topographic surface (pixel/voxel-type=UI8,UI16)
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
     * \return  basins of the watershed transformation
     *
     * Watershed transformation on the topographic surface initialiased by the seeds withoutboundary
     * \code
     * Mat2RGBUI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
     * Mat2UI8 grad;
     * grad = Processing::gradientMagnitudeDeriche(img,1);
     * grad = Processing::dynamic(grad,5);
     * Mat2UI32 minima  = Processing::minimaRegional(grad);
     * Mat2UI32 water  = Processing::watershed(minima,grad);
     * img = pop::Visualization::labelAverageRGB(water,img);
     * img.display();
     * \endcode
     * \image html lenawater.png
     */

    template< typename FunctionLabel,typename FunctionTopo>
    static FunctionLabel watershed(const FunctionLabel & seed, const FunctionTopo & topo, int norm=1 )
    {
        if(NumericLimits<typename FunctionTopo::F>::maximumRange()>NumericLimits<UI16>::maximumRange()){
            typename FunctionTypeTraitsSubstituteF<FunctionTopo,UI16>::Result topoless;
            topoless = topo;
            return pop::ProcessingAdvanced::watershed(seed,topoless,topo.getIteratorENeighborhood(1,norm));
        }
        return pop::ProcessingAdvanced::watershed(seed,topo,topo.getIteratorENeighborhood(1,norm));
    }
    /*! \fn static FunctionLabel watershed(const FunctionLabel & seed, const FunctionTopo & topo, const FunctionMask & mask , int norm  )
     * \brief watershed transformation by flooding in a mask
     * \param seed input seed matrix
     * \param topo input topographic surface (pixel/voxel type=UI8,UI16)
     * \param mask mask matrix
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
     * \return  basins of the watershed transformation
     *
     * Watershed transformation on the topographic surface initialiased by the seeds restricted by the mask
     * \code
     * double porosity=0.3;
     * Distribution dnormal(10,5,"NORMAL");//Poisson generator
     * double moment_order_2 = pop::Statistics::moment(dnormal,2,0,50);
     * double surface_expectation = moment_order_2*3.14159265;
     * Vec2F64 domain(512);//2d field domain
     * double N=-std::log(porosity)/std::log(2.718)/surface_expectation;
     * ModelGermGrain2 grain = RandomGeometry::poissonPointProcess(domain,N);//generate the 2d Poisson VecNd process
     * RandomGeometry::sphere(grain,dnormal);
     * Mat2RGBUI8 lattice = RandomGeometry::continuousToDiscrete(grain);
     * lattice.display();
     * //DECOMPOSITION OF THE PORE SPACE IN TERM OF ELEMENTARY PORES
     * Mat2UI8 porespace;
     * porespace = lattice;
     * Mat2UI8 inverse(porespace);
     * inverse = inverse.opposite();
     * Mat2F64 dist = pop::Processing::distanceEuclidean(inverse);
     * Mat2UI16 distl;
     * distl= dist;
     * distl = distl.opposite();
     * distl = pop::Processing::dynamic(distl,2);
     * Mat2UI32 minima = pop::Processing::minimaRegional(distl,0);
     * Mat2UI32 water = pop::Processing::watershed(minima,distl,porespace,1);
     * Visualization::labelToRandomRGB(water).display();
     * Visualization::labelToRandomRGB(water).save("../doc/image2/watersplit.png");
     * \endcode
     * \image html watersplit.png
    */
    template<typename FunctionLabel,typename FunctionTopo, typename FunctionMask >
    static FunctionLabel watershed(const FunctionLabel & seed, const FunctionTopo & topo, const FunctionMask & mask , int norm )
    {
        if(NumericLimits<typename FunctionTopo::F>::maximumRange()>NumericLimits<UI16>::maximumRange()){
            typename FunctionTypeTraitsSubstituteF<FunctionTopo,UI16>::Result topoless;
            topoless = topo;
            return pop::ProcessingAdvanced::watershed(seed,topoless,mask,topo.getIteratorENeighborhood(1,norm));
        }

        return pop::ProcessingAdvanced::watershed(seed,topo,mask,topo.getIteratorENeighborhood(1,norm));
    }
    /*! \fn FunctionLabel watershedBoundary(const FunctionLabel & seed, const FunctionTopo & topo, int norm=1 )
     * \brief watershed transformation by flooding with a boundary region to split bassins
     * \param seed input seed
     * \param topo input topographic surface (pixel/voxel type=UI8,UI16)
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
      *\return  basins of the watershed transformation
     *
     * Watershed transformation on the topographic surface initialiased by the seeds with a boundary region to separate the basins
     * \code
     * Mat2UI8 iex;
     * iex.load("/home/vincent/Desktop/Population/doc/image/iex.pgm");
     * Mat2UI8 img = Processing::smoothDeriche(iex,0.3);
     * img = Processing::dynamic(img,20);
     * Mat2UI32 minima  = Processing::minimaRegional(img);
     * pop::Visualization::labelForeground(minima,img).display();
     * Mat2UI32 watershed =  Processing::watershedBoundary(minima,img,0);
     * Mat2UI32 boundary;
     * boundary= Processing::threshold(watershed,0,0);
     * pop::Visualization::labelToRandomRGB(boundary).display();
     * minima = Processing::labelMerge(minima,boundary);
     * Mat2UI8 grad;
     * grad = Processing::gradientMagnitudeDeriche(iex,0.7);
     * watershed  = Processing::watershed(minima,grad);
     * Mat2RGBUI8 visu = pop::Visualization::labelForeground(watershed,img);
     * visu.display();
    \endcode
    */

    template<typename FunctionLabel,typename FunctionTopo>
    static FunctionLabel watershedBoundary(const FunctionLabel & seed, const FunctionTopo & topo, int norm=1  )
    {
        if(NumericLimits<typename FunctionTopo::F>::maximumRange()>NumericLimits<UI16>::maximumRange()){
            typename FunctionTypeTraitsSubstituteF<FunctionTopo,UI16>::Result topoless;
            topoless = topo;
            return pop::ProcessingAdvanced::watershedBoundary(seed,topoless,topo.getIteratorENeighborhood(1,norm));
        }
        return pop::ProcessingAdvanced::watershedBoundary(seed,topo,topo.getIteratorENeighborhood(1,norm));
    }

    /*! \fn static FunctionLabel watershedBoundary(const FunctionLabel & seed,const FunctionTopo & topo,const FunctionMask & mask, int norm=1)
     * \brief watershed transformation by flooding in a mask with a boundary region to split bassins
     * \param seed input seed matrix
     * \param topo input topographic surface
     * \param mask mask matrix
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
     * \return  basins of the watershed transformation
     *
     * Watershed transformation on the topographic surface initialiased by the seeds restricted by the mask with a boundary region to separate the basins. The boundary label is 0 in the output label matrix.
    */

    template< typename FunctionLabel,typename FunctionTopo,typename FunctionMask >
    static FunctionLabel watershedBoundary(const FunctionLabel & seed,const FunctionTopo & topo,const FunctionMask & mask, int norm=0)
    {
        if(NumericLimits<typename FunctionTopo::F>::maximumRange()>NumericLimits<UI16>::maximumRange()){
            typename FunctionTypeTraitsSubstituteF<FunctionTopo,UI16>::Result topoless;
            topoless = topo;
            return pop::ProcessingAdvanced::watershedBoundary(seed,topoless,mask,topo.getIteratorENeighborhood(1,norm));
        }
        return pop::ProcessingAdvanced::watershedBoundary(seed,topo,mask,topo.getIteratorENeighborhood(1,norm));
    }
    /*! \fn static Function geodesicReconstruction(const Function & f,const Function & g, int norm=1 )
     * \brief infinitely iterated geodesic erosion \f$E_g^\infty(f)\f$ such as \f$E_g^{t+1}(f)=\sup (E_g^{t}(f)\ominus N,g)\f$ with \f$E_g^{0}(f)=f\f$
     * \param f input matrix
     * \param g input matrix
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
     * \return  the geodesic reconstruction
     *
     * The geodesic reconstruction is the infinitely iterated geodesic erosion \f$E_g^\infty(f)\f$ such as \f$E_g^{t+1}(f)=\sup (E_g^{t}(f)\ominus N,g)\f$ with \f$E_g^{0}(f)=f\f$
     *
     * \code
    Mat2UI8 img;
    img.load("/home/vincent/Dropbox/Vidatis/record/Vincent/record0010.png");
    img = Processing::smoothDeriche(img,1);
    img = img.opposite();
    Mat2UI8 opening = Processing::opening(img,7);//opening
    Mat2UI8 geodesic = Processing::geodesicReconstruction(img,opening);//morphological reconstruction
    Mat2UI8 tophat = img - geodesic;
    tophat.display();
     * \endcode
     *
     * \image html plate.png
     * \image html plateenhance.png
    */

    template<typename Function>
    static Function geodesicReconstruction(const Function & f,const Function & g, int norm=1 )
    {
        return pop::ProcessingAdvanced::geodesicReconstruction(f,g,f.getIteratorENeighborhood(1,norm));
    }
    /*! \brief dynamic filter as vertical filter to fill valle
      * \param f input matrix (UI8, UI16 pixel/voxel type)
      * \param value dynamic value
      * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
      *\return  the dynamic filter
     *
     *  Geodesic reconstruction with f = f+value and g=f. Dynamic filter belongs to the category of the vertical filter. one application is to
     * avoid over-segmentation in watershed transformation:
     *
     * \code
     * Mat2RGBUI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
     * Mat2UI8 grad;
     * grad = Processing::gradientMagnitudeDeriche(img,1);
     * grad = Processing::dynamic(grad,5);
     * Mat2UI32 minima  = Processing::minimaRegional(grad);
     * Mat2UI32 water  = Processing::watershed(minima,grad);
     * img = pop::Visualization::labelAverageRGB(water,img);
     * img.display();
     * \endcode
     * \image html lenawater.png
     *
    */

    template<typename Function1>
    static Function1 dynamic(const Function1 & f, typename Function1::F value, int norm=1 )
    {
        Function1 h(f);
        h+=value;
        return pop::ProcessingAdvanced::geodesicReconstruction(h,f,f.getIteratorENeighborhood(1,norm));
    }
    /*! \brief dynamic filter as vertical filter to fill valle
      * \param f input matrix (any pixel/voxel type)
      * \param value dynamic value
      * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
      * \return  the dynamic filter
     *
     *
    */
    template<typename Function1>
    static Function1 dynamicNoRegionGrowing(const Function1 & f, typename Function1::F value, int norm=1 )
    {
        Function1 h(f);
        h+=value;
        return pop::ProcessingAdvanced::dynamicNoRegionGrowing(h,f,f.getIteratorENeighborhood(1,norm));
    }
    /*! \brief voronoi tesselatin
      * \param seed input seed
      * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
      * \return tesselation
      *
      *  the output is the voronoi tesselation based on the seeds \f$ region_i(x) = \{y :  d(y ,s_i) \leq d(y , s_j), j\neq i\}\f$ (work only for 1-norm and \f$\infty-norm\f$)\n
       * In this code, we generate a voronoi tesselation from a Poisson VecN process
      * \code
                Mat2UI32 field(512,512);

                F64 densityVecNprocess=0.001;
                F64 lambda = densityVecNprocess*field.getDomain().multCoordinate();

                Distribution d(lambda,"POISSON");
                int nbrVecNs = d.randomVariable();
                Distribution d0(0,field.getDomain()(0)-1,"UNIFORMINT");
                Distribution d1(0,field.getDomain()(1)-1,"UNIFORMINT");
                for(int i=0;i<nbrVecNs;i++){
                    field(d0.randomVariable(),d1.randomVariable())=i+1;
                }
                field = Processing::voronoiTesselation(field,0);
                Mat2RGBUI8 visu= pop::Visualization::labelToRandomRGB(field);
                visu.display();
        \endcode
        \image html voronoi2.png
      *
      *
    */
    template<typename FunctionRegion>
    static FunctionRegion voronoiTesselation(const FunctionRegion & seed, int norm=1)
    {
        return pop::ProcessingAdvanced::voronoiTesselationWithoutDistanceFunction(seed,seed.getIteratorENeighborhood(1,norm));
    }
    /*! \brief voronoi tesselation
      * \param seed input seed
      * \param mask restricted set
      * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
      * \return tesselation
      *
      *  The output is the voronoi tesselation based on the seeds \f$ region_i(x) = \{y :  d_{mask}(y ,s_i) \leq d(y , s_j), j\neq i\}\f$ such that the distunce between two VecNs is calculated inside the mask \n
      *
    */
    template<typename FunctionRegion,typename FunctionMask>
    static FunctionRegion voronoiTesselation(const FunctionRegion & seed, const FunctionMask & mask, int norm=1)
    {
        return pop::ProcessingAdvanced::voronoiTesselationWithoutDistanceFunction(seed,mask,seed.getIteratorENeighborhood(1,norm));
    }
    /*! \brief voronoi tesselatin
      * \param seed input seed
      * \return tesselation
      *
      *  Voronoi tesselation based on the seeds \f$ region_i(x) = \{y :  d(y ,s_i) \leq d(y , s_j), j\neq i\}\f$ calculated with the euclidean norm\n
      * In this code, we generate a voronoi tesselation from a Poisson VecN process
      * \code
                Mat2UI32 field(512,512);

                F64 densityVecNprocess=0.001;
                F64 lambda = densityVecNprocess*field.getDomain().multCoordinate();

                Distribution d(lambda,"POISSON");
                int nbrVecNs = d.randomVariable();
                Distribution d0(0,field.getDomain()(0)-1,"UNIFORMINT");
                Distribution d1(0,field.getDomain()(1)-1,"UNIFORMINT");
                for(int i=0;i<nbrVecNs;i++){
                    field(d0.randomVariable(),d1.randomVariable())=i+1;
                }
                field = Processing::voronoiTesselationEuclidean(field);
                Mat2RGBUI8 visu= pop::Visualization::labelToRandomRGB(field);
                visu.display();
        \endcode
        \image html voronoi2eucl.png

    */

    template<typename FunctionRegion>
    static FunctionRegion  voronoiTesselationEuclidean(const FunctionRegion & seed)
    {
        return pop::ProcessingAdvanced::voronoiTesselationEuclidean(seed).first;
    }
    /*!
      * \brief distance function
      * \param seed input seed
      * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
      * \return distance
      *
      *  The distance function based on the seeds \f$ d(x) = \min_{'x\in \cup_{s_i}} d(x' ,x) \f$
      *
    */

    template<typename FunctionRegion>
    static typename FunctionTypeTraitsSubstituteF<FunctionRegion,UI16>::Result distance(const FunctionRegion & seed, int norm=1)
    {
        return pop::ProcessingAdvanced::voronoiTesselation(seed,seed.getIteratorENeighborhood(1,norm)).second;
    }



    /*! \fn  static stypename FunctionTypeTraitsSubstituteF<FunctionRegion,UI32>::Result distance(const FunctionRegion & seed, const FunctionMask & mask, int norm=1)
     * \param seed input seed
     * \param mask restricted set
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
     * \return distance function
     *
     * The distance function based on the seeds \f$ d(x) = \min_{'x\in \cup_{s_i}} d(x' ,x) \f$
     *
     * \code
     * Mat2UI32 field(512,512);
     * F64 densityVecNprocess=0.0001;
     * F64 lambda = densityVecNprocess*field.getDomain().multCoordinate();

     * Distribution d(lambda,"POISSON");
     * int nbrVecNs = d.randomVariable();
     * Distribution d0(0,field.getDomain()(0)-1,"UNIFORMINT");
     * Distribution d1(0,field.getDomain()(1)-1,"UNIFORMINT");
     * for(int i=0;i<nbrVecNs;i++){
     *     field(d0.randomVariable(),d1.randomVariable())=i+1;
     * }
     * field = Processing::distance(field);
     * Mat2RGBUI8 visu= pop::Visualization::labelToRandomRGB(field);
     * visu.display();
     * visu.save("../doc/image2/pointdistance.png");
     * \endcode
     * \image html  pointdistance.png
    */

    template<typename FunctionRegion,typename FunctionMask>
    static typename FunctionTypeTraitsSubstituteF<FunctionRegion,UI16>::Result  distance(const FunctionRegion & seed, const FunctionMask & mask, int norm=1)
    {
        return pop::ProcessingAdvanced::voronoiTesselation(seed,mask,seed.getIteratorENeighborhood(1,norm)).second;
    }
    /*!
     * \param seed input seed
     * \return distance function
     *
     * The distance function based on the seeds \f$ d(x) = \min_{'x\in \cup_{s_i}} d(x' ,x) \f$
     * \code
     * Vec3F64 domain(200,200,200);
     * ModelGermGrain3 grain = RandomGeometry::poissonPointProcess(domain,0.00001);
     * DistributionNormal d(15,10);
     * RandomGeometry::sphere(grain,d);
     * Mat3UI8 img = RandomGeometry::continuousToDiscrete(grain);
     * img = Processing::distanceEuclidean(img);
     * Mat3RGBUI8 visu = Visualization::labelToRGBGradation(img);
     * Scene3d scene;
     * Visualization::marchingCube(scene,visu);
     * scene.display();
     * \endcode
     * \image html Distance.gif
    */

    template<typename FunctionRegion>
    static typename FunctionTypeTraitsSubstituteF<FunctionRegion,F64>::Result distanceEuclidean(const FunctionRegion & seed)
    {
        return pop::ProcessingAdvanced::voronoiTesselationEuclidean(seed).second;
    }

    /*!
     * \brief morphological dilation in fast implementation complexity equal to radius
     * \param f input matrix
     * \param radius radius
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d,   0=8-neighborhood in 2d and 26 in 3d, 2=euclidean norm
     * \return dilation ouput  matrix
     *
     * \f$dilation(x) =  \max_{x'\in B(x,r,n)}f(x') \f$, where \f$B(x,norm)=\{x':|x'-x|_n\leq r\}\f$ the ball centered in 0 of radius r and the norm n
     *
     * \sa Processing::erosionRegionGrowing()
     */
    template<typename Function>
    static Function  dilationRegionGrowing(const Function & f,F64 radius, int norm=1)
    {
        return pop::ProcessingAdvanced::dilationRegionGrowing(f,radius,norm);
    }
    /*!
     * \brief morphological erosion in fast implementation complexity equal to radius
     * \param f input matrix
     * \param radius radius
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d,   0=8-neighborhood in 2d and 26 in 3d, 2=euclidean norm
     * \return erosion ouput matrix
     *
     *  \f$erosion(x) =  \max_{x'\in B(x,r,n)}f(x') \f$, where \f$B(x,norm)=\{x':|x'-x|_n\leq r\}\f$ the ball centered in 0 of radius r and the norm n
     *
     * \code
     * Mat2UI8 img;
     * img.load("/home/vincent/Dropbox/Vidatis/record/Vincent/record0010.png");
     * img = Processing::smoothDeriche(img,1);
     * img = img.opposite();
     * Mat2UI8 opening = Processing::opening(img,7);//opening
     * Mat2UI8 geodesic = Processing::geodesicReconstruction(img,opening);//morphological reconstruction
     * Mat2UI8 tophat = img - geodesic;//enhance the character

     * Mat2UI8 element_structurant_horizontal(3,3);
     * element_structurant_horizontal(1,0)=1;element_structurant_horizontal(1,1)=1;element_structurant_horizontal(1,2)=1;
     * Mat2UI8 cluster = Processing::dilationRegionGrowingStructuralElement(tophat,element_structurant_horizontal,40);//big structural element
     * cluster = Processing::erosionRegionGrowingStructuralElement(cluster,element_structurant_horizontal,20);//big structural element
     * Mat2UI8 element_structurant_vertical(3,3);
     * element_structurant_vertical(0,1)=1;
     * element_structurant_vertical(1,1)=1;
     * element_structurant_vertical(1,2)=1;
     * cluster = Processing::erosionRegionGrowingStructuralElement(cluster,element_structurant_vertical,20);//big structural element
     * double value;
     * cluster = Processing::thresholdOtsuMethod(cluster,value);
     * cluster.display();
     * cluster.save("../doc/image2/plateextraction.png");
     * \endcode
     * \image html plate.png
     * \image html plateextraction.png
     */
    template<typename Function>
    static Function  erosionRegionGrowing(const Function & f,F64 radius, int norm=1)
    {
        return pop::ProcessingAdvanced::erosionRegionGrowing(f,radius,norm);
    }
    /*!
     * \brief morphological erosion in fast implementation complexity equal to radius
     * \param f input matrix
     * \param structural_element structural element
     * \param radius dilation of the structural element
     * \return erosion ouput matrix
     *
     * \sa Processing::erosionRegionGrowing()
     */
    template< typename Function,typename FunctionStructuralElement>
    static Function erosionRegionGrowingStructuralElement(const Function & f,const FunctionStructuralElement & structural_element,double radius=1)throw(pexception)
    {
        return pop::ProcessingAdvanced::erosionRegionGrowing(f,f.getIteratorENeighborhood(structural_element),radius);
    }
    /*!
     * \brief morphological dilation in fast implementation complexity equal to radius
     * \param f input matrix
     * \param structural_element structural element
     * \param radius dilation of the structural element
     * \return erosion ouput matrix
     *
     * \sa Processing::erosionRegionGrowing()
     */
    template< typename Function,typename FunctionStructuralElement>
    static Function dilationRegionGrowingStructuralElement(const Function & f,const FunctionStructuralElement & structural_element,double radius=1)throw(pexception)
    {
        return pop::ProcessingAdvanced::dilationRegionGrowing(f,f.getIteratorENeighborhood(structural_element),radius);
    }
    /*!
     * \brief morphological closing in fast implementation complexity equal to radius
     * \param f input matrix
     * \param structural_element structural element
     * \param radius dilation of the structural element
     * \return erosion ouput matrix
     *
     * \sa Processing::erosionRegionGrowing()
     */
    template< typename Function,typename FunctionStructuralElement>
    static Function  closingRegionGrowingStructuralElement(const Function & f,const FunctionStructuralElement & structural_element,double radius=1)
    {
        Function temp(f.getDomain());
        temp = dilationRegionGrowingStructuralElement(f,structural_element,radius);
        return erosionRegionGrowingStructuralElement(temp,structural_element,radius);
    }
    /*!
     * \brief morphological opening in fast implementation complexity equal to radius
     * \param f input matrix
     * \param structural_element structural element
     * \param radius dilation of the structural element
     * \return erosion ouput matrix
     *
     * \sa Processing::erosionRegionGrowing()
     */
    template< typename Function,typename FunctionStructuralElement>
    static Function  openingRegionGrowingStructuralElement(const Function & f,const FunctionStructuralElement & structural_element,double radius=1)
    {
        Function temp(f.getDomain());
        temp = erosionRegionGrowingStructuralElement(f,structural_element,radius);
        return dilationRegionGrowingStructuralElement(temp,structural_element,radius);
    }
    /*!
     * \brief  Morphological closing in fast implementation complexity equal to radius
     * \param f input matrix
     * \param radius radius
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d,   0=8-neighborhood in 2d and 26 in 3d, 2=euclidean norm
     * \return closing ouput  matrix
          *
     *  closing(f) = dilation(erosion(f))
          \code
        Mat2UI8 lena;
        lena.load("/home/vincent/Desktop/Population/doc/image/Lena.pgm");
        Processing::closingRegionGrowing(lena,15,2).display();
        \endcode
        */
    template<typename Function>
    static Function  closingRegionGrowing(const Function & f,F64 radius, int norm=1)
    {
        return pop::ProcessingAdvanced::closingRegionGrowing(f,radius,norm);
    }
    /*!
     * \brief Morphological opening in fast implementation complexity equal to radius
     * \param f input matrix
     * \param radius radius
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d,   0=8-neighborhood in 2d and 26 in 3d, 2=euclidean norm
     * \return erosion ouput matrix
          *
     *  opening(f) = erosion(dilation(f))
          \code
        Mat2UI8 lena;
        lena.load("/home/vincent/Desktop/Population/doc/image/Lena.pgm");
        Processing::openingRegionGrowing(lena,15,2).display();
        \endcode
        */
    template<typename Function>
    static Function  openingRegionGrowing(const Function & f,F64 radius, int norm=1)
    {
        return pop::ProcessingAdvanced::openingRegionGrowing(f,radius,norm);
    }


    //@}
    //-------------------------------------
    //
    //! \name others
    //@{
    //-------------------------------------

    /*!
    \brief rotate automatically the input matrix to have a geometric horizontallity
    \param f  input function
    \param angle_rot_radian angle of rotation find by the algorithm
    \param sigma_filter parameter of the gaussian filte
    \return rotated matrix
    *
    */
    template<typename Function>
    static Function rotateAtHorizontal(const Function & f,double &angle_rot_radian,double sigma_filter=2){

        int border =5;
        typedef typename FunctionTypeTraitsSubstituteF<Function,double>::Result FunctionFloat;
        FunctionFloat imgf(f);
        FunctionFloat img_gradx= ProcessingAdvanced::gradGaussian(imgf,1,sigma_filter,2*sigma_filter,imgf.getIteratorEDomain());
        FunctionFloat img_grady= ProcessingAdvanced::gradGaussian(imgf,0,sigma_filter,2*sigma_filter,imgf.getIteratorEDomain());
        Vec<double> angles;
            Vec<double> weight;
        typename FunctionFloat::IteratorERectangle it(img_gradx.getIteratorERectangle(border,img_gradx.getDomain()-1-border));
        while(it.next()){
            angles.push_back(std::atan2(img_grady(it.x()),img_gradx(it.x()))*180/pop::PI);
            weight.push_back(std::sqrt(img_grady(it.x())*img_grady(it.x())+img_gradx(it.x())*img_gradx(it.x())));
        }
        DistributionRegularStep d = Statistics::computedStaticticsFromRealRealizationsWithWeight(angles,weight,1);
        d.smoothGaussian(4);
        angle_rot_radian =  pop::PI/2-Statistics::argMax(d,80,100,1)*pop::PI/180;
        return GeometricalTransformation::rotate(f,angle_rot_radian);
    }
    //@}
#ifdef WITHSWIG
    template<int DIM,typename Type>
    static pop::MatN<DIM,UI8>  _threshold(const pop::MatN<DIM,Type> & f,double ymin,double ymax=255){
        return threshold(f,ymin,ymax);

    }
    template<int DIM,typename Type>
    static pop::MatN<DIM,UI8>  _thresholdKMeansVariation(const pop::MatN<DIM,Type>& f){
        return thresholdKMeansVariation(f);
    }

	    template<int DIM,typename Type>
    static pop::MatN<DIM,UI8>  _thresholdOtsuMethod(const pop::MatN<DIM,Type>& f){
        double thresholdvalue;
        return thresholdOtsuMethod(f,thresholdvalue);
    }
	    template<int DIM,typename Type>
    static pop::MatN<DIM,UI8>  _thresholdToggleMappingMorphological (const pop::MatN<DIM,Type> &f, int radius=2, double norm=0){
        return thresholdToggleMappingMorphological(f,radius,norm);
    }
	    template<int DIM,typename Type>
    static pop::MatN<DIM,UI8>  _thresholdToggleMappingMorphologicalFabrizio (const pop::MatN<DIM,Type> &f, double c=16, double p=0.8, int radius=2, double norm=1){
        return thresholdToggleMappingMorphologicalFabrizio2(f,c,  p,  radius,  norm);
    }
		    template<int DIM,typename Type>
    static pop::MatN<DIM,UI8>  _thresholdNiblackMethod (const pop::MatN<DIM,Type> &f, double k=0.2, int radius=5, double offset_value=0){
        return thresholdNiblackMethod(f,k,  radius,  offset_value);
    }
	
    template<int DIM,typename Type>
    static pop::MatN<DIM,UI8> _thresholdMultiValley(const pop::MatN<DIM,Type>& f,double dynamic=0.001){
        return thresholdMultiValley(f,dynamic);
    }
   template<int DIM,typename Type>
    static pop::MatN<DIM,UI8>  _edgeDetectorCanny(const pop::MatN<DIM,Type>& f,double sigma_deriche=1,double low =2,double high=20){
        return edgeDetectorCanny(f,sigma_deriche,low,high);
    }
    template<int DIM,typename Type>
    static pop::MatN<DIM,UI8> _minimaLocalMap(const pop::MatN<DIM,Type> & f,double radius=1,double norm=0)throw(pexception){
        return minimaLocalMap(f,radius,norm);
    }
    template<int DIM,typename Type>
    static pop::MatN<DIM,UI8> _maximaLocalMap(const pop::MatN<DIM,Type> & f,double radius=1,double norm=0)throw(pexception){
        return maximaLocalMap(f,radius,norm);

    }
    template<int DIM,typename Type>
    static pop::MatN<DIM,UI8> _extremaLocalMap(const pop::MatN<DIM,Type> & f,double radius=1,double norm=0)throw(pexception){
        return extremaLocalMap(f,radius,norm);

    }
    template<int DIM,typename Type>
    static pop::MatN<DIM,F64> _gradientSobel(const pop::MatN<DIM,Type> & f,int direction){
        return gradientSobel(f,direction);
    }
    template<int DIM,typename Type>
    static pop::MatN<DIM,VecN<DIM,F64> > _gradientVecSobel(const pop::MatN<DIM,Type>  & f){
        return gradientVecSobel(f);
    }
    template<int DIM,typename Type>
    static pop::MatN<DIM,F64> _gradientGaussian(const pop::MatN<DIM,Type> & f, int direction,F64 sigma=1){
        return gradientGaussian(f,direction,sigma);
    }
    template<int DIM,typename Type>
    static pop::MatN<DIM,VecN<DIM,F64> > _gradientVecGaussian(const pop::MatN<DIM,Type>  & f,double sigma=1){
         return gradientVecGaussian(f,sigma);
    }
    template<int DIM,typename Type>
    static pop::MatN<DIM,F64>  _gradientDeriche(const pop::MatN<DIM,Type> & f,I32 direction, F64 alpha=1){
        return gradientDeriche(f,direction,alpha);
    }
    template<int DIM,typename Type>
    static pop::MatN<DIM,VecN<DIM,F64> > _gradientVecDeriche(const pop::MatN<DIM,Type> & f,double alpha=1){
        return gradientVecDeriche(f,alpha);
    }
    template<int DIM,typename Type>
    static pop::MatN<DIM,UI32>  _clusterToLabel(const pop::MatN<DIM,Type> & cluster,int norm=1){
        return clusterToLabel(cluster,norm);
    }
    template<int DIM,typename Type>
    static pop::MatN<DIM,UI32> _minimaRegional(const pop::MatN<DIM,Type> & topo,int norm=1 ){
        return minimaRegional(topo,norm);
    }
    template<int DIM,typename Type>
    static pop::MatN<DIM,UI16> _distance(const pop::MatN<DIM,Type>& seed, int norm=1){
     return distance(seed,norm);
    }
    template<int DIM,typename Type>
    static pop::MatN<DIM,UI16>  _distanceMask(const pop::MatN<DIM,Type> & seed, const pop::MatN<DIM,UI8> & mask, int norm=1){
        return distance(seed,mask,norm);
    }
    template<int DIM,typename Type>
    static pop::MatN<DIM,F64> _distanceEuclidean(const pop::MatN<DIM,Type> & seed){
        return distanceEuclidean(seed);
    }
#endif
};
}
#endif // PROCESSING_H

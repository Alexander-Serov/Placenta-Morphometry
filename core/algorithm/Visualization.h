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
#ifndef VISUALIZATION_H
#define VISUALIZATION_H
#include"core/data/typeF/TypeF.h"
#include"core/data/mat/MatN.h"
#include"core/data/mat/Mat2x.h"

#include"core/data/3d/GLFigure.h"
#include"core/data/distribution/DistributionAnalytic.h"

#include"core/data/notstable/graph/Graph.h"
#include"core/data/notstable/graph/Vertex.h"
#include"core/data/notstable/graph/Edge.h"
#include"core/algorithm/Draw.h"
namespace pop
{

/*!
\defgroup Visualization Visualization
\ingroup Algorithm
\brief Matrix In -> Matrix Out for 2D or OpenGl scene for 3D

- visualization of the result of a 2d process (segmentation or distance function)
\image html outdistance.png "Distance function"
- 3d visualization
\image html Graph.gif "Topological graph"

*/

struct POP_EXPORTS Visualization
{

    /*!
        \class pop::Visualization
        \ingroup Visualization
        \brief matrix visualization with opengl layer
        \author Tariel Vincent

         This class provides some algorithms to visualize an matrix.\n
         Note : when you display an opengl scene, its closure causes the end of the algorithms due to Glut in Linux.
      \sa MatN
    */
    //-------------------------------------
    //
    //! \name Label matrix
    //@{
    //-------------------------------------
    /*! \fn static typename FunctionTypeTraitsSubstituteF<Function,RGBUI8>::Result labelToRandomRGB(const Function & f )throw(pexception)
     * \param f input label matrix
     * \return  RGB matrix
     *
     * affect a random RGB at each label value
     *
     * \code
     * Mat2UI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/matrix/outil.bmp");
     * Mat2UI32 label = pop::Processing::clusterToLabel(img);
     * Mat2RGBUI8 RGB = Visualization::labelToRandomRGB(label);
     * RGB.save("/home/vincent/Desktop/Population/doc/matrix/outilcluster.bmp");
     * \endcode
     * \image html outilcluster.png
    */
    template< typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,RGBUI8>::Result labelToRandomRGB(const Function & f )throw(pexception)
    {
        typename FunctionTypeTraitsSubstituteF<Function,RGBUI8>::Result fRGB(f.getDomain());
        typename Function::IteratorEDomain it(f.getIteratorEDomain());

        I32 value1=234;
        I32 value2=27;
        I32 value3=33;


        it.init();
        while(it.next())
        {
            if(normValue(f(it.x()))>0 )
            {
                RGBUI8 v;
                I32 r = 0;
                I32 g = 0;
                I32 b = 0;
                int i=normValue(f(it.x()));
                if(i==1)v=RGBUI8(255,0,0);
                else if (i==2)v=RGBUI8(0,255,0);
                else if (i==3)v=RGBUI8(0,0,255);
                else if (i==255)v=RGBUI8(120,120,120);
                else
                {
                    r = absolute((value1*i+56));
                    g = absolute((value2*i+53));
                    b = absolute((value3*i+11));
                    v=RGBUI8(r%255,g%255,b%255);
                }
                fRGB(it.x())=v;
            }
            else
                fRGB(it.x())=0;
        }
        return fRGB;
    }

    /*! \fn static typename FunctionTypeTraitsSubstituteF<Function,RGBUI8>::Result labelToRGBGradation(const Function & f,RGBUI8 cmin=RGBUI8(0,0,255),RGBUI8 cmax=RGBUI8(255,0,0))throw(pexception)
     * \param f input label matrix
     * \param cmin mininum RGB
     * \param cmax maximum RGB
     * \return  RGB matrix
     *
     * affect a RGB at each label value with a gradation from mininum RGB (blue by default) to maximum RGB (red by default)
     *
     * \code
     * Mat2UI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/outil.bmp");
     * Mat2UI32 label;
     * label = pop::Processing::distanceEuclidean(img);
     * Mat2RGBUI8 RGB = Visualization::labelToRGBGradation(label);
     * RGB.save("/home/vincent/Desktop/Population/doc/image/outdistance.bmp");
     * \endcode
     * \image html outdistance.png
    */
    template< typename Function >
    static typename FunctionTypeTraitsSubstituteF<Function,RGBUI8>::Result labelToRGBGradation (const Function & f,RGBUI8 cmin=RGBUI8(0,0,255),RGBUI8 cmax=RGBUI8(255,0,0)  )throw(pexception)
    {
        typename FunctionTypeTraitsSubstituteF<Function,RGBUI8>::Result fRGB(f.getDomain());
        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        typename Function::F maxi = NumericLimits<typename Function::F>::minimumRange();
        typename Function::F mini = NumericLimits<typename Function::F>::maximumRange();
        while(it.next()){
            maxi = maximum(maxi,f(it.x()));
            mini = minimum(mini,f(it.x()));
        }
        //        std::cout<<maxi<<std::endl;
        //        std::cout<<mini<<std::endl;
        RGBUI8 v_init(0);
        std::vector< RGBUI8 > v(maxi+1-mini, v_init);
        RGBF64 cminf,cmaxf;
        cminf = cmin;
        cmaxf = cmax;

        for(I32 i=0;i<=(I32)maxi-(I32)mini;i++)
        {
            double dist = i*1.0/(maxi-mini);
            v[i]=dist*(cmaxf-cminf)+cminf;
        }
        it.init();
        while(it.next())
        {
            if(f(it.x())!=0)
                fRGB(it.x())=v[f(it.x())-mini];
        }
        return fRGB;
    }

    /*!  static typename FunctionTypeTraitsSubstituteF<Function1,RGBUI8 >::Result labelAverageRGB(const Function1 & label,const Function2 & img)throw(pexception)
     * \param label input label matrix
     * \param img matrix grey-level matrix
     * \return RGB matrix
     *
     * out(x)= average(img(x)) where average is the mean value of img in each label
     *
     * \code
     * Mat3UI8 img;
     * img.load("/home/vincent/Desktop/Population/image/rock3d.pgm");
     * img = pop::Processing::median(img,3,2);

     * Mat3UI8 seed1 = pop::Processing::threshold(img,0,50);
     * Mat3UI8 seed2 = pop::Processing::threshold(img,110,140);
     * Mat3UI8 seed3 = pop::Processing::threshold(img,165,255);

     * seed1 = pop::Processing::labelMerge(seed1,seed2);
     * seed1 = pop::Processing::labelMerge(seed1,seed3);

     * Mat3UI8 gradient = pop::Processing::gradientMagnitudeDeriche(img,1);
     * Mat3UI8 basins =  pop::Processing::watershed(seed1,gradient);

     * img= Visualization::labelAverageRGB(basins,img);
     * Scene3d scene;
     * Visualization::cube(scene,img);
     * scene.display();
     * \endcode
     * \image html average.png
    */
    template<typename Function1,typename Function2>
    static Function2 labelAverageRGB(const Function1 & label,const Function2 & img)throw(pexception)
    {
        FunctionAssert(label,img,"Visualization::labelAverageRGB");
        typename Function1::IteratorEDomain it (img.getIteratorEDomain());
        typename FunctionTypeTraitsSubstituteF<Function2,typename FunctionTypeTraitsSubstituteF<typename Function2::F ,F64>::Result>::Result in(img);
        std::vector<typename FunctionTypeTraitsSubstituteF<typename Function2::F ,F64>::Result> vaverage;
        std::vector<UI32> voccurence;
        it.init();
        while(it.next())
        {
            if((UI32)vaverage.size()<=(UI32)label(it.x()))
            {
                vaverage.resize(label(it.x())+1);
                voccurence.resize(label(it.x())+1);
            }
            vaverage[label(it.x())]=vaverage[label(it.x())]+in(it.x());
            voccurence[label(it.x())]++;
        }
        Function2 average(label.getDomain());
        it.init();
        while(it.next())
        {
            F64 div = (1./voccurence[label(it.x())]);
            typename FunctionTypeTraitsSubstituteF<typename Function2::F ,F64>::Result value(vaverage[label(it.x())]);
            value = value*div;
            average(it.x())=value;
        }
        return average;
    }



    /*!  static typename FunctionTypeTraitsSubstituteF<Function1,RGBUI8 >::Result labelForegroundBoundary(const Function1 & label,const Function2 & img,int width=1,int norm=1)throw(pexception)
     * \param label input label matrix
     * \param img matrix grey-level matrix
     * \param norm 1 = 4-neighborhood in 2d and 6 in 3d, and  0=8-neighborhood in 2d and 26 in 3d
     * \param width boundary width
     * \return RGB matrix
     *
     * out(x)=img(x) for label_c(x)=0; img(x)*ratio+label_c(x)*(1-ratio) otherwise, where label_c is random RGB boundary label
     *
     * \code
     * Mat3UI8 img;
     * img.load("/home/vincent/Desktop/Population/image/rock3d.pgm");
     * img = pop::Processing::median(img,3,2);

     * Mat3UI8 seed1 = pop::Processing::threshold(img,0,50);
     * Mat3UI8 seed2 = pop::Processing::threshold(img,110,140);
     * Mat3UI8 seed3 = pop::Processing::threshold(img,165,255);

     * seed1 = pop::Processing::labelMerge(seed1,seed2);
     * seed1 = pop::Processing::labelMerge(seed1,seed3);

     * Mat3UI8 gradient = pop::Processing::gradientMagnitudeDeriche(img,1);
     * Mat3UI8 basins =  pop::Processing::watershed(seed1,gradient);

     * Mat3RGBUI8 RGB = Visualization::labelForegroundBoundary(basins,img,0.5);
     * Scene3d scene;
     * Visualization::cube(scene,RGB);
     * scene.display();
     * \endcode
     * \image html foregroundboundary.png
    */
    template<typename Function1,typename Function2>
    static typename FunctionTypeTraitsSubstituteF<Function1,RGBUI8 >::Result labelForegroundBoundary(const Function1 & label,const Function2 & img,int width=1,int norm=1)throw(pexception)
    {
        FunctionAssert(label,img,"Visualization::labelForeground");
        Function1  labelb(label.getDomain());
        typename Function1::IteratorEDomain it (label.getIteratorEDomain());
        typename Function1::IteratorENeighborhood itn(label.getIteratorENeighborhood(width,norm));
        while(it.next()){
            typename Function1::F l = label(it.x());
            itn.init(it.x());
            bool diff=false;
            while(itn.next()){
                if(label(itn.x())!=l)
                    diff=true;
            }
            if(diff==true)
                labelb(it.x())=l;
        }




        FunctorF::FunctorAccumulatorMin<typename Function1::F > funcmini;
        it.init();
        I32 min = FunctionProcedureFunctorAccumulatorF(labelb,funcmini,it);
        FunctorF::FunctorAccumulatorMax<typename Function1::F > funcmaxi;
        it.init();
        I32 max = FunctionProcedureFunctorAccumulatorF(labelb,funcmaxi,it);


        if(min<0){
            throw(pexception("In Vizualization::labelForegroundBoundary,the label matrix must be possitive"));
        }
        std::vector<RGB<F64> > v(max+1);

        srand ( 1 );

        for(I32 i=0;i<(I32)v.size();i++)
        {
            if(i==1)v[i]=RGB<F64>(255,0,0);
            else if(i==2)v[i]=RGB<F64>(0,255,0);
            else if(i==3)v[i]=RGB<F64>(0,0,255);
            else
                v[i]=RGB<F64>(rand()%256,rand()%256,rand()%256);//255*dist.randomVariable(),255*dist.randomVariable(),255*dist.randomVariable());

        }
        typename FunctionTypeTraitsSubstituteF<Function1,RGBUI8 >::Result foreground(label.getDomain());


        it.init();
        while(it.next())
        {
            if(labelb(it.x())==0)
            {
                RGB<F64> value(img(it.x()));
                foreground(it.x())=value;
            }
            else
            {
                foreground(it.x())=v[labelb(it.x())];
            }
        }
        return foreground;
    }

    /*! static typename FunctionTypeTraitsSubstituteF<Function1,RGBUI8 >::Result labelForeground(const Function1 & label,const Function2 & img,F64 ratio=0.5)throw(pexception)
     * \param label input label matrix
     * \param img matrix grey-level matrix
     * \param ratio ratio between label and grey-level matrixs
     * \return RGB matrix
     *
     * out(x)=img(x) for label(x)=0; img(x)*ratio+label_c(x)*(1-ratio) otherwise, where label_c is random RGB label
     *
     * \code
     * Mat3UI8 img;
     * img.load("/home/vincent/Desktop/Population/image/rock3d.pgm");
     * img = pop::Processing::median(img,3,2);

     * Mat3UI8 seed1 = pop::Processing::threshold(img,0,50);
     * Mat3UI8 seed2 = pop::Processing::threshold(img,110,140);
     * Mat3UI8 seed3 = pop::Processing::threshold(img,165,255);

     * seed1 = pop::Processing::labelMerge(seed1,seed2);
     * seed1 = pop::Processing::labelMerge(seed1,seed3);

     * Mat3UI8 gradient = pop::Processing::gradientMagnitudeDeriche(img,1);
     * Mat3UI8 basins =  pop::Processing::watershed(seed1,gradient);

     * Mat3RGBUI8 RGB = Visualization::labelForeground(basins,img,0.5);
     * Scene3d scene;
     * Visualization::cube(scene,RGB);
     * scene.display();
     * \endcode
     * \image html foreground.png
    */

    template<typename Function1,typename Function2>
    static typename FunctionTypeTraitsSubstituteF<Function1,RGBUI8 >::Result labelForeground(const Function1 & label,const Function2 & img,F64 ratio=0.5)throw(pexception)
    {
        FunctionAssert(label,img,"Visualization::labelForeground");
        typename Function1::IteratorEDomain it (label.getIteratorEDomain());

        FunctorF::FunctorAccumulatorMin<typename Function1::F > funcmini;
        it.init();
        I32 min = FunctionProcedureFunctorAccumulatorF(label,funcmini,it);
        FunctorF::FunctorAccumulatorMax<typename Function1::F > funcmaxi;
        it.init();
        I32 max = FunctionProcedureFunctorAccumulatorF(label,funcmaxi,it);


        if(min<0){
            throw(pexception("In Vizualization::labelForeground, the label matrix must be possitive"));
        }
        std::vector<RGB<F64> > v(max+1);

        srand ( 1 );

        for(I32 i=0;i<(I32)v.size();i++)
        {
            if(i==1)v[i]=RGB<F64>(255,0,0);
            else if(i==2)v[i]=RGB<F64>(0,255,0);
            else if(i==3)v[i]=RGB<F64>(0,0,255);
            else
                v[i]=RGB<F64>(rand()%256,rand()%256,rand()%256);//255*dist.randomVariable(),255*dist.randomVariable(),255*dist.randomVariable());

        }
        typename FunctionTypeTraitsSubstituteF<Function1,RGBUI8 >::Result foreground(label.getDomain());


        it.init();
        while(it.next())
        {
            if(label(it.x())==0)
            {
                RGB<F64> value(img(it.x()));
                foreground(it.x())=value;
            }
            else
            {
                RGB<F64> value(img(it.x()));
                RGB<F64> value2 =  value*ratio+ v[label(it.x())]*(1-ratio);
                foreground(it.x())=value2;
            }
        }
        return foreground;
    }

    //@}
    //-------------------------------------
    //
    //! \name OpenGl
    //@{
    //-------------------------------------

    /*! static void cube(Scene3d& scene,const Function & img)
     * \param scene input/output opengl scene
     * \param img input 3d matrix
     *
     * extract the faces of the 3d matrix and add their to the opengl scene
     *
     * \code
     * Mat3UI8 img;
     * img.load("/home/vincent/Desktop/Population/image/rock3d.pgm");
     * Scene3d scene;
     * Visualization::cube(scene,img);
     * scene.display();
     * \endcode
     * \image html cube.png
    */
    template<typename Function>
    static void cube(Scene3d& scene,const Function & img)throw(pexception)
    {
        if(Function::DIM!=3)
            throw(pexception("In Vizualization::cube, input matrix must be 3d in Visualization::cube"));



        const int DIM =Function::DIM;


        for(int direction=0;direction<3;direction++)
        {
            for(int j=0;j<2;j++)
            {
                int slice;
                if(j==0){

                    slice =0;
                }
                else{

                    slice = img.getDomain()(direction)-1;
                }

                typename Function::Hyperplane hyperff;
                hyperff = img.getPlane(direction,slice);
                typename Function::Hyperplane::IteratorEDomain it (hyperff.getIteratorEDomain());
                VecN<DIM,F64 > normal;

                normal(direction)=1;
                VecN<3,F64 > add1;
                add1=0;
                add1( (direction+1)%DIM)=1;
                VecN<3,F64 > add2;
                add2=0;
                add2( (direction+1)%DIM)=1;add2( (direction+2)%DIM)=1;
                VecN<3,F64 > add3;
                add3=0;
                add3( (direction+2)%DIM)=1;

                VecN<3,F64> x;
                VecN<3,F64> y;
                if(slice>0)slice++;
                x(direction)=slice;

                if(j==0)
                    normal(direction)=-1;
                else
                    normal(direction)=+1;

                while(it.next())
                {
                    FigurePolygon * poly = new FigurePolygon();
                    RGBUI8 c(hyperff(it.x()));
                    poly->setRGB( c);
                    poly->setTransparent(255);
                    poly->normal= VecN<3,F32>(normal);
                    for(int i=0;i<DIM;i++)
                    {
                        if(i<direction)
                            x(i)=it.x()(i);
                        else if(i>direction)
                            x(i)=it.x()(i-1);
                    }
                    y =x;
                    poly->vpos.push_back(x);
                    y = x+add1;
                    poly->vpos.push_back(y);
                    y = x+add2;
                    poly->vpos.push_back(y);
                    y = x+add3;
                    poly->vpos.push_back(y);
                    scene.vfigure.push_back(poly);
                }
            }
        }
    }
    /*! static void lineCube(Scene3d& scene,const Function & img,double width=2,RGBUI8 RGB=RGBUI8(255,0,0))
     * \param scene input/output opengl scene
     * \param img input 3d matrix
     * \param width line width
     * \param RGB line RGB
     *
     * extract the line of cube and add their to the opengl scene
     *
     * \code
     * Mat3UI8 img;
     * img.load("/home/vincent/Desktop/Population/image/rock3d.pgm");
     * Scene3d scene;
     * Visualization::cube(scene,img);
     * Visualization::lineCube(scene,img);
     * scene.display();
     * \endcode
     * \image html cubeline.png
    */
    template<typename Function>
    static void lineCube(Scene3d& scene,const Function & img,double width=2,RGBUI8 RGB=RGBUI8(255,0,0))throw(pexception)
    {
        if(Function::DIM!=3)
            throw(pexception("In Vizualization::lineCube, Input matrix must be 3d in Visualization::cube"));
        int d0 = img.getDomain()(0);
        int d1 = img.getDomain()(1);
        int d2 = img.getDomain()(2);

        VecN<3,F64> x1,x2;
        x1(0)=0;x1(1)=0;x1(2)=0;

        x2(0)=d0;x2(1)=0;x2(2)=0;

        FigureLine * line = new FigureLine();
        line->x1 = x1;
        line->x2 = x2;
        line->setRGB(RGB);
        line->width=width;

        scene.vfigure.push_back(line);
        x2(0)=0;x2(1)=d1;x2(2)=0;


        line = new FigureLine();
        line->x1 = x1;
        line->x2 = x2;
        line->setRGB(RGB);
        line->width=width;


        scene.vfigure.push_back(line);

        x2(0)=0;x2(1)=0;x2(2)=d2;
        line = new FigureLine();
        line->x1 = x1;
        line->x2 = x2;
        line->setRGB(RGB);
        line->width=width;


        scene.vfigure.push_back(line);

        x1(0)=d0;x1(1)=d1;x1(2)=0;
        x2(0)=d0;x2(1)=0;x2(2)=0;

        line = new FigureLine();
        line->x1 = x1;
        line->x2 = x2;
        line->setRGB(RGB);
        line->width=width;


        scene.vfigure.push_back(line);

        x2(0)=0;x2(1)=d1;x2(2)=0;

        line = new FigureLine();
        line->x1 = x1;
        line->x2 = x2;
        line->setRGB(RGB);
        line->width=width;


        scene.vfigure.push_back(line);

        x2(0)=d0;x2(1)=d1;x2(2)=d2;

        line = new FigureLine();
        line->x1 = x1;
        line->x2 = x2;
        line->setRGB(RGB);
        line->width=width;


        scene.vfigure.push_back(line);


        x1(0)=0;x1(1)=d1;x1(2)=d2;

        x2(0)=d0;x2(1)=d1;x2(2)=d2;

        line = new FigureLine();
        line->x1 = x1;
        line->x2 = x2;
        line->setRGB(RGB);
        line->width=width;



        scene.vfigure.push_back(line);

        x2(0)=0;x2(1)=0;x2(2)=d2;

        line = new FigureLine();
        line->x1 = x1;
        line->x2 = x2;
        line->setRGB(RGB);
        line->width=width;



        scene.vfigure.push_back(line);

        x2(0)=0;x2(1)=d1;x2(2)=0;

        line = new FigureLine();
        line->x1 = x1;
        line->x2 = x2;
        line->setRGB(RGB);
        line->width=width;



        scene.vfigure.push_back(line);


        x1(0)=d0;x1(1)=0;x1(2)=d2;

        x2(0)=d0;x2(1)=d1;x2(2)=d2;

        line = new FigureLine();
        line->x1 = x1;
        line->x2 = x2;
        line->setRGB(RGB);
        line->width=width;



        scene.vfigure.push_back(line);

        x2(0)=0;x2(1)=0;x2(2)=d2;

        line = new FigureLine();
        line->x1 = x1;
        line->x2 = x2;
        line->setRGB(RGB);
        line->width=width;



        scene.vfigure.push_back(line);

        x2(0)=d0;x2(1)=0;x2(2)=0;

        line = new FigureLine();
        line->x1 = x1;
        line->x2 = x2;
        line->setRGB(RGB);
        line->width=width;


        scene.vfigure.push_back(line);
    }
    /*! static void marchingCube(Scene3d& scene,const Function & img)throw(pexception)
     * \param scene input/output opengl scene
     * \param img input 3d matrix
     *
     * Apply the marching cube algorithm to the boundary defined between the voxels of value 0 and the others
     *
     * \code
    Mat3UI8 img;
    img.load("../image/rock3d.pgm");
//    img = img(Vec3I32(0,0,0),Vec3I32(64,64,64));

    Mat3UI8 imgfilter= Processing::median(img,2);

    Mat3UI8 grain= Processing::threshold(imgfilter,155);
    Mat3UI8 oil = Processing::threshold(imgfilter,70,110);
    oil = Processing::openingRegionGrowing(oil,2);//To remove the interface artefact
    Mat3UI8 air = Processing::threshold(imgfilter,0,40);
    Mat3UI8 seed = Processing::labelMerge(grain,oil);
    seed = Processing::labelMerge(seed,air);

    Mat3UI8 gradient = Processing::gradientMagnitudeDeriche(img,1.5);
    Mat3UI8 water = Processing::watershed(seed,gradient);
    grain = Processing::labelFromSingleSeed(water,grain);
    grain=Mat3F64(grain)*0.75;
    oil = Processing::labelFromSingleSeed(water,oil);
    oil = Mat3F64(oil)*0.4;
    Scene3d scene;
    Visualization::marchingCube(scene,grain);
    Visualization::marchingCube(scene,oil);
    Visualization::lineCube(scene,img);
    scene.display();
     * \endcode
     * \image html marchingcube.png
    */
    template<typename Function>
    static void marchingCube(Scene3d& scene, Function & img )throw(pexception)
    {
        if(Function::DIM!=3)
            throw(pexception("In Vizualization::marchingCube, Input matrix must be 3d in Visualization::cube"));
        MatN<3,RGBUI8 >   binc;
        binc = img;
        //        if(dynamic_cast< MatN<3,RGBUI8 >  * >(&img)){
        //            binc = img;
        //        }else {
        //            Convertor::fromRGB(img,img,img,binc);
        //        }
        std::vector<std::pair<vertex,RGBUI8 > > vertices = runMarchingCubes2(binc);
        while(vertices.empty()==false)
        {
            vertex vert = vertices.back().first;
            RGBUI8 RGB = vertices.back().second;
            vertices.pop_back();
            FigureTriangle * triangle = new FigureTriangle();
            triangle->normal(0)=vert.normal_x;triangle->normal(1)=vert.normal_y;triangle->normal(2)=vert.normal_z;
            triangle->x(0)=vert.x-2;triangle->x(1)=vert.y-2;triangle->x(2)=vert.z-2;

            triangle->setRGB(RGB);
            scene.vfigure.push_back(triangle);
        }
    }




    /*! static void marchingCubePhaseField(Scene3d& scene,const Function1 & phasefied )throw(pexception)
     * \param scene input/output opengl scene
     * \param phasefied input  phasefied
     *
     * Marching cube on the level set 0 of the input phase field
     * \code
    CollectorExecutionInformationSingleton::getInstance()->setActivate(true);
    Mat3UI8 grain;
    grain.load("/home/vincent/Desktop/Population/doc/image/spinodal.pgm");
    grain = grain(Vec3I32(0,0,0),Vec3I32(100,100,100));
    grain = pop::Processing::greylevelRemoveEmptyValue(grain);//the grain label is now 1 (before 255)
    Mat3F64 phasefield = PDE::allenCahn(grain,20);
    phasefield = PDE::getField(grain,phasefield,1,6);
    Scene3d scene;
    Visualization::marchingCubePhaseField(scene,phasefield);
    Visualization::lineCube(scene,phasefield);
    scene.display();
     * \endcode
     * \image html spinodal.png
    */
    template<typename Function1>
    static void marchingCubePhaseField(Scene3d& scene,const Function1 & phasefied)throw(pexception)
    {
        if(Function1::DIM!=3)
            throw(pexception("In Vizualization::marchingCubePhaseField, Input matrix must be 3d in Visualization::marchingCube"));


        std::vector<vertex> vertices = runMarchingCubes2(phasefied,0);
        //        MatN<3,RGBUI8 >::IteratorENeighborhood itn (maRGB.getIteratorENeighborhood(1,0));
        while(vertices.empty()==false)
        {
            vertex vert = vertices.back();
            vertices.pop_back();
            FigureTriangle * triangle = new FigureTriangle();
            triangle->normal(0)=vert.normal_x;triangle->normal(1)=vert.normal_y;triangle->normal(2)=vert.normal_z;
            triangle->x(0)=vert.x-2;triangle->x(1)=vert.y-2;triangle->x(2)=vert.z-2;

            //            if(maRGB.getDomain()(0)==0){
            RGBUI8 c(200,200,200);
            triangle->setRGB(c);
            //            }else
            //            {
            //                itn.init(triangle->x);
            //                RGBUI8 c;
            //                while(itn.next()){
            //                    c = max(c,maRGBUI8(itn.x()));
            //                }
            //                triangle->setRGB(c);
            //            }
            scene.vfigure.push_back(triangle);
        }
    }
    /*! static void marchingCubePhaseField(Scene3d& scene,const Function1 & phasefied,const Function2 & RGBfield)throw(pexception)
     * \param scene input/output opengl scene
     * \param phasefied input  phasefied
     * \param RGBfield input  RGB fied
     *
     * Marching cube on the level set 0 of the input phase field with the RGB field to dress with RGB the mesh

     \code
    CollectorExecutionInformationSingleton::getInstance()->setActivate(true);
    Mat3UI8 porespace;
    porespace.load("/home/vincent/Desktop/Population/doc/image/spinodal.pgm");
    porespace = porespace(Vec3I32(0,0,0),Vec3I32(100,100,100));

    Mat3UI8 inverse(porespace);
    inverse = inverse.opposite();
    Mat3F64 dist = pop::Processing::distanceEuclidean(inverse);
    Mat3UI32 distl;
    distl= dist;
    distl = distl.opposite();
    distl = pop::Processing::dynamic(distl,2);
    Mat3UI32 minima = pop::Processing::minimaRegional(distl,0);
    Mat3UI32 water = pop::Processing::watershed(minima,distl,porespace,1);
    PDE::allenCahn(water,porespace,5000);
    water =pop::Processing::dilation(water,1,0);
    Mat3RGBUI8 color =  Visualization::labelToRandomRGB(water);
    Mat3F64 phasefield = PDE::allenCahn(porespace,10);
    phasefield = PDE::getField(porespace,phasefield,1,6);
    Scene3d scene;
    Visualization::marchingCubePhaseField(scene,phasefield,color);
    Visualization::lineCube(scene,phasefield);
    scene.display();
    \endcode
    \image html graindecomposition.png
    */
    template<typename Function1,typename Function2>
    static void marchingCubePhaseField(Scene3d& scene,const Function1 & phasefied,const Function2 & RGBfield)throw(pexception)
    {
        if(Function1::DIM!=3)
            throw(pexception("In Vizualization::marchingCubePhaseField, Input matrix must be 3d in Visualization::marchingCube"));


        std::vector<vertex> vertices = runMarchingCubes2(phasefied,0);
        while(vertices.empty()==false)
        {
            vertex vert = vertices.back();
            vertices.pop_back();
            FigureTriangle * triangle = new FigureTriangle();
            triangle->normal(0)=vert.normal_x;triangle->normal(1)=vert.normal_y;triangle->normal(2)=vert.normal_z;
            triangle->x(0)=vert.x-2;triangle->x(1)=vert.y-2;triangle->x(2)=vert.z-2;

            //            if(maRGB.getDomain()(0)==0){


            //            itn.init(triangle->x);
            //            typename Function2::F c(0);
            //            while(itn.next()){
            //                c = max(c,);
            //            }
            typename Function2::E x;
            x= triangle->x;
            if(RGBfield.isValid(x)){
                RGBUI8 fRGB(RGBfield(triangle->x));
                triangle->setRGB(fRGB);
            }
            scene.vfigure.push_back(triangle);
        }

    }


    /*! static void marchingCube(Scene3d& scene,const Function & img)throw(pexception)
     * \param scene input/output opengl scene
     * \param img input 3d matrix
     *
     * Extract the voxel boundary defined between the voxels of value 0 and the others
     *
     * \code
     * Mat3UI8 img;
     * img.load("/home/vincent/Desktop/Population/image/rock3d.pgm");
     * img = pop::Processing::median(img,3,2);

     * Mat3UI8 seed1 = pop::Processing::threshold(img,0,50);
     * Mat3UI8 seed2 = pop::Processing::threshold(img,110,140);
     * Mat3UI8 seed3 = pop::Processing::threshold(img,165,255);

     * seed1 = pop::Processing::labelMerge(seed1,seed2);
     * seed1 = pop::Processing::labelMerge(seed1,seed3);

     * Mat3UI8 gradient = pop::Processing::gradientMagnitudeDeriche(img,1);
     * Mat3UI8 basins =  pop::Processing::watershed(seed1,gradient);

     * img= Visualization::labelAverageRGB(basins,img);

     * img = pop::Processing::labelFromSingleSeed(img,seed3);
     * img=Mat3F64(img)*0.5;
     * Scene3d scene;
     * Visualization::surface(scene,img);
     * Visualization::lineCube(scene,img);
     * scene.display();
     * \endcode
     * \image html cube.png "initial image"
     * \image html surfacegl.png "Segmentation visualization"
    */
    template<typename Function>
    static void surface(Scene3d & scene , const Function & img)throw(pexception)
    {
        if(Function::DIM!=3)
            throw(pexception("In Vizualization::surface, Input matrix must be 3d in Visualization::marchingCube"));
        Function f(img.getDomain()+2);
        typename Function::IteratorEDomain it (img.getIteratorEDomain());

        while(it.next())
        {
            VecN<3,int> x  =it.x()+1;
            x=x+1;
            f(x)=img(it.x());
        }

        typename Function::IteratorEDomain itg(f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn(f.getIteratorENeighborhood(1,1));

        while(itg.next()){
            if(f(itg.x())!=typename Function::F(0) ){
                itn.init(itg.x());
                while(itn.next()){
                    if(f(itn.x())==typename Function::F(0)){
                        FigureUnitSquare * square = new FigureUnitSquare();
                        RGBUI8 c(f(itg.x()));
                        square->setRGB(c);
                        square->setTransparent(255);
                        square->x = itg.x()-1;
                        for(int i = 0;i<=2;i++ ){
                            if(itg.x()(i)!=itn.x()(i)){
                                square->direction = i;
                                square->way = (itn.x()(i)-itg.x()(i));
                                if((itn.x()(i)-itg.x()(i))>0)
                                    square->x(i)++;
                            }
                        }
                        scene.vfigure.push_back(square);
                    }
                }
            }
        }
    }


    /*! static void  plane(Scene3d &scene, const Function & img,int slice=0, int direction=2)throw(pexception)
     * \param scene input/output opengl scene
     * \param img input 3d matrix
     * \param slice slice index
     * \param direction coordinate
     *
     * add a plane of the matrix at a given slice and direrection to the opengl scene
     *
     * \code
     * Mat3UI8 img;
     * img.load("/home/vincent/Desktop/Population/image/rock3d.pgm");
     * Scene3d scene;
     * Visualization::plane(scene,img,50,2);
     * Visualization::plane(scene,img,50,1);
     * Visualization::plane(scene,img,200,0);
     * Visualization::lineCube(scene,img);

     * scene.display();
     * \endcode
     * \image html planegl.png
    */

    template<typename Function>
    static void  plane(Scene3d &scene, const Function & img,int slice=0, int direction=2)throw(pexception)
    {
        if(Function::DIM!=3)
            throw(pexception("In Vizualization::plane, Input matrix must be 3d in Visualization::plane"));
        const int DIM =3;

        typename Function::Hyperplane hyperff;

        if(direction<0||direction>=3)
            direction=2;
        if(slice>=img.getDomain()(direction))
            slice =img.getDomain()(direction)-1;
        if(slice<0)
            slice =0;
        hyperff = img.getPlane(direction,slice);
        typename Function::Hyperplane::IteratorEDomain it (hyperff.getIteratorEDomain());
        VecN<DIM,F64 > normal,normal2;

        normal(direction)=1;
        normal2(direction)=-1;
        VecN<DIM,F64 > add1;
        add1=0;
        add1( (direction+1)%DIM)=1;
        VecN<DIM,F64 > add2;
        add2=0;
        add2( (direction+1)%DIM)=1;add2( (direction+2)%DIM)=1;
        VecN<DIM,F64 > add3;
        add3=0;
        add3( (direction+2)%DIM)=1;

        VecN<3,F64> x;
        VecN<3,F64> y;
        x(direction)=slice;
        while(it.next())
        {
            FigurePolygon * poly = new FigurePolygon();
            RGBUI8 c(hyperff(it.x()));
            poly->setRGB( c);
            poly->setTransparent(255);
            poly->normal= normal;
            for(int i=0;i<DIM;i++)
            {
                if(i<direction)
                    x(i)=it.x()(i);
                else if(i>direction)
                    x(i)=it.x()(i-1);
            }
            y =x;
            poly->vpos.push_back(x);
            y = x+add1;
            poly->vpos.push_back(y);
            y = x+add2;
            poly->vpos.push_back(y);
            y = x+add3;
            poly->vpos.push_back(y);
//            FigurePolygon * poly2 = poly->clone();
//            poly->normal=normal2;
            scene.vfigure.push_back(poly);
            //scene.vfigure.push_back(poly2);
        }
    }
    /*! static void  graph(Scene3d &scene, const GraphAdjencyList<VertexPosition,Edge> & g)
     * \param scene input/output opengl scene
     * \param g input graph
     *
     *
    \code
    CollectorExecutionInformationSingleton::getInstance()->setActivate(true);
    Mat3UI8 grain;
    grain.load("/home/vincent/Desktop/Population/doc/image/spinodal.pgm");
    grain = grain(Vec3I32(0,0,0),Vec3I32(100,100,100));
    grain = pop::Processing::greylevelRemoveEmptyValue(grain);//the grain label is now 1 (before 255)
    //TOLOGICAL GRAPH
    Mat3UI8 grain_hole=   pop::Processing::holeFilling(grain);
    Mat3UI8 skeleton= Analysis::thinningAtConstantTopology3d(grain_hole,"/home/vincent/Desktop/Population/file/topo24.dat");
    std::pair<Mat3UI8,Mat3UI8> vertex_edge = Analysis::fromSkeletonToVertexAndEdge (skeleton);
    Mat3UI32 verteces = pop::Processing::clusterToLabel(vertex_edge.first,0);
    Mat3UI32 edges = pop::Processing::clusterToLabel(vertex_edge.second,0);
    int tore;
    GraphAdjencyList<VertexPosition,Edge> g = Analysis::linkEdgeVertex(verteces,edges,tore);
    Mat3F64 phasefield = PDE::allenCahn(grain,20);
    phasefield = PDE::getField(grain,phasefield,1,6);
    Scene3d scene;
    Visualization::marchingCubePhaseField(scene,phasefield);
    for(int i=0;i<(int)scene.vfigure.size();i++)
        scene.vfigure[i]->setTransparent(40);
    scene.setTransparentMode(true);
    Visualization::graph(scene,g);
    Visualization::lineCube(scene,edges);
    scene.display(false);

    int i=0;
    while(1==1){
        scene.lock();
        i++;
         std::string file = "Graph"+BasicUtility::IntFixedDigit2String(i,4)+".pgm";
        scene.rotateZ(5);
        std::cout<<i<<std::endl;
        if(i==100)
            return 1;
        scene.unlock();
        scene.snapshot(file.c_str());
    }
    \endcode
    \image html Graph.gif
    */
    static void  graph(Scene3d &scene,  GraphAdjencyList<VertexPosition,Edge> & g)
    {
        RGB<unsigned char> c1(255,0,0);
        for ( int i =0; i < (int)g.sizeVertex(); i++ ){
            FigureSphere *  sphere =  new FigureSphere;
            VecF64 v  =g.vertex(i).getPos();
            Vec<F32> vv;
            vv = v;
            sphere->_x=vv;
            sphere->_radius=1;
            sphere->setRGB(c1);
            sphere->setTransparent(255);
            scene.vfigure.push_back(sphere);

        }
        RGB<unsigned char> c2(0,255,0);
        for ( int i =0; i < (int)g.sizeEdge(); i++ ){
            FigureLine * line = new FigureLine;
            std::pair<int,int> p  =g.getLink(i);
            VecF64 v1  =g.vertex(p.first).getPos();
            Vec<F32> vv;
            vv = v1;
            line->x1= vv;
            VecF64 v2  =g.vertex(p.second).getPos();
            vv =v2;
            line->x2=vv;
            line->setTransparent(255);
            line->width=1;
            line->setRGB( c2);
            scene.vfigure.push_back(line);
        }

    }
    template<typename Type>
    static void  topography(Scene3d &scene, const MatN<2,Type> & img)
    {
        Type maxi = 0;
        ForEachDomain(x,img){
            maxi = maximum(maxi,img(x));
            Vec2I32 x1(x),x2(x),x3(x);
            x2=x;x2(0)--;
            if(img.isValid(x2)){
                x3=x;x3(1)--;
                if(img.isValid(x3)){
                    Vec3F64 xx1,xx2,xx3;
                    xx1(0)=x1(0);xx1(1)=x1(1);xx1(2)=img(x1);
                    xx2(0)=x2(0);xx2(1)=x2(1);xx2(2)=img(x2);
                    xx3(0)=x3(0);xx3(1)=x3(1);xx3(2)=img(x3);
                    FigurePolygon * poly = FigurePolygon::createTriangle(xx1,xx2,xx3);
                    if(poly->normal(2)<0)
                        poly->normal =-poly->normal;
                    scene.vfigure.push_back(poly);
                }
                x3=x;x3(1)++;
                if(img.isValid(x3)){
                    Vec3F64 xx1,xx2,xx3;
                    xx1(0)=x1(0);xx1(1)=x1(1);xx1(2)=img(x1);
                    xx2(0)=x2(0);xx2(1)=x2(1);xx2(2)=img(x2);
                    xx3(0)=x3(0);xx3(1)=x3(1);xx3(2)=img(x3);
                    FigurePolygon * poly = FigurePolygon::createTriangle(xx1,xx2,xx3);
                    if(poly->normal(2)<0)
                        poly->normal =-poly->normal;
                    scene.vfigure.push_back(poly);
                }
            }
            x2=x;x2(0)++;
            if(img.isValid(x2)){
                x3=x;x3(1)--;
                if(img.isValid(x3)){
                    Vec3F64 xx1,xx2,xx3;
                    xx1(0)=x1(0);xx1(1)=x1(1);xx1(2)=img(x1);
                    xx2(0)=x2(0);xx2(1)=x2(1);xx2(2)=img(x2);
                    xx3(0)=x3(0);xx3(1)=x3(1);xx3(2)=img(x3);
                    FigurePolygon * poly = FigurePolygon::createTriangle(xx1,xx2,xx3);
                    if(poly->normal(2)<0)
                        poly->normal =-poly->normal;
                    scene.vfigure.push_back(poly);
                }
                x3=x;x3(1)++;
                if(img.isValid(x3)){
                    Vec3F64 xx1,xx2,xx3;
                    xx1(0)=x1(0);xx1(1)=x1(1);xx1(2)=img(x1);
                    xx2(0)=x2(0);xx2(1)=x2(1);xx2(2)=img(x2);
                    xx3(0)=x3(0);xx3(1)=x3(1);xx3(2)=img(x3);
                    FigurePolygon * poly = FigurePolygon::createTriangle(xx1,xx2,xx3);
                    if(poly->normal(2)<0)
                        poly->normal =-poly->normal;
                    scene.vfigure.push_back(poly);
                }
            }

        }
        std::cout<<maxi<<std::endl;
        double sizepeak=maximum(1.,img.getDomain()(0)*0.05);
        FigureArrow * arrow = new FigureArrow;
        arrow->setRGB(RGBUI8(255,0,0));arrow->setArrow(Vec3F64(0,0,0),Vec3F64(img.getDomain()(0)+2,0,0),sizepeak);scene.vfigure.push_back(arrow);
        arrow = new FigureArrow;
        arrow->setRGB(RGBUI8(255,0,0));arrow->setArrow(Vec3F64(0,0,0),Vec3F64(0,img.getDomain()(1)+2,0),sizepeak);scene.vfigure.push_back(arrow);
        arrow = new FigureArrow;
        arrow->setRGB(RGBUI8(255,0,0));arrow->setArrow(Vec3F64(0,0,0),Vec3F64(0,0,maxi+2),sizepeak);scene.vfigure.push_back(arrow);

    }
    void  transparency(Scene3d &scene, UI8 transparency);
    //@}
    //-------------------------------------
    //
    //! \name Vec field
    //@{
    //-------------------------------------

    /*! static typename FunctionTypeTraitsSubstituteF<Function,RGBUI8>::Result vectorField2DToArrows(const Function & vectorfield,RGBUI8 cmin=RGBUI8(0,0,255),RGBUI8 cmax=RGBUI8(255,0,0),int step=32,F64 ratio=0.25)
     * \param vectorfield input vector field
     * \param cmin min RGB
     * \param cmax max RGB
     * \param step step between two arrows
     * \param ratio ratio size of the arrow between the fixed one and the variable given but the norm of the vector
     *
     *
    \code
    CollectorExecutionInformationSingleton::getInstance()->setActivate(true);//execution information in the standard outstream
    Mat2UI8 img;
    img.load("/home/vincent/Desktop/Population/doc/image/outil.bmp");
    img = img.opposite();
    Mat2Vec2F64 vel;
    int dir=0;
    PDE::permeability(img,vel,dir,0.1);
    vel= GeometricalTransformation::scale(vel,Vec2F64(8));
    Mat2RGBUI8 c = Visualization::vectorField2DToArrows(vel);
    c.display("velocity",true,false);
    \endcode
    The matrix is the final matrix of this animation:
    \image html outilvelocity.gif
    */
    template< typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,RGBUI8>::Result vectorField2DToArrows(const Function & vectorfield,RGBUI8 cmin=RGBUI8(0,0,255),RGBUI8 cmax=RGBUI8(255,0,0),int step=32,F64 ratio=0.25){
        typename Function::IteratorEDomain it(vectorfield.getIteratorEDomain());
        typename Identity<typename Function::F>::Result::F maxi = NumericLimits<typename Identity<typename Function::F>::Result::F   >::minimumRange();
        typename Identity<typename Function::F>::Result::F mini = NumericLimits<typename Identity<typename Function::F>::Result::F   >::maximumRange();
        while(it.next()){
            maxi = maximum(maxi,vectorfield(it.x()).norm());
            mini = minimum(mini,vectorfield(it.x()).norm());
        }
        RGBUI8 v_init(0);
        std::vector< RGBUI8 > v(maxi+1-mini, v_init);
        RGBF64 cminf,cmaxf;
        cminf = cmin;
        cmaxf = cmax;

        for(I32 i=0;i<=maxi-mini;i++)
        {
            double dist = i*1.0/(maxi-mini);
            v[i]=dist*(cmaxf-cminf)+cminf;
        }
        typename FunctionTypeTraitsSubstituteF<Function,RGBUI8>::Result fRGB(vectorfield.getDomain());
        typedef typename FunctionTypeTraitsSubstituteF<typename Function::E,F64>::Result VecNFloat;
        it.init();
        while(it.next()){
            if(it.x()(0)%step==0&&it.x()(1)%step==0){
                pop::F64 value = vectorfield(it.x()).norm();
                if(value!=0){
                    VecNFloat x1 = it.x();
                    VecNFloat x2 = x1 + (ratio+(1-ratio)*(value-mini)/(maxi-mini) )*(step-1)*vectorfield(it.x())/vectorfield(it.x()).norm();
                    Draw::arrow(fRGB,x1,x2,v[value-mini]);
                }
            }
        }
        return fRGB;
    }



    //@}

protected:
    struct vertex {
        F32 x, y, z, normal_x, normal_y, normal_z;
    };


    struct Cube {
        vertex p[8];
        F32 val[8];
    };
    struct cubeF {
        vertex p[8];
        F32 val[8];
    };


    static void processCube(Cube cube, std::vector<std::pair<vertex,RGBUI8 > >& vertexList,RGBUI8 value,bool diff=false);
    static void processCubeIso(Cube cube, std::vector<std::pair<vertex,RGBUI8 > >& vertexList,RGBUI8 value,unsigned char isolevel);
    static void processCube(cubeF cube, std::vector<vertex>& vertexList,F32 isolevel =0.5,bool diff=false);
    static std::vector<std::pair<vertex,RGBUI8 > > runMarchingCubes2(const MatN<3,RGB<UI8 > > &voxel);
    static std::vector<vertex > runMarchingCubes2(const MatN<3,F64 > &phasefield,F32 isosurface) ;
    static std::vector<std::pair<vertex,RGBUI8 > > runMarchingCubesSurfaceContact(const MatN<3,RGB<UI8 > > &voxel);
    static Visualization::vertex interpolate(Visualization::vertex p1, Visualization::vertex p2, F32 p1value=1, F32 p2value=0 , F32 iso=0.5 );
public:
#ifdef WITHSWIG
    template<int DIM,typename Type>
    static MatN<DIM,RGBUI8> __labelToRandomRGB(const MatN<DIM,Type> & f )throw(pexception)
    {
        return labelToRandomRGB(f);
    }

    template<int DIM,typename Type>
    static MatN<DIM,RGBUI8> __labelToRGBGradation(const MatN<DIM,Type> & f,RGBUI8 cmin=RGBUI8(0,0,255),RGBUI8 cmax=RGBUI8(255,0,0)  )throw(pexception)
    {
        return labelToRGBGradation(f,cmin,cmax);
    }
    template<int DIM,typename Type1,typename Type2>
    static MatN<DIM,Type2> __labelAverageRGB(const MatN<DIM,Type1> &label ,const MatN<DIM,Type2> &img  )throw(pexception)
    {
        return labelAverageRGB(label,img);
    }
    template<int DIM,typename Type1,typename Type2>
    static MatN<DIM,RGBUI8> __labelForegroundBoundary(const MatN<DIM,Type1> &label ,const MatN<DIM,Type2> &img,int width=1,int norm=1  )throw(pexception)
    {
        return labelForegroundBoundary(label,img,width,norm);
    }
    template<int DIM,typename Type1,typename Type2>
    static MatN<DIM,RGBUI8> __labelForeground(const MatN<DIM,Type1> &label ,const MatN<DIM,Type2> &img,F64 ratio=0.5 )throw(pexception)
    {
        return labelForeground(label,img,ratio);
    }
    template< typename Type>
    MatN<2,RGBUI8> __vectorField2DToArrows(const MatN<2,Type> & vectorfield,RGBUI8 cmin=RGBUI8(0,0,255),RGBUI8 cmax=RGBUI8(255,0,0),int step=32,F64 ratio=0.25){
        return vectorField2DToArrows(vectorfield,cmin,cmax,step,ratio);
    }
#endif
};
}
#endif // VISUALIZATION_H

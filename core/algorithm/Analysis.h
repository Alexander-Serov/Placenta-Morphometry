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

#ifndef ANALYSIS_H
#define ANALYSIS_H

#include"core/data/typeF/TypeF.h"
#include"core/data/mat/MatN.h"
#include"core/data/mat/Mat2x.h"
#include"core/data/distribution/DistributionAnalytic.h"
#include"core/data/typeF/TypeTraitsF.h"
#include"core/data/functor/FunctorF.h"
#include"core/algorithm/FunctionProcedureFunctorF.h"
#include"core/data/mat/MatN.h"
#include"core/data/utility/CollectorExecutionInformation.h"
#include"core/algorithm/ProcessingAdvanced.h"
#include"core/algorithm/Representation.h"
#include"core/data/notstable/graph/Graph.h"
#include"core/data/notstable/graph/Vertex.h"
#include"core/data/notstable/graph/Edge.h"
#include"core/algorithm/Statistics.h"
#include"core/algorithm/AnalysisAdvanced.h"

/*!
     * \defgroup Analysis Analysis
     * \ingroup Algorithm
     * \brief Matrix In -> Measure (2-point correlation function, REV, histogram,...)
     *
     * After segmentation, space is partitioned into phases. One challenging problem deals with the ability to describe the geometrical organisation of these phases.
     * Quantitative knowledge of this organisation is important in many fields, for instance, in porous media, for the understanding of the role of geometric confinement in adsorption,
     * condensation, transport, reaction processes. As seen in my PhD, an analysis can be divided in two categories~:
     *   - geometrical information about forms, shapes and patterns like the average pore size, the mean curvature, the pore shape, the surface roughness, the structural correlation between pixels belonging either to the solid, to the interface or to the pore network,
     *   - topological information about connectivity like the percolation, the coordination number of the topological graph.

     * This class provides a collection of algorithms to analyse a 2d/3d matrix. A simple analysis can be as follows:

     * \code
     * #include"core/data/utility/CollectorExecutionInformation.h"
     * #include"core/data/mat/MatN.h"
     * #include"core/algorithm/Analysis.h"
     * using namespace pop;
     * int main(){
     *     Mat2UI8 lena;
     *     lena.load("/home/vincent/Desktop/Population/doc/image/Lena.pgm");
     *      std::cout<<Analysis::maxValue(lena)<<std::endl;
     *     return 0;
     * }
     * \endcode
     * Most of the algorithms return a matrix containing the information. To plot this information, you can convert it in Distribution and display it as follows:
     * \code
     * Mat2F64 mverpore = Analysis::REVPorosity(grain,VecN<3,F64>(grain.getDomain())*0.5,200);
     * Distribution dverpor(mverpore);
     * dverpor.display();
     * mverpore.saveAscii("ver.txt");
     * \endcode
     * Because this displayer is not nice, an another solution is to save the ascii data and to open it with gnuplot or matlab.\n
     * This code presents a extended analysis of a 3d matrix (to have the 3d visualization, uncomment the lines of code):
     * \code
     * CollectorExecutionInformationSingleton::getInstance()->setActivate(true);//Activate the information manager (3d processing can be long so have
     * Mat3UI8 porespace;
     * porespace.load("/home/vincent/Desktop/Population/doc/image/spinodal.pgm");
     * porespace = pop::Processing::greylevelRemoveEmptyValue(porespace);//the porespace label is now 1 (before 255)
     * //porespace = porespace(Vec3I32(0,0,0),Vec3I32(50,50,50));//for the first execution, you can test the process in small sample
     * \endcode
     * \image html spinodal.png "Spinodal structure (see pop::Visualization::marchingCubePhaseField)"
     * \code
     * //############ Representative Elementary Volume CHECKING ###########################
     * Mat2F64 mverpore = Analysis::REVPorosity(porespace,VecN<3,F64>(porespace.getDomain())*0.5,200);
     * mverpore.saveAscii("spinodal_VER.m");
     * \endcode
     * \image html spinodal_VER.png "The sample size is twice time larger than the stablilzation of the porosity measurement -> so REV oke"
     * \code
     * //############ METRIC ###################
     * Mat2F64 mhisto = Analysis::histogram(porespace);
     * std::cout<<"porespace fraction:"<<mhisto(1,1)<<std::endl;
     * Mat2F64 mchord = Analysis::chord(porespace);
     * mchord.saveAscii("spinodal_chord.m");
     * mchord.deleteCol(1);
     * Distribution dchord_solid(mchord);
     * std::cout<<"Charateristic length of solid space "<<Statistics::moment(dchord_solid,1,0,300,1)<<std::endl;//  \sum_{i=0}^{300} i*d(i)=27.2
     * \endcode
     * \image html spinodal_chord.png "Peak following by exponantial decrease (for the meaning of this signature see Chap 2, p 37. In Handbook of Porous Media. P. Levitz)"
     * \code
     * Mat2F64 mcorre= Analysis::correlation(porespace);
     * mcorre.saveAscii("spinodal_corre.m");
     * \endcode
     * \image html spinodal_corr.png "No long-range oscillion=No periodic structure and REV oke as in REVPorosity"
     * \code
     * Mat3F64 corre= Analysis::correlationDirectionByFFT(porespace);
     * Mat3F64 corre= Analysis::correlationDirectionByFFT(porespace);
     * corre = GeometricalTransformation::translate(corre,corre.getDomain()/2);//centered
     * corre = corre(corre.getDomain()/2-Vec3I32(20,20,20),corre.getDomain()/2+Vec3I32(20,20,20));//take only the core (the correlation length is near 20)
     * Mat3UI8 dcorre;
     * dcorre= pop::Processing::greylevelRange(corre,1,255);//0 is black color so start at 1
     * Mat3RGBUI8 dcorregrad=Visualization::labelToRGBGradation(dcorre);//color gradation
     * Scene3d scene;
     * Visualization::plane(scene,dcorregrad,dcorre.getDomain()(0)/2,0);
     * Visualization::plane(scene,dcorregrad,dcorre.getDomain()(1)/2,1);
     * Visualization::plane(scene,dcorregrad,dcorre.getDomain()(2)/2,2);
     * Visualization::lineCube(scene,dcorregrad);
     * scene.display();//if you close the opengl windows, that stop the program in linux. So comment this line in linux to execute the rest of the code
     * \endcode
     * \image html spinodal_corr3d.png "We observe an isotropic 2-point correlation function->structure is istropic"
     * \code
     * Mat3UI8 fdistance;
     * Mat2F64 mldistance= Analysis::ldistance(porespace,2,fdistance);//euclidean distance only in Population library ;-)
     * mldistance.saveAscii("spinodal_ldistance.m");
     * Mat3RGBUI8 dcorregrad=Visualization::labelToRandomRGB(fdistance);//random color
     * Scene3d scene;
     * Visualization::plane(scene,dcorregrad,dcorregrad.getDomain()(0)/2,0);
     * Visualization::plane(scene,dcorregrad,dcorregrad.getDomain()(1)/2,1);
     * Visualization::plane(scene,dcorregrad,dcorregrad.getDomain()(2)/2,2);
     * Visualization::lineCube(scene,dcorregrad);
     * scene.display();
     * \endcode
     * \image html spinodal_ldistance_pore.png "see Pore-Size Probability Density Function in Torquato's book"
     * \code
     * Mat3UI8 fgranulo;
     * //granulo of the solid space
     * Mat2F64 mlgranulo= Analysis::granulometryMatheron(porespace,2,fgranulo);//quite long algorithm in euclidean distance
     * mlgranulo.saveAscii("spinodal_granulo.m");
     * Mat3RGBUI8 dcorregrad=Visualization::labelToRandomRGB(fgranulo);//random color
     * Scene3d scene;
     * Visualization::plane(scene,dcorregrad,dcorregrad.getDomain()(0)/2,0);
     * Visualization::plane(scene,dcorregrad,dcorregrad.getDomain()(1)/2,1);
     * Visualization::plane(scene,dcorregrad,dcorregrad.getDomain()(2)/2,2);
     * Visualization::lineCube(scene,dcorregrad);
     * scene.display();
     * \endcode
     * \image html spinodal_granulo.png "see X-ray microtomography characterisation of the changes in statistical homogeneity of an unsaturated sand during imbibitions"
     * \code
     * Mat2F64 mgeometrical = Analysis::geometricalTortuosity(porespace);
     * mgeometrical.saveAscii("spinodal_geometrical.m");
     * \endcode
     * We get  1.19465 in 3 directions // see Jeulin's paper estimation of tortuosity and reconstruction of geodesic paths in 3d
     * \code
     * //############ TOPOLOGY #######################
     * double euler = Analysis::eulerPoincare(porespace,"/home/vincent/Desktop/Population/file/eulertab.dat");
     * std::ofstream out("spinodal_euler.m");
     * out<<euler;//euler
     * out.close();
     * Mat2F64 mpercolationopening = Analysis::percolationOpening(porespace,2);//->charactertic length related to permeability
     * mpercolationopening.saveAscii("spinodal_percolationopening.m");//output is 6 in three direction-> the structure sill percolates after an opening of ball of radius of 6 but not with a radius of size 7
     * Mat2F64 mpercolationerosion = Analysis::percolationErosion(porespace,2);
     * mpercolationerosion.saveAscii("spinodal_percolationerosion.m");//output is 5 in three direction-> the structure sill percolates after an erosion of ball of radius of 5 but not with a radius of size 6


     * Mat3UI8 porespace_hole=   pop::Processing::holeFilling(porespace);
     * Mat3UI8 skeleton= Analysis::thinningAtConstantTopology3d(porespace_hole,"/home/vincent/Desktop/Population/file/topo24.dat");
     * Scene3d scene;
     * pop::Visualization::surface(scene,skeleton);
     * pop::Visualization::lineCube(scene,skeleton);
     * scene.display();
     * \endcode
     * \image html spinodal_skeleton.png "Topological skeleton"
     * \code
     * std::pair<Mat3UI8,Mat3UI8> vertex_edge = Analysis::fromSkeletonToVertexAndEdge (skeleton);
     * Mat3UI32 verteces = pop::Processing::clusterToLabel(vertex_edge.first,0);
     * Mat3UI32 edges = pop::Processing::clusterToLabel(vertex_edge.second,0);
     * pop::Visualization::surface(scene,pop::Visualization::labelToRandomRGB(edges));
     * pop::Visualization::lineCube(scene,edges);
     * scene.display();
     * \endcode
     * \image html spinodal_edge.png "Labelisation of the edges of the topological skeleton"
     * \code
     * int tore;
     * GraphAdjencyList<VertexPosition,Edge> g = Analysis::linkEdgeVertex(verteces,edges,tore);
     * Scene3d scene;
     * pop::Visualization::graph(scene,g);
     * pop::Visualization::lineCube(scene,edges);
     * scene.display();
     * \endcode
     * \image html spinodal_graph.png "Topological graph"
     * \code
     * std::cout<<euler/g.sizeVertex()<<std::endl;//N_3/alpha_0 normalised topogical characteristic (connectivity number in my phd)

     * //############ PHYSICAL ###################
     * Mat2F64 mdiffusion = PDE::randomWalk(porespace);
     * mdiffusion.saveAscii("spinodal_self_diffusion.m");
     * \endcode
     * \image html spinodal_self_diffusion.png "Coefficient of self diffusion"
     * \code
     * MatN<3,Vec3F64> velocityfield;
     * Mat2F64 K(3,3);
     * VecF64 kx = PDE::permeability(porespace,velocityfield,0,0.05);//permeability in x-direction
     * VecF64 ky = PDE::permeability(porespace,velocityfield,1,0.05);//permeability in y-direction
     * VecF64 kz = PDE::permeability(porespace,velocityfield,2,0.05);//permeability in z-direction
     * //merge the results in the permeability matrix
     * K.setCol(0,kx);
     * K.setCol(1,ky);
     * K.setCol(2,kz);

     * //display the norm of the last valocity field
     * Mat3F64 velocityfield_norm(velocityfield);
     * ForEachDomain(x,velocityfield)
     * {
     *     velocityfield_norm(x)=normValue(velocityfield(x));
     * }
     * Mat3RGBUI8 velocityfield_norm_grad= pop::Visualization::labelToRGBGradation(velocityfield_norm);
     * Scene3d scene;
     * Visualization::plane(scene,velocityfield_norm_grad,velocityfield_norm_grad.getDomain()(0)/2,0);
     * Visualization::plane(scene,velocityfield_norm_grad,velocityfield_norm_grad.getDomain()(1)/2,1);
     * Visualization::plane(scene,velocityfield_norm_grad,velocityfield_norm_grad.getDomain()(2)/2,2);
     * Visualization::lineCube(scene,velocityfield_norm_grad);
     * scene.display();
     * \endcode
     * \image html spinodal_permeability.png "Amplitude of the velocity field"


*/

namespace pop
{
struct POP_EXPORTS Analysis
{

    /*!
     * \class pop::Analysis
     * \ingroup Analysis
     * \brief Analyse a 2D/3D matrix
     * \author Tariel Vincent
     *
     * After segmentation, space is partitioned into phases. One challenging problem deals with the ability to describe the geometrical organisation of these phases.
     * Quantitative knowledge of this organisation is important in many fields, for instance, in porous media, for the understanding of the role of geometric confinement in adsorption,
     * condensation, transport, reaction processes. As seen in my PhD, an analysis can be divided in two categories~:
     *   - geometrical information about forms, shapes and patterns like the average pore size, the mean curvature, the pore shape, the surface roughness, the structural correlation between pixels belonging either to the solid, to the interface or to the pore network,
     *   - topological information about connectivity like the percolation, the coordination number of the topological graph.

     * This class provides a collection of algorithms to analyse a 2d/3d matrix. A simple analysis can be as follows:

     * \code
     * #include"core/data/utility/CollectorExecutionInformation.h"
     * #include"core/data/mat/MatN.h"
     * #include"core/algorithm/Analysis.h"
     * using namespace pop;
     * int main(){
     *     Mat2UI8 lena;
     *     lena.load("/home/vincent/Desktop/Population/doc/image/Lena.pgm");
     *      std::cout<<Analysis::maxValue(lena)<<std::endl;
     *     return 0;
     * }
     * \endcode
     * Most of the algorithms return a matrix containing the information. To plot this information, you can convert it in Distribution and display it as follows:
     * \code
     * Mat2F64 mverpore = Analysis::REVPorosity(grain,VecN<3,F64>(grain.getDomain())*0.5,200);
     * Distribution dverpor(mverpore);
     * dverpor.display();
     * mverpore.saveAscii("ver.txt");
     * \endcode
     * Because this displayer is not nice, an another solution is to save the ascii data and to open it with gnuplot or matlab.\n
     * This code presents a extended analysis of a 3d matrix (to have the 3d visualization, uncomment the lines of code):
     * \code
     * CollectorExecutionInformationSingleton::getInstance()->setActivate(true);//Activate the information manager (3d processing can be long so have
     * Mat3UI8 porespace;
     * porespace.load("/home/vincent/Desktop/Population/doc/image/spinodal.pgm");
     * porespace = pop::Processing::greylevelRemoveEmptyValue(porespace);//the porespace label is now 1 (before 255)
     * //porespace = porespace(Vec3I32(0,0,0),Vec3I32(50,50,50));//for the first execution, you can test the process in small sample
     * \endcode
     * \image html spinodal.png "Spinodal structure (see pop::Visualization::marchingCubePhaseField)"
     * \code
     * //############ Representative Elementary Volume CHECKING ###########################
     * Mat2F64 mverpore = Analysis::REVPorosity(porespace,VecN<3,F64>(porespace.getDomain())*0.5,200);
     * mverpore.saveAscii("spinodal_VER.m");
     * \endcode
     * \image html spinodal_VER.png "The sample size is twice time larger than the size when the porosity measurement is stable->so REV oke"
     * \code
     * //############ METRIC ###################
     * Mat2F64 mhisto = Analysis::histogram(porespace);
     * std::cout<<"porespace fraction:"<<mhisto(1,1)<<std::endl;
     * Mat2F64 mchord = Analysis::chord(porespace);
     * mchord.saveAscii("spinodal_chord.m");
     * mchord.deleteCol(1);
     * Distribution dchord_solid(mchord);
     * std::cout<<"Charateristic length of solid space "<<Statistics::moment(dchord_solid,1,0,300,1)<<std::endl;//  \sum_{i=0}^{300} i*d(i)=27.2
     * \endcode
     * \image html spinodal_chord.png "Peak following by exponantial decrease (for the meaning of this signature see Chap 2, p 37. In Handbook of Porous Media. P. Levitz)"
     * \code
     * Mat2F64 mcorre= Analysis::correlation(porespace);
     * mcorre.saveAscii("spinodal_corre.m");
     * \endcode
     * \image html spinodal_corr.png "No long-range oscillion=No periodic structure and REV oke as in REVPorosity"
     * \code
     * Mat3F64 corre= Analysis::correlationDirectionByFFT(porespace);
     * Mat3F64 corre= Analysis::correlationDirectionByFFT(porespace);
     * corre = GeometricalTransformation::translate(corre,corre.getDomain()/2);//centered
     * corre = corre(corre.getDomain()/2-Vec3I32(20,20,20),corre.getDomain()/2+Vec3I32(20,20,20));//take only the core (the correlation length is near 20)
     * Mat3UI8 dcorre;
     * dcorre= pop::Processing::greylevelRange(corre,1,255);//0 is black color so start at 1
     * Mat3RGBUI8 dcorregrad=Visualization::labelToRGBGradation(dcorre);//color gradation
     * Scene3d scene;
     * Visualization::plane(scene,dcorregrad,dcorre.getDomain()(0)/2,0);
     * Visualization::plane(scene,dcorregrad,dcorre.getDomain()(1)/2,1);
     * Visualization::plane(scene,dcorregrad,dcorre.getDomain()(2)/2,2);
     * Visualization::lineCube(scene,dcorregrad);
     * scene.display();//if you close the opengl windows, that stop the program in linux. So comment this line in linux to execute the rest of the code
     * \endcode
     * \image html spinodal_corr3d.png "We observe an isotropic 2-point correlation function->structure is istropic"
     * \code
     * Mat3UI8 fdistance;
     * Mat2F64 mldistance= Analysis::ldistance(porespace,2,fdistance);//euclidean distance only in Population library ;-)
     * mldistance.saveAscii("spinodal_ldistance.m");
     * Mat3RGBUI8 dcorregrad=Visualization::labelToRandomRGB(fdistance);//random color
     * Scene3d scene;
     * Visualization::plane(scene,dcorregrad,dcorregrad.getDomain()(0)/2,0);
     * Visualization::plane(scene,dcorregrad,dcorregrad.getDomain()(1)/2,1);
     * Visualization::plane(scene,dcorregrad,dcorregrad.getDomain()(2)/2,2);
     * Visualization::lineCube(scene,dcorregrad);
     * scene.display();
     * \endcode
     * \image html spinodal_ldistance_pore.png "see Pore-Size Probability Density Function in Torquato's book"
     * \code
     * Mat3UI8 fgranulo;
     * //granulo of the solid space
     * Mat2F64 mlgranulo= Analysis::granulometryMatheron(porespace,2,fgranulo);//quite long algorithm in euclidean distance
     * mlgranulo.saveAscii("spinodal_granulo.m");
     * Mat3RGBUI8 dcorregrad=Visualization::labelToRandomRGB(fgranulo);//random color
     * Scene3d scene;
     * Visualization::plane(scene,dcorregrad,dcorregrad.getDomain()(0)/2,0);
     * Visualization::plane(scene,dcorregrad,dcorregrad.getDomain()(1)/2,1);
     * Visualization::plane(scene,dcorregrad,dcorregrad.getDomain()(2)/2,2);
     * Visualization::lineCube(scene,dcorregrad);
     * scene.display();
     * \endcode
     * \image html spinodal_granulo.png "see X-ray microtomography characterisation of the changes in statistical homogeneity of an unsaturated sand during imbibitions"
     * \code
     * Mat2F64 mgeometrical = Analysis::geometricalTortuosity(porespace);
     * mgeometrical.saveAscii("spinodal_geometrical.m");
     * \endcode
     * We get  1.19465 in 3 directions // see Jeulin's paper estimation of tortuosity and reconstruction of geodesic paths in 3d
     * \code
     * //############ TOPOLOGY #######################
     * double euler = Analysis::eulerPoincare(porespace,"/home/vincent/Desktop/Population/file/eulertab.dat");
     * std::ofstream out("spinodal_euler.m");
     * out<<euler;//euler
     * out.close();
     * Mat2F64 mpercolationopening = Analysis::percolationOpening(porespace,2);//->charactertic length related to permeability
     * mpercolationopening.saveAscii("spinodal_percolationopening.m");//output is 6 in three direction-> the structure sill percolates after an opening of ball of radius of 6 but not with a radius of size 7
     * Mat2F64 mpercolationerosion = Analysis::percolationErosion(porespace,2);
     * mpercolationerosion.saveAscii("spinodal_percolationerosion.m");//output is 5 in three direction-> the structure sill percolates after an erosion of ball of radius of 5 but not with a radius of size 6


     * Mat3UI8 porespace_hole=   pop::Processing::holeFilling(porespace);
     * Mat3UI8 skeleton= Analysis::thinningAtConstantTopology3d(porespace_hole,"/home/vincent/Desktop/Population/file/topo24.dat");
     * Scene3d scene;
     * pop::Visualization::surface(scene,skeleton);
     * pop::Visualization::lineCube(scene,skeleton);
     * scene.display();
     * \endcode
     * \image html spinodal_skeleton.png "Topological skeleton"
     * \code
     * std::pair<Mat3UI8,Mat3UI8> vertex_edge = Analysis::fromSkeletonToVertexAndEdge (skeleton);
     * Mat3UI32 verteces = pop::Processing::clusterToLabel(vertex_edge.first,0);
     * Mat3UI32 edges = pop::Processing::clusterToLabel(vertex_edge.second,0);
     * pop::Visualization::surface(scene,pop::Visualization::labelToRandomRGB(edges));
     * pop::Visualization::lineCube(scene,edges);
     * scene.display();
     * \endcode
     * \image html spinodal_edge.png "Labelisation of the edges of the topological skeleton"
     * \code
     * int tore;
     * GraphAdjencyList<VertexPosition,Edge> g = Analysis::linkEdgeVertex(verteces,edges,tore);
     * Scene3d scene;
     * pop::Visualization::graph(scene,g);
     * pop::Visualization::lineCube(scene,edges);
     * scene.display();
     * \endcode
     * \image html spinodal_graph.png "Topological graph"
     * \code
     * std::cout<<euler/g.sizeVertex()<<std::endl;//N_3/alpha_0 normalised topogical characteristic (connectivity number in my phd)

     * //############ PHYSICAL ###################
     * Mat2F64 mdiffusion = PDE::randomWalk(porespace);
     * mdiffusion.saveAscii("spinodal_self_diffusion.m");
     * \endcode
     * \image html spinodal_self_diffusion.png "Coefficient of self diffusion"
     * \code
     * MatN<3,Vec3F64> velocityfield;
     * Mat2F64 K(3,3);
     * VecF64 kx = PDE::permeability(porespace,velocityfield,0,0.05);//permeability in x-direction
     * VecF64 ky = PDE::permeability(porespace,velocityfield,1,0.05);//permeability in y-direction
     * VecF64 kz = PDE::permeability(porespace,velocityfield,2,0.05);//permeability in z-direction
     * //merge the results in the permeability matrix
     * K.setCol(0,kx);
     * K.setCol(1,ky);
     * K.setCol(2,kz);

     * //display the norm of the last valocity field
     * Mat3F64 velocityfield_norm(velocityfield);
     * ForEachDomain(x,velocityfield)
     * {
     *     velocityfield_norm(x)=normValue(velocityfield(x));
     * }
     * Mat3RGBUI8 velocityfield_norm_grad= pop::Visualization::labelToRGBGradation(velocityfield_norm);
     * Scene3d scene;
     * Visualization::plane(scene,velocityfield_norm_grad,velocityfield_norm_grad.getDomain()(0)/2,0);
     * Visualization::plane(scene,velocityfield_norm_grad,velocityfield_norm_grad.getDomain()(1)/2,1);
     * Visualization::plane(scene,velocityfield_norm_grad,velocityfield_norm_grad.getDomain()(2)/2,2);
     * Visualization::lineCube(scene,velocityfield_norm_grad);
     * scene.display();
     * \endcode
     * \image html spinodal_permeability.png "Amplitude of the velocity field"
     * For an extented analysis, you can decompose the structure in term of elmentary parts and to analyse statiscally this elements as follows:
     * \code
     * double porosity=0.2;
     * Distribution dnormal(10,0.1,"NORMAL");//Poisson generator
     * double moment_order_2 = pop::Statistics::moment(dnormal,2,0,1024);
     * double surface_expectation = moment_order_2*3.14159265;
     * Vec2F64 domain(2048);//2d field domain
     * double N=-std::log(porosity)/std::log(2.718)/surface_expectation;
     * ModelGermGrain2 grain = RandomGeometry::poissonPointProcess(domain,N);//generate the 2d Poisson VecNd process
     * RandomGeometry::sphere(grain,dnormal);
     * Mat2RGBUI8 lattice = RandomGeometry::continuousToDiscrete(grain);

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
     * water.saveAscii("_label.pgm");
     * //ANALYSE THE PORE SPACE
     * PDE::allenCahn(water,porespace,5000);
     * water=pop::Processing::greylevelRemoveEmptyValue(water);
     * VecF64 varea = Analysis::areaByLabel(water);
     * Distribution dvarea = pop::Statistics::computedStaticticsFromRealRealizations(varea,0.1);
     * Mat2F64 mvarea = pop::Statistics::toMatrix(dvarea,dvarea.getXmin(),dvarea.getXmax(),dvarea.getStep());
     * mvarea.saveAscii("label_area.m");
     * VecF64 vcontact = Analysis::perimeterContactBetweenLabel(water);
     * Distribution dcontact = pop::Statistics::computedStaticticsFromRealRealizations(vcontact,0.1);
     * Mat2F64 mcontact = pop::Statistics::toMatrix(dcontact,dcontact.getXmin(),dcontact.getXmax(),dcontact.getStep());
     * mcontact.saveAscii("label_contact.m");
     * VecF64 vferet = Analysis::feretDiameterByLabel(water);
     * Distribution dferet = pop::Statistics::computedStaticticsFromRealRealizations(vferet,0.1);
     * Mat2F64 mferet = pop::Statistics::toMatrix(dferet,dferet.getXmin(),dferet.getXmax(),dferet.getStep());
     * mferet.saveAscii("label_feret.m");
     * \endcode
     *
    */
    //-------------------------------------
    //
    //! \name Profile along a segment
    //@{
    //-------------------------------------
    /*!
     * \param f input matrix
     * \param x1 first segment point
     * \param x2 second segment point
     * \return  profile along the segment
     *
     * \code
     * Mat2UI8 img;
     * img.load("lena.pgm");
     * DistributionRegularStep(Analysis::profile(img,Vec2I32(800,0),Vec2I32(800,img.getDomain()(1)-1))).display();
     * \endcode
     */
    template<int DIM,typename Type>
    static Mat2F64 profile(const MatN<DIM,Type> f, const VecN<DIM,int>& x1,const VecN<DIM,int>& x2){
        double dist = (x2-x1).norm();
        VecN<DIM,F64> direction = VecN<DIM,F64>(x2-x1)/dist;
        VecN<DIM,F64> x=x1;
        Mat2F64 profile(std::floor(dist),2);
        for(int i =0;i<profile.getDomain()(0);i++){
            profile(i,0)=i;
            if(f.isValid(x)){
                profile(i,1)=f.interpolateLinear(x);
            }
            x+=direction;
        }
        return profile;
    }

    //@}

    //-------------------------------------
    //
    //! \name Representative Elementary Volume
    //@{
    //-------------------------------------

    /*! \fn Mat2F64 REVHistogram(const Function & f, typename Function::E x, int rmax,int norm )
     * \param f input matrix
     * \param x center point
     * \param rmax max radius
     * \param norm distance norm
     * \return  Mat2F64 M
     *
     * Calculate the grey-level histogram inside the ball centered in x by progressively increased the radius from 0 to rmax\n
     * M(i,0)=i and M(i,j) = \f$\frac{|X_{j-1}\cap B(x,i)|}{|B(x,i)|}\f$ where \f$ B(x,i)=\{x': |x'-x|<i \}\f$ the ball centered in x of radius i
     * and \f$X_{j}=\{x:f(x)=j \}\f$ the level set of f
     * \code
    Mat2UI8 iex;
    iex.load("/home/vincent/Desktop/Population/doc/image/iex.pgm");
    Mat2F64 mverpore = Analysis::REVHistogram(iex,VecN<2,F64>(iex.getDomain())*0.5,250);

    VecF64 vindex = mverpore.getCol(0);//get the first column containing the grey-level range
    VecF64 v100 = mverpore.getCol(100);//get the col containing the histogram for r=100
    VecF64 v150 = mverpore.getCol(150);
    VecF64 v200 = mverpore.getCol(200);
    VecF64 v250 = mverpore.getCol(250);

    Mat2F64 mhistoradius100(v100.size(),2);
    mhistoradius100.setCol(0,vindex);
    mhistoradius100.setCol(1,v100);

    Mat2F64 mhistoradius150(v150.size(),2);
    mhistoradius150.setCol(0,vindex);
    mhistoradius150.setCol(1,v150);

    Mat2F64 mhistoradius200(v200.size(),2);
    mhistoradius200.setCol(0,vindex);
    mhistoradius200.setCol(1,v200);

    Mat2F64 mhistoradius250(v250.size(),2);
    mhistoradius250.setCol(0,vindex);
    mhistoradius250.setCol(1,v250);

    Distribution d100(mhistoradius100);
    Distribution d150(mhistoradius150);
    Distribution d200(mhistoradius200);
    Distribution d250(mhistoradius250);
    std::vector<Distribution> v;
    v.push_back(d100);v.push_back(d150);v.push_back(d200);v.push_back(d250);
    Distribution::multiDisplay(v);
     * \endcode
     * \image html REVhistogram.png "Histogram in balls of different radius"
    */

    template<typename Function>
    static  Mat2F64 REVHistogram(const Function & f, typename Function::E x, int rmax=10000,int norm=2 )
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("REVHistogram");
        typename Function::IteratorEDomain ittotal(f.getIteratorEDomain());
        int max_value = Analysis::maxValue(f);
        int maxsize = NumericLimits<int>::maximumRange();
        for(int i =0;i<Function::DIM;i++){
            maxsize = minimum(maxsize,f.getDomain()(i));
        }
        if(rmax>maxsize/2)
            rmax=maxsize/2;

        Mat2F64 m(max_value+1,1);
        for(unsigned int i =0;i<m.sizeI();i++)
            m(i,0)=i;

        for( int r =0;r<rmax;r++){
            m.resizeInformation(m.sizeI(),m.sizeJ()+1);
            CollectorExecutionInformationSingleton::getInstance()->progression(1.0*r/rmax);


            typename Function::E R( r*2+1);
            typename Function::IteratorEDomain it(R);
            bool inside =true;
            typename Function::E add =x- r;
            while(it.next()&&inside==true){
                typename Function::E xprime = it.x()-r;
                if(xprime.norm(norm)<=r){
                    typename Function::E xprime = it.x()+add;
                    if(f.isValid(xprime)){
                        m(f(xprime),r+1) ++;
                    }else{
                        inside = false;
                    }
                }
            }
            if(inside==true){
                int count =0;
                for(unsigned int i =0;i<m.sizeI();i++){
                    count +=m(i,r+1);
                }
                for(unsigned int i =0;i<m.sizeI();i++){
                    m(i,r+1)/=count;
                }
            }else{
                r=rmax;
                m.resizeInformation(m.sizeI(),m.sizeJ()-1);
            }
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("REVHistogram");
        return m;

    }
    /*! \fn Mat2F64 REVPorosity(const Function & bin, typename Function::E x, int rmax,int norm )
     * \param bin input binary matrix
     * \param x center point
     * \param rmax max radius
      * \param norm distance norm
     * \return  Mat2F64 M
     *
     *
     *  M(i,0)=i, M(i,1)=\f$\frac{|X^c\cap B(x,i)|}{|B(x,i)|}\f$ such that  \f$X^c=\{x:bin(x)=0\}\f$ is the pore space and \f$B(x,i)\f$
     * is the ball centered in x of radius i.
    */

    template<typename Function>
    static  Mat2F64 REVPorosity(const Function & bin, typename Function::E x, int rmax=10000,int norm=2)throw(pexception)
    {

        CollectorExecutionInformationSingleton::getInstance()->startExecution("REVPorosity");
        Mat2F64 m;
        int maxsize = NumericLimits<int>::maximumRange();
        for(int i =0;i<Function::DIM;i++){
            maxsize = minimum(maxsize,bin.getDomain()(i));
        }
        if(rmax>maxsize/2)
            rmax=maxsize/2;


        for(int r =1;r<rmax;r++){
            F64 rr = r*r;
            CollectorExecutionInformationSingleton::getInstance()->progression(1.0*r/rmax);
            m.resizeInformation(m.sizeI()+1,2);
            m(r-1,0) = r;
            typename Function::E R( r*2+1);
            typename Function::IteratorEDomain it(R);
            bool inside =true;
            int countpore=0;
            int count=0;

            typename Function::E add =x- r;
            while(it.next()&&inside==true){
                typename Function::E xprime = it.x()-r;
                F64 rrcurent = xprime.norm(norm);
                if(rrcurent<=rr){
                    typename Function::E xprime = it.x()+add;
                    if(bin.isValid(xprime)){
                        count++;
                        if(bin(xprime)==0) countpore++;
                    }else{
                        inside = false;
                    }
                }
            }
            if(inside==true){

                m(r-1,1) = 1.0*countpore/count;
            }else
                r=rmax;
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("REVPorosity");
        return m;
    }

    //@}

    //-------------------------------------
    //
    //! \name Metric
    //@{
    //-------------------------------------


    /*! \fn typename Function::F  maxValue(const Function& f)
     *  \brief return the maximum value
     * \param f input function
     * \return max value with the pixel/voxel type
     *
     * Return the maximum value of the matrix, \f$\max_{\forall x\in E}f(x) \f$.\n
     * For instance, this code
     * \code
    Mat2RGBUI8 lena;
    lena.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
    std::cout<<Analysis::maxValue(lena)<<std::endl;
    return 1;
     \endcode
     produces this output 255\<C\>246\<C\>205\<C\>.
    */
    template<typename Function>
    static typename Function::F  maxValue(const Function& f){

        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        return AnalysisAdvanced::maxValue(f,it);
    }

    /*! \fn typename Function::F  minValue(const Function& f)
     *  \brief return the minimum value
     * \param f input function
     * \return min value with the pixel/voxel type
     *
     * Return the minimum value of the matrix, \f$\min_{\forall x\in E}f(x) \f$.\n
     * For instance, this code
     * \code
    Mat2RGBUI8 lena;
    lena.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
    std::cout<<Analysis::minValue(lena)<<std::endl;
    return 1;
     \endcode
     produces this output 53<C\>0\<C\>46\<C\>.
    */
    template<typename Function>
    static typename Function::F  minValue(const Function& f){

        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        return AnalysisAdvanced::minValue(f,it);
    }
    /*! \fn typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64>::Result meanValue(const Function1 & f, Iterator & it)throw(pexception)
     * \brief return the mean value
     * \param f input function
     * \return mean value with a float type
     *
     * Return the mean value of the matrix:  \f$\frac{ \int_{x\in E} f(x)dx}{\int_{x\in E} 1 dx} \f$
     * For instance, this code
     * \code
    Mat2RGBUI8 lena;
    lena.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
    RGBF64 mean = Analysis::meanValue(lena);
    std::cout<<mean<<std::endl;
    \endcode
     produces this output 180.217<C\>99.0461\<C\>46\<C\>.
    */
    template<typename Function>
    static typename FunctionTypeTraitsSubstituteF<typename Function::F,F64>::Result meanValue(const Function & f)throw(pexception)
    {
        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        return AnalysisAdvanced::meanValue( f,it);
    }
    /*! \fn typename FunctionTypeTraitsSubstituteF<typename Function1::F,F64>::Result standardDeviationValue(const Function1 & f, Iterator & it)throw(pexception)
     * \brief return the standard deviation value
     * \param f input function
     * \return mean value with a float type
     *
     * Return the standard deviation value of the matrix:  \f$\sigma=\sqrt{\frac{ \int_{x\in E} (f(x)-\mu)*(f(x)-\mu)dx}{\int_{x\in E} 1 dx}}= \sqrt{\operatorname E[(f(x) - \mu)^2]} \f$ with \f$\mu\f$ the mean value
     * For instance, this code
     * \code
    Mat2RGBUI8 lena;
    lena.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
    RGBF64 mean = Analysis::meanValue(lena);
    std::cout<<mean<<std::endl;
    \endcode
     produces this output 180.217<C\>99.0461\<C\>46\<C\>.
    */
    template<typename Function>
    static typename FunctionTypeTraitsSubstituteF<typename Function::F,F64>::Result standardDeviationValue(const Function & f)throw(pexception)
    {
        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        return AnalysisAdvanced::standardDeviationValue( f,it);
    }

    /*! \fn Mat2F64 histogram(const Function & f)
     * \param f input grey-level matrix
     * \return  Mat2F64 M
     *
     *
     *  M(i,0)=i, M(i,1)=P(f(x)=i)
     * \code
     * Mat2UI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
     * Analysis analysis;
     * Mat2F64 m = analysis.histogram(img);
     * Distribution d(m);
     * d.display();
     * \endcode
     * \sa Matrix Distribution
    */
    template<typename Function>
    static Mat2F64 histogram(const Function & f)
    {
        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        return AnalysisAdvanced::histogram(f,it);
    }

    /*! \fn Mat2F64 area(const Function & f)
     * \param f input matrix
     * \return  Mat2F64 M
     *
     * M(i,0)=i and M(i,1) = \f$|X_{i}|\f$ where
     *  \f$X_{j}=\{x:f(x)=j \}\f$ is the level set of f and \f$|X|\f$  is the cardinal of the set
     * \code
     * Mat2UI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/Lena.bmp");
     * Analysis analysis;
     * Mat2F64 m = analysis.area(img);
     * Distribution d(m);
     * d.display();
     * \endcode

    */
    template<typename Function>
    static Mat2F64 area(const Function & f)
    {
        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        return AnalysisAdvanced::area(f,it);
    }
    /*! \fn Mat2F64 perimeter(const Function & f)
     * \param f input matrixE
     * \return  Mat2F64 M
     *
     * M(i,0)=i and M(i,1) = \f$|\partial X_{i}|\f$ where
     * \f$X_{j}=\{x:f(x)=j \}\f$ the level set of f and \f$ \partial X\f$  the set boundary. We count the number of edges where one adjacent pixel is occupied by the phase ''i''
     *  and at one other pixel adjacent by an other phase :
     * \f[s_i=\frac{\sum_{x\in\Omega}\sum_{y\in N(x)}1_{f(x)=i}1_{f(y)\neq i}   }{2|\Omega|}\f] where N(x) is the 4-connex pixel  of x in 2D,  the 6-connex voxel  of x in 3D.
     */
    template<typename Function>
    static Mat2F64 perimeter(const Function & f)
    {
        typename Function::IteratorEDomain itg(f.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn(f.getIteratorENeighborhood(1,1));
        return AnalysisAdvanced::perimeter(f,itg,itn);
    }

    /*! \fn Mat2F64 fractalBox(const Function & bin)
     * \param bin input binary matrix
     * \return  Mat2F64 M
     *
     *  Allow the evaluation of the fractal dimension that is the slot of the graph defined by the first column of the Mat2F64 for X-axis and the second one for the Y-axis \n
     *  M(i,0)=log(r), M(i,1) =log(N(r)), where N(r) is the number of boxes of a size r needed to cover the binary set.
     * \code
     * Mat2UI8 img = RandomGeometry::diffusionLimitedAggregation2D(1024,30000);
     * img.save("DLA.pgm");
     * Mat2F64 m=  Analysis::fractalBox(img);
     * m.saveAscii("fractal.m");
     * return 0;
     * \endcode
     *
     * \image html DLA.png
     * \image html DLAplot.png
    */

    template<typename Function>
    static  Mat2F64 fractalBox(const Function & bin )
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("FractalBox");
        int mini=NumericLimits<int>::maximumRange();
        for(int i = 0; i<Function::DIM;i++)
            mini=minimum(mini,bin.getDomain()(i));
        Mat2F64 m;
        m.setInformation("#log(r)\tlog(N)");

        int i = mini/10;
        while(i>1)
        {
            m.resizeInformation(m.sizeI()+1,2);
            typename Function::E boxlength=i;
            typename Function::IteratorEDomain itlocal(boxlength);

            typename Function::E sizeglobal;
            for(int k = 0; k<Function::DIM;k++)
                sizeglobal(k) = std::floor(bin.getDomain()(k)*1.0/i);
            typename Function::IteratorEDomain itglobal(sizeglobal);
            int count = 0;
            itglobal.init();
            while(itglobal.next())
            {
                typename Function::E x = itglobal.x()*i;
                bool touch = false;
                itlocal.init();
                while(itlocal.next())
                {
                    typename Function::E z = itlocal.x() + x;
                    if(bin(z)!=0)
                        touch=true;
                }
                if(touch==true)
                    count++;
            }
            F64 vv = std::log(i*1.0)/std::log(2.);
            m(m.sizeI()-1,0)=vv;
            m(m.sizeI()-1,1)=std::log(count*1.0)/std::log(2.);
            //            std::cout<<m<<std::endl;
            if(i==1)
                i=0;
            else
                i/=1.25;

            CollectorExecutionInformationSingleton::getInstance()->info("size="+BasicUtility::Any2String(i));
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("FractalBox");
        return m;
    }


    /*! \fn Mat2F64 correlation(const Function & f, int length=100, int nbrtest=100000 )
     * \param f input labelled matrix
     * \param nbrtest nbrtest for sampling
     * \param length max length for the correlation
     * \return  Mat2F64 M
     *
     *
     *  M(i<length,0)=i, M(i,j) =P(f(x)=j and f(x+r)=j) with r any vector of size i. To estimate this value, we sample nbrtest*length*dim times.
    */

    template<typename Function>
    static  Mat2F64 correlation(const Function & f, int length=100, int nbrtest=100000 )
    {
        if(nbrtest<100)
            nbrtest=100;
        CollectorExecutionInformationSingleton::getInstance()->startExecution("Correlation");

        int maxsize = NumericLimits<int>::maximumRange();
        for(int i =0;i<Function::DIM;i++){
            maxsize = minimum(maxsize,f.getDomain()(i));
        }
        if(length<0)
            length=maxsize/2;
        if(length>maxsize)
            length=maxsize-1;

        typename Function::F value = Analysis::maxValue(f);

        Mat2F64 m(length+1,value+2),mcount(length+1,value+2);
        std::vector<DistributionUniformInt> dist;
        for(int i=0;i<Function::DIM;i++)
            dist.push_back(DistributionUniformInt(0,f.getDomain()(i)-1));

        for(int i=0;i<nbrtest;i++)
        {
            if(i%(nbrtest/100)==0)
                CollectorExecutionInformationSingleton::getInstance()->progression(1.0*i/nbrtest);
            typename Function::E  x ;
            for(int i=0;i<Function::DIM;i++)
                x(i)=dist[i].randomVariable();
            typename Function::F etat1 = f(x);
            for(int i=0;i<Function::DIM;i++)
            {
                typename Function::E y=x;
                y(i)=x(i)-length;
                for(;y(i)<=x(i)+length;y(i)++)
                {
                    int r = absolute(y(i)-x(i));
                    typename Function::E z=y;
                    if(f.isValid(z))
                    {

                        typename Function::F etat2 = f(z);

                        for(typename Function::F k=1;k<=value+1;k++)
                        {
                            mcount(r,k)++;
                            if(k==etat1+1&& etat1 ==  etat2)
                                m(r,k)++;
                        }


                    }

                }


            }
        }
        for(unsigned int j=0;j<m.sizeJ();j++){
            for(unsigned int i=0;i<m.sizeI();i++){
                if(j==0)
                    m(i,j)=i;
                else
                    m(i,j)/=mcount(i,j);
            }
        }
        m.setInformation("M(i<length,0)=i, M(i,j) =P(f(x)=j+1 and f(x+r)=j+1) with r any std::vector of size i.");
        CollectorExecutionInformationSingleton::getInstance()->endExecution("Correlation");
        return m;
    }

    /*! \fn Mat2F64 autoCorrelationFunctionGreyLevel(const Function & f, int length=100, int nbrtest=100000 )
     * \param f input grey-level matrix
     * \param nbrtest nbrtest for sampling
     * \param length max length for the correlation
     * \return  Mat2F64 M
     *
     *
     *  M(i<length,0)=i, M(i,1)=\f$ \frac{\operatorname{E}[(f(x) - \mu)(f(x+i) - \mu)]}{\sigma^2}\f$ with \f$\mu\f$ the mean value and \f$\sigma\f$ the standard deviation see http://en.wikipedia.org/wiki/Autocorrelation. To estimate this value, we sample nbrtest*length*dim times.
    */
    template<typename Function>
    static  Mat2F64 autoCorrelationFunctionGreyLevel(const Function & f, int length=100, int nbrtest=100000 )
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("CorrelationGreyLevel");
        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        int maxsize = NumericLimits<int>::maximumRange();
        for(int i =0;i<Function::DIM;i++){
            maxsize = minimum(maxsize,f.getDomain()(i));
        }
        if(length<0)
            length=maxsize/2;
        if(length>maxsize)
            length=maxsize-1;
        F64 mu = meanValue(f);
        F64 sigma = standardDeviationValue(f);
        Mat2F64 m(length+1,2),mcount(length+1,2);
        std::vector<DistributionUniformInt> dist;
        for(int i=0;i<Function::DIM;i++)
            dist.push_back(DistributionUniformInt(0,f.getDomain()(i)-1));
        for(int i=0;i<nbrtest;i++)
        {
            if(nbrtest/100==0||i%(nbrtest/100)==0)
                CollectorExecutionInformationSingleton::getInstance()->progression(1.0*i/nbrtest);
            typename Function::E  x ;
            for(int i=0;i<Function::DIM;i++)
                x(i)=dist[i].randomVariable();
            for(int i=0;i<Function::DIM;i++)
            {
                typename Function::E y=x;
                y(i)-=length;
                for(;y(i)<=x(i)+length;y(i)++)
                {
                    int r = absolute(y(i)-x(i));
                    typename Function::E z=y;
                    if(f.isValid(z))
                    {
                        m(r,1)+= (f(x)-mu)*(f(z)-mu)/(sigma*sigma) ;
                        mcount(r,1)++;
                    }

                }

            }
        }
        for(unsigned int j=0;j<m.sizeJ();j++){
            for(unsigned int i=0;i<m.sizeI();i++){
                if(j==0)
                    m(i,j)=i;
                else
                    m(i,j)/=(mcount(i,j));
            }
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("CorrelationGreyLevel");
        return m;
    }
    /*! \fn static typename FunctionTypeTraitsSubstituteF<Function,F64>::Result correlationDirectionByFFT(const Function & f)
     * \param f input matrix
     * \return output matrix with a float as pixel/voxel type
     *
     *  calculated the 2-VecNd correlation function in any direction by FFT  P = FFT^(-1)(FFT(f)FFT(f)^*)
    */

    template<typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,F64>::Result correlationDirectionByFFT(const Function & f){

        Function bint;
        bint = pop::Representation::truncateMulitple2(f);
        typename FunctionTypeTraitsSubstituteF<Function,F64>::Result binfloat(bint);
        typename Function::IteratorEDomain it (binfloat.getIteratorEDomain());
        binfloat = pop::ProcessingAdvanced::greylevelRange(binfloat,it,0,1);


        typename FunctionTypeTraitsSubstituteF<Function,ComplexF64 >::Result bin_complex(bint.getDomain());
        Convertor::fromRealImaginary(binfloat,bin_complex);
        typename FunctionTypeTraitsSubstituteF<Function,ComplexF64 >::Result fft = pop::Representation::FFT(bin_complex,1);

        it.init();
        while(it.next()){
            ComplexF64 c = fft(it.x());
            ComplexF64 c1 = fft(it.x());
            fft(it.x()).real() = (c*c1.conjugate()).real();
            fft(it.x()).img() =0;
        }
        fft  = pop::Representation::FFT(fft,0);
        typename FunctionTypeTraitsSubstituteF<Function,F64>::Result fout(bint.getDomain());
        Convertor::toRealImaginary(fft,fout);
        return  fout;

    }
    /*! \fn Mat2F64 chord(const Function & f, int nbrchord )
     * \param f input matrix
     * \param nbrchord  number of sampling
     * \return  Mat2F64 M
     *
     * P(i,0)=i and P(i,1) = Proba(|c|=i) with c a random chord, To estimate it, we random n chords\n
    */
    template<typename Function>
    static Mat2F64 chord(const Function & f, int nbrchord=20000000)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("Chord");
        Mat2F64 m;
        std::vector<DistributionUniformInt> dist;
        for(int i=0;i<Function::DIM;i++)
            dist.push_back(DistributionUniformInt(0,f.getDomain()(i)-1));
        DistributionUniformInt c (0,Function::DIM-1);
        for(int i=0;i<nbrchord;i++)
        {
            if(nbrchord/10000==0||i%(nbrchord/10000)==0)
                CollectorExecutionInformationSingleton::getInstance()->progression(1.0*i/nbrchord);
            int direction = c.randomVariable();
            typename Function::E  x ;
            for(int i=0;i<Function::DIM;i++)
                x(i)=dist[i].randomVariable();
            int phase = f(x);
            bool boundary =false;
            int dist =-1;
            int phasetemp = phase;
            typename Function::E y1 =x;
            while(phase==phasetemp)
            {
                y1(direction)++;
                dist++;
                if(f.isValid(y1)==true)
                {
                    phasetemp = f(y1);
                }
                else
                {
                    boundary= true;
                    phasetemp=phase+1;
                }
            }
            typename Function::E y2 =x;
            phasetemp = phase;
            while(phase==phasetemp)
            {
                y2(direction)--;
                dist++;
                if(f.isValid(y2)==true)
                {
                    phasetemp = f(y2);
                }
                else
                {
                    boundary= true;
                    phasetemp=phase+1;
                }
            }
            if(boundary==false)
            {
                if(dist>=static_cast<int>(m.sizeI()))
                    m.resizeInformation(dist+1,m.sizeJ());
                if(phase>=static_cast<int>(m.sizeJ())-1)
                    m.resizeInformation(m.sizeI(),phase+2);

                m(dist,phase+1)++;
            }
        }
        for(unsigned int j=0;j<m.sizeJ();j++)
        {
            if(j==0)
            {
                for(unsigned int i=0;i<m.sizeI();i++)
                    m(i,j)=i;
            }
            else
            {
                int count=0;
                for(unsigned int i=0;i<m.sizeI();i++)
                {
                    count+=m(i,j);
                }
                for(unsigned int i=0;i<m.sizeI();i++)
                {
                    m(i,j)/=count;
                }
            }
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("Chord");
        return m;
    }

    /*! \fn Mat2F64 ldistance(const Function & bin,int norm,Function & distance)
     * \param bin input binary matrix
     * \param norm norm of the ball
     * \param distance distance map
     * \return Mat2F64 M
     *
     *  ldistance allows the evaluation of the size distribution between any VecNd and the complementary space
     *  This algorithm works in any dimension and in any norms (in particular the euclidean norm for norm=2)\n
     *  M(i,0)=i, M(i,1) =P(d(x,X^c)) where X^c is the complementary set of the input binary set
    */

    template<typename Function>
    static Mat2F64 ldistance(const Function & bin,int norm,Function & distance){

        CollectorExecutionInformationSingleton::getInstance()->startExecution("ldistance");
        Function bin_minus(bin.getDomain());
        typename Function::IteratorEDomain it(bin_minus.getIteratorEDomain());
        while(it.next()){
            if(bin(it.x())!=0)
                bin_minus(it.x())=0;
            else
                bin_minus(it.x())=1;
        }
        if(norm!=2)
            distance = pop::ProcessingAdvanced::voronoiTesselation(bin_minus,  bin.getIteratorENeighborhood(1,norm)).second;
        else{
            distance =pop::ProcessingAdvanced::voronoiTesselationEuclidean(bin_minus).second;
        }
        it.init();
        Mat2F64 m;
        while(it.next()==true){
            if(static_cast<int>(distance(it.x()))>=m.sizeI()){
                m.resizeInformation(distance(it.x())+1,2);
            }
            m(distance(it.x()),1)++;
        }
        int sum=0;
        for(unsigned int i=0;i<m.sizeI();i++){
            sum+=m(i,1);
            m(i,0)=i;
        }
        for(unsigned int i=0;i<m.sizeI();i++){
            m(i,1)/=sum;
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("ldistance");
        m.setInformation("Percolation until the erosion with a ball of radius r  (for the diameter 2*r+1)  in the given coordinate");
        return m;
    }
    /*! \fn Mat2F64 granulometryMatheron(const Function & bin, F64 norm ,Function1& fgranulo)
     * \param bin input binary matrix
     * \param norm norm of the ball
     * \param fgranulo granulometric map
     * \return granulometric Mat2F64 M
     *
     *  Granulometry allows the evaluation of the size distribution of grains in binary matrixs (<a href=http://en.wikipedia.org/wiki/Granulometry_%28morphology%29>wiki</a> ).
     *  This algorithm works in any dimension and in any norms (in particular the euclidean norm for norm=2)\n
     *  M(i,0)=i, M(i,1) =\f$ | (X\circ B(i,norm)|\f$, M(i,2)=M(i,1)-M(i-1,1), where \f$X\f$ is the binary set defined by the input binary matrix,
     *  \f$\circ\f$ the opening operator, \f$B(i,norm)=\{x:|x|_n\leq i\}\f$ the ball centered in 0 of radius i with the given norm and \f$|X|\f$ is the cardinality of the set X
     *
    */

    template<typename Function>
    static Mat2F64 granulometryMatheron(const Function & bin, F64 norm ,Function& fgranulo){

        CollectorExecutionInformationSingleton::getInstance()->startExecution("MatheronGranulometry",INFO);
        FunctorF::FunctorThreshold<typename Function::F,typename Function::F, typename Function::F> func(1,NumericLimits<typename Function::F>::maximumRange());
        Function bin2(bin.getDomain());
        fgranulo.resize(bin.getDomain());
        typename Function::IteratorEDomain it_total(bin2.getIteratorEDomain());
        FunctionProcedureFunctorUnaryF(bin,func,it_total,bin2);
        it_total.init();


        int size = bin2.getDomain().multCoordinate();
        Mat2F64 marea = Analysis::area(bin2);
        int area = size - marea(0,1);
        Mat2F64 m(1,3);
        m(0,0) = 0;
        m(0,1) = area;
        m(0,2) = 0;
        int radius =1;

        Function opening(bin2.getDomain());
        while(area!=0){

            it_total.init();
            opening =pop::ProcessingAdvanced::openingRegionGrowing(bin2,radius,norm);
            it_total.init();
            area = 0;
            while(it_total.next()){
                if(opening(it_total.x())!=0){
                    fgranulo(it_total.x())=radius;
                    area++;
                }
            }
            m.resizeInformation(m.sizeI()+1,3);
            m(m.sizeI()-1,0) = m.sizeI()-1;
            m(m.sizeI()-1,1) = area;
            if(m.sizeI()>1&&m(m.sizeI()-1,1)>m(m.sizeI()-2,1))
                m(m.sizeI()-1,1) =  m(m.sizeI()-2,1);
            m(m.sizeI()-1,2) =  m(m.sizeI()-2,1)-m(m.sizeI()-1,1);
            CollectorExecutionInformationSingleton::getInstance()->info("RADIUS ="+BasicUtility::Any2String(radius)+" and cardinality="+BasicUtility::Any2String(area));
            radius++;

        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("MatheronGranulometry");
        return m;
    }
    /*!
     * \fn static Mat2F64 geometricalTortuosity( const FunctionBinary& bin, int norm=1)
     * \param bin input binary matrix
     * \param norm norm of the ball (1=4-connectivity, 0=8-connectivity in 2D)
     * \return Mat2F64 containing the geometrical tortuosity following each coordinate
     *
     *  Calculated the geometrical tortuosity (geodesical path) following each coordinate
     *
     */

    template<typename FunctionBinary>
    static Mat2F64 geometricalTortuosity( const FunctionBinary& bin, int norm=1)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("GeometricalTortuosity",NOINFO);
        CollectorExecutionInformationSingleton::getInstance()->info("Fast algorithm");

        typename FunctionBinary::IteratorEDomain it(bin.getIteratorEDomain());
        Mat2F64 m(FunctionBinary::DIM,2);
        for(int i=0;i<FunctionBinary::DIM;i++){
            int count=0;
            int sum=0;
            for(int j=0;j<=1;j++){
                FunctionBinary seed(bin.getDomain());
                it.init();
                while(it.next()){
                    if(j==0&&it.x()(i)==0&&bin(it.x())!=0)
                        seed(it.x())=1;
                    if(j==1&&it.x()(i)==bin.getDomain()(i)-1&&bin(it.x())!=0)
                        seed(it.x())=1;
                }
                typename FunctionTypeTraitsSubstituteF<FunctionBinary,UI16>::Result dist = pop::ProcessingAdvanced::voronoiTesselation(seed,bin,bin.getIteratorENeighborhood(1,norm)).second;
                it.init();
                while(it.next()){
                    if(j==0&&it.x()(i)==bin.getDomain()(i)-1&&dist(it.x())!=0){
                        count++;
                        sum+=dist(it.x())+1;

                    }
                    if(j==1&&it.x()(i)==0                    &&dist(it.x())!=0){
                        count++;
                        sum+=dist(it.x())+1;
                    }

                }
            }
            m(i,0)=i;
            m(i,1)=sum*1.0/(count*(bin.getDomain()(i)-1));
        }

        CollectorExecutionInformationSingleton::getInstance()->endExecution("GeometricalTortuosity");
        return m;
    }
    /*! \fn Function1 medialAxis(const Function1 & bin,int norm=1)
     * \param bin input binary matrix
     * \param norm norm of the ball (1=4-connectivity, 0=8-connectivity in 2D)
     * \return median medial-axis
     *
     * \f$ m= \cup_{i=0}^\infty (X\ominus i\lambda)\setminus((X\ominus i\lambda )\circ\lambda    )\f$  where \f$X\f$ is the binary set defined by the input binary matrix,  \f$\lambda\f$  is the structural element
     * , \f$\ominus\f$ the erosion operator, \f$i\lambda\f$ the scale of lambda by i, \f$\circ\f$ the opening operator.

     \code
    Mat2UI8 img;
    img.load("/home/vincent/Desktop/Population/doc/image/outil.bmp");
    img =Analysis::medialAxis(img);
    img.display();
    \endcode
    */

    template<typename Function1>
    static Function1 medialAxis(const Function1 & bin,int norm=1){
        CollectorExecutionInformationSingleton::getInstance()->startExecution("MedialAxis",INFO);


        Function1 bin_minus(bin.getDomain());
        typename Function1::IteratorEDomain it(bin_minus.getIteratorEDomain());
        while(it.next()){
            if(bin(it.x())!=0)
                bin_minus(it.x())=0;
            else
                bin_minus(it.x())=1;
        }
        Function1 distance(bin.getDomain());
        if(norm!=2)
            distance = pop::ProcessingAdvanced::voronoiTesselation(bin_minus,  bin.getIteratorENeighborhood(1,norm)).second;
        else{
            distance =pop::ProcessingAdvanced::voronoiTesselationEuclidean(bin_minus).second;
        }

        Function1 medial(bin.getDomain());
        Function1 zero(bin.getDomain());
        Function1 erosion(bin.getDomain());
        Function1 opening(bin.getDomain());


        typename Function1::IteratorENeighborhood itn (bin_minus.getIteratorENeighborhood(1,norm));
        bool isempty=false;
        int radius=0;
        while(isempty==false)
        {
            CollectorExecutionInformationSingleton::getInstance()->progression(radius,"Size of the structural element");
            it.init();
            FunctorF::FunctorThreshold<typename Function1::F,typename Function1::F,typename Function1::F> func(radius+1,NumericLimits<typename Function1::F>::maximumRange());
            FunctionProcedureFunctorUnaryF(distance,func,it,erosion);

            if(zero==erosion)
                isempty = true;
            it.init();
            opening = pop::ProcessingAdvanced::opening(erosion,it,itn);
            erosion -=opening;
            medial= maximum(medial,erosion);
            radius++;
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("MedialAxis");
        return medial;
    }


    //@}

    //-------------------------------------
    //
    //! \name Topology
    //@{
    //-------------------------------------

    /*! \fn Mat2F64 percolation(const Function & bin,int norm=1)
     * \param bin input binary matrix
     * \param norm norm of the ball
     * \return Mat2F64 M
     *
     *  A(i,0)=i and A(i,1) = r if it exists  path included in the binary set eroded touching the two opposites faces for the i-coordinate, 0 otherwise\n
     * Numerically, we compute the max cluster of the binary test and we test if it touch the opposite direction for each coordinate
     * \sa pop::Processing::clusterMax(const FunctionBinary & bin, int norm)
    */

    template<typename Function>
    static Mat2F64 percolation(const Function & bin,int norm=1){
        Function max_cluster = pop::ProcessingAdvanced::clusterMax( bin,   bin.getIteratorENeighborhood(1,norm));
        Mat2F64 m(Function::DIM,2);
        for(int i = 0;i<Function::DIM;i++)
            m(i,0)=i;
        for(int i =0;i<Function::DIM;i++)
        {
            bool percolleft=false;
            bool percolright=false;
            typename Function::Hyperplane::IteratorEDomain b(bin.getPlaneDomain(i));
            typename Function::E x;
            while(b.next()==true)
            {
                for(int j =0;j<Function::DIM;j++){
                    if(j<i)
                        x(j)=b.x()(j);
                    else if(j>i)
                        x(j)=b.x()(j-1);
                }
                x(i)=0;
                if(max_cluster(x)!=0)
                    percolleft=true;
                x(i)=bin.getDomain()(i)-1;
                if(max_cluster(x)!=0)
                    percolright=true;
            }
            if(percolleft==true && percolright==true)
                m(i,1)=1;
            else
                m(i,1)=0;
        }
        m.setInformation("Percolation (1=yes, 0=no) in the given coordinate");
        return m;
    }

    /*! \fn static Mat2F64 percolationErosion(const  Function   & bin,int norm=1)
     * \param bin input binary matrix
     * \param norm norm of the ball
     * \return Mat2F64 M
     *
     *  A(i,0)=i and A(i,1) = R for the maximum erosion of the binary set with tha ball of radius R such that it exists  path included
     * in the eroded binary set touching the two opposites faces for the i-coordinate
     * Numerically, by increasing the radius of ball, we compute the erosion of the binary set until no percolation
     * \sa percolation(const Function & bin,int norm,Function & max_cluster) pop::Processing::clusterMax(const FunctionBinary & bin, int norm)
    */
    template<typename Function>
    static Mat2F64 percolationErosion(const  Function   & bin,int norm=1){
        CollectorExecutionInformationSingleton::getInstance()->startExecution("percolationErosion",INFO);
        Function bin_minus(bin.getDomain());
        typename Function::IteratorEDomain it(bin.getIteratorEDomain());
        while(it.next()){
            if(bin(it.x())!=0)
                bin_minus(it.x())=0;
            else
                bin_minus(it.x())=255;
        }
        Function erosion(bin.getDomain());
        typename FunctionTypeTraitsSubstituteF<Function,UI16>::Result dist(erosion.getDomain());
        if(norm!=2)
            dist = pop::ProcessingAdvanced::voronoiTesselation(bin_minus, bin_minus.getIteratorENeighborhood(1,norm)).second;
        else
            dist = pop::ProcessingAdvanced::voronoiTesselationEuclidean(bin_minus).second;
        Mat2F64 m(Function::DIM,2);
        for(int i = 0;i<Function::DIM;i++){
            m(i,0)=i;
            m(i,1)=-1;
        }
        bool ispercol=true;
        int radius=0;
        while(ispercol==true){
            it.init();
            erosion = pop::ProcessingAdvanced::threshold(dist,radius+1,NumericLimits<UI16>::maximumRange(),it);
            Mat2F64 mradius =  Analysis::percolation(erosion, norm);
            ispercol=false;
            for(int i = 0;i<Function::DIM;i++){
                if(mradius(i,1)==1){
                    m(i,1)=radius+1;
                    ispercol=true;
                }
            }
            radius++;
            CollectorExecutionInformationSingleton::getInstance()->progression(radius,"Radius size to test percolation");

        }
        m.setInformation("Percolation until the erosion with a ball of radius r (diameter 2*r) in the given coordinate, -1 means no percolation for the original binary matrix");
        CollectorExecutionInformationSingleton::getInstance()->endExecution("percolationErosion");
        return m;
    }

    /*! \fn Mat2F64 percolationOpening(const Function & bin,int norm=1)
     * \param bin input binary matrix
     * \param norm norm of the ball
     * \return Mat2F64 M
     *
     *  A(i,0)=i and A(i,1) = R for the maximum opening of the binary set with tha ball of radius R such that it exists  path included
     * in the opened binary set touching the two opposites faces for the i-coordinate
     * Numerically, by increasing the radius of ball, we compute the opening of the binary set until no percolation
     * \sa clusterMax
    */
    template<typename Function>
    static Mat2F64 percolationOpening(const Function & bin,int norm=1){
        CollectorExecutionInformationSingleton::getInstance()->startExecution("percolationOpening",INFO);
        Function bin_minus(bin.getDomain());
        typename Function::IteratorEDomain it(bin.getIteratorEDomain());
        while(it.next()){
            if(bin(it.x())!=0)
                bin_minus(it.x())=0;
            else
                bin_minus(it.x())=255;
        }
        Function opening(bin.getDomain());

        typename FunctionTypeTraitsSubstituteF<Function,UI16>::Result dist(opening.getDomain());
        if(norm!=2)
            dist = pop::ProcessingAdvanced::voronoiTesselation(bin_minus, bin_minus.getIteratorENeighborhood(1,norm)).second;
        else
            dist = pop::ProcessingAdvanced::voronoiTesselationEuclidean(bin_minus).second;

        Mat2F64 m(Function::DIM,2);
        for(int i = 0;i<Function::DIM;i++){
            m(i,0)=i;
            m(i,1)=-1;
        }
        bool ispercol=true;
        int radius=0;
        while(ispercol==true){
            it.init();
            opening = pop::ProcessingAdvanced::threshold(dist,radius+1,NumericLimits<UI16>::maximumRange(),it);
            opening = pop::ProcessingAdvanced::dilationRegionGrowing(opening,radius,norm);
            Mat2F64 mradius =  Analysis::percolation(opening, norm);
            ispercol=false;
            for(int i = 0;i<Function::DIM;i++){
                if(mradius(i,1)==1){
                    m(i,1)=radius+1;
                    ispercol=true;
                }
            }
            radius++;
            CollectorExecutionInformationSingleton::getInstance()->progression(radius,"Radius size to test percolation");
        }
        m.setInformation("Percolation until the opening with a ball of radius r (diameter 2*r) in the given coordinate, -1 means no percolation for the original binary matrix");
        CollectorExecutionInformationSingleton::getInstance()->endExecution("percolationErosion");
        return m;
    }
    /*! \fn static F64 eulerPoincare(const Function & bin,const char * file_eulertab )
     * \param bin input binary matrix
     * \param file_eulertab path of the lock-up table file named  eulertab.dat "Your_Directory/Population/file/eulertab.dat"
     * \return euler-poincar number
     *
     *  compute the euler-poincare number thanks to a lock-up table named eulertab.dat that you can find in "Your_Directory/Population/file/eulertab.dat"
     * \code
     * Mat2UI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/outil.bmp");
     * img = pop::Processing::threshold(img,120);
     * double euler = Analysis::eulerPoincare(img,"/home/vincent/Desktop/Population/file/eulertab.dat");
     * std::cout<<euler<<std::endl;
     * \endcode
    */
    template<typename Function>
    static F64 eulerPoincare(const Function & bin,const char * file_eulertab )
    {
        if(Function::DIM==2)
        {
            typedef typename FunctionTypeTraitsSubstituteDIM< Function,Function::DIM+1>::Result T3D;
            typename T3D::E size;
            size(0)=bin.getDomain()(0);
            size(1)=bin.getDomain()(1);
            size(2)=1;
            T3D Img3d(size);
            Img3d.setPlane(2,0,bin);
            return AnalysisAdvanced::eulerPoincare3D(Img3d,file_eulertab);
        }
        else
            return AnalysisAdvanced::eulerPoincare3D(bin,file_eulertab);
    }
    /*!
     * \param bin input binary matrix
     * \param file_topo24  path of the lock-up table file named  topo24.dat "Your_Directory/Population/file/topo24.dat"
     * \return topological skeleton
     *
     *  compute the thining at constant topology thanks to a lock-up table named topo24.dat
     * \code
     * Mat2UI8 img;
     * img.load("/home/vincent/Desktop/Population/doc/image/outil.bmp");
     * img = pop::Processing::threshold(img,120);
     * Mat2UI8 skeleton= Analysis::thinningAtConstantTopology3d(img,"/home/vincent/Desktop/Population/file/topo24.dat");
     * skeleton.display();
     * \endcode
    */
    template< typename Function>
    static Function thinningAtConstantTopology3d( const Function & bin,const char * file_topo24)
    {
        return AnalysisAdvanced::thinningAtConstantTopology(bin,file_topo24);
    }
    template< typename Function>
    static Function thinningAtConstantTopology2d( const Function & bin)
    {
        return AnalysisAdvanced::thinningAtConstantTopologyGrowingRegion(bin,"");
    }
    template< typename Function>
    static Function thinningAtConstantTopology2dWire( const Function & bin,double ratio_filter=0.5,int length_edge=4)
    {
        Function img(bin);
        img = img.opposite();
        Function dist = pop::ProcessingAdvanced::voronoiTesselation(img,img.getIteratorENeighborhood(1,0)).second;;
        img = img.opposite();
        Function granulo;
        Mat2F64 m = Analysis::granulometryMatheron(img,0,granulo);
        m.deleteCol(1);
        DistributionRegularStep d(m);
        double mean = Statistics::moment(d,1,d.getXmin(),d.getXmax(),1);
        typename Function::IteratorEDomain it(dist.getIteratorEDomain());
        Function fixed_VecNd(img.getDomain());
        while(it.next()){
            if( img(it.x())>0 && dist(it.x())>mean*ratio_filter && dist(it.x())>=granulo(it.x()) )
                fixed_VecNd(it.x())=255;
            else
                fixed_VecNd(it.x())=0;
        }
        Mat2UI8 skeleton = AnalysisAdvanced::thinningAtConstantTopologyWire(img,fixed_VecNd);

        std::pair<Function,Function> p_vertex_edge = Analysis::fromSkeletonToVertexAndEdge(skeleton);
        typename FunctionTypeTraitsSubstituteF<Function,UI32>::Result label_edge = ProcessingAdvanced::clusterToLabel(p_vertex_edge.second,img.getIteratorENeighborhood(1,0),img.getIteratorEDomain());


        Mat2UI32 label_vertex = ProcessingAdvanced::clusterToLabel(p_vertex_edge.first,img.getIteratorENeighborhood(1,0),img.getIteratorEDomain());
        typename Mat2UI32::IteratorEDomain itg(label_edge.getIteratorEDomain());
        typename Mat2UI32::IteratorENeighborhood itn(label_edge.getIteratorENeighborhood(1,0));
        VecI32 v_length;
        std::vector<int> v_neight_edge;
        while(itg.next()){
            if(label_edge(itg.x())!=0){
                if((int)label_edge(itg.x())>=(int)v_length.size()){
                    v_length.resize(label_edge(itg.x())+1);
                    v_neight_edge.resize(label_edge(itg.x())+1,-1);
                }
                v_length(label_edge(itg.x())) ++;

                itn.init(itg.x());
                while(itn.next()){
                    if(label_vertex(itn.x())!=0&& (v_neight_edge[label_edge(itg.x())]!=static_cast<int>(label_vertex(itn.x())))){
                        if(v_neight_edge[label_edge(itg.x())]==-1)
                            v_neight_edge[label_edge(itg.x())] = label_vertex(itn.x());
                        else
                            v_neight_edge[label_edge(itg.x())] =0;
                    }

                }

            }
        }
        pop::Private::Topology<Mat2UI8> topo("");
        itg.init();
        while(itg.next()){
            if(label_edge(itg.x())!=0&&v_neight_edge[label_edge(itg.x())]>0&&v_length(label_edge(itg.x()))<length_edge){
                skeleton(itg.x())=0;
            }

        }
        std::vector<pop::Vec2I32> v_VecNd;
        itg.init();
        while(itg.next()){
            if(p_vertex_edge.first(itg.x())!=0)
                if(topo.isIrrecductibleVecN(skeleton,itg.x(),Loki::Int2Type<2>())==false)
                    v_VecNd.push_back(itg.x());
        }
        for(unsigned int i=0;i<v_VecNd.size();i++){
            if(topo.isIrrecductibleVecN(skeleton,v_VecNd[i],Loki::Int2Type<2>())==false)
                skeleton(v_VecNd[i])=0;
        }
        return skeleton;//AnalysisAdvanced::thinningAtConstantTopologyWire(bin,f_allow_branch);
    }
    /*! \fn std::pair<Function,Function> fromSkeletonToVertexAndEdge(const Function & skeleton)
     * \param skeleton skeleton
     * \return the first element contain the matrix with the verteces and the second one with the edges
     * extract the vecteces and the edges from the topological skeleton
     *
     *  extract the vecteces and the edges from the topological skeleton
     \sa thinningAtConstantTopology3d( const Function & bin,const char * file_topo24)
    */
    template<typename Function>
    static std::pair<Function,Function> fromSkeletonToVertexAndEdge(const Function & skeleton)
    {
        Function edge(skeleton.getDomain());
        Function vertex(skeleton.getDomain());

        typename Function::IteratorEDomain b(skeleton.getDomain());
        typename Function:: IteratorENeighborhood V(skeleton.getIteratorENeighborhood(1,0));

        b.init();
        while(b.next()==true )
        {
            (edge)(b.x())=0;
            (vertex)(b.x())=0;
            if(skeleton(b.x())!=0)
            {
                V.init(b.x());
                int neighbor=-1;

                while(V.next()==true ){
                    if(skeleton(V.x())!=0)neighbor++;
                }
                if(neighbor<=2) edge(b.x())=NumericLimits<typename Function::F>::maximumRange();
                else vertex(b.x())=NumericLimits<typename Function::F>::maximumRange();
            }
        }
        return std::make_pair(vertex,edge);
    }

    /*! GraphAdjencyList<VertexPosition,Edge> linkEdgeVertex(  const Function & vertex,const Function & edge,int & tore)
     * \param vertex labelled verteces
     * \param edge labelled edge
     * \param tore number of tores
     * \return the topological graph
     *
     *  After the extraction of  the vecteces and the edges from the topological skeleton with the VertexAndEdgeFromSkeleton algorithm
     * you affect a label at each vectex/edge cluster with Cluster2Label, then you apply this procedure to get the topological graph
     * and because a tore edge is not connected to any verteces, you return the number of tore edges
     *
     \sa VertexAndEdgeFromSkeleton Cluster2Label GraphAdjencyList
    */
    template< typename Function>
    static GraphAdjencyList<VertexPosition,Edge> linkEdgeVertex(  const Function & vertex,const Function & edge,int & tore)
    {

        typename Function::IteratorEDomain b(vertex.getIteratorEDomain());
        typename Function::IteratorENeighborhood V(edge.getIteratorENeighborhood(1,0));
        GraphAdjencyList<VertexPosition,Edge> g;

        std::vector<std::pair<int,int> > v_link;
        b.init();
        while(b.next()){
            if((edge)(b.x())>0){
                while((int)(edge)(b.x())>(int)v_link.size()){
                    v_link.push_back(std::make_pair(-1,-1));
                }
                //edge_length[(*edge)[b.x()]]++;
                V.init(b.x());
                while(V.next()==true)
                {
                    if(vertex(V.x())>0)
                    {

                        while((int)vertex(V.x())>(int)g.sizeVertex())
                            g.addVertex();
                        int edgeindex = ((edge)(b.x()) -1);
                        int vertexindex = (vertex)(V.x()) -1;
                        VecF64 vv;
                        vv= V.x();
                        //                    std::cout<<vv<<std::endl;
                        g.vertex(vertexindex).setPos(vv);
                        int link = v_link[edgeindex].first;
                        //                    std::cout<<"hit vertex "<<(vertex)[V.x()]<<"by edge "<< (edge)[b.x()] <<std::endl;
                        //                    getchar();
                        if( link==-1 )
                            v_link[edgeindex].first= vertexindex;
                        else {
                            v_link[edgeindex].second= vertexindex;
                        }
                        //                    std::cout<<"link "<<v_link[edgeindex].first <<" and "<<v_link[edgeindex].second  <<std::endl;
                    }
                }
            }
        }
        b.init();
        while(b.next()){
            if((vertex)(b.x())>0)
            {
                int vertexindex = (vertex)(b.x()) -1;
                VecF64 vv;
                vv= b.x();
                while((int)(vertex)(b.x())>(int)g.sizeVertex())
                    g.addVertex();
                g.vertex(vertexindex).setPos(vv);
            }

        }
        tore=0;
        for(int i =0;i<(int)v_link.size();i++)
        {
            //      std::cout<<"i "<<v_link[i].first<<" et "<< v_link[i].second<<std::endl;
            if(v_link[i].first!=-1&& v_link[i].second!=-1){
                int edge = g.addEdge();
                g.connection(edge,v_link[i].first,  v_link[i].second);
            }
            else if(v_link[i].first==-1&& v_link[i].second==-1){
                tore++;
            }
            else
                std::cerr<<"Error LinkEdgeVertex "<<std::endl;

        }
        return g;

    }


    template<typename Vertex, typename Edge>
    static Distribution coordinationNumberStatistics(GraphAdjencyList<Vertex,Edge>&g ){
        VecI32 v;
        for(int i=0;i<g.sizeEdge();i++)
        {
            std::pair<int,int> p  =g.getLink(i);
            int vv = maximum(p.first,p.second);
            if(vv>=(int)v.size()){
                v.resize(vv+1);
            }
            v(p.first)++;
            v(p.second)++;
        }
        return pop::Statistics::computedStaticticsFromIntegerRealizations(v);
    }

    //@}



    //-------------------------------------
    //
    //! \name Labelling statistics
    //@{
    //-------------------------------------

    /*! \fn VecI32 areaByLabel(const Function & label)
     * \param label input label matrix
     * \return  Vec M
     *
     * V(i)=|{x:f(x)=i-1}| with area=M(i,0) where we count the areas of each label
    */
    template<typename Function>
    static VecI32 areaByLabel(const Function & label)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("areaByLabel");
        VecI32 v;
        typename Function::IteratorEDomain itg(label.getIteratorEDomain());
        while(itg.next()){
            if(label(itg.x())!=0){
                if((int)label(itg.x())>(int)v.size()){
                    v.resize(label(itg.x()));
                }
                v(label(itg.x())-1) ++;
            }
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("areaByLabel");
        return v;

    }
    /*! \fn VecI32 perimeterByLabel(const Function & label)
     * \param label input label matrix
     * \return  Mat2F64 M
     *
     * V(i)=Perimter({x:f(x)=i-1}) where we count the perimeters of each label
    */
    template<typename Function>
    static VecI32 perimeterByLabel(const Function & label)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("RepartitionAreaLabel");

        typename Function::IteratorEDomain itg(label.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn(label.getIteratorENeighborhood(1,1));
        VecI32 v;
        while(itg.next()){
            if(label(itg.x())!=0){
                if(label(itg.x())>(typename Function::F)v.size()){
                    v.resize(label(itg.x()));
                }
                itn.init(itg.x());
                while(itn.next()){
                    if(label(itg.x())!=label(itn.x())){
                        v(label(itg.x())-1) ++;
                    }
                }
            }
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("RepartitionAreaLabel");
        return v;

    }
    /*! \fn VecI32 perimeterContactBetweenLabel(const Function & label, IteratorGlobal & itg, IteratorNeighborhood & itn)
     * \param label input label matrix
     * \return  Mat2F64 M
     *
     * we count the perimemter between contact labels
    */
    template<typename Function>
    static VecI32 perimeterContactBetweenLabel(const Function & label)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("CumulativeDistributionPerimeterContactBetweenLabel");
        typename Function::IteratorEDomain itg(label.getIteratorEDomain());
        typename Function::IteratorENeighborhood itn(label.getIteratorENeighborhood(1,1));
        VecI32 v;
        while(itg.next()){
            if(label(itg.x())!=0){
                if((int)label(itg.x())>(int)v.size()){
                    v.resize(label(itg.x()));
                }
                itn.init(itg.x());
                while(itn.next()){
                    if(label(itn.x())!=0 && label(itg.x())!=label(itn.x())){
                        v(label(itg.x())-1) ++;
                    }
                }
            }
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("CumulativeDistributionPerimeterContactBetweenLabel");
        return v;


    }

    /*! \fn Vec feretDiameterByLabel(const Function & label, int norm)
     * \param label input label matrix
     * \param norm norm
     * \return  Mat2F64 M
     *
     *  We count the feret dimater for each label where for norm=1 , D= 1/n*sum_i diameter(i) and otherwise D= (mult_i diameter(i))^{1/n}
    */
    template<typename Function>
    static VecF64 feretDiameterByLabel(const Function & label, int norm=1)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("RepartitionAreaLabel");
        typename Function::IteratorEDomain itg(label.getIteratorEDomain());
        std::vector<typename Function::E> v_xmin;
        std::vector<typename Function::E> v_xmax;
        while(itg.next()){

            if(label(itg.x())!=0){
                if(    (int)label(itg.x())  >= (int) v_xmin.size() ){
                    v_xmax.resize(label(itg.x())+1);
                    v_xmin.resize(label(itg.x())+1,label.getDomain());
                }
                v_xmin[label(itg.x())]=minimum(v_xmin[label(itg.x())],itg.x());
                v_xmax[label(itg.x())]=maximum(v_xmax[label(itg.x())],itg.x());
            }

        }
        VecF64 v;
        for(int index=0; index<(int)v_xmin.size();index++){
            if( v_xmin[index]!=label.getDomain() ){
                if(norm==0){
                    int sum=0;
                    for(int i=0;i<Function::DIM;i++){
                        sum+=v_xmax[index](i)-v_xmin[index](i);
                    }
                    v.push_back(sum*1.0/Function::DIM);
                }else {
                    int mult=1;
                    for(int i=0;i<Function::DIM;i++){
                        mult*=v_xmax[index](i)-v_xmin[index](i);
                    }
                    v.push_back(    std::pow(mult*1.0, 1.0/Function::DIM));
                }
            }
        }
        CollectorExecutionInformationSingleton::getInstance()->endExecution("RepartitionAreaLabel");
        return v;
    }




    /*! \fn std::vector<typename FunctionTypeTraitsSubstituteF<Function,UI8>::Result > labelToMatrices(const Function & label )
     * \param label input label matrix
     * \param v_xmin vector of xmin positions
     * \param v_xmax vector of xmax positions
     * \return std::vector of binary matrix
     *
     *  Extract each label of the input matrix to form a std::vector of binary matrixs
    */

    template<typename Function>
    static pop::Vec<typename FunctionTypeTraitsSubstituteF<Function,UI8>::Result > labelToMatrices(const Function & label,  pop::Vec<typename Function::E> & v_xmin,pop::Vec<typename Function::E>&  v_xmax)
    {
        v_xmin.clear();
        v_xmax.clear();

        typename Function::IteratorEDomain it (label.getIteratorEDomain());
        while(it.next()){
            if(static_cast<unsigned int>(label(it.x()))>v_xmin.size()){
                typename Function::E xmin(+NumericLimits<int>::maximumRange());
                typename Function::E xmax(-NumericLimits<int>::maximumRange());
                v_xmin.resize(label(it.x()),xmin);
                v_xmax.resize(label(it.x()),xmax);
            }
            if(label(it.x())>0){
                v_xmin[label(it.x())-1]= minimum(it.x(),v_xmin[label(it.x())-1]);
                v_xmax[label(it.x())-1]= maximum(it.x(),v_xmax[label(it.x())-1]) ;
            }
        }
        pop::Vec<typename FunctionTypeTraitsSubstituteF<Function,UI8>::Result > v(v_xmin.size());
        for(int i =0;i<(int)v_xmin.size();i++){
            if(v_xmin[i][0]!=NumericLimits<int>::maximumRange()){
                typename Function::E size = (v_xmax[i]-v_xmin[i])+1;
                typedef typename FunctionTypeTraitsSubstituteF<Function,UI8>::Result FunctionOut;
                v[i] = FunctionOut(size);
            }
        }
        it.init();
        while(it.next()){
            if(label(it.x())>0){
                int index = label(it.x())-1;
                typename Function::E pos = it.x()-v_xmin[label(it.x())-1];
                v(index)(pos)=255;
            }
        }
        return v;

    }
    //@}

#ifdef WITHSWIG
    template<typename Function>
    static F64 _meanValue(const Function & f)throw(pexception)
    {
        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        return AnalysisAdvanced::meanValue( f,it);
    }
    template<typename Function>
    static F64 _standardDeviationValue(const Function & f)throw(pexception)
    {
        typename Function::IteratorEDomain it(f.getIteratorEDomain());
        return AnalysisAdvanced::standardDeviationValue( f,it);
    }
    template<int DIM,typename Type>
    static MatN<DIM,F64> _correlationDirectionByFFT(const MatN<DIM,Type> & f){
        return correlationDirectionByFFT(f);
    }
#endif
};
}
#endif // ANALYSIS_H

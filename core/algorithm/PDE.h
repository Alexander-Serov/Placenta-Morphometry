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
#ifndef PDE_H
#define PDE_H

#include"core/data/typeF/TypeF.h"
#include"core/algorithm/Processing.h"
#include"core/data/functor/FunctorPDE.h"
#include"core/data/population/PopulationPDE.h"
#include"core/algorithm/PDEAdvanced.h"
#include"core/algorithm/Statistics.h"

namespace pop
{

/*!
\defgroup PDE PDE
\ingroup Algorithm
\brief Matrix In -> Matrix Out (anisotropic non-linear diffusion equation, permeability, random walk)

In mathematics, a partial differential equation (PDE) is a differential equation that contains unknown multivariable functions and their partial derivatives.
In this module, we use dinite-difference methods a for approximating the solutions to these equations using finite difference equations to approximate derivatives.
The most classical equation is the diffusion : \f$:\frac{\partial\phi(\mathbf{r},t)}{\partial t} = D\nabla^2\phi(\mathbf{r},t), \f$. The approximation
is coded as follows:
\code
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
    img_timet.save
    disp.display(img_timet);//display the current image
}
\endcode
\image html diffusion2.gif "heat equation on Lena image"
For a more flexible implementation,  I define a collection of functors  pop::FunctorPDE for the classical differential operators in finite difference scheme.
*/

class POP_EXPORTS PDE
{

    /*!
        \class pop::PDE
        \ingroup PDE
        \brief Algorithm based on partial differential equations
        \author Tariel Vincent
    */
public:
    //-------------------------------------
    //
    //! \name Filter
    //@{
    //-------------------------------------

    /*! \fn  static Function nonLinearAnisotropicDiffusionDeriche(const Function& f,int Nstep,F64 K,F64  alpha)
      * \param f input matrix
      * \param Nstep number of time-steps
      * \param K kappa
      * \param alpha deriche paremeter for the gradient
      * \return out matrix h
      *
      * Application of the non-linear anistropic diffusion with the deriche' gradient. The field \f$\phi\f$ is gouverned by this equation:
      * \f[\frac{\partial\phi(\mathbf{r},t)}{\partial t} = \nabla \cdot \big[ D(\phi,\mathbf{r},t) \, \vec{\nabla\phi}(\mathbf{r},t) \big] .\f]
      * The input matrix is the field at time t=0  \f$\phi(\mathbf{r},t=0) = f(\mathbf{r})  \f$ and the output matrix is the field at time=t \f$\phi(\mathbf{r},t=\text{time}) = h(\mathbf{r})  \f$.
      * The time/space step is normalized at 1 and the coefficient of diffusion \f$D(\phi,\mathbf{r},t)=\begin{pmatrix}
      *  D_0(\phi,\mathbf{r},t)    & 0      & \ldots & 0      \\
      *    0      & D_2(\phi,\mathbf{r},t)  & \ddots & \vdots \\
      *  \vdots & \ddots & \ddots & 0      \\
      *  0      & \ldots & 0      & D_{n-1}(\phi,\mathbf{r},t)
      * \end{pmatrix}\f$ where \f$D_i(\phi,\mathbf{r},t)=P\left( \frac{\|\nabla_i^{\sigma}\phi(\mathbf{r},t)\|}{\kappa}\right)\f$, \f$
      * P(x) =\frac{1}{1+x^2}\f$ and \f$\nabla_i^{\sigma}\f$ is a the directional scale gradient operator following the direction i and the scale \f$\sigma\f$.\n
      * The typical calibration is:
      * - Nstep is equal 20 ,
      * - \f$\kappa\f$ is little bit smaller than the grey-level dynamic between phases
      * The parameter K should be choose as the mean difference of grey-level between the phase (to estimate this value you can plot the histogram and calculate the grey-level difference between two peaks),
      * - \f$\sigma\f$ is in the gradient parameter as an inverse of a scale parameter
      * (alpha=2=low scale in keeping the feature of small lenght characteristic, alpha=0.5  high scale in removing the feature of small lenght characteristic).
      *
      * For instance, this code produce the following matrix
      * \code
      * CollectorExecutionInformationSingleton::getInstance()->setActivate(true);
      * Mat3UI8 img;
      * img.load("/home/vincent/Desktop/Population/image/rock3d.pgm");
      * img = PDE::nonLinearAnisotropicDiffusionDeriche(img,20,50,1);
      * img.display();
      * Scene3d scene;
      * pop::Visualization::cube(scene,img);
      * pop::Visualization::lineCube(scene,img);
      * scene.display();
      * \endcode
      * \image html nonlinearderiche.png
      * \image html diffusion.gif "Evolution"
      * \sa pop::Processing::gradientMagnitudeDeriche(const Function1 & f, F64 alpha)
    */
    template<typename Function>
    static Function nonLinearAnisotropicDiffusionDeriche(const Function& f,int Nstep=20,F64 K=50,F64  alpha=1)
    {
        typedef typename FunctionTypeTraitsSubstituteF<typename Function::F, F64>::Result F_Float;
        typedef typename FunctionTypeTraitsSubstituteF<Function,F_Float>::Result             Function_Float;

        Function_Float fin(f);
        FunctorPDE::DiffusionMalikPeronaDeriche  D(K,alpha);
        FunctorPDE::Gradient<FunctorPDE::PartialDerivateForward> grad;
        FunctorPDE::Divergence<FunctorPDE::PartialDerivateBackward> div;
        typename Function_Float::IteratorEDomain it = fin.getIteratorEDomain();
        PDEAdvanced::diffusion(fin,Nstep,it,D,grad,div);
        Function fout(fin);
        return fout;
    }
    /*! \fn  static Function nonLinearAnisotropicDiffusionDericheFast(const Function& in,int Nstep,F64 K,F64  alpha)
      * \param in input matrix
      * \param Nstep number of time-steps
      * \param K kappa
      * \param alpha deriche paremeter for the gradient
      * \return filter matrix
      *
      * see pop::Processing::gradientMagnitudeDeriche
    */
    template<typename Function>
    static Function nonLinearAnisotropicDiffusionDericheFast(const Function& in,int Nstep=20,F64 K=50,F64  alpha=1)
    {
        typedef typename FunctionTypeTraitsSubstituteF<typename Function::F, F64>::Result F_Float;
        typedef typename FunctionTypeTraitsSubstituteF<Function,F_Float>::Result             Function_Float;
        typedef typename FunctionTypeTraitsSubstituteF<Function_Float,VecN<Function_Float::DIM, typename Function_Float::F> >::Result Function_Veciel;
        Function_Float fin(in);
        FunctorPDE::DiffusionMalikPeronaDericheOneTime<Function_Veciel>  D(K,alpha);
        FunctorPDE::Gradient<FunctorPDE::PartialDerivateForward> grad;
        FunctorPDE::Divergence<FunctorPDE::PartialDerivateBackward> div;
        typename Function_Float::IteratorEDomain it = fin.getIteratorEDomain();
        PDEAdvanced::diffusion(fin,Nstep,it,D,grad,div);
        Function fout(fin);
        return fout;
    }

    /*! \fn  static Function nonLinearAnisotropicDiffusionGaussian(const Function& in,int Nstep=20,F64 K=50,F64  sigma=1)
      * \param in input matrix
      * \param Nstep number of time-steps
      * \param K kappa
      * \param sigma deriche paremeter for the gradient
      * \return filter matrix
      *
      * see pop::Processing::gradientMagnitudeDeriche except that the paremeter alpha is the gradient parameter as a scale parameter
      * \code
      * CollectorExecutionInformationSingleton::getInstance()->setActivate(true);
      * Mat3UI8 img;
      * img.load("/home/vincent/Desktop/Population/image/rock3d.pgm");
      * img = PDE::nonLinearAnisotropicDiffusionGaussian(img);
      * img.display();
      * Scene3d scene;
      * pop::Visualization::cube(scene,img);
      * pop::Visualization::lineCube(scene,img);
      * scene.display();
      * \endcode
      * \image html nonlineargaussian.png
    */
    template<typename Function>
    static Function nonLinearAnisotropicDiffusionGaussian(const Function& in,int Nstep=20,F64 K=50,F64  sigma=1)
    {
        typedef typename FunctionTypeTraitsSubstituteF<typename Function::F, F64>::Result F_Float;
        typedef typename FunctionTypeTraitsSubstituteF<Function,F_Float>::Result             Function_Float;
        Function_Float fin(in);
        FunctorPDE::DiffusionMalikPeronaGaussian  D(K,sigma,2*sigma);
        FunctorPDE::Gradient<FunctorPDE::PartialDerivateForward> grad;
        FunctorPDE::Divergence<FunctorPDE::PartialDerivateBackward> div;
        typename Function_Float::IteratorEDomain it = fin.getIteratorEDomain();
        PDEAdvanced::diffusion(fin,Nstep,it,D,grad,div);
        Function fout(fin);
        return fout;
    }
    //@}

    //-------------------------------------
    //
    //! \name Physical properties
    //@{
    //-------------------------------------

    /*! \fn    static VecF64  permeability(const  Function& bulk, int direction=0,F64 errorpressuremax=0.01,typename FunctionTypeTraitsSubstituteF<Function,VecN<Function::DIM,F64> >::Result& velocity= typename FunctionTypeTraitsSubstituteF<Function,VecN<Function::DIM,F64> >::Result()   )
      * \param bulk  pore space defined by \f$\{x:bulk(x)\neq 0 \}\f$
      * \param direction direction of the pressure gradient
      * \param errorpressuremax the default value insures  the convergence but that can be long
      * \param velocity velocity field with a type equal to 2d = MatN<2,Vec2F64>  3d = MatN<3,Vec3F64>
      * \return permeability in the different direction
      *
\section Formalism Formalism

From the second Newton's law, one gets the general form of the equations of fluid motion:
\f[
\overbrace{\rho \Big(
\underbrace{\frac{\partial \mathbf{v}}{\partial t}}_{
\begin{smallmatrix}
  \text{Unsteady}\\
  \text{acceleration}
\end{smallmatrix}} +
\underbrace{\mathbf{v} \cdot \nabla \mathbf{v}}_{
\begin{smallmatrix}
  \text{Convective} \\
  \text{acceleration}
\end{smallmatrix}}\Big)}^{\text{Inertia (per volume)}} =
\overbrace{\underbrace{-\nabla p}_{
\begin{smallmatrix}
  \text{Pressure} \\
  \text{gradient}
\end{smallmatrix}} +
\underbrace{\nabla \cdot  T}_{\begin{smallmatrix}
  \text{other} \\
  \text{part}
\end{smallmatrix}}}^{\text{Divergence of stress}} +
\underbrace{\mathbf{f}}_{
\begin{smallmatrix}
  \text{Other} \\
  \text{body} \\
  \text{forces}
\end{smallmatrix}}.
\f]
where \f$\mathbf{v}\f$is the flow velocity, \f$\rho\f$is the fluid density, \f$T\f$is the deviatoric stress tensor, and \f$\mathbf{f} \f$represents body forces (per unit volume) acting on the fluid and \f$\nabla \cdot\f$is the divergence operator.<br>
Due to mass conservation, we have this continuity equation :

\f[ {\partial \rho \over \partial t} + \nabla \cdot (\rho \mathbf{v}) = 0\f]


\subsection Newtonian Newtonian fluid

For an incompressible, homogeneous, Newtonian fluid, the divergence of the deviatoric stress tensor is equal to:
\f[\nabla \cdot T  = \mu \nabla^2\mathbf{v}\f]
where \f$ \mu\f$is the dynamic viscosity and \f$\nabla^2 \f$is the Laplace operator. The incompressible fluid reduces the continuity law (mass conservation) in:
\f$\nabla \cdot \mathbf{v}  = 0 \f$

\subsection Steady_state Steady state

The flow velocity does not depend on time:
\f[\frac{\partial \mathbf{v}}{\partial t}=0 \f]

\subsection Reynold Small Reynold number

By dimensional analysis, one compares the ratio of  inertial force to viscous force:
\f[\frac{\|\rho\mathbf{v} \cdot \nabla \mathbf{v} \|}{\|\mu\nabla^2 \mathbf{v}\|}  \sim \frac{\rho v v/L }{\mu v /L^2}\f]
\f[\frac{\|\rho\mathbf{v} \cdot \nabla \mathbf{v} \|}{\|\mu\nabla^2 \mathbf{v}\|}  \sim \frac{\rho v L }{\mu}\f]
where L is the characteristic size of the system. A small length, L, as the general case in porous media,  leads to a Small Reynold numbers allowing to vanish the convective term.

\subsection NoSlip no slip

In the interface pore-solid, the fluid has zero velocity due to no slip condition :
\f[\mathbf{v}=0 \f]

\subsection Sum Sum-up hypotheses

The continuity and second Newton's law reduce to in the pore space \f$\mathbb{P}\f$, known as Stokes equations
\f[\nabla\cdot \mathbf{v}  = 0, \f]
\f[-\nabla p+ \mu \nabla^2 \mathbf{v}=0.\f]
with this boundary condition on the pore-solid interface \f$\partial\mathbb{P}\f$:
\f[\mathbf{v}=0\f]

\section Numerical Numerical resolution-microscopic scale

\subsection Domain Domain boundary condition

Given a cubic porous sample defined in the domain \f$\mathbb{M}\subset\mathbb{R}^3\f$, divided in a pore space \f$\mathbb{P}\f$and a solid space \f$\mathbb{S}\f$. We apply
 - a macroscopic pressure gradient along one direction
\f$e_i\f$with \f$p_{x_i=0}\f$the pressure applied to the face \f$x_i=0\f$and \f$p_{x_i=L_{x_i}}\f$ the pressure applied to the opposite face.
 - a closure with solid on the other faces \f$F_{i\neq j}\f$,
\f[ \mathbf{v}(\mathbf{r})=0, \mathbf{r}\in F_{i\neq j} \f]

\subsection Artificial Artificial Compressibility Method

\subsubsection Adding Adding a source term in the continuity equation

Numerically, it is tricky to solve  coupled  equations. In order to solve it, one introduces a source term in the incompressible equation:
\f[\nabla\cdot \mathbf{v}=-\beta \frac{\partial p }{\partial t}  \f]
with \f$\beta\f$a constant parameter superior to 0. With this adding term, we observe a relaxation: when ''t'' tends to infinity, the steady state is reached. Thus, we come back to our initial problem

\subsubsection Numerical Numerical process

Numerically, at initial time, we start with a given initial pressure field in the bulk. At each time ''t'', we calculate the flow velocity using this equation:
\f[-\nabla p(\mathbf{r})+ \mu \nabla^2 \mathbf{v}(\mathbf{r})=0 , \mathbf{r}\in\mathbb{P},\f]

and

 \f[\mathbf{v}=0 , \mathbf{r}\in\partial\mathbb{P}\f]
Then, we calculate the pressure at time ''t + dt'' using this equation:
\f[\nabla\cdot \mathbf{v}=-\beta \frac{\partial p }{\partial t}, \mathbf{r}\in\mathbb{P}.\f]

\subsubsection Linear Linear algebra

The solution of this partial differential equation:
\f[-\nabla p(\mathbf{r})+ \mu \nabla^2 \mathbf{v}(\mathbf{r})=0, \mathbf{r}\in\mathbb{P}\f]
\f[\mathbf{v}=0 , \mathbf{r}\in\partial\mathbb{P}\f]
may be found by solving the linear algebra counterpart. I use a variant of Gauss–Seidel method [http://en.wikipedia.org/wiki/Successive_over-relaxation]  to solve the associated linear algebra equation.

\subsubsection Finite Finite difference method in MAC grid

We use the Marker And Cell (MAC) grid in finite difference method to approximate the partial differential equations in the lattice defined by the matrix

\image html MACGrid.png




\section Darcy Law-Macroscopic scale

Phenomenologically see Darcy works and theoretically see Whitaker works,  the derivation of the microscopic Stokes flow in porous media leads to the macroscopic Darcy Law related the macroscopic pressure gradient and the macroscopic velocity:
\f[ \bar{\mathbf{v}}=-\frac{1 }{\mu}\mathbb{K}  \overline{ \nabla p} \f]

where \f$\mathbb{K} \f$is the permeability tensor, and the bar above the parameters represents the macroscopic scale.\n
The macroscopic pressure gradient is defined as:
\f[ \overline{ \nabla p} = \frac{p_{x_i=L_{x_i}}  -  p_{x_i=0}  }{L_{x_i}} \f]
The apparent macroscopic velocity is defined as:
\f[ \bar{\mathbf{v}}=-\frac{1}{| \mathbb{M} |}\int_\mathbb{M} \mathbf{v}(\mathbf{r}) d \mathbf{r} \f]
After solving the boundary value problem, we calculate the apparent macroscopic velocity with the previous formula. Then,
the first row of the permeability tensor (here the problem is solved in the x-direction) is:
\f[\mathbf{k}_x=(k_{xx},k_{xy},k_{xz})= \frac{\mu }{ \overline{ \nabla p} }  \bar{\mathbf{v}}.\f]
In this simulation, the numerical permeability is estimated with \f$\mu\f$and \f$\overline{ \nabla p} \f$ normalized at Lx/r where r is the voxel resolution meanning that \f$\overline{ \nabla p}\f$ is equal to the size of your matrix in the given direction. So the permeability of your real sample is the multiplication of the numerical permeability by the dynamic viscosity of the fluid multiply by the voxel resolution.

As example, this code produces
 \code
#include"core/data/utility/CollectorExecutionInformation.h"
#include"core/algorithm/Processing.h"
#include"core/algorithm/Visualization.h"
#include"core/algorithm/PDE.h"
#include"core/algorithm/RandomGeometry.h"
#include"core/data/mat/MatNDisplay.h"
using namespace pop;
int main(){
    CollectorExecutionInformationSingleton::getInstance()->setActivate(true);
    Mat2UI8 img;
    img.load("/home/vincent/Desktop/Population/doc/image/outil.bmp");
    img = img.opposite();
    Mat2Vec2F64 vel;
    int dir=0;
    VecF64 kx = PDE::permeability(img,vel,dir,0.01);
    vel= GeometricalTransformation::scale(vel,Vec2F64(8));
    Mat2RGBUI8 c = Visualization::vectorField2DToArrows(vel);
    c.display("velocity",true,false);
    dir=1;
    VecF64 ky = PDE::permeability(img,vel,dir,0.01);
//    pop::Visualization::labelToRGBGradation(vel.toVecScalar()[dir]).display();
    Mat2F64 K(2,2);
    K.setCol(0,kx);
    K.setCol(1,ky);
    std::cout<<K<<std::endl;
}
 \endcode
 the final matrix of this animation:
\image html outilvelocity.gif "Velocity field"
\image html outilpressure.gif "Pressure field"

 Of the course the code works in 3D.
*/

    template<typename Function>
    static VecF64  permeability(const  Function& bulk,typename FunctionTypeTraitsSubstituteF<Function,VecN<Function::DIM,F64> >::Result& velocity, int direction=0,F64 errorpressuremax=0.01  )
    {
        VecF64  permeability(Function::DIM);
        F64 relaxationSOR=0.9;
        F64 relaxationpressure=1;
        PDEAdvanced::permeability(bulk,direction,errorpressuremax, relaxationSOR,  relaxationpressure,velocity,permeability);
        return permeability;
    }
#ifdef WITHSWIG
    template<int DIM,typename Type>
    static VecF64  ___permeability(const  MatN<DIM,Type>& bulk,  MatN<DIM,VecN<DIM,F64> >& velocity, int direction=0,F64 errorpressuremax=0.01  )
    {
        return permeability(bulk, velocity,  direction, errorpressuremax );
    }
#endif
    /*!
      * \param bulk bulk field
      * \param nbrwalkers number of walkers
      * \param standard_deviation standard deviation of elementary jumps
      * \param time_max time max for the simulation
      * \param delta_time_write write result at each delta_time_write time
      * \return matrix
      *
      * This algorithm simulate a random walker as the Brownian motion in the bulk of a particle described by a stochastic differential equation \f$dX_t = + \sigma  dW_t\f$, where
      * \f$X_t\f$ is the particle position, \f$\sigma\f$ is the standard deviation and \f$W_t\f$ is the  normal Wiener process. We impose reflecting boundary condition
      * at the bulk boundary. Numerically, at each elementary jump, we increment the time by \f$\sigma^2/(2 D_0) \f$ with \f$D_0 \f$ the free diffusion. At each delta_time_write,
      * we calculate the distance \f$(X(t)-X(t=0))^2\f$.\n
      * We iterate this process in order to average over the number of walkers.
      * \code
      * Mat3UI8 img;
      * img.load("/home/vincent/Desktop/Population/doc/image/spinodal.pgm");
      * Mat2F64 m = PDE::randomWalk(img);
      * m.saveAscii("random_walk.m");
      * Distribution d(m);
      * d.display();
      * \endcode
     * \image html spinodal_self_diffusion.png "Coefficient of self diffusion"
     */
    template<typename Function>
    static Mat2F64   randomWalk(const Function &  bulk, int nbrwalkers=50000, F64 standard_deviation=0.5 ,  F64 time_max=2000,F64 delta_time_write=10)
    {
        CollectorExecutionInformationSingleton::getInstance()->startExecution("RandomWalk");
        typedef typename FunctionTypeTraitsSubstituteF<  typename Function::E,F64>::Result VecNFloat;
        VecNFloat x_current,x_start;
        DistributionNormal distnorm(0,standard_deviation);

        std::vector<DistributionUniformReal>randimage;
        for(int i =0;i<Function::DIM;i++)
            randimage.push_back(DistributionUniformReal(0,bulk.getDomain()(i)-1));


        Mat2F64  t(static_cast<int>(std::floor(time_max/delta_time_write))+2,2+Function::DIM);

        for(int j =0;j<nbrwalkers;j++){
            if(nbrwalkers/100==0||j%(nbrwalkers/100)==0)
                CollectorExecutionInformationSingleton::getInstance()->progression(1.0*j/nbrwalkers);
            int timestepwrite= 0;
            VecNFloat v_sum  = 0;
            F64 timecurrent=0;
            x_start = start_VecN(bulk,randimage);
            x_current = x_start;
            while(timecurrent <=time_max){
                //increment the time at each jumb by sigma*sigma/(2*D) with D=1
                timecurrent += (standard_deviation*standard_deviation)/(2);
                if(next_VecN(bulk,distnorm,x_current)==false){
                    //if the walker is outside the domain defined by the input image. We restart the walker randomly in the bulk and we add the distance to current one
                    v_sum += ( x_current -x_start);
                    x_start = start_VecN(bulk,randimage);
                    x_current = x_start;
                }
                if(timecurrent>= (timestepwrite+1)*delta_time_write) {
                    timestepwrite++;
                    VecNFloat distance = x_current -x_start +v_sum;
                    t(timestepwrite,0)+=timecurrent;
                    t(timestepwrite,1)+=distance.normPower();
                    for(int dim1=0;dim1<Function::DIM;dim1++)
                        t(timestepwrite,2+ dim1)+=distance(dim1)*distance(dim1);
                }
            }
        }
        t(0,0)=0;
        t(0,1)=1;
        t(0,2)=1;
        for(unsigned int i=1;i<t.sizeI();i++){
            t(i,1)=t(i,0)/t(i,1)*(2*Function::DIM);
            for(unsigned int j=2;j<t.sizeJ();j++){
                t(i,j)=t(i,0)/t(i,j)*(2);
            }
            t(i,0)/=nbrwalkers;
        }
        t = t.deleteRow(t.sizeI()-1);
        if(Function::DIM==2)
            t.setInformation("time\t D_0/D(t)\t D_0/D_x(t)\t D_0/D_y(t)  ");
        else
            t.setInformation("time\t D_0/D(t)\t D_0/D_x(t)\t D_0/D_y(t)\t D_0/D_z(t)  ");
        CollectorExecutionInformationSingleton::getInstance()->endExecution("RandomWalk");
        return t;
    }
    //@}
    //-------------------------------------
    //
    //! \name Phase field (not stable work in progress)
    //@{
    //-------------------------------------

    /*!
      * \param labelfield input/output label matrix
      * \param nbrsteps number of time-steps
      * \return phase field at the end of the simulation
      *
      * The classical allen-cahn equation. I will publish soon an article related to its application in matrix processing.
      * \code
      * Mat2UI8 img(512,512);
      * Mat2UI8::IteratorEDomain it(img.getIteratorEDomain());

      * Distribution d(0,20,"UNIFORMINT");
      * while(it.next()){
      *     img(it.x())=d.randomVariable();
      * }
      * MatNDisplay windows;
      * while(1==1){
      *     PDE::allenCahn(img,200);
      *     Mat2RGBUI8 RGB = pop::Visualization::labelToRandomRGB(img);
      *     windows.display(RGB);
      * }
      * \endcode
      * \image html spinodaldecomposition.gif
      * This algorithm can be used for the regularisation:
      * \code
      * Vec3F64 domain(256);//2d field domain
      * ModelGermGrain3 grain = RandomGeometry::poissonPointProcess(domain,0.01);//generate the 2d Poisson VecN process
      * Distribution dnormal(30,20,"NORMAL");
      * RandomGeometry::sphere(grain,dnormal);
      * Mat3RGBUI8 img_VecN = RandomGeometry::continuousToDiscrete(grain);
      * Mat3UI8 img_VecN_grey;
      * img_VecN_grey = img_VecN;
      * img_VecN_grey = pop::Processing::greylevelRemoveEmptyValue(img_VecN_grey);
      * Mat3F64 phasefield = PDE::allenCahn(img_VecN_grey,40);
      * phasefield = PDE::getField(img_VecN_grey,phasefield,1,6);
      * Scene3d scene;
      * pop::Visualization::marchingCubePhaseField(scene,phasefield);
      * pop::Visualization::lineCube(scene,phasefield);
      * scene.display();
      * \endcode
      * \image html regularization.png
    */

    template<typename Function>
    static typename FunctionTypeTraitsSubstituteF<Function,F64>::Result allenCahn( Function&  labelfield, int nbrsteps)throw(pexception)
    {

        labelfield = pop::Processing::greylevelRemoveEmptyValue(labelfield);
        FunctorPDE::Laplacien<FunctorPDE::PartialDerivateSecondCenteredMultiPhaseField<Function> > laplacien;
        laplacien.partialsecond.setLabelPhase(labelfield);
        typename FunctionTypeTraitsSubstituteF<Function,F64>::Result   phasefield(labelfield.getDomain());
        phasefield=1;
        typename Function::IteratorEDomain it = labelfield.getIteratorEDomain();

        RegionGrowingMultiPhaseField<typename FunctionTypeTraitsSubstituteF<Function,F64>::Result > pop(phasefield,labelfield,0.95);
        PDEAdvanced::allenCahnInMutliPhaseField(phasefield,labelfield,nbrsteps,pop.getIterator(),laplacien);
        return phasefield;

    }
    /*!
      * \param labelfield input/output label matrix
      * \param bulk bulk matrix
      * \param nbrsteps number of time-steps
      * \return phase field at the end of the simulation
      *
      * The classical allen-cahn equation. I will publish soon an article related to its application in matrix processing.
      * This algorithm can be used for the grain partition regularisation:
      * \code
    CollectorExecutionInformationSingleton::getInstance()->setActivate(true);
    Vec2F64 domain(512);//2d field domain
    ModelGermGrain2 grain = RandomGeometry::poissonPointProcess(domain,0.01);//generate the 2d Poisson VecN process
    Distribution dnormal(30,5,"NORMAL");
    RandomGeometry::sphere(grain,dnormal);
    Mat2RGBUI8 lattice = RandomGeometry::continuousToDiscrete(grain);
    Mat2UI8 solid,inverse;
    solid = lattice;
    inverse = solid;
    inverse = inverse.opposite();
    Mat2F64 dist = pop::Processing::distanceEuclidean(inverse);
    Mat2UI16 distl;
    distl= dist;
    distl = distl.opposite();
    distl = pop::Processing::dynamic(distl,2);
    Mat2UI32 minima = pop::Processing::minimaRegional(distl,0);
    Mat2UI32 water = pop::Processing::watershed(minima,distl,solid,1);
    PDE::allenCahn(water,solid,20000);
      * \endcode
      * \image html allencahn.gif
    */
    template<typename Function1,typename Function2>
    static typename FunctionTypeTraitsSubstituteF<Function1,F64>::Result allenCahn( Function1&  labelfield,const Function2 & bulk,int nbrsteps)throw(pexception)
    {

        labelfield = pop::Processing::greylevelRemoveEmptyValue(labelfield);
        FunctorPDE::Laplacien<FunctorPDE::PartialDerivateSecondCenteredInBulkMultiPhaseField<Function2,Function1> > laplacien;
        laplacien.partialsecond.setLabelPhase(labelfield);
        laplacien.partialsecond.setBulk(bulk);
        typename FunctionTypeTraitsSubstituteF<Function1,F64>::Result   phasefield(labelfield.getDomain());
        phasefield=1;

        RegionGrowingMultiPhaseField<typename FunctionTypeTraitsSubstituteF<Function1,F64>::Result > pop(phasefield,labelfield,0.95,bulk);
        //        typename Function1::IteratorEDomain it = labelfield.getIteratorEDomain();
        PDEAdvanced::allenCahnInMutliPhaseField(phasefield,labelfield,nbrsteps,pop.getIterator(),laplacien);
        return phasefield;

    }
    /*! \fn static Function2 getField(const Function1 & labelfield,const Function2 & scalarfield, typename Function1::F label,int width=3)throw(pexception)
      * \param labelfield label field
      * \param scalarfield scalar field
      * \param label selected label
      * \param width width
      * \return scalar field at the end of the simulation
      *
      * The classical allen-cahn equation. I will publish soon an article related to its application in image processing.
    */

    template<typename Function1,typename Function2>
    static Function2 getField(const Function1 & labelfield,const Function2 & scalarfield, typename Function1::F label,int width=3)throw(pexception)
    {

        typename Function1::IteratorEDomain itg(labelfield.getIteratorEDomain()) ;
        typename FunctionTypeTraitsSubstituteF<Function1,UI8 >::Result selectfield(scalarfield.getDomain());
        while(itg.next()){
            if(labelfield(itg.x())==label)
                selectfield(itg.x())=1;
        }
        typename FunctionTypeTraitsSubstituteF<Function1,UI8 >::Result dilateselectfield(scalarfield.getDomain());
        dilateselectfield = pop::Processing::dilationRegionGrowing(selectfield,width,1);
        itg.init();
        Function2 scalarfield_phase(scalarfield.getDomain()) ;
        itg.init();
        while(itg.next()){
            if(dilateselectfield(itg.x())==1){
                if(labelfield(itg.x())==label){
                    scalarfield_phase(itg.x())=scalarfield(itg.x());

                }else{
                    scalarfield_phase(itg.x())=-scalarfield(itg.x());
                }
            }
            else
            {
                scalarfield_phase(itg.x())=-1;
            }
        }
        return scalarfield_phase;
    }

    /*! \fn static Distribution curvaturePhaseField(const Function & binary,typename FunctionTypeTraitsSubstituteF<Function,F64>::Result & curvature,typename FunctionTypeTraitsSubstituteF<Function,F64>::Result& phasefield, F64 cutoffmin=-100 , F64 cutoffmax=100 , int nbriteration=30, F64 b=-0.5,F64 a=0.5)
      * \param binary input binary matrix
      * \param curvature output curvature matrix with type equal to 2d=Mat2F64 or 2d=Mat3F64
      * \param phasefield output phasefield
      * \param cutoffmin minimum curvature
      * \param cutoffmax maximum curvature
      * \param nbriteration number of iterations
      * \param b minimum level set value
      * \param a maximum level set value
      * \return distribution curvature
      *
      *  \f$1/k = \overrightarrow{\nabla}\cdot \frac{\overrightarrow{\nabla} \phi(x)}{|\overrightarrow{\nabla} \phi(x)|} \f$
      * \code
    double porosity=0.85;
    Distribution d1(20,"DIRAC");
    Distribution d2(25,"DIRAC");
    DistributionExpression dexp("4");
    d1 = dexp*d1+d2;
    d1 = pop::Statistics::toProbabilityDistribution(d1,10,40);
    double moment_order_3 = pop::Statistics::moment(d1,3,0,1024);
    double surface_expectation = 4./3.*moment_order_3*3.14159265;
    Vec3F64 domain(300);//2d field domain
    double lambda=-std::log(porosity)/std::log(2.718)/surface_expectation;
    ModelGermGrain3 grains = RandomGeometry::poissonPointProcess(domain,lambda);//generate the 2d Poisson VecN process
    RandomGeometry::sphere(grains,d1);
    Mat3RGBUI8 img_VecN = RandomGeometry::continuousToDiscrete(grains);
    Mat3UI8 img_VecN_grey;
    img_VecN_grey = img_VecN;
    Mat3F64 curvature;
    Mat3F64 phasefield;
    Distribution dcurvature = PDE::curvaturePhaseField(img_VecN_grey,curvature,phasefield,-50,-5);
    dcurvature.display();
    img_VecN_grey = pop::Processing::greylevelRange(curvature,0,255);
    img_VecN = pop::Visualization::labelToRGBGradation(curvature);
    Scene3d scene;
    pop::Visualization::marchingCubePhaseField(scene,phasefield,img_VecN);
    pop::Visualization::lineCube(scene,phasefield);
    scene.display(false);
    int i=0;
    while(1==1){
        scene.lock();
        i++;
         std::string file = "Curvature"+BasicUtility::IntFixedDigit2String(i,4)+".pgm";
        scene.rotateZ(5);
        std::cout<<i<<std::endl;
        if(i==100)
            return 1;
        scene.unlock();
        scene.snapshot(file.c_str());
    }
      * \endcode
    */

    template<typename Function>
    static Distribution curvaturePhaseField(const Function & binary,typename FunctionTypeTraitsSubstituteF<Function,F64>::Result & curvature,typename FunctionTypeTraitsSubstituteF<Function,F64>::Result& phasefield, F64 cutoffmin=-100 , F64 cutoffmax=100 ,int nbriteration=30, F64 b=-0.5,F64 a=0.5){
        typedef typename FunctionTypeTraitsSubstituteF<Function,F64>::Result FunctionFloat;
        phasefield.resize(binary.getDomain());
        typename FunctionFloat::IteratorEDomain it(binary.getIteratorEDomain());
        while(it.next()){
            if(binary(it.x())>0)
                phasefield(it.x())=1;
            else
                phasefield(it.x())=-1;
        }
        PDE::LaplacienSmooth<FunctionFloat > laplacien(phasefield,1,2);
        PDEAdvanced::allenCahnInSinglePhaseField(phasefield,nbriteration,it,laplacien,3);


        typename FunctionTypeTraitsSubstituteF<FunctionFloat,VecN<Function::DIM, typename FunctionFloat::F> >::Result gradnormalized(phasefield.getDomain());
        FunctorPDE::Gradient<FunctorPDE::PartialDerivateCentered > func_grad;

        it.init();
        while(it.next()){
            gradnormalized(it.x()) = func_grad(phasefield,it.x());
            F64 v =gradnormalized(it.x()).norm();
            if(v>0.0001)
                gradnormalized(it.x())= gradnormalized(it.x())*(1.0/v);
            else
                gradnormalized(it.x())= 0;
        }
        FunctorPDE::Divergence<FunctorPDE::PartialDerivateCentered > func_div;
        VecF64 vcurve;
        curvature.resize(phasefield.getDomain());
        curvature=0;

        typename FunctionFloat::IteratorERectangle it2( binary.getIteratorERectangle(2,phasefield.getDomain()-3));
        it2.init();
        while(it2.next()){
            typename FunctionFloat::E x = it2.x();
            if(phasefield(x)>b&&phasefield(x)<a){
                typename FunctionFloat::F divergence = func_div(gradnormalized,x);
                if(FunctionFloat::DIM==3)
                    divergence /=2;
                if(divergence!=0&&1/divergence>cutoffmin&&1/divergence<cutoffmax){
                    vcurve.push_back(1/divergence);
                    curvature(x)=1/divergence;
                }else{
                    curvature(x)=0;
                }

            }
        }
        Distribution d = pop::Statistics::computedStaticticsFromRealRealizations(vcurve,0.05);
        return d;
    }
    template<typename FunctionPhase1,typename FunctionPhase2>
    static Distribution curvaturePhaseFieldBetweenPhase(const FunctionPhase1 & phase1,const FunctionPhase2 & phase2,typename FunctionTypeTraitsSubstituteF<FunctionPhase1,F64>::Result & curvature,typename FunctionTypeTraitsSubstituteF<FunctionPhase1,F64>::Result& phasefield, F64 cutoffmin=-100 , F64 cutoffmax=100 ,int nbriteration=30,   F64 b=-0.5,F64 a=0.5){
        typedef typename FunctionTypeTraitsSubstituteF<FunctionPhase1,F64>::Result FunctionFloat;
        phasefield.resize(phase1.getDomain());
        typename FunctionFloat::IteratorEDomain it(phase1.getIteratorEDomain());
        while(it.next()){
            if(phase1(it.x())>0)
                phasefield(it.x())=1;
            else
                phasefield(it.x())=-1;
        }
        PDE::LaplacienSmooth<FunctionFloat > laplacien(phasefield,1,2);
        it.init();
        PDEAdvanced::allenCahnInSinglePhaseField(phasefield,nbriteration,it,laplacien,3);
        //        FunctionFloat c(phasefield);
        //        FunctionPhase1 bb;
        //        bb =Processing::greylevelRange(c,0,255);
        //        bb.display();
        //        Scene3d scene;
        //        pop::Visualization::marchingCubePhaseField(scene,phasefield);
        //        pop::Visualization::lineCube(scene,phasefield);
        //        scene.display();
        typename FunctionTypeTraitsSubstituteF<FunctionFloat,VecN<FunctionPhase1::DIM, typename FunctionFloat::F> >::Result gradnormalized(phasefield.getDomain());
        FunctorPDE::Gradient<FunctorPDE::PartialDerivateCentered > func_grad;
        FunctionPhase2  phasedilation = pop::Processing::dilationRegionGrowing(phase2,10);
        it.init();
        while(it.next()){

            gradnormalized(it.x()) = func_grad(phasefield,it.x());
            F64 v =gradnormalized(it.x()).norm();
            if(v>0.0001)
                gradnormalized(it.x())= gradnormalized(it.x())*(1.0/v);
            else
                gradnormalized(it.x())= 0;

        }
        FunctorPDE::Divergence<FunctorPDE::PartialDerivateCentered > func_div;
        VecF64 vcurve;
        curvature.resize(phasefield.getDomain());
        curvature = 0;

        typename FunctionFloat::IteratorERectangle it2(2,phasefield.getDomain()-3);
        it2.init();
        while(it2.next()){
            if(phasedilation(it2.x())!=0){
                typename FunctionFloat::E x = it2.x();
                if(phasefield(x)>b&&phasefield(x)<a){

                    typename FunctionFloat::F divergence = func_div(gradnormalized,x);
                    if(FunctionFloat::DIM==3)
                        divergence /=2;
                    //                    std::cout<<1/divergence<<std::endl;
                    if(divergence!=0&&1/divergence>cutoffmin&&1/divergence<cutoffmax){
                        vcurve.push_back(1/divergence);
                        curvature(x)=1/divergence;
                    }else{
                        curvature(x)=0;
                    }

                }
            }
        }
        Distribution d = pop::Statistics::computedStaticticsFromRealRealizations(vcurve,0.05);
        return d;
    }
    //@}




private:

    template<typename Function>
    static typename FunctionTypeTraitsSubstituteF<  typename Function::E,F64>::Result start_VecN(const Function &  img, const std::vector<DistributionUniformReal> & d )
    {
        typename FunctionTypeTraitsSubstituteF<  typename Function::E,F64>::Result x;
        do{
            for(int i =0;i<Function::DIM;i++)
                x(i)= d[i].randomVariable();
        }while(img(x)==0);
        return x;
    }
    template<typename Function,typename VecN>
    static bool next_VecN(const Function &  img,DistributionNormal& distnorm,VecN& x )
    {
        VecN xtemp = x;
        for(int i =0;i<Function::DIM;i++)
            xtemp(i)+= distnorm.randomVariable();
        typename Function::E xint =xtemp;
        if(img.isValid(xint)==false){
            return false;
        }
        else{
            //reflecting boundary condition
            if(img(xtemp)!=0)
                x = xtemp;
            return true;
        }
    }
    template<typename Function>
    class LaplacienSmooth
    {
    private:
        typename Function::IteratorENeighborhoodMirror itn;
        std::vector<F64> v_value;
        double sigma;
        int radius_kernel;
    public:
        LaplacienSmooth(const Function& f,F64 sigma,int radius_kernel=1)
            :itn(f.getIteratorENeighborhood(radius_kernel,0)),sigma(sigma),radius_kernel(radius_kernel){
            F64 sum=0;
            itn.init(0);
            while(itn.next()){
                double dist = itn.xreal().normPower();
                if(dist!=0){
                    F64  value = std::exp(-0.5*dist/(sigma*sigma));
                    v_value.push_back(value);
                    sum+=value;
                }
                else
                    v_value.push_back(-2*Function::DIM);

            }
            //normalisation
            for(int i=0;i<(int)v_value.size();i++){
                if(v_value[i]>0)
                    v_value[i]*=(2*Function::DIM/sum);

            }
        }
        typename Function::F operator ()(const Function& f, const typename Function::E& x) {
            typedef typename FunctionTypeTraitsSubstituteF<typename Function::F,F64>::Result TypeFloat;
            typename FunctionTypeTraitsSubstituteF<typename Function::F,F64>::Result sum=0;
            FunctorF::FunctorMultiplicationF2< typename Function::F,F64,TypeFloat> op;
            itn.init( x);
            std::vector<F64>::iterator it = v_value.begin();
            while(itn.next()){
                sum += op((*it),f(itn.x()));
                it ++;
            }
            return sum;
        }
    };
};
}
#endif // PDE_H

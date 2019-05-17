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

#ifndef FUNCTIONMatNITERATORE_HPP
#define FUNCTIONMatNITERATORE_HPP
#include<vector>
#include<iostream>
#include"core/data/typeF/TypeF.h"
#include<algorithm>
#include"core/data/vec/Vec.h"
namespace pop
{

template<typename VecN>
class MatNIteratorEDomain
{
private:
    VecN _domain;
    VecN _x;
    int _e;
public:
    typedef VecN  Domain;
    MatNIteratorEDomain(const MatNIteratorEDomain& it)
        :_domain(it.getDomain())
    {
        init();
    }

    MatNIteratorEDomain(const Domain & domain)
        :    _domain(domain){init();}
    void init()
    {
        _x=0;_x(0)--;
        _e=0;
        for(int i =0;i<VecN::DIM;i++){
            if(_domain(i)<=0){
                _e  = VecN::DIM;
            }
        }
    }
    bool next()
    {
        if(_e==VecN::DIM)return false;
        _x(_e)++;
        if(_x(_e)<_domain(_e))
        {
            _e=0;
            return true;
        }
        else
        {
            _x(_e)=0;
            _e++;
            return next();
        }
    }
    VecN & x()
    {
        return _x;
    }
    VecN getDomain()const
    {
        return _domain;
    }
};




template<typename MatN>
class MatNIteratorEROI
{
private:
    typename MatN::E _domain;
    typename MatN::E _x;
    I32 _e;
    const MatN & _ROI;
public:
    typedef const MatN & Domain;
    MatNIteratorEROI(const MatNIteratorEROI & it)
        :_ROI(it.getDomain())
    {
        _domain = _ROI.getDomain();
        init();
    }

    MatNIteratorEROI(const MatN & ROI)
        :_domain(ROI.getDomain()),_ROI(ROI)
    {
        init();
    }
    void init()
    {
        _x=0;
        _x(0)--;
        _e=0;
    }
    bool next()
    {
        bool test;
        do
        {
            if(_e==MatN::DIM)return false;
            _x(_e)++;
            if(_x(_e)<_domain(_e))
            {
                _e=0;
                test=false;
            }
            else
            {
                _x(_e)=0;
                _e++;
                test=true;
            }
        }while(test==true|| (_ROI)(_x)==0);
        return true;
    }
    typename MatN::E & x()
    {
        return _x;
    }
    const MatN & getDomain()const{
        return _ROI;
    }
};





template<typename VecN,typename BoundaryCondition>
class MatNIteratorENeighborhood:public BoundaryCondition
{
protected:
    VecN _domain;
    Vec<VecN> _tab;
    typename Vec<VecN>::iterator _it;
    VecN _x;
    VecN _xprime;
	bool _init;


    void initBall(){
        _tab.clear();
        VecN ddomain (2*_radius+1);
        VecN r (_radius);
        double radiuspower2=_radius*_radius;
        MatNIteratorEDomain<VecN> it(ddomain);
        while(it.next())
        {
            VecN translate= it.x() - r;
            if(translate.normPower(_norm)<=radiuspower2)
            {
                _tab.push_back(translate);
            }
        }
    }

public:
    bool _isball;
    double _radius;
    double _norm;
    typedef std::pair<VecN,Vec<VecN> > Domain;

    MatNIteratorENeighborhood(const MatNIteratorENeighborhood& it)
        :_domain(it.getDomain().first),_tab(it.getDomain().second),_init(true),_isball(it._isball),_radius(it._radius),_norm(it._norm)
    {}
    template<typename IteratorENeighborhood>
    MatNIteratorENeighborhood(const IteratorENeighborhood& it)
        :_domain(it.getDomain().first),_tab(it.getDomain().second),_init(true),_isball(it._isball),_radius(it._radius),_norm(it._norm)
    {}


    MatNIteratorENeighborhood(const VecN &domain,Vec<VecN> v_neighborhood)
        :_domain(domain),_tab(v_neighborhood),_init(true),_isball(false){}

    MatNIteratorENeighborhood(const VecN &domain,  double radius , double norm)
        :_domain(domain),_init(true),_isball(true),_radius(radius),_norm(norm){
        initBall();
    }

    MatNIteratorENeighborhood(const Domain &domain)
        :  _domain(domain.first),_tab(domain.second),_init(true),_isball(false){}

    Domain getDomain()const
    {
        return std::make_pair(_domain,_tab);
    }

    void dilate(MatNIteratorENeighborhood & v_it){
        if(_isball==false){
        typename Vec<VecN>::iterator ittab,it_local;
        Vec<VecN> tab =_tab;
        for(ittab =_tab.begin();ittab!=_tab.end();ittab++){
            for(it_local =v_it.tab().begin();it_local!=v_it.tab().end();it_local++){
                VecN x = *it_local+*ittab;
                if(std::find (tab.begin(), tab.end(),x)==tab.end()){
                    tab.push_back(x);
                }
            }
        }
        _tab=tab;
        }else{
            _radius+=v_it._radius;
            initBall();
        }
    }
    void removeCenter()
    {
        typename Vec<VecN>::iterator it;
        for(it=_tab.begin();it!=_tab.end();it++)
        {
            bool test=false;
            for(I32 i=0;i<VecN::DIM;i++)
            {
                if( it->operator()(i)!=0)test =true;
            }
            if(test==false)
            {
                it= _tab.erase (it);
                break;
            }
        }
    }
    void init(const VecN& x)
    {
        _x = x;
        _it=_tab.begin();
		_init=true;
    }
    bool next()
    {
		if(_init==true)
			_init=false;
		else
			_it++;
        if(_it==_tab.end())return false;
        else
        {
            _xprime= (*_it)+_x;//translation
            if(BoundaryCondition::VecNInsideDomain(_domain,_xprime)==false)return next();
            return true;
        }
    }
    VecN & x()
    {
        return _xprime;
    }
    VecN  xreal()
    {
        return (*_it)+_x;
    }
    VecN  xWithoutTranslation()
    {
        return (*_it);
    }
    Vec<VecN> &  tab()
    {
        return _tab;
    }
    void setDomainMatN(const VecN & domain){
        _domain =domain;
    }

};

template<typename VecN>
class MatNIteratorEOrder
{
private:
    VecN _domain;
    VecN _inverselooporder;
    VecN _direction;
    VecN _x;
    typename VecN::E _e;
    void initInverseLoopOrder(I32 lastloop)
    {
        _inverselooporder(0) = lastloop;
        for(I32 i=0;i<VecN::DIM;i++)
        {

            if(i<lastloop)
                _inverselooporder(i+1)=i;
            else if(i>lastloop)
                _inverselooporder(i)=i;

        }
    }
    void check()
    {
        VecN check;
        for(I32 i=0;i<VecN::DIM;i++)
        {
            check(i)=i;
        }
        for(I32 i=0;i<VecN::DIM;i++)
        {
            bool test=false;
            for(I32 j=0;j<VecN::DIM;j++)
            {
                if(_inverselooporder(j)==check(i))
                {
                    test=true;
                }
            }
            if(test==false)
            {
                std::cerr<<"Error in MatNIteratorEOrder\n Your looporder is not correct: "<<_inverselooporder<<std::endl;
                _inverselooporder=check;
                std::cerr<<"By default your looporder becomes "<<_inverselooporder<<std::endl;
            }

        }
    }
public:
    typedef std::pair<VecN,std::pair<VecN,VecN> >  Domain;
    std::pair<VecN,std::pair<VecN,VecN> > getDomain()const{
        return std::make_pair(_domain,std::make_pair( _inverselooporder ,_direction ));
    }
    MatNIteratorEOrder(const MatNIteratorEOrder& it)
        :    _domain(it.getDomain().first),_inverselooporder(it.getDomain().second.first),_direction(it.getDomain().second.second)
    {
    }
    MatNIteratorEOrder(const Domain & domain)
        :    _domain(domain.first),_inverselooporder(domain.second.first),_direction(domain.second.second)
    {
        init();
    }

    MatNIteratorEOrder(const VecN & domain,const VecN & inverselooporder,const VecN & direction )
        :    _domain(domain),_inverselooporder(inverselooporder),_direction(direction)
    {
        init();
    }
    MatNIteratorEOrder(const VecN & domain,I32 lastloop,const VecN & direction )
        :    _domain(domain),_direction(direction)
    {
        initInverseLoopOrder(lastloop);
        init();
    }

    void setLastLoop(typename VecN::E lastloop)
    {
        initInverseLoopOrder(lastloop);
        init();
    }

    void setOrder(VecN order)
    {
        _inverselooporder=order;
        init();
    }

    void setDirection(const VecN & direction)
    {
        _direction= direction;
        init();
    }
    void init()
    {
        check();
        for(I32 i=0;i<VecN::DIM;i++)
        {
            if(_direction(i)==-1)_x(i)=_domain(i)-1;
            else
            {
                _direction(i)=1;
                _x(i)=0;
            }
        }
        _x(_inverselooporder(0))-=_direction(_inverselooporder(0));
        _e=0;
    }

    I32 getBordeLenghtLastLoop()
    {
        if (_direction(_inverselooporder(_e))==1 )
            return _x[_inverselooporder(_e)];
        else
            return _domain[_inverselooporder(_e)]-1 - _x[_inverselooporder(_e)];
    }
    typename VecN::E getIndexLastLoop()
    {
        return _inverselooporder(0);
    }
    typename VecN::E  getWayLastLoop()
    {
        return _direction(_inverselooporder(0));
    }

    bool next()
    {
        if(_e==VecN::DIM)
        {
            return false;
        }
        _x[_inverselooporder(_e)]+=_direction(_inverselooporder(_e));
        if( (_direction(_inverselooporder(_e))==1 &&  _x[_inverselooporder(_e)]<_domain[_inverselooporder(_e)])
                ||(_direction(_inverselooporder(_e))==-1 &&  _x[_inverselooporder(_e)]>=0))
        {
            if(_e!=0) _e=0;
            return true;
        }
        else
        {
            if(_direction(_inverselooporder(_e))==-1)_x(_inverselooporder(_e))=_domain(_inverselooporder(_e))-1;
            else
                _x(_inverselooporder(_e))=0;
            _e++;
            return next();

        }
    }
    VecN & x()
    {
        return _x;
    }
};


template<typename VecN>
class MatNIteratorERectangle
{
private:
    VecN _xmin;
    VecN _xmax;
    VecN _x;
    int _e;
public:
    typedef std::pair<VecN,VecN>  Domain;
    MatNIteratorERectangle(const MatNIteratorERectangle& it)
        :_xmin(it.getDomain().first),_xmax(it.getDomain().second)
    {
        init();
    }

    MatNIteratorERectangle(const Domain & domain)
        :    _xmin(domain.first),_xmax(domain.second)
    {
        init();
    }
    void init()
    {
        _x=_xmin;_x(0)--;
        _e=0;
        for(int i =0;i<VecN::DIM;i++){
            if(_xmax(i)<=0){
                _e  = VecN::DIM;
            }
        }
    }
    bool next()
    {
        if(_e==VecN::DIM)return false;
        _x(_e)++;
        if(_x(_e)<=_xmax(_e))
        {
            _e=0;
            return true;
        }
        else
        {
            _x(_e)=_xmin(_e );
            _e++;
            return next();
        }
    }
    VecN & x()
    {
        return _x;
    }
    Domain getDomain()const
    {
        return std::make_pair(_xmin,_xmax);
    }
    VecN & xMin()
    {
        return _xmin;
    }
    VecN & xMax()
    {
        return _xmax;
    }
};


template<int DIM>
struct VecNIndice
{
    static   int VecN2Indice(const VecN<DIM,int> & size, const VecN<DIM,int> & x)
    {
        int value=0;
        int temp=1;
        for(int i=0;i<DIM;i++)
        {
            value+=temp*x(i);
            temp*=size(i);
        }
        return value;

    }
    static   int VecN2Indice(const VecN<DIM,int> & size, const Vec<int> & x)
    {
        int value=0;
        int temp=1;
        for(int i=0;i<DIM;i++)
        {
            value+=temp*x(i);
            temp*=size(i);
        }
        return value;

    }
    static   VecN<DIM,int> Indice2VecN(const VecN<DIM,int> & size, int indice)
    {
        VecN<DIM,int> x;
        for(int i=0;i<DIM;i++)
        {
            int temp=1;
            for(int j=0;j<DIM-(i+1);j++)
                temp*=size(j);
            x(DIM-(i+1)) = indice/temp ;
            indice -= (x(DIM-(i+1)) *temp);
        }
        return x;
    }
};
template<>
struct VecNIndice<2>
{
    static   int VecN2Indice(const Vec2I32 & size, const Vec2I32 & x)
    {
        return x(1)+x(0)*size(1);
    }
    static   int VecN2Indice(const Vec2I32 & size, const Vec<int> & x)
    {
        return x(1)+x(0)*size(1);
    }
    static   Vec2I32 Indice2VecN(const Vec2I32 & size, int indice)
    {
        Vec2I32 x;
        x(0)=indice/size(1);
        x(1)=indice - x(0)*size(1);
        return x;
    }
};
template<>
struct VecNIndice<3>
{
    static   int VecN2Indice(const VecN<3,int> & size, const VecN<3,int> & x)
    {
        return x(1)+size(1)*(x(0)+size(0)*x(2));
    }
    static   int VecN2Indice(const VecN<3,int> & size, const Vec<int> & x)
    {
        return x(1)+size(1)*(x(0)+size(0)*x(2));
    }
    static   VecN<3,int> Indice2VecN(const VecN<3,int> & size, int indice)
    {
        VecN<3,int> x;
        int s =size(1)*size(0);
        x(2)=indice/s;
        indice-=x(2)*s;
        x(0)=indice/size(1);
        x(1)=indice - x(0)*size(1);
        return x;
    }
};

}
#endif // FUNCTIONMatNITERATORE_HPP

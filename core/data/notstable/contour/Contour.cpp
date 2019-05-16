#include "Contour.h"
namespace pop{

Contour2::Contour2(){
    disti=0;
    distj=0;
    isclosed =false;
}
Contour2::Contour2(const Contour2 &contour)
    :isclosed(contour.isclosed),contourdir(contour.contourdir),disti(contour.disti),distj(contour.distj),startpoint(contour.startpoint),currentpoint(contour.currentpoint),index(contour.index)
{

}
void Contour2::init()const {
    index=0;
    currentpointiterator = startpoint;
}


bool Contour2::next()const{
    if(index>=contourdir.size()){
        return false;
    }
    else{
        switch (contourdir[index]) {
        case Right:
            currentpointiterator[1]++;
            break;
        case RightUp:
            currentpointiterator[0]--;currentpointiterator[1]++;
            break;
        case Up:
            currentpointiterator[0]--;
            break;
        case UpLeft:
            currentpointiterator[0]--;currentpointiterator[1]--;
            break;
        case Left:
            currentpointiterator[1]--;
            break;
        case LeftDown:
            currentpointiterator[0]++;currentpointiterator[1]--;
            break;
        case Down:
            currentpointiterator[0]++;
            break;
        case DownRight:
            currentpointiterator[0]++;currentpointiterator[1]++;
            break;
        }
        index++;
        return true;
    }
}
Vec2I32 Contour2::x()const{
    return currentpointiterator;
}
int Contour2::distNorm1(){
    return absolute(disti)+absolute(distj);
}

void Contour2::initStartPoint(const Vec2I32 point){
    currentpoint = point;
    startpoint   = point;
    disti=0;
    distj=0;
    isclosed =false;
}

void Contour2::addPoint(const Vec2I32 point){
    int diffi = point(0)-currentpoint(0);
    int diffj = point(1)-currentpoint(1);
    if(diffi==0&&diffj==0)
        return;
    disti += diffi;
    distj += diffj;
    std::cout<<absolute(disti)+absolute(distj)<<std::endl;
    if(absolute(disti)+absolute(distj)<=2&&contourdir.size()>=4)
        isclosed =true;
    currentpoint = point;
    //matrix notation: i vertical from top to down
    //matrix notation: j horizontal from left to down
    if(diffi<0){
        if(diffj<0)
            contourdir.push_back(UpLeft);
        else if(diffj==0)
            contourdir.push_back(Up);
        else
            contourdir.push_back(RightUp);
    }else if(diffi>0){
        if(diffj<0)
            contourdir.push_back(LeftDown);
        else if(diffj==0)
            contourdir.push_back(Down);
        else
            contourdir.push_back(DownRight);
    }else{
        if(diffj<0)
            contourdir.push_back(Left);
        else
            contourdir.push_back(Right);
    }
}
bool Contour2::isClosed(){
    return isclosed;
}
Contour2::Direction OperatorOppositeDirection::operator ()(Contour2::Direction d ){
    switch (d) {
    case Contour2::Right:
        return Contour2::Left;
    case Contour2::RightUp:
        return Contour2::LeftDown;
    case Contour2::Up:
        return Contour2::Down;
    case Contour2::UpLeft:
        return Contour2::DownRight;
    case Contour2::Left:
        return Contour2::Right;
    case Contour2::LeftDown:
        return Contour2::RightUp;
    case Contour2::Down:
        return Contour2::Up;
    case Contour2::DownRight:
        return Contour2::UpLeft;
    default:
        return Contour2::Left;
    }
}
Contour2  mergeContourSameStartingPoint(const Contour2 &contour1, const Contour2 &contour2){
    Contour2 c;
    contour1.init();
    while(contour1.next());
    c.startpoint = contour1.x();
    c.contourdir.resize(contour1.contourdir.size());
    c.contourdir.resize(contour1.contourdir.size()+contour2.contourdir.size());
    std::transform(contour1.contourdir.rbegin(),contour1.contourdir.rend(),c.contourdir.begin(),OperatorOppositeDirection());
    std::copy(contour2.contourdir.begin(),contour2.contourdir.end(),c.contourdir.begin()+contour1.contourdir.size());
    return c;
}


Contour2 findContour(pop::Mat2Vec2F64 grad, VecI32 xstart,F64 threshold,F64 step){
    Contour2 cleft;
    cleft.initStartPoint(xstart);
    Vec2F64 x(Vec2F64(xstart)+0.5);

    F64 tpower2 = threshold*threshold;
    while(1==1){
        Vec2F64 v=grad(x);
        double normPower=v.normPower();
        if(normPower>tpower2){
            F64  d = v(1);
            v(1)=v(0);
            v(0)=-d;
            v= v*step/std::sqrt(normPower);
            x+=v;
            if(grad.isValid(x)==false){
                break;
            }
            cleft.addPoint(x);
            if(cleft.isClosed()==true)
                return cleft;
        }else{
            break;
        }
    }
    Contour2 cright;
    cright.initStartPoint(xstart);
    x =Vec2F64(xstart)+0.5;
    while(1==1){
        Vec2F64 v=grad(x);
        double normPower=v.normPower();
        if(normPower>tpower2){
            F64  d = v(0);
            v(0)=v(1);
            v(1)=-d;
            v= v*step/std::sqrt(normPower);
            x+=v;
            if(grad.isValid(x)==false){
                break;
            }
            cright.addPoint(x);
            if(cright.isClosed()==true)
                return cright;
        }else{
            break;
        }
    }
//    return cright;
    return mergeContourSameStartingPoint(cleft,cright);
}
}

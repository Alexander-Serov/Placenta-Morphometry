#ifndef CONTOUR_H
#define CONTOUR_H
#include"core/data/mat/MatN.h"

namespace pop{
class Contour2
{
public:
    enum Direction{
        Right=0,
        RightUp=1,
        Up=2,
        UpLeft=3,
        Left=4,
        LeftDown=5,
        Down=6,
        DownRight=7
    } ;


    Contour2();

    void init()const ;


    bool next()const;
    Vec2I32 x()const;
    int distNorm1();
    Contour2(const Contour2 &contour);
    void initStartPoint(const Vec2I32 point);
    void addPoint(const Vec2I32 point);
    bool isClosed();
    bool isclosed;
    std::vector<Contour2::Direction> contourdir;
    int disti;
    int distj;
    Vec2I32 startpoint;
    Vec2I32 currentpoint;
    mutable Vec2I32 currentpointiterator;
    mutable unsigned int index;

};
struct OperatorOppositeDirection
{
    Contour2::Direction operator ()(Contour2::Direction d );
};
Contour2  mergeContourSameStartingPoint(const Contour2 &contour1, const Contour2 &contour2);

Contour2 findContour(pop::Mat2Vec2F64 grad, VecI32 xstart,F64 threshold,F64 step=1);
}
#endif // CONTOUR_H

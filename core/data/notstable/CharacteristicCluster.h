#ifndef CARACTERISTICCLUSTER_H
#define CARACTERISTICCLUSTER_H
#include"core/data/vec/VecN.h"
#include"core/data/mat/MatN.h"
namespace pop {
class CharacteristicCluster
{
public:
    CharacteristicCluster();
    void setLabel(int label);
    void addPoint(const Vec2I32 & x);
    Vec2I32 size()const;
    Vec2I32 center()const;
    int _label;
    int _mass;
    Vec2I32 _min;
    Vec2I32 _max;
//    Vec2F64 _barycentre;
};



struct CharacteristicClusterDistance{
    virtual double operator ()(const CharacteristicCluster& a,const CharacteristicCluster& b)=0;
};
struct CharacteristicClusterDistanceMass : public CharacteristicClusterDistance{
    double operator ()(const CharacteristicCluster& a,const CharacteristicCluster& b);
};
struct CharacteristicClusterDistanceHeight : public CharacteristicClusterDistance{
    double operator ()(const CharacteristicCluster& a,const CharacteristicCluster& b);
};
struct CharacteristicClusterDistanceWidth : public CharacteristicClusterDistance{
    double operator ()(const CharacteristicCluster& a,const CharacteristicCluster& b);
};
struct CharacteristicClusterDistanceWidthInterval : public CharacteristicClusterDistance{
    double operator ()(const CharacteristicCluster& a,const CharacteristicCluster& b);
};
struct CharacteristicClusterDistanceHeightInterval : public CharacteristicClusterDistance{
    double operator ()(const CharacteristicCluster& a,const CharacteristicCluster& b);
};

struct CharacteristicClusterFilter{
    double _min;
    double _max;
    CharacteristicClusterFilter();
    virtual ~CharacteristicClusterFilter();
    virtual bool operator ()(const CharacteristicCluster& a);
};
struct CharacteristicClusterFilterMass : public CharacteristicClusterFilter{

    bool operator ()(const CharacteristicCluster& a);
};
struct CharacteristicClusterFilterHeight : public CharacteristicClusterFilter{

    bool operator ()(const CharacteristicCluster& a);
};
struct CharacteristicClusterFilterWidth : public CharacteristicClusterFilter{

    bool operator ()(const CharacteristicCluster& a);
};
struct CharacteristicClusterFilterAsymmetryHeightPerWidth : public CharacteristicClusterFilter{

    bool operator ()(const CharacteristicCluster& a);
};
Vec<CharacteristicCluster> applyCharacteristicClusterFilter(const Vec<CharacteristicCluster>& v_cluster, CharacteristicClusterFilter * filter);

pop::Mat2UI32 applyClusterFilter(const pop::Mat2UI32& labelled_image,const Vec<CharacteristicClusterFilter*> v_filter  );
Vec<Vec<Mat2UI8> >  applyGraphCluster(const pop::Mat2UI32& labelled_image, pop::Vec<CharacteristicClusterDistance*> v_dist,  pop::Vec<double> v_weight,double threshold  );
}
#endif // CARACTERISTICCLUSTER_H

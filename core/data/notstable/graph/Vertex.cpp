#include"core/data/notstable/graph/Vertex.h"

namespace pop{
VecF64 VertexPosition::getPos(){
    return _pos;
}
VecF64 VertexPosition::getPos()const
{
    return _pos;
}
void VertexPosition::setPos(const VecF64 & pos){
    _pos =pos;
}
std::ostream& operator << (std::ostream& out, const VertexPosition &v){
    out<<v.getPos();
    return out;
}
std::istream& operator >> (std::istream& in,  VertexPosition &v){
    VecF64 ve;
    in>>ve;
    v.setPos(ve);
    return in;
}
}

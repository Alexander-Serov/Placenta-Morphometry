#include"core/data/notstable/graph/Edge.h"
namespace pop{
Edge::Edge()
{
}
std::ostream& operator << (std::ostream& out, const Edge &){
    return out;
}

std::istream& operator >> (std::istream& in,  Edge &){
    return in;
}
}

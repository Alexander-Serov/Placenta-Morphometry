#include"core/data/typeF/TypeTraitsF.h"
#include"core/data/typeF/Complex.h"
#include"core/data/typeF/RGB.h"
namespace pop
{
Type2Id<UI8>::Type2Id(){id.push_back("UI8");}
Type2Id<UI16>::Type2Id(){id.push_back("UI16");}
Type2Id<I32>::Type2Id(){id.push_back("I32");}
Type2Id<F64>::Type2Id(){id.push_back("F64");}
}

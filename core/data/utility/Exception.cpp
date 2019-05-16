#include"core/data/utility/Exception.h"
#ifndef NOCIMG
#include"core/dependency/CImg.h"
#endif
namespace pop
{
pexception::pexception(){_message = "ERROR";}
pexception::pexception(const char * message) {_message = message;}
pexception::pexception(std::string message) {_message = message;}
const char * pexception::what() const throw() { return _message.c_str(); }
void pexception::display() const throw() {
#ifndef NOCIMG
    cimg_library::cimg::dialog("Population exception", _message.c_str(),"Abort");
#endif
}
pexception::~pexception() throw() {}

}

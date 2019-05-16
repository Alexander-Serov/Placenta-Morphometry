#include"core/data/distribution/DistributionArithmetic.h"
#include"core/data/distribution/DistributionAnalytic.h"
namespace pop{
DistributionArithmetic::DistributionArithmetic()
{
}

void DistributionArithmetic::setDistributionLeft(const Distribution &f_left){
    _fleft=f_left;
}
void DistributionArithmetic::setStep(F64 step)const{
    _fleft.setStep(step);
    _fright.setStep(step);
}

void DistributionArithmetic::setDistributionRight( const Distribution & f_right){
    _fright=f_right;
}

Distribution & DistributionArithmetic::getDistributionLeft(){
    return  _fleft;
}
const Distribution & DistributionArithmetic::getDistributionLeft()const{
    return  _fleft;
}
Distribution & DistributionArithmetic::getDistributionRight(){
    return  _fright;
}

const Distribution & DistributionArithmetic::getDistributionRight()const{
    return  _fright;
}
F64 DistributionArithmetic::randomVariable()const throw(pexception)
{
        throw(pexception("In DistributionArithmetic::randomVariable()const ,  no  probability distribution, you have to use pop::Statistics::toProbabilityDistribution"));
}
void  DistributionArithmetic::fromString(std::string ){
}

std::string  DistributionArithmetic::toString()const throw(pexception){
    return "";
}

const  std::string DistributionArithmeticAddition::KEY="ADDITION";

DistributionArithmeticAddition::DistributionArithmeticAddition()
    :DistributionArithmetic()
{
    this->_key = DistributionArithmeticAddition::KEY;
}

DistributionArithmeticAddition::DistributionArithmeticAddition(const DistributionArithmeticAddition & dist)
    :DistributionArithmetic()
{
    this->_key = DistributionArithmeticAddition::KEY;
    this->setDistributionLeft(dist.getDistributionLeft());
    this->setDistributionRight(dist.getDistributionRight());
}

DistributionArithmeticAddition * DistributionArithmeticAddition::clone()const throw(pexception){
    return new DistributionArithmeticAddition(*this);
}
F64 DistributionArithmeticAddition::operator ()( F64  value)const throw(pexception){
    return this->getDistributionLeft().operator ()(value)+ this->getDistributionRight().operator ()(value);
}

namespace
{
    const bool registered_DistributionArithmeticAddition = DistributionFactory::getInstance()->Register(  DistributionArithmeticAddition::KEY, new DistributionArithmeticAddition());
}

const  std::string DistributionArithmeticSubtraction::KEY="Subtraction";





DistributionArithmeticSubtraction::DistributionArithmeticSubtraction()
    :DistributionArithmetic()
{
    this->_key = DistributionArithmeticSubtraction::KEY;

}

DistributionArithmeticSubtraction::DistributionArithmeticSubtraction(const DistributionArithmeticSubtraction & dist)
    :DistributionArithmetic()
{
    this->_key = DistributionArithmeticSubtraction::KEY;
    this->setDistributionLeft(dist.getDistributionLeft());
    this->setDistributionRight(dist.getDistributionRight());
}

DistributionArithmeticSubtraction * DistributionArithmeticSubtraction::clone()const throw(pexception){
    return new DistributionArithmeticSubtraction(*this);
}
F64 DistributionArithmeticSubtraction::operator ()( F64  value)const throw(pexception){
    return this->getDistributionLeft().operator ()(value)- this->getDistributionRight().operator ()(value);
}
namespace
{
    const bool registered_DistributionArithmeticSubtraction = DistributionFactory::getInstance()->Register(  DistributionArithmeticSubtraction::KEY, new DistributionArithmeticSubtraction());
}

const  std::string DistributionArithmeticMultiplication::KEY="Multiplication";

DistributionArithmeticMultiplication::DistributionArithmeticMultiplication()
    :DistributionArithmetic()
{
    this->_key = DistributionArithmeticMultiplication::KEY;
}

DistributionArithmeticMultiplication::DistributionArithmeticMultiplication(const DistributionArithmeticMultiplication & dist)
    :DistributionArithmetic()
{
    this->_key = DistributionArithmeticMultiplication::KEY;
    this->setDistributionLeft(dist.getDistributionLeft());
    this->setDistributionRight(dist.getDistributionRight());
}

DistributionArithmeticMultiplication * DistributionArithmeticMultiplication::clone()const throw(pexception){
    return new DistributionArithmeticMultiplication(*this);
}
F64 DistributionArithmeticMultiplication::operator ()( F64  value)const throw(pexception){
    return this->getDistributionLeft().operator ()(value)* this->getDistributionRight().operator ()(value);
}
namespace
{
    const bool registered_DistributionArithmeticMultiplication = DistributionFactory::getInstance()->Register(  DistributionArithmeticMultiplication::KEY, new DistributionArithmeticMultiplication());
}

const  std::string DistributionArithmeticDivision::KEY="Division";

DistributionArithmeticDivision::DistributionArithmeticDivision()
    :DistributionArithmetic()
{
    this->_key = DistributionArithmeticDivision::KEY;
}

DistributionArithmeticDivision::DistributionArithmeticDivision(const DistributionArithmeticDivision & dist)
    :DistributionArithmetic()
{
    this->_key = DistributionArithmeticDivision::KEY;
    this->setDistributionLeft(dist.getDistributionLeft());
    this->setDistributionRight(dist.getDistributionRight());
}

DistributionArithmeticDivision * DistributionArithmeticDivision::clone()const throw(pexception){
    return new DistributionArithmeticDivision(*this);
}
F64 DistributionArithmeticDivision::operator ()( F64  value)const throw(pexception){
    return this->getDistributionLeft().operator ()(value)/this->getDistributionRight().operator ()(value);
}
namespace
{
    const bool registered_DistributionArithmeticDivision = DistributionFactory::getInstance()->Register(  DistributionArithmeticDivision::KEY, new DistributionArithmeticDivision());
}
const  std::string DistributionArithmeticComposition::KEY="Composition";

DistributionArithmeticComposition::DistributionArithmeticComposition()
    :DistributionArithmetic()
{
    this->_key = DistributionArithmeticComposition::KEY;
    DistributionUniformReal d(0,1);
}

DistributionArithmeticComposition::DistributionArithmeticComposition(const DistributionArithmeticComposition & dist)
    :DistributionArithmetic()
{
    this->_key = DistributionArithmeticComposition::KEY;
    this->setDistributionLeft(dist.getDistributionLeft());
    this->setDistributionRight(dist.getDistributionRight());
}

DistributionArithmeticComposition * DistributionArithmeticComposition::clone()const throw(pexception){
    return new DistributionArithmeticComposition(*this);
}
F64 DistributionArithmeticComposition::operator ()( F64  value)const throw(pexception){
    return this->getDistributionLeft().operator ()(this->getDistributionRight().operator ()(value));
}
namespace
{
    const bool registered_DistributionArithmeticComposition = DistributionFactory::getInstance()->Register(  DistributionArithmeticComposition::KEY, new DistributionArithmeticComposition());
}

const  std::string DistributionArithmeticMax::KEY="Max";

DistributionArithmeticMax::DistributionArithmeticMax()
    :DistributionArithmetic()
{
    this->_key = DistributionArithmeticMax::KEY;
}

DistributionArithmeticMax::DistributionArithmeticMax(const DistributionArithmeticMax & dist)
    :DistributionArithmetic()
{
    this->_key = DistributionArithmeticMax::KEY;
    this->setDistributionLeft(dist.getDistributionLeft());
    this->setDistributionRight(dist.getDistributionRight());
}

DistributionArithmeticMax * DistributionArithmeticMax::clone()const throw(pexception){
    return new DistributionArithmeticMax(*this);
}
F64 DistributionArithmeticMax::operator ()( F64  value)const throw(pexception){
    return maximum(this->getDistributionLeft().operator ()(value),this->getDistributionRight().operator ()(value));
}
namespace
{
    const bool registered_DistributionArithmeticMax = DistributionFactory::getInstance()->Register(  DistributionArithmeticMax::KEY, new DistributionArithmeticMax());
}
const  std::string DistributionArithmeticMin::KEY="Min";

DistributionArithmeticMin::DistributionArithmeticMin()
    :DistributionArithmetic()
{
    this->_key = DistributionArithmeticMin::KEY;
}

DistributionArithmeticMin::DistributionArithmeticMin( const DistributionArithmeticMin &dist)
    :DistributionArithmetic()
{
    this->_key = DistributionArithmeticMin::KEY;
    this->setDistributionLeft(dist.getDistributionLeft());
    this->setDistributionRight(dist.getDistributionRight());
}

DistributionArithmeticMin * DistributionArithmeticMin::clone()const throw(pexception){
    return new DistributionArithmeticMin(*this);
}
F64 DistributionArithmeticMin::operator ()( F64  value)const throw(pexception){
    return minimum(this->getDistributionLeft().operator ()(value),this->getDistributionRight().operator ()(value));
}
namespace
{
    const bool registered_DistributionArithmeticMin = DistributionFactory::getInstance()->Register(  DistributionArithmeticMin::KEY, new DistributionArithmeticMin());
}
}

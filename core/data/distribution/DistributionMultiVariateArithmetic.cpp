#include"core/data/distribution/DistributionMultiVariateArithmetic.h"
#include"core/data/distribution/DistributionMultiVariateFromDataStructure.h"


namespace pop{
DistributionMultiVariateArithmetic::DistributionMultiVariateArithmetic()

{
}


void DistributionMultiVariateArithmetic::setDistributionMultiVariateLeft(const DistributionMultiVariate &f_left){
    _fleft=f_left;
}
void DistributionMultiVariateArithmetic::setStep(F64 step)const{
    _fleft.setStep(step);
    _fright.setStep(step);
}

void DistributionMultiVariateArithmetic::setDistributionMultiVariateRight(const DistributionMultiVariate & f_right){
    _fright=f_right;
}

DistributionMultiVariate & DistributionMultiVariateArithmetic::getDistributionMultiVariateLeft(){
    return  _fleft;
}
const DistributionMultiVariate & DistributionMultiVariateArithmetic::getDistributionMultiVariateLeft()const{
    return  _fleft;
}
DistributionMultiVariate & DistributionMultiVariateArithmetic::getDistributionMultiVariateRight(){
    return  _fright;
}

const DistributionMultiVariate & DistributionMultiVariateArithmetic::getDistributionMultiVariateRight()const{
    return  _fright;
}
VecF64 DistributionMultiVariateArithmetic::randomVariable()const  throw(pexception)
{
    throw(pexception("In distributionMultiVariateArithmetic::randomVariable(), no  probability distribution, you have to use pop::Statistics::toProbabilityDistribution"));
}
void  DistributionMultiVariateArithmetic::fromString(std::string ){
}

std::string  DistributionMultiVariateArithmetic::toString()const{
    return "";
}

const  std::string DistributionMultiVariateArithmeticAddition::KEY="ADDITION";

DistributionMultiVariateArithmeticAddition::DistributionMultiVariateArithmeticAddition()
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateArithmeticAddition::KEY;
}

DistributionMultiVariateArithmeticAddition::DistributionMultiVariateArithmeticAddition( const DistributionMultiVariateArithmeticAddition &dist)
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateArithmeticAddition::KEY;
    this->setDistributionMultiVariateLeft(dist.getDistributionMultiVariateLeft());
    this->setDistributionMultiVariateRight(dist.getDistributionMultiVariateRight());
}

DistributionMultiVariateArithmeticAddition * DistributionMultiVariateArithmeticAddition::clone()const throw(pexception){
    return new DistributionMultiVariateArithmeticAddition(*this);
}
F64 DistributionMultiVariateArithmeticAddition::operator()(const VecF64&  value)const{
    return this->getDistributionMultiVariateLeft().operator ()(value)+ this->getDistributionMultiVariateRight().operator ()(value);
}

namespace
{
const bool registered_DistributionMultiVariateArithmeticAddition = DistributionMultiVariateFactory::getInstance()->Register(  DistributionMultiVariateArithmeticAddition::KEY, new DistributionMultiVariateArithmeticAddition());
}

const  std::string DistributionMultiVariateArithmeticSubtraction::KEY="Subtraction";





DistributionMultiVariateArithmeticSubtraction::DistributionMultiVariateArithmeticSubtraction()
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateArithmeticSubtraction::KEY;
}

DistributionMultiVariateArithmeticSubtraction::DistributionMultiVariateArithmeticSubtraction(const DistributionMultiVariateArithmeticSubtraction & dist)
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateArithmeticSubtraction::KEY;
    this->setDistributionMultiVariateLeft(dist.getDistributionMultiVariateLeft());
    this->setDistributionMultiVariateRight(dist.getDistributionMultiVariateRight());
}

DistributionMultiVariateArithmeticSubtraction * DistributionMultiVariateArithmeticSubtraction::clone()const throw(pexception){
    return new DistributionMultiVariateArithmeticSubtraction(*this);
}
F64 DistributionMultiVariateArithmeticSubtraction::operator()(const VecF64&  value)const{
    return this->getDistributionMultiVariateLeft().operator ()(value)- this->getDistributionMultiVariateRight().operator ()(value);
}
namespace
{
const bool registered_DistributionMultiVariateArithmeticSubtraction = DistributionMultiVariateFactory::getInstance()->Register(  DistributionMultiVariateArithmeticSubtraction::KEY, new DistributionMultiVariateArithmeticSubtraction());
}

const  std::string DistributionMultiVariateArithmeticMultiplication::KEY="Multiplication";

DistributionMultiVariateArithmeticMultiplication::DistributionMultiVariateArithmeticMultiplication()
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateArithmeticMultiplication::KEY;
}

DistributionMultiVariateArithmeticMultiplication::DistributionMultiVariateArithmeticMultiplication(const DistributionMultiVariateArithmeticMultiplication & dist)
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateArithmeticMultiplication::KEY;
    this->setDistributionMultiVariateLeft(dist.getDistributionMultiVariateLeft());
    this->setDistributionMultiVariateRight(dist.getDistributionMultiVariateRight());
}

DistributionMultiVariateArithmeticMultiplication * DistributionMultiVariateArithmeticMultiplication::clone()const throw(pexception){
    return new DistributionMultiVariateArithmeticMultiplication(*this);
}
F64 DistributionMultiVariateArithmeticMultiplication::operator()(const VecF64&  value)const{
    return this->getDistributionMultiVariateLeft().operator ()(value)* this->getDistributionMultiVariateRight().operator ()(value);
}
namespace
{
const bool registered_DistributionMultiVariateArithmeticMultiplication = DistributionMultiVariateFactory::getInstance()->Register(  DistributionMultiVariateArithmeticMultiplication::KEY, new DistributionMultiVariateArithmeticMultiplication());
}

const  std::string DistributionMultiVariateArithmeticDivision::KEY="Division";

DistributionMultiVariateArithmeticDivision::DistributionMultiVariateArithmeticDivision()
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateArithmeticDivision::KEY;
}

DistributionMultiVariateArithmeticDivision::DistributionMultiVariateArithmeticDivision(const DistributionMultiVariateArithmeticDivision & dist)
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateArithmeticDivision::KEY;
    this->setDistributionMultiVariateLeft(dist.getDistributionMultiVariateLeft());
    this->setDistributionMultiVariateRight(dist.getDistributionMultiVariateRight());
}

DistributionMultiVariateArithmeticDivision * DistributionMultiVariateArithmeticDivision::clone()const throw(pexception){
    return new DistributionMultiVariateArithmeticDivision(*this);
}
F64 DistributionMultiVariateArithmeticDivision::operator()(const VecF64&  value)const{
    return this->getDistributionMultiVariateLeft().operator ()(value)/this->getDistributionMultiVariateRight().operator ()(value);
}
namespace
{
const bool registered_DistributionMultiVariateArithmeticDivision = DistributionMultiVariateFactory::getInstance()->Register(  DistributionMultiVariateArithmeticDivision::KEY, new DistributionMultiVariateArithmeticDivision());
}
const  std::string DistributionMultiVariateArithmeticComposition::KEY="Composition";

DistributionMultiVariateArithmeticComposition::DistributionMultiVariateArithmeticComposition()
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateArithmeticComposition::KEY;
}

DistributionMultiVariateArithmeticComposition::DistributionMultiVariateArithmeticComposition(const DistributionMultiVariateArithmeticComposition & dist)
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateArithmeticComposition::KEY;
    this->setDistributionMultiVariateLeft(dist.getDistributionMultiVariateLeft());
    this->setDistributionMultiVariateRight(dist.getDistributionMultiVariateRight());
}

DistributionMultiVariateArithmeticComposition * DistributionMultiVariateArithmeticComposition::clone()const throw(pexception){
    return new DistributionMultiVariateArithmeticComposition(*this);
}
F64 DistributionMultiVariateArithmeticComposition::operator()(const VecF64&  )const{
    return 0;
    //    return this->getDistributionMultiVariateLeft().operator ()(this->getDistributionMultiVariateRight().operator ()(value));
}
namespace
{
const bool registered_DistributionMultiVariateArithmeticComposition = DistributionMultiVariateFactory::getInstance()->Register(  DistributionMultiVariateArithmeticComposition::KEY, new DistributionMultiVariateArithmeticComposition());
}

const  std::string DistributionMultiVariateArithmeticMin::KEY="Min";

DistributionMultiVariateArithmeticMin::DistributionMultiVariateArithmeticMin()
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateArithmeticMin::KEY;
}

DistributionMultiVariateArithmeticMin::DistributionMultiVariateArithmeticMin(const DistributionMultiVariateArithmeticMin & dist)
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateArithmeticMin::KEY;
    this->setDistributionMultiVariateLeft(dist.getDistributionMultiVariateLeft());
    this->setDistributionMultiVariateRight(dist.getDistributionMultiVariateRight());
}

DistributionMultiVariateArithmeticMin * DistributionMultiVariateArithmeticMin::clone()const throw(pexception){
    return new DistributionMultiVariateArithmeticMin(*this);
}
F64 DistributionMultiVariateArithmeticMin::operator()(const VecF64&  value)const{
    return minimum(this->getDistributionMultiVariateLeft().operator ()(value),this->getDistributionMultiVariateRight().operator ()(value));
}
namespace
{
const bool registered_DistributionMultiVariateArithmeticMin = DistributionMultiVariateFactory::getInstance()->Register(  DistributionMultiVariateArithmeticMin::KEY, new DistributionMultiVariateArithmeticMin());
}
const  std::string DistributionMultiVariateArithmeticMax::KEY="Max";

DistributionMultiVariateArithmeticMax::DistributionMultiVariateArithmeticMax()
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateArithmeticMax::KEY;
}

DistributionMultiVariateArithmeticMax::DistributionMultiVariateArithmeticMax(const DistributionMultiVariateArithmeticMax & dist)
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateArithmeticMax::KEY;
    this->setDistributionMultiVariateLeft(dist.getDistributionMultiVariateLeft());
    this->setDistributionMultiVariateRight(dist.getDistributionMultiVariateRight());
}

DistributionMultiVariateArithmeticMax * DistributionMultiVariateArithmeticMax::clone()const throw(pexception){
    return new DistributionMultiVariateArithmeticMax(*this);
}
F64 DistributionMultiVariateArithmeticMax::operator()(const VecF64&  value)const{
    return maximum(this->getDistributionMultiVariateLeft().operator ()(value),this->getDistributionMultiVariateRight().operator ()(value));
}
namespace
{
const bool registered_DistributionMultiVariateArithmeticMax = DistributionMultiVariateFactory::getInstance()->Register(  DistributionMultiVariateArithmeticMax::KEY, new DistributionMultiVariateArithmeticMax());
}
const  std::string DistributionMultiVariateSeparationProduct::KEY="SeparationProduct";

DistributionMultiVariateSeparationProduct::DistributionMultiVariateSeparationProduct()
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateSeparationProduct::KEY;
}
int DistributionMultiVariateSeparationProduct::getNbrVariable()const{
    return this->getDistributionMultiVariateLeft().getNbrVariable()+this->getDistributionMultiVariateRight().getNbrVariable();
}
DistributionMultiVariateSeparationProduct::DistributionMultiVariateSeparationProduct(const DistributionMultiVariateSeparationProduct & dist)
    :DistributionMultiVariateArithmetic()
{
    this->_key = DistributionMultiVariateSeparationProduct::KEY;
    this->setDistributionMultiVariateLeft(dist.getDistributionMultiVariateLeft());
    this->setDistributionMultiVariateRight(dist.getDistributionMultiVariateRight());
}

DistributionMultiVariateSeparationProduct * DistributionMultiVariateSeparationProduct::clone()const throw(pexception){
    return new DistributionMultiVariateSeparationProduct(*this);
}
F64 DistributionMultiVariateSeparationProduct::operator()(const VecF64&  v)const{
    VecF64 vleft(this->getDistributionMultiVariateLeft().getNbrVariable());
    for(int i=0;i<this->getDistributionMultiVariateLeft().getNbrVariable();i++)
        vleft(i)=v(i);

    VecF64 vright(this->getDistributionMultiVariateRight().getNbrVariable());
    for(int i=0;i<this->getDistributionMultiVariateRight().getNbrVariable();i++)
        vright(i)=v(i+this->getDistributionMultiVariateLeft().getNbrVariable());

    return this->getDistributionMultiVariateLeft().operator ()(vleft)*this->getDistributionMultiVariateRight().operator ()(vright);
}
VecF64 DistributionMultiVariateSeparationProduct::randomVariable()const throw(pexception){
    VecF64 vleft = this->getDistributionMultiVariateLeft().randomVariable();
    VecF64 vright = this->getDistributionMultiVariateRight().randomVariable();

    int size = vleft.size();
    vleft.resize(vleft.size()+vright.size());
    for(int i =0;i<(int)vright.size();i++)
        vleft(size+i) = vright(i);
    return vleft;

}
namespace
{
const bool registered_DistributionMultiVariateMerge = DistributionMultiVariateFactory::getInstance()->Register(  DistributionMultiVariateSeparationProduct::KEY, new DistributionMultiVariateSeparationProduct());
}


const  std::string DistributionMultiVariateCoupled::KEY;
DistributionMultiVariateCoupled::DistributionMultiVariateCoupled()
    :DistributionMultiVariate()
{
    _nbr_variable_coupled =0;
}
DistributionMultiVariateCoupled::DistributionMultiVariateCoupled(const DistributionMultiVariateCoupled & dist)
    :DistributionMultiVariate()
{
    _single = dist.getSingleDistribution();
    _nbr_variable_coupled = dist.getNbrVariableCoupled();
}
void DistributionMultiVariateCoupled::setNbrVariableCoupled(int nbr_variable_coupled){
    _nbr_variable_coupled=nbr_variable_coupled;
}


int DistributionMultiVariateCoupled::getNbrVariableCoupled() const{
    return _nbr_variable_coupled;
}


void DistributionMultiVariateCoupled::setSingleDistribution(const Distribution &distsingle){
    _single = distsingle;
}
Distribution DistributionMultiVariateCoupled::getSingleDistribution()const{
    return _single;
}




DistributionMultiVariateCoupled * DistributionMultiVariateCoupled::clone()const throw(pexception){
    return new DistributionMultiVariateCoupled(*this);
}

F64 DistributionMultiVariateCoupled::operator()(const VecF64&  value)const{
    for(unsigned int i=1;i<value.size();i++){
        if(value(1)<value(0)-_step/2 || value(1)>value(0)+_step/2)
            return 0;
    }
    return _single.operator() (value(0));
}
VecF64 DistributionMultiVariateCoupled::randomVariable()const throw(pexception){
    F64 v = _single.randomVariable();
    VecF64 vout(_nbr_variable_coupled,v);
    return vout;
}

 int DistributionMultiVariateCoupled::getNbrVariable()const{
     return _nbr_variable_coupled;
}


}

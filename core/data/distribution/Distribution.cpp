#include <ctime>
#include"core/data/distribution/Distribution.h"
#include"core/data/distribution/DistributionFromDataStructure.h"
#include"core/data/utility/BasicUtility.h"
#include"core/data/distribution/DistributionArithmetic.h"
#include"core/data/distribution/DistributionAnalytic.h"
#include"core/data/mat/MatN.h"
#include"core/data/mat/MatNInOut.h"
#include"core/data/mat/MatNDisplay.h"
#include"core/algorithm/Draw.h"
#include"core/algorithm/Statistics.h"
namespace pop
{
unsigned long Distribution::init[] = {static_cast<unsigned long>(time(NULL)), 0x234, 0x345, 0x456};
unsigned long Distribution::length = 4;

MTRand_int32 & Distribution::MTRand(){
    return irand;
}

MTRand_int32 Distribution::irand(Distribution::init, Distribution::length);
Distribution::Distribution()
    :_deriveddistribution(NULL)
{
}

Distribution::Distribution(const Distribution & d)
    :_deriveddistribution(d.clone())
{
}
Distribution & Distribution::operator =(const Distribution& d){
    this->___setPointererImplementation( d.clone());
    return *this;
}

Distribution::Distribution(double param , const char* type)throw(pexception)
{
    if(type==DistributionPoisson::KEY ){
        _deriveddistribution= new DistributionPoisson(param);
    }else if(type==DistributionExponential::KEY){
        _deriveddistribution= new DistributionExponential(param);
    }else if(type==DistributionDirac::KEY)
        _deriveddistribution= new DistributionDirac(param);
    else
        throw(pexception("In Distribution::Distribution(double param , const char* type), type must be equal to one accepted distribution: " +DistributionPoisson::KEY+" "+DistributionExponential::KEY+" "+DistributionDirac::KEY ));
}
Distribution::Distribution(double param1,double param2, const char* type)throw(pexception)
{
    if(type==DistributionBinomial::KEY){
        _deriveddistribution= new DistributionBinomial(param1,param2);
    }else if(type==DistributionNormal::KEY){
        _deriveddistribution= new DistributionNormal(param1,param2);
    }else if(type==DistributionUniformInt::KEY){
        _deriveddistribution= new DistributionUniformInt(param1,param2);
    }else if(type==DistributionUniformReal::KEY)
        _deriveddistribution= new DistributionUniformReal(param1,param2);
    else
        throw(pexception("In Distribution::Distribution(double param1,double param2 , const char* type), type must be equal to one accepted distribution: "+DistributionBinomial::KEY+" "+DistributionNormal::KEY+" "+DistributionUniformInt::KEY+" "+DistributionUniformReal::KEY ));
}

Distribution::Distribution(const Mat2F64& param,const char* type)throw(pexception){
    if(type==DistributionRegularStep::KEY ){
        _deriveddistribution= new DistributionRegularStep(param);
    }else if(type==DistributionIntegerRegularStep::KEY){
        _deriveddistribution= new DistributionIntegerRegularStep(param);
    }
    else
        throw(pexception("In Distribution::Distribution(Mat2F64 param,const char* type), type must be equal to one accepted distribution: "+DistributionRegularStep::KEY+" "+DistributionIntegerRegularStep::KEY));
}

Distribution::Distribution(const char* param,const char* type)throw(pexception){
    if(type==DistributionExpression::KEY ){
        _deriveddistribution= new DistributionExpression(param);
    }else
        throw(pexception("In Distribution::Distribution(const char* param, const char* type), type must be equal to one accepted distribution: "+DistributionExpression::KEY));
}

std::string Distribution::getKey()const{
    return _key;
}

Distribution::~Distribution()
{
    if(_deriveddistribution!=NULL)
        delete _deriveddistribution;
}

void Distribution::setStep(F64 )const{

}


F64 Distribution::getStep()const{
    if(_deriveddistribution!=NULL)
        return _deriveddistribution->getStep();
    else
        return 0.01;
}

F64 Distribution::getXmin()const{
    if(_deriveddistribution!=NULL)
        return _deriveddistribution->getXmin();
    else
        return -NumericLimits<F64>::maximumRange();
}
F64 Distribution::getXmax()const{
    if(_deriveddistribution!=NULL)
        return _deriveddistribution->getXmax();
    else
        return NumericLimits<F64>::maximumRange();
}
F64 Distribution::randomVariable(F64 value)const throw(pexception)
{
    if(_deriveddistribution!=NULL)
        return _deriveddistribution->operator ()(value);
    else
        throw(pexception("In Distribution::randomVariable(F64 value), empty distributio"));
}



F64 Distribution::operator()(F64 value)const throw(pexception){
    if(_deriveddistribution!=NULL)
        return _deriveddistribution->operator ()(value);
    else
        throw(pexception("In Distribution::operator()(F64 value), empty distribution"));
}

F64 Distribution::randomVariable()const throw(pexception){
    if(_deriveddistribution!=NULL)
        return _deriveddistribution->randomVariable();
    else
        throw(pexception("In Distribution::randomVariable(), empty distribution"));
}
F64 Distribution::operator()()const throw(pexception){
    return this->randomVariable();
}

Distribution * Distribution::clone()const throw(pexception){
    if(_deriveddistribution!=NULL)
        return _deriveddistribution->clone();
    else
        throw(pexception("In Distribution::clone(), empty distribution"));
}
void Distribution::fromString(std::string str){
    if(_deriveddistribution!=NULL)
        _deriveddistribution->fromString(str);
}

std::string Distribution::toString()const throw(pexception){
    if(_deriveddistribution!=NULL)
        return _deriveddistribution->toString();
    else
        throw(pexception("In Distribution::toString(), empty distribution"));
}



const Distribution * Distribution::___getPointerImplementation()const{

    return _deriveddistribution;

}

Distribution * Distribution::___getPointerImplementation(){

    return _deriveddistribution;

}

void Distribution::___setPointererImplementation(Distribution * d){

    delete _deriveddistribution;
    _deriveddistribution = d;
}

Distribution  Distribution::rho(const Distribution &d)const{
    DistributionArithmeticComposition *dist = new DistributionArithmeticComposition;
    dist->setDistributionLeft(*this);
    dist->setDistributionRight(d);
    Distribution dout;
    dout.___setPointererImplementation(dist);
    return dout;
}
Distribution  Distribution::operator +(const Distribution& d)const
{
    DistributionArithmeticAddition *dist = new DistributionArithmeticAddition;
    dist->setDistributionLeft(*this);
    dist->setDistributionRight(d);

    Distribution dout;
    dout.___setPointererImplementation(dist);
    return dout;
}
Distribution  Distribution::operator -(const Distribution& d)const
{
    DistributionArithmeticSubtraction *dist = new DistributionArithmeticSubtraction;
    dist->setDistributionLeft(*this);
    dist->setDistributionRight(d);
    Distribution dout;
    dout.___setPointererImplementation(dist);
    return dout;
}
Distribution  Distribution::operator *(const Distribution& d)const
{
    DistributionArithmeticMultiplication *dist = new DistributionArithmeticMultiplication;
    dist->setDistributionLeft(*this);
    dist->setDistributionRight(d);
    Distribution dout;
    dout.___setPointererImplementation(dist);
    return dout;
}
Distribution  Distribution::operator /(const Distribution& d)const
{
    DistributionArithmeticDivision *dist = new DistributionArithmeticDivision;
    dist->setDistributionLeft(*this);
    dist->setDistributionRight(d);
    Distribution dout;
    dout.___setPointererImplementation(dist);
    return dout;
}
Distribution  Distribution::operator -()const
{
    DistributionArithmeticSubtraction *dist = new DistributionArithmeticSubtraction;
    Distribution  exp ("0","EXPRESSION");
    dist->setDistributionLeft(exp);
    dist->setDistributionRight(*this);
    Distribution dout;
    dout.___setPointererImplementation(dist);
    return dout;
}





void Distribution::load(const char * file) throw(pexception)
{
    std::ifstream  in(file);
    if (in.fail())
    {
        throw(pexception("In Distribution::load,  cannot open file: "+std::string(file)));
    }
    else
    {
        in>>*this;
    }
}

void Distribution::save(const char * file)const throw(pexception)
{
    std::ofstream  out(file);
    if (out.fail())
    {
        throw(pexception("In Distribution::save, cannot open file: "+std::string(file)));
    }
    else
    {
        out<<*this;
    }
}
Mat2RGBUI8 Distribution::multiDisplay( Distribution & d1,Distribution & d2,Distribution & d3,double xmin,double xmax,double ymin,double ymax,int sizex,int sizey){
    std::vector<Distribution> d;
    d.push_back(d1);
    d.push_back(d2);
    d.push_back(d3);
    return Distribution::multiDisplay( d,    xmin, xmax, ymin, ymax, sizex, sizey);
}
Mat2RGBUI8 Distribution::multiDisplay( Distribution & d1,Distribution & d2,double xmin,double xmax,double ymin,double ymax,int sizex,int sizey){
    std::vector<Distribution> d;
    d.push_back(d1);
    d.push_back(d2);
    return Distribution::multiDisplay( d,    xmin, xmax, ymin, ymax, sizex, sizey);
}
Mat2RGBUI8 Distribution::multiDisplay( std::vector<Distribution> & d,double xmin,double xmax,double ymin,double ymax,int sizex,int sizey){
    if(xmin == -NumericLimits<double>::maximumRange()){
        xmin = d[0].getXmin();
        if(xmin == -NumericLimits<double>::maximumRange())
            xmin = 0;
    }
    if(xmax == NumericLimits<double>::maximumRange()){
        xmax = d[0].getXmax();
        if(xmax == NumericLimits<double>::maximumRange())
            xmax = 1;
    }
    if(ymin == -NumericLimits<double>::maximumRange()){
        ymin = pop::Statistics::minValue(d[0],xmin,xmax);
    }
    if(ymax == NumericLimits<double>::maximumRange()){
        ymax = pop::Statistics::maxValue(d[0],xmin,xmax);
    }
    std::vector<RGBUI8> v_RGB;
    for(int i=0;i<(int)d.size();i++)
        v_RGB.push_back(RGBUI8::randomRGB());
    MatN<2,RGBUI8> img (sizey,sizex);
    const char * c = "Simple plot: left(right)-arrow to move x, up(down)-to move y and +(-) to (un)zoom";
#ifdef NOCIMG
      MatNDisplayInterface main_disp;
#else
        MatNDisplay main_disp;
#endif

    main_disp.display(img);
    main_disp.set_title(c);
    while (!main_disp.is_closed() ) {

        if(main_disp.is_keyARROWDOWN()){
            double diff =ymax-ymin;
            ymin -= diff*0.02;
            ymax -= diff*0.02;
        }
        if(main_disp.is_keyARROWUP())
        {
            double diff =ymax-ymin;
            ymin += diff*0.02;
            ymax += diff*0.02;
        }
        if(main_disp.is_keyARROWLEFT()){
            double diff =xmax-xmin;
            xmin -= diff*0.02;
            xmax -= diff*0.02;
        }
        if(main_disp.is_keyARROWRIGHT())
        {
            double diff =xmax-xmin;
            xmin += diff*0.02;
            xmax += diff*0.02;
        }
        if(main_disp.is_keyPADADD())
        {
            double diffx =xmax-xmin;
            double diffy =ymax-ymin;
            xmin += diffx*0.02;
            xmax -= diffx*0.02;
            ymin += diffy*0.02;
            ymax -= diffy*0.02;
        }
        if(main_disp.is_keyPADSUB())
        {
            double diffx =xmax-xmin;
            double diffy =ymax-ymin;
            xmin -= diffx*0.02;
            xmax += diffx*0.02;
            ymin -= diffy*0.02;
            ymax += diffy*0.02;
        }

        img=RGBUI8(0);
        Draw::axis(img, xmin, xmax, ymin, ymax,255);


        for(int i =0;i<(int)d.size();i++){
            VecN<2,int> x1,x2;
            x1(1)=img.getDomain()(1)-40;x2(1)=img.getDomain()(1)-20;
            x1(0)=(i+1)*10;x2(0)=(i+1)*10;

            Draw::line(img,x1,x2,v_RGB[i],1);
            Draw::text(img,BasicUtility::Any2String(i).c_str(),Vec2I32(x2(0)-5,x2(1)+5),v_RGB[i]);
            Draw::distribution(d[i], xmin, xmax, ymin, ymax,v_RGB[i],img);

        }
        main_disp.display(img).waitTime();
    }
    return img;
}

Mat2RGBUI8 Distribution::display(double xmin,double xmax,double ymin,double ymax,int sizewidth,int sizeheight){
    if(xmin == -NumericLimits<double>::maximumRange()){
        xmin = this->getXmin();
        if(xmin == -NumericLimits<double>::maximumRange())
            xmin = 0;
    }
    if(xmax == NumericLimits<double>::maximumRange()){
        xmax = this->getXmax();
        if(xmax == NumericLimits<double>::maximumRange())
            xmax = 1;
    }
    if(ymin == -NumericLimits<double>::maximumRange()){
        ymin = pop::Statistics::minValue(*this,xmin,xmax);
    }
    if(ymax == NumericLimits<double>::maximumRange()){
        ymax = pop::Statistics::maxValue(*this,xmin,xmax);
    }


    MatN<2,unsigned char> img (sizeheight, sizewidth);
    Draw::distribution(*this, xmin, xmax, ymin, ymax,255,img);
    Draw::axis(img, xmin, xmax, ymin, ymax,255);
#ifdef NOCIMG
      MatNDisplayInterface main_disp;
#else
        MatNDisplay main_disp;
#endif
    main_disp.set_title("Simple plot: left(right)-arrow to move x, up(down)-to move y and +(-) to (un)zoom");
    main_disp.display(img);
    while (!main_disp.is_closed() ) {

        if(main_disp.is_keyARROWDOWN()){
            double diff =ymax-ymin;
            ymin -= diff*0.02;
            ymax -= diff*0.02;
        }
        else if(main_disp.is_keyARROWUP())
        {
            double diff =ymax-ymin;
            ymin += diff*0.02;
            ymax += diff*0.02;
        }
        else if(main_disp.is_keyARROWLEFT()){
            double diff =xmax-xmin;
            xmin -= diff*0.02;
            xmax -= diff*0.02;
        }
        else if(main_disp.is_keyARROWRIGHT())
        {
            double diff =xmax-xmin;
            xmin += diff*0.02;
            xmax += diff*0.02;
        }
        else if(main_disp.is_keyPADADD())
        {
            double diffx =xmax-xmin;
            double diffy =ymax-ymin;
            xmin += diffx*0.02;
            xmax -= diffx*0.02;
            ymin += diffy*0.02;
            ymax -= diffy*0.02;
        }
        else if(main_disp.is_keyPADSUB())
        {
            double diffx =xmax-xmin;
            double diffy =ymax-ymin;
            xmin -= diffx*0.02;
            xmax += diffx*0.02;
            ymin -= diffy*0.02;
            ymax += diffy*0.02;
        }else if(main_disp.is_keyS())
        {
            img.save("snaphot.png");
        }

        img=0;
        Draw::distribution(*this, xmin, xmax, ymin, ymax,255,img);
        Draw::axis(img, xmin, xmax, ymin, ymax,255);
        main_disp.display(img).waitTime();
    }
    return img;
}
pop::Distribution maximum(const pop::Distribution & d1, const pop::Distribution & d2){
    pop::DistributionArithmeticMax *dist = new pop::DistributionArithmeticMax;
    dist->setDistributionLeft(d1);
    dist->setDistributionRight(d2);
    pop::Distribution dout;
    dout.___setPointererImplementation(dist);
    return dout;
}

pop::Distribution minimum(const pop::Distribution & d1, const pop::Distribution & d2){
    pop::DistributionArithmeticMin *dist = new pop::DistributionArithmeticMin;
    dist->setDistributionLeft(d1);
    dist->setDistributionRight(d2);
    pop::Distribution dout;
    dout.___setPointererImplementation(dist);
    return dout;
}
std::ostream& operator << (std::ostream& out, const pop::Distribution& d){
    const pop::Distribution * dist = d.___getPointerImplementation();
    if(dist==NULL)
        dist = &d;
    if(const pop::DistributionArithmetic*dar =dynamic_cast<const pop::DistributionArithmetic*>(dist) ){

        out<<dar->getDistributionLeft();
        out<<dar->getDistributionRight();
        out<<dar->getKey()<<"|";
    }
    else
        out<<dist->getKey()<<"|"<<dist->toString()<<"|";
    return out;
}

std::istream& operator >> (std::istream& in, pop::Distribution& m){
    std::vector<pop::Distribution*> stack;
    std::string str;
    while(!getline(in,str,'|').eof())
    {
        pop::Distribution *d = pop::DistributionFactory::getInstance()->createObject(str);
        if(pop::DistributionArithmetic*dar =dynamic_cast<pop::DistributionArithmetic*>(d) ){
            pop::Distribution * drigh = stack.back();
            stack.pop_back();
            pop::Distribution * dleft = stack.back();
            stack.pop_back();
            dar->setDistributionLeft(*dleft);
            dar->setDistributionRight(*drigh);

            delete drigh;
            delete dleft;
            stack.push_back(dar);
        }else{
            getline(in,str,'|');
            d->fromString(str);
            stack.push_back(d);
        }
    }
    m.___setPointererImplementation( stack[0]);
    return in;
}
}

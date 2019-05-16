#include"core/data/utility/Configuration.h"
#include "OCR.h"
#include"core/data/utility/Exception.h"
#include "core/data/vec/VecN.h"
#include "core/algorithm/GeometricalTransformation.h"
#include "core/algorithm/Processing.h"
#include"core/data/notstable/CharacteristicCluster.h"
#include"core/data/mat/MatNDisplay.h"
#ifdef TESSERACT
#if Pop_OS==2
#ifdef _MSC_VER
#include "tesseract/baseapi.h"
#else
#define __BLOB_T_DEFINED
#include "core/dependency/tesseract/include/tesseract/baseapi.h"
#endif
#else
#include "tesseract/baseapi.h"
#include <locale.h>
#endif
#endif



namespace pop
{
OCR::~OCR()
{

}
NeuralNetworkFeedForward &   OCRNeuralNetwork::neuralNetworkFeedForward(){
    return _n;
}

const NeuralNetworkFeedForward &   OCRNeuralNetwork::neuralNetworkFeedForward()const{
    return _n;
}



std::string OCR::parseText(const Mat2UI8 & m,int nbr_pixels_width_caracter)throw(pop::pexception){

    std::string str1 = _parseTextByContrast(m,nbr_pixels_width_caracter);
    Mat2UI8 m_contrast = m.opposite();
    std::string str2 =  _parseTextByContrast(m_contrast,nbr_pixels_width_caracter);
    return str1+"\n"+str2;
}
std::string OCR::_parseTextByContrast(const Mat2UI8 & m,int nbr_pixels_width_caracter)throw(pop::pexception){
    Mat2UI8 threhold =        Processing::thresholdNiblackMethod(m,0.2,3*nbr_pixels_width_caracter,-20);

    Mat2UI32 label = Processing::clusterToLabel(threhold,0);


    CharacteristicClusterFilterMass filter_mass;
    filter_mass._min = nbr_pixels_width_caracter*10;
    filter_mass._max = nbr_pixels_width_caracter*1000;

    CharacteristicClusterFilterAsymmetryHeightPerWidth filter_asymmetry;
    filter_asymmetry._min =0.5;
    filter_asymmetry._max = 20;
    Vec<CharacteristicClusterFilter*> v_filter;
    v_filter.push_back(&filter_mass);
    v_filter.push_back(&filter_asymmetry);

    label =  applyClusterFilter(label,v_filter );

    pop::Vec<CharacteristicClusterDistance*> v_dist;
    pop::Vec<double> v_weight;


    CharacteristicClusterDistanceHeight dist_height;
    CharacteristicClusterDistanceWidthInterval dist_interval_width;
    CharacteristicClusterDistanceHeightInterval dist_interval_height;
    v_dist.push_back(&dist_height);
    v_dist.push_back(&dist_interval_width);
    v_dist.push_back(&dist_interval_height);
    v_weight.push_back(10);
    v_weight.push_back(0.1);
    v_weight.push_back(4);
    Vec<Vec<Mat2UI8> > v_v_img = applyGraphCluster(label,v_dist,v_weight,0.5);

    std::string str;
    for(unsigned int i=0;i<v_v_img.size();i++){
        std::string str2;
        bool parse=false;
        for(unsigned int j=0;j<v_v_img(i).size();j++){

            char c = this->parseMatrix(v_v_img(i)(j));
//            std::cout<<i<<" "<<c<<" "<<this->characterConfidence() <<std::endl;
//            v_v_img(i)(j).display();
            if(this->isRecognitionCharacter()){
                str2.push_back(c);
                parse=true;
            }
        }
//        std::cout<<str2<<std::endl;
        if(str.size()!=0&&parse==true){

            str=str+" ";
          }
        str=str+str2;
    }
    return str;
}

char OCRNeuralNetwork::parseMatrix(const Mat2UI8 & m)throw(pop::pexception){
    if(_n.layers().size()==0)
    {
        throw(pop::pexception("Neural network is empty. Used setDictionnary to construct it. I give one for digit number.  the folder $${PopulationPath}/file/handwrittendigitneuralnetwork.xml, you can find the handwritten dictionnary. So the code is ocr.setDictionnary($${PopulationPath}/file/neuralnetwork.xml) "));
    }
    else{
        VecF64 vin= _n.inputMatrixToInputNeuron(m);
        VecF64 vout;
        _n.propagateFront(vin,vout);
        VecF64::iterator itt = std::max_element(vout.begin(),vout.end());
        int label_max = std::distance(vout.begin(),itt);
        double value_max = *itt;
        if(value_max<0)
            _isrecognized=false;
        else
            _isrecognized=true;
        _confidence = value_max*100;
        std::string c= _n.label2String()[label_max];
        return c[0];

    }
    return '?';
}
int OCRNeuralNetwork::characterConfidence(){
    return _confidence;
}
void OCRNeuralNetwork::setDictionnary(std::string xmlfile){
    if(BasicUtility::isFile(xmlfile))
        _n.load(xmlfile.c_str());

}
bool OCRNeuralNetwork::isRecognitionCharacter(){
    return _isrecognized;

}
#ifdef TESSERACT
class OCRTesseract::impl
{
public:
    bool _isdicset;
    tesseract::TessBaseAPI myOCR;

    impl()
        :_isdicset(false)
    {
    }

    ~impl()
    {
        myOCR.Clear();
        myOCR.End();
    }

};

OCRTesseract::OCRTesseract()
{
    _pimpl = new   impl;
}
OCRTesseract::~OCRTesseract()
{
    delete _pimpl;
}

void OCRTesseract::setCharacter(std::string characterlist){
    _pimpl->myOCR.SetVariable("tessedit_char_whitelist", characterlist.c_str());
}

char OCRTesseract::parseMatrix(const Mat2UI8 & img)throw(pop::pexception){
    if(_pimpl->_isdicset==false){
        throw(pop::pexception("OCR::parseMatrix, you have to set a dictionnary, for instance ocr.setDictionnary(\"/usr/share/tesseract-ocr/tessdata/eng.traineddata\");"));
    }
    _pimpl->myOCR.SetImage(img.data(),img.getDomain()(1),img.getDomain()(0),1,img.getDomain()(1));
    char *elment = _pimpl->myOCR.GetUTF8Text();
    std::string parse = elment;
    if(parse.size()>=3)
        parse.erase(parse.end()-2,parse.end());
    delete[]elment;
    if(parse.empty()==false)
        return parse[0];
    else
        return '?';
}
bool OCRTesseract::isRecognitionCharacter(){
    int value =characterConfidence();
    if(value==0)
        return false;
    else
        return true;

}

int OCRTesseract::characterConfidence(){
    int * v =_pimpl->myOCR.AllWordConfidences();
    std::vector<int> v_conf;
    int index=0;
    while(v[index]!=-1){
        v_conf.push_back(v[index]);
        index++;
    }
    delete[]v;
    if(v_conf.empty()==false)
        return v_conf[0];
    else
        return 0;
}
void OCRTesseract::setDictionnary(std::string path_dic){
#if Pop_OS==1
    setlocale (LC_NUMERIC, "C");
#endif
    std::string path =pop::BasicUtility::getPath(path_dic);
    std::size_t    found=path.find("tessdata");
    if (found==std::string::npos){
        pexception p("Could not initialize tesseract because the dictionnary must be in directory name /tessdata/");
        p.display();
    }
    path =pop::BasicUtility::getPath(path)+"/";
    path = "TESSDATA_PREFIX="+path;
    char * nostr =const_cast<char*>(path.c_str());
#if Pop_OS==2
    _putenv(nostr);
#else
    putenv(nostr);
#endif
    std::string name =pop::BasicUtility::getBasefilename(path_dic);
    if (_pimpl->myOCR.Init(NULL, name.c_str())) {
        pexception p("Could not initialize tesseract with this dictionnary "+name);
        p.display();
    }else{
        _pimpl->_isdicset=true;
    }
}
#endif
}



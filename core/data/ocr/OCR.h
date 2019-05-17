#ifndef OCR_H
#define OCR_H
#include<string>
#include"core/data/mat/MatN.h"
#include"core/data/neuralnetwork/NeuralNetwork.h"
namespace pop
{
/*! \ingroup Other
* \defgroup OCR OCR
* \brief
* @{
*/
class POP_EXPORTS OCR
{
public:
    virtual ~OCR();

    std::string parseText(const Mat2UI8 & binary,int nbr_pixels_width_caracter)throw(pop::pexception);
    /*!
    \brief apply the OCR on a binary matrix containing a single caracter
    \param binary binary input matrix
    \return OCR single character
    !*/
    virtual char parseMatrix(const Mat2UI8 & binary)throw(pop::pexception)=0;
    virtual bool isRecognitionCharacter()=0;

    /*!
    \brief return the the character confidence after OCR parsing (between 0 and 100) in an array
    \return  confidence value of the character
    !*/
    virtual int characterConfidence()=0;

    virtual void setDictionnary(std::string path_dic)=0;

private:
    std::string _parseTextByContrast(const Mat2UI8 & binary,int nbr_pixels_width_caracter)throw(pop::pexception);
};

class POP_EXPORTS OCRNeuralNetwork : public OCR
{
private:
    NeuralNetworkFeedForward _n;
    int _confidence;
    bool _isrecognized;
public:
    NeuralNetworkFeedForward &   neuralNetworkFeedForward();
    const NeuralNetworkFeedForward &   neuralNetworkFeedForward()const;
    char parseMatrix(const Mat2UI8 & binary)throw(pop::pexception);
    bool isRecognitionCharacter();
    int characterConfidence();
    void setDictionnary(std::string path_dic);

};
#ifdef TESSERACT
class POP_EXPORTS OCRTesseract
{
private:
    class impl;
    impl *_pimpl;
public:
    OCRTesseract();
    virtual ~OCRTesseract();
    void setCharacter(std::string characterlist);
    char parseMatrix(const Mat2UI8 & binary)throw(pop::pexception);
    bool isRecognitionCharacter();
    int characterConfidence();
    void setDictionnary(std::string path_dic);
};
#endif
/*!
@}
*/



}
#endif // OCR_H

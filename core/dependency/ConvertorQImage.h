#ifndef CONVERTORQIMAGE_H
#define CONVERTORQIMAGE_H

#include"core/data/mat/MatN.h"
#include"core/data/GP/Dynamic2Static.h"
#include"core/data/mat/MatNListType.h"
#include<QImage>
#include<QColor>
namespace pop{
class ConvertorQImage
{
private:

    template<pop::I32 DIM,typename Result>
    static MatN<DIM,pop::UI8> fromQImage(const QImage &,Loki::Int2Type<DIM>,Loki::Type2Type<Result>)throw(pexception)
    {
        throw(pexception("In Convertor::fromQImage, Pixel/voxel type must be pop::UI8 or RGB"));
    }


    static MatN<2,pop::UI8> fromQImage(const QImage & qimg,bool isfastconversion,Loki::Int2Type<2>,Loki::Type2Type<pop::UI8>)throw(pexception);
    static MatN<2,RGBUI8> fromQImage(const QImage & qimg,bool isfastconversion,Loki::Int2Type<2>,Loki::Type2Type<RGBUI8>)throw(pexception);
public:
    /*!
    \fn QImage toQImage(const MatN<DIM,Result> & img)throw(pexception)
    \param img input population image
    \return QImage QImage object
    \exception  std::string ion Image must be bidimensionnel and its pixel/voxel type must be pop::UI8 or RGB
    *
    * Convert in QImage object
    */
    template<pop::I32 DIM,typename Result>
    static QImage toQImage(const MatN<DIM,Result> & img,bool isfastconversion=false)throw(pexception){
        if(DIM!=2)
            throw(pexception("In Convertor::toQImage, Image must be bidimensionnel"));
        else {
            MatN<DIM,pop::UI8> temp (img);
            return ConvertorQImage::toQImage(temp,isfastconversion);
        }
    }
    template<pop::I32 DIM>
    static QImage toQImage(const MatN<DIM,pop::UI8> & img,bool isfastconversion=false)throw(pexception){
        if(DIM!=2)
            throw(pexception("In Convertor::toQImage, Image must be bidimensionnel"));
        if(isfastconversion==true){
            QImage qimg( img.getDomain()(1),img.getDomain()(0),QImage::Format_Indexed8);
            std::copy(img.begin(),img.end(),qimg.bits());
            return qimg;
        }else{
            return toQImage(MatN<DIM,RGBUI8>(img));
        }
        return QImage();

    }
    template<pop::I32 DIM>
    static QImage toQImage(const MatN<DIM,RGBUI8> & img,bool isfastconversion=false)throw(pexception){
        if(DIM!=2)
            throw(pexception("In Convertor::toQImage, Image must be bidimensionnel"));
        QImage qimg( img.getDomain()(1),img.getDomain()(0),QImage::Format_RGB888);
        if(isfastconversion==false){
            for (int i = 0; i < qimg.width(); ++i){
                for (int j = 0; j < qimg.height(); ++j){
                    QColor col(img(j,i).r(),img(j,i).g(),img(j,i).b());
                    qimg.setPixel(i,j,col.rgb());
                }
            }
        }else{
            const unsigned char * data=reinterpret_cast<const unsigned char *>(img.data());
            std::copy(data,data+img.getDomain().multCoordinate(),qimg.bits());
        }

        return qimg;
    }
    /*!
    \fn MatN<DIM,Result> fromQImage(const QImage & qimg)throw(pexception)
    \param qimg input QImage object
    \return
    \exception  std::string ion Image must be bidimensionnel, its pixel/voxel type must be pop::UI8 or RGBUI8 and qimg is not empty
    *
    * Convert in QImage object
    */
    template<pop::I32 DIM,typename Result>
    static MatN<DIM,Result> fromQImage(const QImage & qimg,bool isfastconversion=false)throw(pexception)
    {
        if(DIM!=2)
            throw(pexception("In Convertor::fromQImage, Image must be bidimensionnel"));
        return ConvertorQImage::fromQImage(qimg,isfastconversion,Loki::Int2Type<DIM>(),Loki::Type2Type<Result>());
    }


};
}
#endif // CONVERTORQIMAGE_H

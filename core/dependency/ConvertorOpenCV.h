#ifndef CONVERTOROPENCV_H
#define CONVERTOROPENCV_H
#include"core/data/mat/MatN.h"
#include"opencv2/highgui/highgui.hpp"

namespace pop{

template<typename _Tp>
class OpenCVModel : public cv::Mat_<_Tp>
{
public:
    typedef _Tp value_type;
    typedef typename cv::DataType<_Tp>::channel_type channel_type;
    typedef cv::MatIterator_<_Tp> iterator;
    typedef cv::MatConstIterator_<_Tp> const_iterator;
    typedef VecN<2,int> Domain;
    typedef VecN<2,int> E;
    typedef _Tp F;
    typedef MatNIteratorEDomain<E >  IteratorEDomain;
    typedef MatNIteratorENeighborhood<E,BoundaryConditionBounded >  IteratorENeighborhood;

    enum{DIM=2};
    Domain getDomain()const{
        return E(this->rows,this->cols);
    }
    MatN<2,_Tp>  toMatN(){
        MatN<2,_Tp> img(this->getDomain());
        IteratorEDomain it(this->getDomainIteratorEDomain());
        while(it.next())
        {
            img(it.x())=this->operator ()(it.x());
        }
        return img;
    }
    void  fromMatN(const MatN<2,_Tp>& img){
        this->resize(img.getDomain());
        IteratorEDomain it(this->getDomainIteratorEDomain());
        while(it.next())
        {
            this->operator ()(it.x()) = img(it.x());
        }
    }

    OpenCVModel(){}
    OpenCVModel(int rowSize,int colSize)
        :cv::Mat_<_Tp>(rowSize,colSize)
    {}
    OpenCVModel(const Domain & d)
        : cv::Mat_<_Tp>(d(1),d(0))
    {
    }
    OpenCVModel(const Domain & d,const _Tp& v)
        : cv::Mat_<_Tp>(d(1),d(0),v)
    {
    }
    OpenCVModel(const cv::Mat& m)
        : cv::Mat_<_Tp>(m)
    {
    }
    typename IteratorEDomain::Domain getDomainIteratorEDomain()const
    {
        return E(this->cols,this->rows);
    }
    typename IteratorENeighborhood::Domain getDomainIteratorENeighborhood(F64 radius=1 ,F64 norm=1 )const
    {
        std::vector<E> _tab;
        E domain (2*radius+1);
        E r (radius);
        IteratorEDomain it(domain);
        while(it.next())
        {
            E translate= it.x() - r;
            if(translate.norm(norm)<=radius)
            {
                _tab.push_back(translate);
            }
        }
        return std::make_pair(getDomain(),_tab);
    }

    inline _Tp& operator ()(int x, int y)
    {
        return ((_Tp*)(this->data + this->step.p[0]*y))[x];
    }
    inline const _Tp& operator ()(int x, int y) const
    {
        return ((const _Tp*)(this->data + this->step.p[0]*y))[x];
    }

    inline _Tp& operator ()(const E& pt)
    {
        return ((_Tp*)(cv::Mat_<_Tp>::data + this->step.p[0]*pt(1)))[pt(0)];
    }

    inline const _Tp& operator ()(const E& pt) const
    {
        return ((const _Tp*)(cv::Mat_<_Tp>::data + this->step.p[0]*pt(1)))[pt(0)];
    }
    void resize(const Domain & d){
        *this = OpenCVModel(d);
    }


};
template<typename F1, typename F2>
struct FunctionTypeTraitsSubstituteF<OpenCVModel<F1>,F2 >
{
    typedef OpenCVModel<F2> Result;
};

struct ConvertorOpenCVMat
{
    /*!
        \class pop::ConvertorOpenCVMat
        \brief  bridge between opencv and population
        \author Tariel Vincent

         From opencv, the input image
        \code
    cv::Mat opencv_image = cv::imread("/usr/share/doc/opencv-doc/examples/cpp/lena.jpg", CV_LOAD_IMAGE_COLOR);
    cv::Mat dest;
    cv::cvtColor(opencv_image, dest,CV_BGR2RGB);
    Mat2RGBUI8 pop_img_color = pop::ConvertorOpenCVMat::fromCVMatRGBUI8ToPopMatRGBUI8(dest);
    pop_img_color.display();
    cv::cvtColor(opencv_image, dest,CV_BGR2GRAY);
    Mat2RGBUI8 pop_img_grey = pop::ConvertorOpenCVMat::fromCVMatUI8ToPopMatUI8(dest);
    pop_img_grey.display();
        \endcode
        From Population
        \code
    Mat2RGBUI8 pop_img_color ("/usr/share/doc/opencv-doc/examples/cpp/lena.jpg");
    cv::Mat cv_img_color = pop::ConvertorOpenCVMat::fromPopMatToCVMat(pop_img_color);
    Mat2UI8 pop_img_grey(pop_img_color);
    cv::Mat cv_img_grey =pop::ConvertorOpenCVMat::fromPopMatToCVMat(pop_img_grey);
    cv::namedWindow( "Color", CV_WINDOW_AUTOSIZE );
    cv::imshow( "Color", cv_img_color );
    cv::namedWindow( "Grey", CV_WINDOW_AUTOSIZE );
    cv::imshow( "Grey", cv_img_grey );
    cv::waitKey(0);
         \endcode


    */




    static pop::MatN<2,RGBUI8> fromCVMatRGBUI8ToPopMatRGBUI8(const cv::Mat& mat)throw(pexception);
    static MatN<2,UI8> fromCVMatUI8ToPopMatUI8(const cv::Mat& mat)throw(pexception);

    static cv::Mat fromPopMatToCVMat(const pop::MatN<2,RGBUI8>& mat);

    static cv::Mat fromPopMatToCVMat(const pop::MatN<2,UI8>& mat);
};
}
#endif // CONVERTOROPENCV_H

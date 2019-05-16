#ifndef PROCESSINGVIDEO_H
#define PROCESSINGVIDEO_H

namespace pop
{


template<typename Function>
class  TrackingVideoYang
{
    /*!
    \class pop::Feature::TrackingVideoYang
    \brief Implementation of Yang et al Real-time and accurate segmentation of moving objects in dynamic scene
    *
    *
    *
    * \code
    string dir = "/home/vtariel/Bureau/DataBase/Acquisition/record/";
    Vec<string> files = BasicUtility::getFilesInDirectory(dir);
    std::sort(files.begin(),files.end());
    MatNDisplay windows;
    Feature::TrackingVideoYang<Mat2UI8> tracking;
    for(int i=0;i<files.size();i++){
        Mat2RGBUI8 img;
        string path = dir+files[i];

        img.load(path.c_str());
        Mat2UI8 grey(img);
        grey = Draw::mergeTwomatrixHorizontal(grey,tracking.processFrame(grey));
        windows.display(grey);
    }
    * \endcode
    *
    */
private:
    Function dynamicmatric;
    Function img_old;
    Function background;
    bool first ;
    //        pop::MatNDisplay display;
    //        pop::MatNDisplay display2;
    //        pop::MatNDisplay display3;

private:
    void dynamicMatrixModifiedTaoAlgorithm(const Function & frame_timet,const Function & frame_timet_minus_deltat,Function & dynamic_matrix,Function &background,double Tf=10,typename Function::F lambda=10,double alpha=0.2)
    {

        //frameToFrameDifferenceImage
        Function frametoframefifferenceimage(frame_timet);
        typename Function::IteratorEDomain it = frametoframefifferenceimage.getIteratorEDomain();

        while(it.next()){
            double value1 = normValue(frame_timet(it.x()));
            double value2 = normValue(frame_timet_minus_deltat(it.x()));
            double value = value1 - value2;
            if(absolute(value)>Tf){
                dynamic_matrix(it.x())=lambda;
            }
            else{
                dynamic_matrix(it.x())=maximum(dynamic_matrix(it.x())-1,0);
            }
            if(dynamic_matrix(it.x())==0){
                background(it.x())=(1-alpha)*background(it.x())+alpha*frame_timet(it.x());
            }
        }
        //           display2.display(Draw::mergeTwoImageHorizontal(dynamic_matrix*10,background));
    }

public:
    //init the image if the size of the frame changed!!!
    void init(){
        first =true;
    }
    TrackingVideoYang(){
        init();
    }
    Function processFrame( const Function& in,double sigma=2,double Tf=3,typename Function::F lambda=10,double alpha=0.1,double T=10){
        Function img(in);
        img = pop::Processing::smoothGaussian(img,sigma);
        if(!(in.getDomain()==background.getDomain()))
        {
            first=true;
        }
        if(first==true){
            first = false;
            background =img;
            img_old =img;
            dynamicmatric.resize(img.getDomain());
            dynamicmatric = 0;
            return dynamicmatric;
        }else{
            dynamicMatrixModifiedTaoAlgorithm(img,img_old,dynamicmatric,background,Tf,lambda,alpha);
            Function moving(img.getDomain());
            typename Function::IteratorEDomain it(background.getIteratorEDomain());
            while(it.next()){
                if(normValue((double)img(it.x())-background(it.x()))>T){
                    moving(it.x())=255;
                }
                else
                    moving(it.x())=0;
            }
            img_old = img;
            return moving;
        }
    }
};
}

#endif // PROCESSINGVIDEO_H

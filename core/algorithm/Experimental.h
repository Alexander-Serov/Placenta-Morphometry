#ifndef EXPIRMENTAL_H
#define EXPIRMENTAL_H
#include"core/data/mat/MatN.h"
#include"core/data/notstable/Descriptor.h"
#include"core/algorithm/Feature.h"
////////////// NOT STABLE, NOT TESTED code !!!!!!!!!!!! ////////////////
namespace pop {

struct FeatureLetter
{
    static void letter()
    {

        std::string path_linux = "D:/Users/vtariel/Desktop/_0015.png";
        Mat2UI8 img(path_linux.c_str());
        img = Processing::median(img,1);

        Mat2F64 smooth = Processing::smoothDeriche(Mat2F64(img),2);
        Mat2F64::IteratorERectangle itg =smooth.getIteratorERectangle(Vec2I32(10,10),smooth.getDomain()-Vec2I32(10,10));
        Mat2F64::IteratorENeighborhood itn (smooth.getIteratorENeighborhood(2,2));
        Vec<Vec2I32> maxima = pop::ProcessingAdvanced::extremaLocal(smooth,itg,itn);
        double sigma = 1.2;
        double nbr_layers = 3;
        Pyramid<2,F64> pyramid  = Feature::pyramidGaussian(img,sigma,-1,nbr_layers,1);
        Pyramid<2,F64> pyramid_diff  = Feature::pyramidDifference(pyramid);

        Vec<std::pair<int,int> > v_scale;
        for(unsigned int index_max=0;index_max<maxima.size();index_max++){
            int index_octave_max;
            int index_layer_max;
            for(unsigned int index_octave=0;index_octave<pyramid_diff.nbrOctave();index_octave++){
                const MatN<3,F64> & foctave = pyramid_diff.octave(index_octave);

                for(int index_layer=0;index_layer<foctave.getDomain()(2);index_layer++){
                    Vec3F64 x;
                    x(0)=maxima[index_max](0)/std::pow(2.,static_cast<double>(index_octave));
                    x(1)=maxima[index_max](1)/std::pow(2.,static_cast<double>(index_octave));
                    x(2)=index_layer;
                    if(foctave.isValid(x)){
                        std::cout<<foctave(x)<<std::endl;
                    }
                }
            }
            v_scale.push_back(std::make_pair(index_octave_max,index_layer_max));
            std::cout<<std::endl;
        }
    }
    template<typename Function,typename IteratorNeigh>
    static Mat2F64   _pieChartInPieChartDoubleWeighByGradienMagnitudeAndGaussianCircularWindow(const Function& f, IteratorNeigh& itn,VecN<Function::DIM,F64> &orientation,int nbr_orientation)
    {
        FunctorPDE::Gradient<> gradient;
        PieChartInPieChart<Function::DIM> disk(orientation,nbr_orientation);
        while(itn.next()){
            VecN<Function::DIM,F64> orientationglobal =itn.xWithoutTranslation();
            VecN<Function::DIM,F64> orientationlocal = gradient(f,itn.x());
            double norm = normValue(orientationlocal);
            disk.addValue(orientationglobal,orientationlocal,norm);
        }
        disk.normalize();
        return disk._data;
    }


    template<typename Function,typename IteratorNeigh,typename Function2>
    VecN<Function::DIM,F64>   _orientationMeanDoubleWeighByGradienMagnitudeAndGaussianCircularWindow(const Function& f, IteratorNeigh& itn,Function2& gf)
    {
        FunctorPDE::Gradient<> gradient;
        Vec<double>  gradient_magnitude;
        Vec< VecN<Function::DIM,F64> >  gradient_orientation;

        while(itn.next()){
            VecN<Function::DIM,F64> g = gradient(f,itn.x());
            F64 norm = g.normPower()*g.normPower();
            gf(itn.x())=norm;
            gradient_magnitude.push_back(norm);
            if(norm!=0)
                gradient_orientation.push_back(g*(1/norm));
            else
                gradient_orientation.push_back(g);
        }


        return _orientationPrincipalHistogram(gradient_orientation,gradient_magnitude,20);
    }

    template<int DIM,typename Type>
    Vec<Descriptor<KeyPointPyramid<DIM> > > _descriptorPyramidPieChart(const Pyramid<DIM,Type>& pyramid_G,const Vec<KeyPointPyramid<DIM> >   & keypoints, double sigma=1.6,double scale_factor_gaussien=4,int nbr_orientation=6)
    {
        Vec<Descriptor<KeyPointPyramid<DIM> > > descriptors;


        const MatN<DIM+1,Type> & octaveinit = pyramid_G.octave(0);
        typename MatNReference<DIM,Type>::IteratorENeighborhood itn (const_cast<MatN<DIM+1,Type>*>(&octaveinit)->getPlaneByReference(0).getIteratorENeighborhood(sigma*scale_factor_gaussien,2));
        double k = std::pow( 2., 1. / (octaveinit.getDomain()(DIM)-pyramid_G.getNbrExtraLayerPerOctave()) );



        for(int index_extrema=0;index_extrema<(int)keypoints.size();index_extrema++){
            const KeyPointPyramid<DIM>  & keypoint =keypoints[index_extrema];
            int octave =keypoint.octave();
            const MatN<DIM+1,Type> & moctave = pyramid_G.octave(octave);
            double layer=round(keypoint.layer());
            MatNReference<DIM,Type> plane= const_cast<MatN<DIM+1,Type>*>(&moctave)->getPlaneByReference(layer);
            itn.setDomainMatN(plane.getDomain());
            VecN<DIM,F64> x =keypoint.xInPyramid();
            itn.init(round(x));
            double sigma_layer = std::pow(k,keypoint.layer())*sigma*scale_factor_gaussien;
            MatN<DIM,Type> gg(plane.getDomain());
            VecN<DIM,F64> orientation = _orientationMeanDoubleWeighByGradienMagnitudeAndGaussianCircularWindow(plane,itn,gg);
            //        if(index_extrema==239||index_extrema==209){
            //            MatN<DIM,Type> img22(plane.getDomain());
            //            ForEachDomain(x,img22)
            //                    img22(x)=plane(x);
            //            img22.display();
            //            gg.display();
            //        }
            itn.init(round(x));
            Mat2F64 data = _pieChartInPieChartDoubleWeighByGradienMagnitudeAndGaussianCircularWindow(plane,itn,orientation,nbr_orientation);
            Descriptor<KeyPointPyramid<DIM> >  descriptor;
            descriptor.keyPoint() = keypoint;
            descriptor.orientation() = orientation;
            descriptor.data() = data;
            descriptors.push_back(descriptor);
        }
        return descriptors;
    }
    template<int DIM,typename Type,typename TKeyPoint>
    static Vec<Descriptor<TKeyPoint> > descriptorProfilePyramid(const Pyramid<DIM,Type>& pyramid_G,const Vec<TKeyPoint >   & keypoints)
    {
        Vec<Descriptor<TKeyPoint> > descriptors;

        for(int index_extrema=0;index_extrema<(int)keypoints.size();index_extrema++){
            const TKeyPoint  & keypoint =keypoints[index_extrema];

            VecF64 profile = pyramid_G.getProfile(keypoint.x());
            Mat2F64 m(profile.size(),1);
            m.setCol(0,profile);
            Descriptor<TKeyPoint>  descriptor;
            descriptor.keyPoint() = keypoint;
            descriptor.orientation() = 0;
            descriptor.data() = m;
            descriptors.push_back(descriptor);
        }
        return descriptors;
    }

    template<int DIM,typename Type>
    Vec<KeyPointPyramid<DIM> > _keyPointSIFT(const Pyramid<DIM,Type> & pyramid_gaussian,double threshold_low_contrast=0.005,double ratio_edge_response=10)
    {
        Pyramid<DIM,F64>         pyramid_difference=  Feature::pyramidDifference(pyramid_gaussian);
        Vec<KeyPointPyramid<DIM> > extrema   =   Feature::pyramidExtrema(pyramid_difference);
        Vec<KeyPointPyramid<DIM> > extrema2  =   Feature::pyramidExtremaAdjust(pyramid_difference,extrema);
        Vec<KeyPointPyramid<DIM> > extrema3  =   Feature::pyramidExtremaThresholdLowContrast(pyramid_difference,extrema2,threshold_low_contrast);
        return extrema2;
    }
    struct DistanceOctave
    {
        double _alpha;
        double _alpha2;
        DistanceOctave(double alpha=0.1,double alpha2=0.005)
            :_alpha(alpha),_alpha2(alpha2)
        {

        }


        double operator()(const Descriptor<KeyPointPyramid<2> > & a, const Descriptor<KeyPointPyramid<2> >& b) {
            std::cout<<distance( a, b ,2)<<" "<<_alpha*std::abs(1.0*(a.keyPoint().scale()-b.keyPoint().scale()))<<" " <<_alpha2*(a.keyPoint().x()-b.keyPoint().x()).norm(2)*(1./a.keyPoint().scale()+1./b.keyPoint().scale())<<std::endl;
            return distance( a, b ,2)
                    +_alpha*std::abs(1.0*(a.keyPoint().scale()-b.keyPoint().scale()))
                    + _alpha2*(a.keyPoint().x()-b.keyPoint().x()).norm()*(1./a.keyPoint().scale()+1./b.keyPoint().scale());
            ;
        }
    };

    template<int DIM,typename TDescriptor>
    static void  drawSingleDescriptorArrow( MatN<DIM,RGBUI8> & f,const TDescriptor     & descriptor,int length=20,double sigma=1.6)
    {
        VecN<DIM,F64> x= descriptor.keyPoint().x();
        RGBUI8 color = RGBUI8::randomRGB();
        Draw::circle(f,round(x),sigma*descriptor.keyPoint().scale(),color,1);
        Draw::arrow(f,round(x),round(x)+descriptor.orientation()*length,color,1);
    }
    void letterExtraction2()
    {
        std::string path_linux = "D:/Users/vtariel/Desktop/DATA/test.jpg";
        Mat2UI8 img(path_linux.c_str());
    }


    void letterExtraction()
    {
        std::string path_linux = "D:/Users/vtariel/Desktop/DATA/test.jpg";
        Mat2UI8 img(path_linux.c_str());


        //img = GeometricalTransformation::scale(img,Vec2F64(0.25,0.25));
        double sigma = 1.2;
        //double nbr_layers = 3;
        Pyramid<2,F64> pyramid  = Feature::pyramidGaussian(img,sigma);
        Pyramid<2,F64> pyramiddiff  = Feature::pyramidDifference(pyramid);
        Vec<KeyPointPyramid<2> > keypoints = _keyPointSIFT(pyramid);

        Mat2RGBUI8 temp2(img);
        for(unsigned int i=0;i<keypoints.size();i++){
            keypoints[i]._label = i;
            Draw::circle(temp2,Vec2I32(keypoints[i].x()),2,RGBUI8(0));
            Draw::text(temp2,BasicUtility::Any2String(i),keypoints[i].x(),RGBUI8(255,0,0),1);
        }
        //temp2.display();



        //Feature::drawKeyPointsCircle(img,keypoints,3).display();
        double scale=2;
        Vec<Descriptor<KeyPointPyramid<2> > > descriptor = _descriptorPyramidPieChart(pyramid,keypoints,scale*sigma,1,6);



        Mat2RGBUI8 temp3(img);

        int index1=209;
        int index2=239;
        int index3=74;
        Feature::drawDescriptorArrow(temp3,descriptor,20,scale*sigma).display();
        drawSingleDescriptorArrow(temp3,descriptor[index1],20,scale*sigma);
        drawSingleDescriptorArrow(temp3,descriptor[index2],20,scale*sigma);
        drawSingleDescriptorArrow(temp3,descriptor[index3],20,scale*sigma);
        temp3.display();
        std::cout<<descriptor[index1].orientation()<<std::endl;
        std::cout<<descriptor[index2].orientation()<<std::endl;
        std::cout<<descriptor[index3].orientation()<<std::endl;
        std::cout<<descriptor[index1].data()<<std::endl;
        std::cout<<descriptor[index2].data()<<std::endl;
        std::cout<<descriptor[index3].data()<<std::endl;


        //    VecF64 v1 = pyramiddiff.getProfile(descriptor[index1].x());
        //    VecF64 v2 = pyramiddiff.getProfile(descriptor[index2].x());
        //    VecF64 v3 = pyramiddiff.getProfile(descriptor[index2].x());
        //    for(int i =0;i<v1.size();i++){
        //        std::cout<<v1[i]<<" "<<v2[i]<<" "<<v3[i]<<std::endl;
        //    }
        std::cout<<pop::distance(descriptor[index1],descriptor[index2])<<std::endl;
        std::cout<<pop::distance(descriptor[index1],descriptor[index3])<<std::endl;
        return ;

        VpTree<Descriptor<KeyPointPyramid<2> >,DistanceOctave > c;
        //c.op_dist._norm=2;
        c.create(descriptor);
        //    MatNDisplay disp;
        //    return ;
        GraphAdjencyList<UI8,UI8> graph;
        for(unsigned int i=0;i<descriptor.size();i++){
            int vertex1 = graph.addVertex();
            graph(vertex1)=255;
        }
        for(unsigned int i=0;i<descriptor.size();i++){
            Vec<Descriptor<KeyPointPyramid<2> > > v_target;
            Vec<double > v_distance;
            c.search(descriptor[i],3,v_target,v_distance);
            for(unsigned int k=1;k<v_target.size();k++){
                int edge = graph.addEdge();
                graph.connection(edge,descriptor[i].keyPoint()._label,v_target[k].keyPoint()._label);
            }
        }
        GraphAdjencyList<UI32,UI32> graph_label = Processing::clusterToLabel(graph);



        int max_label = Analysis::maxValue(graph_label);


        RGBUI8 color=RGBUI8::randomRGB();
        MatNDisplay disp2;
        for(int i=0;i<=max_label;i++){

            Mat2RGBUI8 temp(img);
            for(unsigned int j=0;j<graph_label._v_vertex.size();j++){
                if(graph_label(j)==i)
                    Draw::circle(temp,Vec2I32(descriptor[j].x()),4,color);
            }
            disp2.display(temp);//.display("false",false);
            Sleep(1000);
        }

        //
        //
        //    temp= GeometricalTransformation::scale(temp,Vec2F64(3,3));
        //    temp.display("appariement",true,false);
        //    std::sort(v_match.begin(),v_match.end());
        //    return v_match;
        //    KeyPoint key;

        //  Processing::clusterToLabel



        //    Vec<std::pair<int,int> > v_scale;
        //    for(unsigned int index_max=0;index_max<maxima.size();index_max++){
        //        int index_octave_max;
        //        int index_layer_max;
        //        for(unsigned int index_octave=0;index_octave<pyramid_diff.nbrOctave();index_octave++){
        //            const MatN<3,F64> & foctave = pyramid_diff.octave(index_octave);

        //            for(int index_layer=0;index_layer<foctave.getDomain()(2);index_layer++){
        //                Vec3F64 x;
        //                x(0)=maxima[index_max](0)/std::pow(2.,static_cast<double>(index_octave));
        //                x(1)=maxima[index_max](1)/std::pow(2.,static_cast<double>(index_octave));
        //                x(2)=index_layer;
        //                if(foctave.isValid(x)){
        //                    std::cout<<foctave(x)<<std::endl;
        //                }
        //            }
        //        }
        //        v_scale.push_back(std::make_pair(index_octave_max,index_layer_max));
        //        std::cout<<std::endl;
        //    }
    }

};

template<class FunctorRow>
struct Argument
{
    int thread_number;
    FunctorRow *func;

};
template<class FunctorRow>
void worker(void * aArg){
    Argument<FunctorRow> * arg = static_cast<Argument<FunctorRow>*>(aArg);
    arg->func->operator()(arg->thread_number);
}



template<int DIM,typename Type>
struct FunctorRowConvolutionSerarable
{
    typedef typename FunctionTypeTraitsSubstituteF<Type,F64>::Result TypeFloat;
    template<typename Type2>
    struct AccumulateMult
    {
        typename MatN<2,Type2>::const_iterator _it_matrix;
        int _incr;
        typedef typename FunctionTypeTraitsSubstituteF<Type2,F64>::Result TypeFloat;
        TypeFloat operator ()(TypeFloat res, F64 x)
        {
            res += x*(*_it_matrix);
            _it_matrix+=_incr;
            return res;

        }
    };

    int _coordinate;
    VecN<DIM,Vec<F64> > _tab;
    MatN<DIM,Type> _in;
    MatN<DIM,Type> _out;
    int _width;

    int _nbr_thread;
    int _nbr_row_per_thread;
    int _extra_row_last_thread;


    void setThreadNumber(int nbr_thread){
        _nbr_thread = nbr_thread;
    }

    void setInput(const MatN<DIM,Type> & in,const Vec<F64> conv, int boundary_condition = Boundary_Condition_Mirror){
        _width = std::floor(conv.size()/2.);
        _in = in;
        Draw::addBorder(_in,_width,Type(0),boundary_condition);
        _out.resize(_in.getDomain());
        for(int i=0;i<DIM;i++){
            _tab(i)=conv;
        }
    }
    MatN<DIM,Type> getOutput(){
        return _out(Vec2I32(_width,_width),_out.getDomain()-1-Vec2I32(_width,_width));
    }

    void operator ()(int thread_number){
        _nbr_row_per_thread =  std::floor((_in.getDomain()(0)-2*_width)*1.0/_nbr_thread);

        _extra_row_last_thread = _in.getDomain()(0)-2*_width-_nbr_row_per_thread*_nbr_thread;

        int index_i_begin = _width+thread_number*_nbr_row_per_thread;
        int index_i_end =  _width+(thread_number+1)*_nbr_row_per_thread;
        if(thread_number==_nbr_thread-1)
            index_i_end +=_extra_row_last_thread;


//        std::cout<<_in.getDomain()<<std::endl;

        AccumulateMult<Type> _mult;
        _mult._incr=1;

        int _coordinate=1;
        for(int index_i=index_i_begin;index_i<index_i_end;index_i++){
            for(int index_j=_width;index_j<static_cast<int>(_in.sizeJ())-_width;index_j++){
                int global_sift = index_j+ _width  +  index_i*_in.getDomain()(1);
                _mult._it_matrix = _in.begin()+ global_sift;
                _out(index_i,index_j)=std::accumulate(_tab(_coordinate).begin(), _tab(_coordinate).end(), TypeFloat(0.),_mult);
            }
        }
    }
};


class ProcessParallel
{
    int _nbr_thread;
public:
    ProcessParallel(int nbr_thread=-1){
        if(nbr_thread==-1){
            _nbr_thread = tthread::thread::hardware_concurrency ();
        }else
            _nbr_thread = nbr_thread;
    }
    template<typename Functor>
    void operator()(  Functor &f){

        f.setThreadNumber(_nbr_thread);
        std::vector<tthread::thread*> v_thread;
        std::vector< Argument<Functor>*> v_arg;
        for(int i=0;i<_nbr_thread;i++){
            Argument<Functor> *argument = new Argument<Functor>;
            argument->func =&f;
            argument->thread_number=i;
            v_arg.push_back(argument);

            tthread::thread *thread = new tthread::thread(&worker<Functor>,(void*)(argument));  // spawn new thread that calls bar(0)
            v_thread.push_back(thread);
        }
        for(int i=0;i<_nbr_thread;i++){
            v_thread[i]->join();
        }
        for(int i=0;i<_nbr_thread;i++){
            delete v_thread[i];
            delete v_arg[i];
        }
    }
};
void toto(){
    Mat2UI8 m(20000,10000);
//    m.load("../Lena.bmp");
    VecF64 tab(21);
    tab(0)=0.1;tab(1)=0.4;tab(2)=0.1;
    FunctorRowConvolutionSerarable<2,UI8> f;
    f.setInput(m,tab);


    ProcessParallel proce(4);
    int time1=time(NULL);
    for(int i=0;i<100;i++){
        std::cout<<i<<std::endl;
        proce.operator ()(f);
        std::cout<<time(NULL)-time1<<std::endl;
    }

    f.getOutput().display();
    return 0;
}



}

#endif // EXPIRMENTAL_H

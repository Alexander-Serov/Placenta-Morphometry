#include "NeuralNetwork.h"

#include"core/data/distribution/DistributionAnalytic.h"

#include"core/data/mat/MatN.h"
#include"core/data/mat/MatNInOut.h"
#include"core/data/mat/MatNDisplay.h"
namespace pop {
#define SIGMOID(x) (1.7159*tanh(0.66666667*x))
#define DSIGMOID(S) (0.66666667/1.7159*(1.7159+(S))*(1.7159-(S)))  // derivative of the sigmoid as a function of the sigmoid's output
///////////////////////////////////////////////////////////////////////
//
//  NeuralNetwork class definition

NeuralNetworkFeedForward::NeuralNetworkFeedForward()
    :_is_input_layer(false)
{
}
NeuralNetworkFeedForward::NeuralNetworkFeedForward(const NeuralNetworkFeedForward& n){
    *this = n;
}
NeuralNetworkFeedForward & NeuralNetworkFeedForward::operator=(const NeuralNetworkFeedForward& n){
    this->_label2string = n._label2string;
    for(unsigned int i=0;i<n._layers.size();i++){
        NNLayer * layer = n._layers[i];
        if(i==0)
        {
            if(layer->_type==NNLayer::INPUTMATRIX){
                NNLayerMatrix* inputlayer = dynamic_cast<NNLayerMatrix*>(layer);
                this->addInputLayerMatrix(inputlayer->_neurons_matrix[0].getDomain()(0),inputlayer->_neurons_matrix[0].getDomain()(1));
            }else{
                this->addInputLayer(layer->_neurons.size());
            }
        }else{
            if(layer->_type==NNLayer::MATRIXCONVOLUTIONNAL){
                NNLayerMatrixConvolutionalPlusSubScaling* inputlayer = dynamic_cast<NNLayerMatrixConvolutionalPlusSubScaling*>(layer);

                this->addLayerConvolutionalPlusSubScaling(inputlayer->_neurons_matrix.getDomain(),inputlayer->_size_convolutution,inputlayer->_sub_scaling);
                NNLayer* my_layer = *(this->_layers.rbegin());
                for(unsigned int index_w=0;index_w<inputlayer->_weights.size();index_w++){
                    NNWeight & weight = layer->_weights[index_w];
                    my_layer->_weights[index_w]._Wn = weight._Wn;
                }
            }else if(layer->_type==NNLayer::MATRIXCONVOLUTIONNAL2){
                NNLayerMatrixConvolutional* inputlayer = dynamic_cast<NNLayerMatrixConvolutional*>(layer);

                this->addLayerConvolutional(inputlayer->_neurons_matrix.getDomain(),inputlayer->_size_convolutution);
                NNLayer* my_layer = *(this->_layers.rbegin());
                for(unsigned int index_w=0;index_w<inputlayer->_weights.size();index_w++){
                    NNWeight & weight = layer->_weights[index_w];
                    my_layer->_weights[index_w]._Wn = weight._Wn;
                }
            }else if(layer->_type==NNLayer::MATRIXSUBSALE){
                NNLayerMatrixSubScale* inputlayer = dynamic_cast<NNLayerMatrixSubScale*>(layer);

                this->addLayerSubScaling(inputlayer->_neurons_matrix.getDomain(),inputlayer->_sub_scaling);
                NNLayer* my_layer = *(this->_layers.rbegin());
                for(unsigned int index_w=0;index_w<inputlayer->_weights.size();index_w++){
                    NNWeight & weight = layer->_weights[index_w];
                    my_layer->_weights[index_w]._Wn = weight._Wn;
                }

            }
            else if(layer->_type==NNLayer::FULLYCONNECTED)
            {
                this->addLayerFullyConnected(layer->_neurons.size());
                NNLayer* my_layer = *(this->_layers.rbegin());
                for(unsigned int index_w=0;index_w<layer->_weights.size();index_w++){
                    NNWeight & weight = layer->_weights[index_w];
                    my_layer->_weights[index_w]._Wn = weight._Wn;
                }
            }
        }
    }
    return *this;
}


void NeuralNetworkFeedForward::propagateFront(const pop::VecF64& in , pop::VecF64 &out)
{
    // first layer is imput layer: directly set outputs of all of its neurons
    // to the input vector
    NNLayer & layerfirst = *(_layers[0]);
    POP_DbgAssert( in.size() == layerfirst._neurons.size() );
    for(unsigned int i_neuron=0;i_neuron<layerfirst._neurons.size();i_neuron++){
        NNNeuron & neuron = (layerfirst._neurons[i_neuron]);
        neuron._Xn =  in(i_neuron );
    }

    // propagate layer by layer
    for(unsigned int i_layer=1;i_layer<_layers.size();i_layer++){
        NNLayer & layer = *(_layers[i_layer]);
        layer.propagateFront();
    }


    // load up output vector with results
    NNLayer & layerout = *(_layers[_layers.size()-1]);
    out.resize(layerout._neurons.size());
    for(unsigned int i_neuron=0;i_neuron<layerout._neurons.size();i_neuron++){
        NNNeuron & neuron = (layerout._neurons[i_neuron]);
        out(i_neuron ) = neuron._Xn;
    }
}
void NeuralNetworkFeedForward::propagateBackFirstDerivate(const pop::VecF64& desired_output)
{
    NNLayer & layerlast = *(_layers[_layers.size()-1]);
    POP_DbgAssert( desired_output.size() == layerlast._neurons.size() );
    for(unsigned int i_neuron=0;i_neuron<layerlast._neurons.size();i_neuron++){
        NNNeuron & neuron = (layerlast._neurons[i_neuron]);
        neuron._dErr_dXn = neuron._Xn -  desired_output(i_neuron);
    }
    for(unsigned int i_layer=_layers.size()-1;i_layer>0;i_layer--){
        NNLayer & layer = *(_layers[i_layer]);
        layer.propagateBackFirstDerivate();
    }
}
void NeuralNetworkFeedForward::propagateBackSecondDerivate()
{
    NNLayer & layerlast = *(_layers[_layers.size()-1]);
    for(unsigned int i_neuron=0;i_neuron<layerlast._neurons.size();i_neuron++){
        NNNeuron & neuron = (layerlast._neurons[i_neuron]);
        neuron._d2Err_dXn2 = 1;
    }
    for(unsigned int i_layer=_layers.size()-1;i_layer>0;i_layer--){
        NNLayer & layer = *(_layers[i_layer]);
        layer.propagateBackSecondDerivate();
    }
}




void NeuralNetworkFeedForward::learningFirstDerivate()
{
    for(unsigned int i_layer=_layers.size()-1;i_layer>0;i_layer--){
        NNLayer & layer = *(_layers[i_layer]);
        layer.learningFirstDerivate();
    }
}

void NeuralNetworkFeedForward::addInputLayer(int nbr_neuron){
    POP_DbgAssertMessage(_is_input_layer==false,"Add an input neuron before to add weighed layer");
    _is_input_layer = true;
    _layers.push_back( new NNLayer(nbr_neuron) );
    NNLayer & layer = *(_layers[_layers.size()-1]);
    layer._type = NNLayer::INPUT;

}

void NeuralNetworkFeedForward::addInputLayerMatrix(unsigned int height,unsigned int width){
    POP_DbgAssertMessage(_is_input_layer==false,"Add an input neuron before to add weighed layer");
    _is_input_layer = true;
    _layers.push_back( new NNLayerMatrix(1,height,width) );
    NNLayer & layer = *(_layers[_layers.size()-1]);
    layer._type = NNLayer::INPUTMATRIX;
}




void NeuralNetworkFeedForward::addLayerFullyConnected(unsigned int nbr_neuron,double standart_deviation_weight){

    NNLayer & layerprevious = *(_layers[_layers.size()-1]);
    _layers.push_back( new NNLayer(nbr_neuron) );
    NNLayer & layer = *(_layers[_layers.size()-1]);
    layer._type = NNLayer::FULLYCONNECTED;
    // This layer is a fully-connected layer
    // with nbr_neuron units.  Since it is fully-connected,
    // each of the nbr_neuron neurons in the layer
    // is connected to all nbr_neuron_previous_layer in
    // the previous layer.
    // So, there are nbr_neuron neurons and nbr_neuron*(nbr_neuron_previous_layer+1) weights
    //    pop::DistributionUniformInt d(0,1);


    //normalize tbe number inverse square root of the connection feeding into the nodes)
    pop::DistributionNormal d(0,standart_deviation_weight/std::sqrt(layerprevious._neurons.size()*1.0));
    unsigned int nbr_neuron_previous_layer = layerprevious._neurons.size();
    unsigned int nbr_weight = nbr_neuron*(nbr_neuron_previous_layer+1);
    for (unsigned int  i_weight=0; i_weight<nbr_weight; ++i_weight )
    {
        double initweight =  d.randomVariable();
        layer._weights.push_back( NNWeight(initweight ) );
    }

    // Interconnections with previous layer: fully-connected

    unsigned int i_weight = 0;  // weights are not shared in this layer
    for (unsigned int  i_neuron=0; i_neuron<nbr_neuron; ++i_neuron )
    {
        NNNeuron& n = (layer._neurons[ i_neuron ]) ;
        n.addConnectionBiais(&layer._weights[i_weight] );  // bias weight
        i_weight++;
        for (unsigned int  i_neuron_previous_layer=0; i_neuron_previous_layer<nbr_neuron_previous_layer; ++i_neuron_previous_layer )
        {
            NNNeuron& nprevious = layerprevious._neurons[ i_neuron_previous_layer ] ;
            n.addConnection(&layer._weights[i_weight], &nprevious );
            i_weight++;
        }
    }


}
void NeuralNetworkFeedForward::addLayerConvolutional( unsigned int nbr_map, unsigned int kernelsize,double standart_deviation_weight){
    if(NNLayerMatrix * layerprevious =dynamic_cast<NNLayerMatrix *>(_layers[_layers.size()-1])){
        unsigned int height_previous = layerprevious->_neurons_matrix(0).getDomain()(0);
        unsigned int width_previous  = layerprevious->_neurons_matrix(0).getDomain()(1);


        unsigned int height = (height_previous-(kernelsize-1));
        unsigned int width  = (width_previous-(kernelsize-1));
        std::cout<<width<<std::endl;
        std::cout<<height<<std::endl;
        NNLayerMatrixConvolutional * layer = new NNLayerMatrixConvolutional(nbr_map,height,width);
        layer->_size_convolutution = kernelsize;

        layer->_type = NNLayer::MATRIXCONVOLUTIONNAL2;
        _layers.push_back(layer);

        // This layer is a convolutional connected layer
        // with nbr_neuron units.
        // (kernelsize*kernelsize+1)* nbr_map_previous_layer*nbr_map_layer
        unsigned int nbr_map_previous = layerprevious->_neurons_matrix.size();


        //normalize tbe number inverse square root of the connection feeding into the nodes)
        pop::DistributionNormal d(0,standart_deviation_weight/kernelsize);

        unsigned int nbr_weight = nbr_map*nbr_map_previous*(kernelsize*kernelsize+1);
        for (unsigned int  i_weight=0; i_weight<nbr_weight; ++i_weight )
        {
            double initweight =  d.randomVariable();
            layer->_weights.push_back(NNWeight(initweight ) );
        }



        for(unsigned int i_width=0;i_width<width;i_width++)
        {
            for(unsigned int i_height=0;i_height<height;i_height++)
            {
                for(unsigned int i_pattern_previous = 0;i_pattern_previous<nbr_map_previous;i_pattern_previous++)
                {
                    for(unsigned int i_pattern = 0;i_pattern<nbr_map;i_pattern++)
                    {

                        NNNeuron& n = *(layer->_neurons_matrix(i_pattern)(i_height,i_width)) ;
                        unsigned int i_weight_begin = i_pattern*(kernelsize*kernelsize+1)+i_pattern_previous*nbr_map*(kernelsize*kernelsize+1);
                        n.addConnectionBiais(&layer->_weights[i_weight_begin]);
                        for(unsigned int i_heigh_kernel=0;i_heigh_kernel<kernelsize;i_heigh_kernel++)
                        {
                            for(unsigned int i_width_kernel=0;i_width_kernel<kernelsize;i_width_kernel++)
                            {

                                int i_width_previous =  i_width + i_width_kernel ;
                                int i_height_previous =  i_height+ i_heigh_kernel;

                                NNNeuron& nprevious = *(layerprevious->_neurons_matrix(i_pattern_previous)(i_height_previous,i_width_previous)) ;
                                unsigned int i_weight = 1+i_width_kernel + i_heigh_kernel*kernelsize + i_weight_begin;
                                n.addConnection(&layer->_weights[i_weight], &nprevious );
                            }

                        }
                    }

                }

            }
        }
    }
}
void NeuralNetworkFeedForward::addLayerIntegral(unsigned int nbr_integral,double standart_deviation_weight){
    if(NNLayerMatrix * layerprevious =dynamic_cast<NNLayerMatrix *>(_layers[_layers.size()-1])){
        unsigned int height_previous = layerprevious->_neurons_matrix(0).getDomain()(0);
        unsigned int width_previous  = layerprevious->_neurons_matrix(0).getDomain()(1);
        unsigned int nbr_map_previous = layerprevious->_neurons_matrix.size();



        NNLayer * layer = new NNLayer(nbr_integral*nbr_map_previous);



        _layers.push_back(layer);

        //normalize tbe number inverse square root of the connection feeding into the nodes)
        pop::DistributionNormal d(0,standart_deviation_weight/std::sqrt(height_previous*width_previous));

        unsigned int nbr_weight = nbr_integral*(height_previous*width_previous+1);
        for (unsigned int  i_weight=0; i_weight<nbr_weight; ++i_weight )
        {
            double initweight =  d.randomVariable();
            layer->_weights.push_back(NNWeight(initweight ) );
        }

        for(unsigned int i_integral = 0;i_integral<nbr_integral;i_integral++)
        {
            for(unsigned int i_pattern_previous = 0;i_pattern_previous<nbr_map_previous;i_pattern_previous++)
            {
                unsigned int i_weight_begin = i_integral*(height_previous*width_previous+1);
                NNNeuron& n = (layer->_neurons[i_pattern_previous+i_integral*nbr_map_previous ]) ;
                n.addConnectionBiais(&layer->_weights[i_weight_begin]);
                for(unsigned int i_height=0;i_height<height_previous;i_height++)
                {
                    for(unsigned int i_width=0;i_width<width_previous;i_width++)
                    {
                        NNNeuron& nprevious = *(layerprevious->_neurons_matrix(i_pattern_previous)(i_height,i_width)) ;

                        unsigned int i_weight = 1+i_width + i_height*width_previous + i_weight_begin;
                        n.addConnection(&layer->_weights[i_weight], &nprevious );
                    }

                }
            }

        }

    }
}

void NeuralNetworkFeedForward::addLayerSubScaling(unsigned int sub_scale_factor,double standart_deviation_weight){
    if(NNLayerMatrix * layerprevious =dynamic_cast<NNLayerMatrix *>(_layers[_layers.size()-1])){
        unsigned int height_previous = layerprevious->_neurons_matrix(0).getDomain()(0);
        unsigned int width_previous  = layerprevious->_neurons_matrix(0).getDomain()(1);
        unsigned int nbr_map_previous = layerprevious->_neurons_matrix.size();

        unsigned int height = (height_previous/sub_scale_factor);
        unsigned int width  = (width_previous/sub_scale_factor);

        NNLayerMatrixSubScale * layer = new NNLayerMatrixSubScale(nbr_map_previous,height,width);
        layer->_sub_scaling = sub_scale_factor;

        layer->_type = NNLayer::MATRIXSUBSALE;
        _layers.push_back(layer);




        //normalize tbe number inverse square root of the connection feeding into the nodes)
        pop::DistributionNormal d(0,standart_deviation_weight/sub_scale_factor);

        unsigned int nbr_weight = nbr_map_previous*height*width*(sub_scale_factor*sub_scale_factor+1);
        for (unsigned int  i_weight=0; i_weight<nbr_weight; ++i_weight )
        {
            double initweight =  d.randomVariable();
            layer->_weights.push_back(NNWeight(initweight ) );
        }




        for(unsigned int i_pattern_previous = 0;i_pattern_previous<nbr_map_previous;i_pattern_previous++)
        {
            for(unsigned int i_height=0;i_height<height;i_height++)
            {
                for(unsigned int i_width=0;i_width<width;i_width++)
                {

                    NNNeuron& n = *(layer->_neurons_matrix(i_pattern_previous)(i_height,i_width)) ;
                    unsigned int i_weight_begin =i_pattern_previous*width*height*(sub_scale_factor*sub_scale_factor+1) +  (i_width + i_height*width)*(sub_scale_factor*sub_scale_factor+1) ;
                    n.addConnectionBiais(&layer->_weights[i_weight_begin]);
                    for(unsigned int i_heigh_kernel=0;i_heigh_kernel<sub_scale_factor;i_heigh_kernel++)
                    {
                        for(unsigned int i_width_kernel=0;i_width_kernel<sub_scale_factor;i_width_kernel++)
                        {
                            int i_width_previous =  i_width*sub_scale_factor + i_width_kernel ;
                            int i_height_previous = i_height*sub_scale_factor+ i_heigh_kernel;

                            NNNeuron& nprevious = *(layerprevious->_neurons_matrix(i_pattern_previous)(i_height_previous,i_width_previous)) ;
                            unsigned int i_weight =i_weight_begin+     1+i_width_kernel + i_heigh_kernel*sub_scale_factor ;
                            n.addConnection(&layer->_weights[i_weight], &nprevious );
                        }

                    }
                }

            }

        }
    }
}


void NeuralNetworkFeedForward::addLayerConvolutionalPlusSubScaling( unsigned int nbr_map, unsigned int kernelsize,unsigned int sub_scale_sampling,double standart_deviation_weight){
    if(NNLayerMatrix * layerprevious =dynamic_cast<NNLayerMatrix *>(_layers[_layers.size()-1])){


        unsigned int step_previous = (kernelsize-1)/2;
        unsigned int height_previous = layerprevious->_neurons_matrix(0).getDomain()(0);
        unsigned int width_previous  = layerprevious->_neurons_matrix(0).getDomain()(1);

        if ((height_previous-(kernelsize-sub_scale_sampling)) % sub_scale_sampling!=0) {
            std::cerr<<"The heigh of the input matrix must be pair "<<std::endl;
        }


        unsigned int height = (height_previous-(kernelsize-sub_scale_sampling))/sub_scale_sampling;
        unsigned int width  = (width_previous-(kernelsize-sub_scale_sampling))/sub_scale_sampling;
        std::cout<<(height_previous-(kernelsize-1)/2)<<std::endl;
        std::cout<<height<<std::endl;
        NNLayerMatrixConvolutionalPlusSubScaling * layer = new NNLayerMatrixConvolutionalPlusSubScaling(nbr_map,height,width);
        layer->_size_convolutution = kernelsize;
        layer->_sub_scaling = sub_scale_sampling;
        layer->_type = NNLayer::MATRIXCONVOLUTIONNAL;
        _layers.push_back(layer);

        // This layer is a convolutional connected layer
        // with nbr_neuron units.
        // (kernelsize*kernelsize+1)* nbr_map_previous_layer*nbr_map_layer
        unsigned int nbr_map_previous = layerprevious->_neurons_matrix.size();


        //normalize tbe number inverse square root of the connection feeding into the nodes)
        pop::DistributionNormal d(0,standart_deviation_weight/kernelsize);

        unsigned int nbr_weight = nbr_map*nbr_map_previous*(kernelsize*kernelsize+1);
        for (unsigned int  i_weight=0; i_weight<nbr_weight; ++i_weight )
        {
            double initweight =  d.randomVariable();
            layer->_weights.push_back(NNWeight(initweight ) );
        }



        for(unsigned int i_width=0;i_width<width;i_width++)
        {
            for(unsigned int i_height=0;i_height<height;i_height++)
            {
                for(unsigned int i_pattern_previous = 0;i_pattern_previous<nbr_map_previous;i_pattern_previous++)
                {
                    for(unsigned int i_pattern = 0;i_pattern<nbr_map;i_pattern++)
                    {

                        NNNeuron& n = *(layer->_neurons_matrix(i_pattern)(i_height,i_width)) ;
                        unsigned int i_weight = i_pattern*(kernelsize*kernelsize+1)+i_pattern_previous*nbr_map*(kernelsize*kernelsize+1);
                        n.addConnectionBiais(&layer->_weights[i_weight]);
                        for(unsigned int i_width_kernel=0;i_width_kernel<kernelsize;i_width_kernel++)
                        {
                            for(unsigned int i_heigh_kernel=0;i_heigh_kernel<kernelsize;i_heigh_kernel++)
                            {

                                int i_width_previous =  i_width*2 + step_previous + i_width_kernel - step_previous;
                                int i_height_previous =  i_height*2 + step_previous + i_heigh_kernel - step_previous;

                                NNNeuron& nprevious = *(layerprevious->_neurons_matrix(i_pattern_previous)(i_height_previous,i_width_previous)) ;
                                unsigned int i_weight = 1+i_width_kernel + i_heigh_kernel*kernelsize + i_pattern*(kernelsize*kernelsize+1)+i_pattern_previous*nbr_map*(kernelsize*kernelsize+1);
                                n.addConnection(&layer->_weights[i_weight], &nprevious );
                            }

                        }
                    }

                }

            }
        }
    }
}
NeuralNetworkFeedForward::~NeuralNetworkFeedForward()
{
    init();
}

Vec<std::string>& NeuralNetworkFeedForward::label2String(){
    return _label2string;
}
const Vec<std::string>& NeuralNetworkFeedForward::label2String()const{
    return _label2string;
}
Vec<NNLayer*>& NeuralNetworkFeedForward::layers(){
    return _layers;
}

const Vec<NNLayer*>& NeuralNetworkFeedForward::layers()const{
    return _layers;
}

void NeuralNetworkFeedForward::init(){
    for(unsigned int i=0;i<_layers.size();i++){
        delete _layers[i];
    }
    _layers.clear();
}
void NeuralNetworkFeedForward::setLearningRate(double eta)
{
    for(unsigned int i_layer=0;i_layer<_layers.size();i_layer++)
    {
        NNLayer & layer = *(_layers[i_layer]);
        layer.setLearningRate(eta);
    }
}

void NeuralNetworkFeedForward::save(const char * file)const
{
    XMLDocument doc;

    XMLNode node1 = doc.addChild("label2String");
    node1.addAttribute("id",BasicUtility::Any2String(_label2string));
    XMLNode node = doc.addChild("layers");
    for(unsigned int i=0;i<this->_layers.size();i++){
        NNLayer * layer = this->_layers[i];
        if(i==0)
        {
            if(layer->_type==NNLayer::INPUTMATRIX){
                NNLayerMatrix* inputlayer = dynamic_cast<NNLayerMatrix*>(layer);
                XMLNode nodechild = node.addChild("layer");
                nodechild.addAttribute("type","NNLayer::INPUTMATRIX");
                nodechild.addAttribute("size",BasicUtility::Any2String(inputlayer->_neurons_matrix[0].getDomain()));
            }else{

                XMLNode nodechild = node.addChild("layer");
                nodechild.addAttribute("type","NNLayer::INPUT");
                nodechild.addAttribute("size",BasicUtility::Any2String(layer->_neurons.size()));
            }
        }else{
            if(layer->_type==NNLayer::MATRIXCONVOLUTIONNAL){
                NNLayerMatrixConvolutionalPlusSubScaling* inputlayer = dynamic_cast<NNLayerMatrixConvolutionalPlusSubScaling*>(layer);
                XMLNode nodechild = node.addChild("layer");
                nodechild.addAttribute("type","NNLayer::MATRIXCONVOLUTIONNAL");
                nodechild.addAttribute("nbrpattern",BasicUtility::Any2String(inputlayer->_neurons_matrix.getDomain()));
                nodechild.addAttribute("sizekernel",BasicUtility::Any2String(inputlayer->_size_convolutution));
                nodechild.addAttribute("subsampling",BasicUtility::Any2String(inputlayer->_sub_scaling));

                std::string weight_str;
                for(unsigned int index_w=0;index_w<inputlayer->_weights.size();index_w++){
                    NNWeight & weight = inputlayer->_weights[index_w];
                    weight_str+=BasicUtility::Any2String(weight._Wn)+";";
                }
                nodechild.addAttribute("weight",weight_str);
            }else if(layer->_type==NNLayer::MATRIXCONVOLUTIONNAL2){
                NNLayerMatrixConvolutional* inputlayer = dynamic_cast<NNLayerMatrixConvolutional*>(layer);
                XMLNode nodechild = node.addChild("layer");
                nodechild.addAttribute("type","NNLayer::MATRIXCONVOLUTIONNAL2");
                nodechild.addAttribute("nbrpattern",BasicUtility::Any2String(inputlayer->_neurons_matrix.getDomain()));
                nodechild.addAttribute("sizekernel",BasicUtility::Any2String(inputlayer->_size_convolutution));

                std::string weight_str;
                for(unsigned int index_w=0;index_w<inputlayer->_weights.size();index_w++){
                    NNWeight & weight = inputlayer->_weights[index_w];
                    weight_str+=BasicUtility::Any2String(weight._Wn)+";";
                }
                nodechild.addAttribute("weight",weight_str);
            }else if(layer->_type==NNLayer::MATRIXSUBSALE){
                NNLayerMatrixSubScale* inputlayer = dynamic_cast<NNLayerMatrixSubScale*>(layer);
                XMLNode nodechild = node.addChild("layer");
                nodechild.addAttribute("type","NNLayer::MATRIXSUBSALE");
                nodechild.addAttribute("nbrpattern",BasicUtility::Any2String(inputlayer->_neurons_matrix.getDomain()));
                nodechild.addAttribute("subsampling",BasicUtility::Any2String(inputlayer->_sub_scaling));
                std::string weight_str;
                for(unsigned int index_w=0;index_w<inputlayer->_weights.size();index_w++){
                    NNWeight & weight = inputlayer->_weights[index_w];
                    weight_str+=BasicUtility::Any2String(weight._Wn)+";";
                }
                nodechild.addAttribute("weight",weight_str);
            }
            else if(layer->_type==NNLayer::FULLYCONNECTED)
            {
                XMLNode nodechild = node.addChild("layer");
                nodechild.addAttribute("type","NNLayer::FULLYCONNECTED");
                nodechild.addAttribute("size",BasicUtility::Any2String(layer->_neurons.size()));

                std::string weight_str;
                for(unsigned int index_w=0;index_w<layer->_weights.size();index_w++){
                    NNWeight & weight = layer->_weights[index_w];
                    weight_str+=BasicUtility::Any2String(weight._Wn)+";";
                }
                nodechild.addAttribute("weight",weight_str);
            }
        }
    }
    doc.save(file);

}

void NeuralNetworkFeedForward::load(const char * file)
{
    XMLDocument doc;
    doc.load(file);
    load(doc);
}
void NeuralNetworkFeedForward::load(XMLDocument &doc)
{
    this->init();
    XMLNode node1 = doc.getChild("label2String");
    std::string type1 = node1.getAttribute("id");
    BasicUtility::String2Any(type1,_label2string);
    XMLNode node = doc.getChild("layers");
    int i=0;
    for (XMLNode tool = node.firstChild(); tool; tool = tool.nextSibling(),++i)
    {
        std::string type = tool.getAttribute("type");
        if(i==0)
        {
            if(type=="NNLayer::INPUTMATRIX"){
                Vec2I32 domain;
                BasicUtility::String2Any(tool.getAttribute("size"),domain);
                this->addInputLayerMatrix(domain(0),domain(1));
            }else{
                int domain;
                BasicUtility::String2Any(tool.getAttribute("size"),domain);
                this->addInputLayer(domain);
            }
        }
        else
        {
            if(type=="NNLayer::MATRIXCONVOLUTIONNAL"){

                std::string str = tool.getAttribute("nbrpattern");
                int nbr_map;
                BasicUtility::String2Any(str,nbr_map);

                str = tool.getAttribute("sizekernel");
                int sizekernel;
                BasicUtility::String2Any(str,sizekernel);

                str = tool.getAttribute("subsampling");
                int subsampling;
                BasicUtility::String2Any(str,subsampling);

                this->addLayerConvolutionalPlusSubScaling(nbr_map,sizekernel,subsampling);

                str = tool.getAttribute("weight");
                NNLayer* layer = *(this->_layers.rbegin());
                std::istringstream stream(str);
                for(unsigned int i=0;i<layer->_weights.size();i++){
                    double weight ;
                    str = pop::BasicUtility::getline( stream, ";" );
                    pop::BasicUtility::String2Any(str,weight );
                    layer->_weights[i]._Wn = weight;
                }
            }else if(type=="NNLayer::MATRIXCONVOLUTIONNAL2"){

                std::string str = tool.getAttribute("nbrpattern");
                int nbr_map;
                BasicUtility::String2Any(str,nbr_map);

                str = tool.getAttribute("sizekernel");
                int sizekernel;
                BasicUtility::String2Any(str,sizekernel);

                this->addLayerConvolutional(nbr_map,sizekernel);

                str = tool.getAttribute("weight");
                NNLayer* layer = *(this->_layers.rbegin());
                std::istringstream stream(str);
                for(unsigned int i=0;i<layer->_weights.size();i++){
                    double weight ;
                    str = pop::BasicUtility::getline( stream, ";" );
                    pop::BasicUtility::String2Any(str,weight );
                    layer->_weights[i]._Wn = weight;
                }
            }else if(type=="NNLayer::MATRIXSUBSALE"){

                std::string str = tool.getAttribute("nbrpattern");
                int nbr_map;
                BasicUtility::String2Any(str,nbr_map);


                str = tool.getAttribute("subsampling");
                int subsampling;
                BasicUtility::String2Any(str,subsampling);

                this->addLayerSubScaling(subsampling);

                str = tool.getAttribute("weight");
                NNLayer* layer = *(this->_layers.rbegin());
                std::istringstream stream(str);
                for(unsigned int i=0;i<layer->_weights.size();i++){
                    double weight ;
                    str = pop::BasicUtility::getline( stream, ";" );
                    pop::BasicUtility::String2Any(str,weight );
                    layer->_weights[i]._Wn = weight;
                }
            }
            else if(type=="NNLayer::FULLYCONNECTED")
            {
                std::string str = tool.getAttribute("size");
                int size;
                BasicUtility::String2Any(str,size);
                this->addLayerFullyConnected(size);

                str = tool.getAttribute("weight");
                NNLayer* layer = *(this->_layers.rbegin());
                std::istringstream stream(str);
                for(unsigned int i=0;i<layer->_weights.size();i++){
                    double weight ;
                    str = pop::BasicUtility::getline( stream, ";" );
                    pop::BasicUtility::String2Any(str,weight );
                    layer->_weights[i]._Wn = weight;
                }
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////
//
//  NNLayerMatrix class definition


NNLayerMatrix::NNLayerMatrix(unsigned int nbr_map,unsigned int height,unsigned int width )
    :NNLayer(nbr_map*height*width)
{
    _neurons_matrix.resize(nbr_map,MatN<2,NNNeuron*>(Vec2I32(height,width)));
    for (unsigned int i_pattern=0; i_pattern<nbr_map; ++i_pattern )
    {

        for (unsigned int i_width=0; i_width<width; ++i_width )
        {
            for (unsigned int i_height=0; i_height<height; ++i_height )
            {
                _neurons_matrix(i_pattern)(i_height,i_width)=&this->_neurons[i_width + i_height*width+i_pattern*width*height ];
            }
        }
    }
}
NNLayerMatrixConvolutionalPlusSubScaling::NNLayerMatrixConvolutionalPlusSubScaling(unsigned int nbr_map,unsigned int height,unsigned int width )
    :NNLayerMatrix(nbr_map,height, width )
{
}

NNLayerMatrixConvolutional::NNLayerMatrixConvolutional(unsigned int nbr_map,unsigned int height,unsigned int width )
    :NNLayerMatrix(nbr_map,height, width )
{
}
NNLayerMatrixSubScale::NNLayerMatrixSubScale(unsigned int nbr_map,unsigned int height,unsigned int width )
    :NNLayerMatrix(nbr_map,height, width )
{
}

void NNLayerMatrix::setLearningRate(double eta){
    if(this->_type==MATRIXCONVOLUTIONNAL){
        for(unsigned int i=0;i<_weights.size();i++)
        {
            _weights[i]._eta = eta/std::sqrt(_neurons_matrix(0).getDomain()(0)*_neurons_matrix(0).getDomain()(1)*1.);
        }
    }else{
        NNLayer::setLearningRate(eta);
    }

}

///////////////////////////////////////////////////////////////////////
//
//  NNLayer class definition


NNLayer::NNLayer(unsigned int nbr_neuron )
{   
    for (unsigned int i_index=0; i_index<nbr_neuron; ++i_index ){
        _neurons.push_back( NNNeuron() );
    }
}
NNLayer::~NNLayer(){
}



void NNLayer::setLearningRate(double eta)
{
    for(unsigned int i=0;i<_weights.size();i++){
        _weights[i]._eta = eta;
    }
}
void NNLayer::propagateFront()
{
    for( unsigned int i_neuron =0;i_neuron< _neurons.size(); i_neuron++ )
    {
        NNNeuron& n =(_neurons[i_neuron]);
        n.propagateFront();
    }
}
void NNLayer::initPropagateBackFirstDerivate(){
    for( unsigned int i_neuron =0;i_neuron< _neurons.size(); i_neuron++ )
    {
        NNNeuron& n =(_neurons[i_neuron]);
        n.initPropagateBackFirstDerivate();
    }
}

void NNLayer::propagateBackFirstDerivate()
{
    initPropagateBackFirstDerivate();
    for( unsigned int i_neuron =0;i_neuron< _neurons.size(); i_neuron++ )
    {
        NNNeuron& n =(_neurons[i_neuron]);
        n.propagateBackFirstDerivate();
    }
}
void NNLayer::initPropagateBackSecondDerivate(){
    for( unsigned int i_neuron =0;i_neuron< _neurons.size(); i_neuron++ )
    {
        NNNeuron& n =(_neurons[i_neuron]);
        n.initPropagateBackSecondDerivate();
    }
}



void NNLayer::propagateBackSecondDerivate()
{
    initPropagateBackSecondDerivate();
    for( unsigned int i_neuron =0;i_neuron< _neurons.size(); i_neuron++ )
    {
        NNNeuron& n =(_neurons[i_neuron]);
        n.propagateBackSecondDerivate();
    }
}


void NNLayer::learningFirstDerivate()
{
    for( unsigned int i_weight =0;i_weight< _weights.size(); i_weight++ )
    {
        NNWeight & w =(_weights[i_weight]);
        w.learningFirstDerivate();
    }
}

///////////////////////////////////////////////////////////////////////
//
//  NNWeight


NNWeight::NNWeight(  double Wn, double eta) :
    _Wn( Wn ),_eta(eta)
{
}
void NNWeight::learningFirstDerivate()
{
    _Wn = _Wn-_eta*_dE_dWn;
}

///////////////////////////////////////////////////////////////////////
//
//  NNNeuron


NNNeuron::NNNeuron(ActivationFunction f_act) :
    _f_act(f_act), _Xn( 0.0 )
{
}


void NNNeuron::addConnection(NNWeight* weight , NNNeuron* neuron)
{
    _connections.push_back( NNConnection( weight, neuron ) );
}

void NNNeuron::addConnectionBiais(NNWeight* weight)
{
    _connections.push_back( NNConnection( weight ) );
}
void NNNeuron::propagateFront(){

    _Yn=0;
    for ( unsigned int i_connection =0;i_connection< _connections.size(); i_connection++  )
    {
        NNConnection& c = (_connections[i_connection]);
        if(c.isBiais()==false)
        {
            double weight = c._weight->_Wn;
            double neuron_out_previous = c._neuron->_Xn;
            _Yn += weight*neuron_out_previous;
        }
        else
        {
            _Yn += c._weight->_Wn;
        }
    }
    switch ( _f_act) {
    case SigmoidFunction:
        _Xn = SIGMOID( _Yn );
        break;
    case IdentityFunction:
        _Xn = _Yn;
        break;
    default:
        _Xn = SIGMOID( _Yn );
        POP_DbgAssertMessage(false,"error not known function");
        break;
    }
}
void NNNeuron::initPropagateBackFirstDerivate(){
    _dErr_dYn=0;
    for ( unsigned int i_connection =0;i_connection< _connections.size(); i_connection++  )
    {
        NNConnection& c = (_connections[i_connection]);
        c._weight->_dE_dWn =0;
        if(c.isBiais()==false)
        {
            c._neuron->_dErr_dXn=0;
        }
    }
}
void NNNeuron::initPropagateBackSecondDerivate(){
    _d2Err_dYn2=0;
    for ( unsigned int i_connection =0;i_connection< _connections.size(); i_connection++  )
    {
        NNConnection& c = (_connections[i_connection]);
        c._weight->_d2E_dWn2 =0;
        if(c.isBiais()==false)
        {
            c._neuron->_d2Err_dXn2=0;
        }
    }
}

void NNNeuron::propagateBackFirstDerivate(){

    double fprime_Y;
    switch ( _f_act) {
    case SigmoidFunction:
        fprime_Y = DSIGMOID( _Xn );
        break;
    case IdentityFunction:
        fprime_Y = 1;
        break;
    default:
        fprime_Y =  DSIGMOID( _Xn );
        POP_DbgAssertMessage(false,"error not known function");
        break;
    }

    _dErr_dYn = fprime_Y*_dErr_dXn;
    for ( unsigned int i_connection =0;i_connection< _connections.size(); i_connection++  )
    {
        NNConnection& c = (_connections[i_connection]);
        double Xnm1;
        if(c.isBiais()==false)
        {
            Xnm1  = c._neuron->_Xn;
        }
        else
        {
            Xnm1  =1 ;
        }
        c._weight->_dE_dWn += Xnm1*_dErr_dYn;
        if(c.isBiais()==false)
        {

            c._neuron->_dErr_dXn+= c._weight->_Wn*_dErr_dYn;
        }
    }
}
void NNNeuron::propagateBackSecondDerivate(){

    double fprime_Y;
    switch ( _f_act) {
    case SigmoidFunction:
        fprime_Y = DSIGMOID( _Xn );
        break;
    case IdentityFunction:
        fprime_Y = 1;
        break;
    default:
        fprime_Y =  DSIGMOID( _Xn );
        POP_DbgAssertMessage(false,"error not known function");
        break;
    }

    _d2Err_dYn2 = fprime_Y*fprime_Y*_d2Err_dXn2;
    for ( unsigned int i_connection =0;i_connection< _connections.size(); i_connection++  )
    {
        NNConnection& c = (_connections[i_connection]);
        double Xnm1;
        if(c.isBiais()==false)
        {
            Xnm1  = c._neuron->_Xn;
        }
        else
        {
            Xnm1  =1 ;
        }
        c._weight->_d2E_dWn2 += Xnm1*Xnm1*_d2Err_dYn2;
        if(c.isBiais()==false)
        {
            c._neuron->_d2Err_dXn2+= c._weight->_Wn*c._weight->_Wn*_d2Err_dYn2;
        }
    }
}

NNConnection::NNConnection(NNWeight* weight , NNNeuron* neuron)
    :_weight(weight),_neuron(neuron)
{

}

bool NNConnection::isBiais()const{
    if(_neuron==NULL)
        return true;
    else
        return false;
}



void TrainingNeuralNetwork::neuralNetworkForRecognitionForHandwrittenDigits(std::string path, NeuralNetworkFeedForward &n)
{
    // from http://cis.jhu.edu/~sachin/digit/digit.html
    path+="/data";
    Vec<Vec<Mat2UI8> > numbers;
    for(unsigned int i=0;i<10;i++){
        Mat3UI8 number;
#if Pop_OS==2
        number.loadRaw(std::string(path+BasicUtility::Any2String(i)+".txt").c_str(),Vec3I32(28,28,1000));
#else
        number.loadRaw(std::string(path+BasicUtility::Any2String(i)).c_str(),Vec3I32(28,28,1000));
#endif
        Vec<Mat2UI8> v_number;
        for( int j=0;j<number.getDomain()(2);j++)
            v_number.push_back(number.getPlane(2,j));

        std::random_shuffle(v_number.begin(),v_number.end());
        numbers.push_back(v_number);
    }
    //lenet5
    //    Vec2I32 domain(28,28);
    //    n.load("neuralnetwork.xml");
    //    n.addInputLayerMatrix(domain(0),domain(1));
    //    n.addLayerConvolutional2(6,5,1);
    //    n.addLayerSubScaling(2);
    //    n.addLayerConvolutional2(10,5,1);
    //    n.addLayerSubScaling(2);
    //    n.addLayerFullyConnected(100,1);

    //SIMARD network
    Vec2I32 domain(29,29);
    n.addInputLayerMatrix(domain(0),domain(1));
    n.addLayerConvolutionalPlusSubScaling(6,5,2,1);
    n.addLayerConvolutionalPlusSubScaling(50,5,2,1);
    n.addLayerFullyConnected(100,1);

    n.addLayerFullyConnected(numbers.size(),1);

    Vec<std::string> label_digit;
    for(int i=0;i<10;i++)
        label_digit.push_back(BasicUtility::Any2String(i));
    n.label2String() = label_digit;


    double sigma_min =0.12;
    double sigma_max =0.17;
    double alpha =0.11;

    double standard_deviation_angle = PI/6;
    double standard_deviation_shear_j = PI/10;
    DistributionUniformReal dAngle(-standard_deviation_angle,standard_deviation_angle);
    DistributionUniformReal dShear(-standard_deviation_shear_j,standard_deviation_shear_j);

    DistributionUniformReal d_deviation_length(sigma_min,sigma_max);
    DistributionUniformReal d_correlation_lenght(0,alpha);


    //  90% of the supervised data for the training of the neural network
    Vec<VecF64> vtraining_in;
    Vec<VecF64> vtraining_out;
    for(unsigned int i=0;i<numbers.size();i++){
        for(unsigned int j=0;j<numbers(i).size()*0.9;j++){
            Mat2UI8 binary = numbers(i)(j);
            int size_i = binary.sizeI();
            Draw::addBorder(binary,10, UI8(0));
            //            binary.display("base",false);
            // We apply elastic deformation and affine transformation (rotation, shear)
            for(unsigned int k=0;k<20;k++){
                double deviation_length_random = std::abs(d_deviation_length.randomVariable());
                double correlation_lenght_random = std::abs(d_correlation_lenght.randomVariable());
                Mat2UI8 m= GeometricalTransformation::elasticDeformation(binary,deviation_length_random*size_i,correlation_lenght_random*size_i);

                double angle = dAngle.randomVariable();
                double shear = dShear.randomVariable();
                Mat2x33F64 maffine  = GeometricalTransformation::translation2DHomogeneousCoordinate(m.getDomain()/2);//go back to the buttom left corner (origin)
                maffine *=  GeometricalTransformation::shear2DHomogeneousCoordinate(shear,0);
                maffine *= GeometricalTransformation::rotation2DHomogeneousCoordinate(angle);//rotate
                maffine *= GeometricalTransformation::translation2DHomogeneousCoordinate(-m.getDomain()/2);
                m = GeometricalTransformation::transformAffine2D (maffine, m, 1);
                //                m.display();
                VecF64 vin = n.inputMatrixToInputNeuron(m);
                vtraining_in.push_back(vin);
                VecF64 v_out(numbers.size(),-1);
                v_out(i)=1;
                vtraining_out.push_back(v_out);
            }
            VecF64 vin = n.inputMatrixToInputNeuron(binary);
            vtraining_in.push_back(vin);
            VecF64 v_out(numbers.size(),-1);
            v_out(i)=1;
            vtraining_out.push_back(v_out);

        }
    }
    //  10% of the supervised data for the test of the neural network
    Vec<VecF64> vtest_in;
    Vec<VecF64> vtest_out;
    for(unsigned int i=0;i<numbers.size();i++){
        for(unsigned int j=numbers(i).size()*0.9;j<numbers(i).size();j++){
            Mat2UI8 binary = numbers(i)(j);
            VecF64 vin = n.inputMatrixToInputNeuron(binary);
            vtest_in.push_back(vin);
            VecF64 v_out(numbers.size(),-1);
            v_out(i)=1;
            vtest_out.push_back(v_out);
        }
    }
    trainingFirstDerivative(n,vtraining_in,vtraining_out,vtest_in,vtest_out,0.001,50,true);
}
void TrainingNeuralNetwork::trainingFirstDerivative(NeuralNetworkFeedForward&n,const Vec<VecF64>& trainingins,const Vec<VecF64>& trainingouts,double eta,unsigned int nbr_epoch,bool display_error_classification)
{
    n.setLearningRate(eta);
    std::vector<int> v_global_rand(trainingins.size());
    for(unsigned int i=0;i<v_global_rand.size();i++)
        v_global_rand[i]=i;
    std::cout<<"iter_epoch\t error_train"<<std::endl;
    for(unsigned int i=0;i<nbr_epoch;i++){
        std::random_shuffle ( v_global_rand.begin(), v_global_rand.end() ,Distribution::MTRand());
        int error=0;
        for(unsigned int j=0;j<v_global_rand.size();j++){
            VecF64 vout;
            n.propagateFront(trainingins(v_global_rand[j]),vout);
            n.propagateBackFirstDerivate(trainingouts(v_global_rand[j]));
            n.learningFirstDerivate();
            if(display_error_classification==true){
                int label1 = std::distance(vout.begin(),std::max_element(vout.begin(),vout.end()));
                int label2 = std::distance(trainingouts(v_global_rand[j]).begin(),std::max_element(trainingouts(v_global_rand[j]).begin(),trainingouts(v_global_rand[j]).end()));
                if(label1!=label2)
                    error++;
            }
        }
        if(display_error_classification==true)
            std::cout<<i<<"\t"<<error*1.0/v_global_rand.size()<<std::endl;
    }
}

void TrainingNeuralNetwork::trainingFirstDerivative(NeuralNetworkFeedForward&n,const Vec<VecF64>& trainingins,const Vec<VecF64>& trainingouts,const Vec<VecF64>& testins,const Vec<VecF64>& testouts,double eta,unsigned int nbr_epoch,bool display_error_classification)
{
    n.setLearningRate(eta);
    std::vector<int> v_global_rand(trainingins.size());
    for(unsigned int i=0;i<v_global_rand.size();i++)
        v_global_rand[i]=i;
    if(display_error_classification==true)
        std::cout<<"iter_epoch\t error_train\t error_test\t learning rate"<<std::endl;
    for(unsigned int i=0;i<nbr_epoch;i++){
        std::random_shuffle ( v_global_rand.begin(), v_global_rand.end() ,Distribution::MTRand());
        int error_training=0,error_test=0;
        for(unsigned int j=0;j<v_global_rand.size();j++){
            VecF64 vout;
            n.propagateFront(trainingins(v_global_rand[j]),vout);
            n.propagateBackFirstDerivate(trainingouts(v_global_rand[j]));
            n.learningFirstDerivate();
            if(display_error_classification==true){
                int label1 = std::distance(vout.begin(),std::max_element(vout.begin(),vout.end()));
                int label2 = std::distance(trainingouts(v_global_rand[j]).begin(),std::max_element(trainingouts(v_global_rand[j]).begin(),trainingouts(v_global_rand[j]).end()));
                if(label1!=label2)
                    error_training++;
            }
        }
//        std::cout<<testins.size()<<std::endl;
        for(unsigned int j=0;j<testins.size();j++){
            VecF64 vout;
            n.propagateFront(testins(j),vout);
            if(display_error_classification==true){
                int label1 = std::distance(vout.begin(),std::max_element(vout.begin(),vout.end()));
                int label2 = std::distance(testouts(j).begin(),std::max_element(testouts(j).begin(),testouts(j).end()));
                if(label1!=label2)
                    error_test++;
            }
        }
        n.save(("neuralnetwork"+BasicUtility::IntFixedDigit2String(i,2)+".xml").c_str() );
        if(display_error_classification==true)
            std::cout<<i<<"\t"<<error_training*1./trainingins.size()<<"\t"<<error_test*1.0/testins.size() <<"\t"<<eta<<std::endl;
        eta *=0.9;
        n.setLearningRate(eta);
    }
}


//void NeuralNetworkFeedForwardMerge::propagateFront(const pop::VecF64& in , pop::VecF64 &out){
//    pop::VecF64 out1;
//    n1->propagateFront(in,out1);
//    pop::VecF64 out2;
//    n2->propagateFront(in,out2);
//    out1.insert( out1.end(), out2.begin(), out2.end() );
//    n.propagateFront(out1,out);
//}

//void NeuralNetworkFeedForwardMerge::propagateBackFirstDerivate(const pop::VecF64& desired_output){
//    n.propagateBackFirstDerivate(desired_output);
//    NNLayer & layerfirst = *(n._layers[0]);

//    int size1 = n1->_layers[n1->_layers.size()-1]->_neurons.size();
//    int size2 = n2->_layers[n2->_layers.size()-1]->_neurons.size();
//    pop::VecF64 desired_output1(size1);
//    pop::VecF64 desired_output2(size2);

//    for(unsigned int i =0;i<layerfirst._neurons.size();i++){
//        NNNeuron & neuron = layerfirst._neurons[i];
//        if(i<size1)
//            desired_output1(i)=neuron._Xn;
//        if(i>=size1)
//            desired_output2(i-size1)=neuron._Xn;
//    }
//    n1->propagateBackFirstDerivate(desired_output1);
//    n2->propagateBackFirstDerivate(desired_output2);
//}

//void NeuralNetworkFeedForwardMerge::learningFirstDerivate(){
//    n.learningFirstDerivate();
//    n1->learningFirstDerivate();
//    n2->learningFirstDerivate();
//}

//void NeuralNetworkFeedForwardMerge::setLearningRate(double eta){
//    n.setLearningRate(eta);
//    n1->setLearningRate(eta);
//    n2->setLearningRate(eta);
//}

//void NeuralNetworkFeedForwardMerge::addLayerFullyConnected(int nbr_output_neuron,NeuralNetworkFeedForward * nn1,NeuralNetworkFeedForward* nn2){
//    n1= nn1;
//    n2= nn2;
//    n.addInputLayer(n1->_layers[n1->_layers.size()-1]->_neurons.size()+n2->_layers[n2->_layers.size()-1]->_neurons.size());
//    n.addLayerFullyConnected(nbr_output_neuron);
//}


}

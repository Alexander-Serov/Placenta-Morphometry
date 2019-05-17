#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <iostream>
#include <math.h>
#include <vector>
#include <algorithm>
#include <math.h>
#include <vector>
#include "core/data/vec/Vec.h"
#include "core/data/mat/MatN.h"
#include"core/algorithm/GeometricalTransformation.h"
#include"core/data/utility/XML.h"

namespace pop {

/*! \defgroup NeuralNetwork NeuralNetwork
 *  \ingroup Other
 *  \brief Layer neural network with backpropagation training
 *
 * For an introduction of neural network, you can read this <a href="http://www.dkriesel.com/en/science/neural_networks">book</a> .\n
 * My code is inspired by this <a href="http://www.codeproject.com/Articles/16650/Neural-Network-for-Recognition-of-Handwritten-Digi">this one</a>. But, mine is
 *   - simple: removing the multi-threading aspect,
 *   - optimized: including the derivates in the neuron and weight classes allows to avoid their iterative construction/destruction in the training part,
 *   - complete: some facitily for building a neural network,
 *   - normalisation: implementation of theses <a href="http://yann.lecun.com/exdb/publis/pdf/lecun-98b.pdf">recommendations</a> allowing fast convergence (few epochs)
 *
 *
 * The neural network described here is a  multi-layer neural network framework.
 *
 *
 *
 * In the following example, we train a neural network to reproduce a XOR gate. The neural network has one hidden fully connected layer with 3 neurons.
 * \code
    NeuralNetworkLayer n;
    n.addInputLayer(2);//2 scalar input
    n.addLayerFullyConnected(3);// 1 fully connected layer with 3 neurons
    n.addLayerFullyConnected(1);// 1 scalar output
    //create the training set
    // (-1,-1)->-1
    // ( 1,-1)-> 1
    // (-1, 1)-> 1
    // ( 1, 1)->-1
    Vec<VecF64> v_in(4,VecF64(2));//4 vector of two scalar values
    v_in(0)(0)=-1;v_in(0)(1)=-1; // (-1,-1)
    v_in(1)(0)= 1;v_in(1)(1)=-1; // ( 1,-1)
    v_in(2)(0)=-1;v_in(2)(1)= 1; // (-1, 1)
    v_in(3)(0)= 1;v_in(3)(1)= 1; // ( 1, 1)

    Vec<VecF64> v_out(4,VecF64(1));//4 vector of one scalar value
    v_out(0)(0)=-1;// -1
    v_out(1)(0)= 1;//  1
    v_out(2)(0)= 1;//  1
    v_out(3)(0)=-1;// -1
    //use the backprogation algorithm with first order method
    TrainingNeuralNetwork::trainingFirstDerivative(n,v_in,v_out,0.01,1000);
    //test the training
    for(int j=0;j<4;j++){
        VecF64 vout;
        n.propagateFront(v_in(j),vout);
        std::cout<<vout<<std::endl;// we obtain the expected value -1 , 1 , 1 , -1
    }
 * \endcode
*/


class POP_EXPORTS NNWeight
{
    /*!
     * \if DEV
     * \class pop::NNWeight
     * \ingroup NeuralNetwork
     * \brief the weight of the connection between neuron i and neuron j
     * \author Tariel Vincent
     *
     * We include the first and second derivate of the error function over the weight in order to
     * \endif
    */
public:
    NNWeight(double Wn = 0.0,double eta=0.001 );

    /*!
     *  learning following this formula Wn <- Wn - eta*dErr_dWn
     *
    */
    void learningFirstDerivate();
    /**
     *  weight
    */
    double _Wn;
    /**
     *  learning rate
    */
    double _eta;
    /**
     *   first derivate of the error function over the weight
    */
    double _dE_dWn;
    /**
     *  second derivate of the error function over the weight
    */
    double _d2E_dWn2;

};

class NNNeuron;

class POP_EXPORTS NNConnection
{
    /*!
     * \if DEV
     * \class pop::NNConnection
     * \ingroup NeuralNetwork
     * \brief connection with a neuron affected by a weight
     * \author Tariel Vincent
     * \endif
     *
     *  This connection carry information that is processed by \a neuron belonging to a previous layer
    */
public:
    /*!
     * \param weight adresss of the weight
     * \param neuron adress of neuro (NULL is the connection with biais neuron)
     *
     * construct with a neuron affected by a weight
    */
    NNConnection(NNWeight* weight , NNNeuron* neuron = NULL/*NULL is the connection with biais neuron*/ );


    /*!
     * \return is biais neuron
     *
    */
    bool isBiais()const;


    NNWeight* _weight;
    NNNeuron* _neuron;
};


class POP_EXPORTS NNNeuron
{
public:
    /*!
     * \if DEV
     * \class pop::NNNeuron
     * \ingroup NeuralNetwork
     * \brief connection with a neuron affected by a weight
     * \author Tariel Vincent
     * \endif
     *
     * A neuron collects all informations carried by connected neurons as follows  \f$Y^n_i = \sum_{j\in V_i} W^n_{j\rightarrow i}  X^{n-1}_j   \f$ where \f$V_i\f$ is the set of connected neurons and
     * activated its ouput value as follows   \f$ X^n_i = f_{act}(Y^n_i )\f$. Because the neuron is included in a layer, we add the letter n in the notation of the attributes
     * to facilate the algorithm undestanding.
    */

    /*!
    * Activation function with Sigmoid Function recommended by LeCun \f$1.7159 \tanh (\frac{2}{3} x)\f$
    */
    enum ActivationFunction{
        SigmoidFunction=0, ///< Sigmoid Function
        IdentityFunction=1 ///< identity Function
    };
    /*!
    *  \param   f_act activation function
    *
    * constructor
    */
    NNNeuron(ActivationFunction f_act=SigmoidFunction);

    /*!
    *  \param   weight weight
    *  \param   neuron previous neuron
    *
    * connect this neuron with this previous neuron affected by this neight
    */
    void addConnection(NNWeight* weight , NNNeuron* neuron);
    /*!
    *  \param   weight weight
    *
    * connect this neuron with a biais neuron affected by this neight
    */
    void addConnectionBiais(NNWeight* weight );

    /*!
    *  Data processing of a neuron: propagation \f$Y^n_i = \sum_{j\in V_i} W^{n}_{j\rightarrow i}  X^{n-1}_j   \f$  and activation \f$ X^n_i = f_{act}(Y^n_i )\f$
    */
    void propagateFront();

    /*!
    *  Back propagation of the first error derivate:
    *  -  \f$\frac{\partial Err}{\partial Y^n_i} = f'_{act}(Y_n) \frac{\partial Err}{\partial X^n_i}\f$
    *  -  \f$\frac{\partial Err}{\partial W^n_{j\rightarrow i}} \mathrel{+}=  X^{n-1}_j \frac{\partial Err}{\partial Y^n_i}\f$ for all j in \f$V_i\f$
    *  -  \f$\frac{\partial Err}{\partial X^{n-1}_j} \mathrel{+}=  W^n_{j\rightarrow i} \frac{\partial Err}{\partial Y^n_i}\f$ for all j in \f$V_i\f$
    */
    void propagateBackFirstDerivate();
    /*!
    *  Back propagation of the second error derivate following the approximations in the diagonal Levenberg-Marquardt method:
    *  -  \f$\frac{\partial^2 Err}{\left.\partial Y^n_i\right.^2} = f'_{act}(Y_n)^2 \frac{\partial^2 Err}{\left.\partial X^n_i \right.^2}\f$
    *  -  \f$\frac{\partial^2 Err}{\left.\partial W^n_{j\rightarrow i}\right.^2} \mathrel{+}=  \left.X^{n-1}_j\right.^2 \frac{\partial^2 Err}{\left.\partial Y^n_i\right.^2}\f$ for all j in \f$V_i\f$
    *  -  \f$\frac{\partial^2 Err}{\left.\partial X^{n-1}_j\right.^2} \mathrel{+}=  \left.W^n_{j\rightarrow i}\right.^2 \frac{\partial^2 Err}{\left.\partial Y^n_i\right.^2}\f$ for all j in \f$V_i\f$
   */
    void propagateBackSecondDerivate();

    /*!
    *  Before to add values in back propagation of the first error derivate, for \f$\frac{\partial Err}{\partial W^n_{j\rightarrow i}}\f$ and \f$\frac{\partial Err}{\partial X^{n-1}_j}\f$,
    * we have to initiate their.
    */
    void initPropagateBackFirstDerivate();
    /*!
    *  Before to add values in back propagation of the second error derivate, for \f$\frac{\partial^2 Err}{\left.\partial W^n_{j\rightarrow i}\right.^2}\f$ and \f$\frac{\partial^2 Err}{\left.\partial X^{n-1}_j\right.^2}\f$,
    * we have to initiate their.
    */
    void initPropagateBackSecondDerivate();

    /*!
    *  the previous neighborhood neurons defining the set \f$V_i\f$
    */
    Vec<NNConnection> _connections;
    /*!
    *  the activation function
    */
    ActivationFunction _f_act;


    /*!
    *  propagation value \f$Y^n_i = \sum_{j\in V_i} W^{n}_{j\rightarrow i}  X^{n-1}_j   \f$
    */
    double _Yn;
    /*!
    *  output value \f$X^n_i = f_{act}(Y^n_i )  \f$
    */
    double _Xn;
    /*!
    * partial derivated of the error function over the output value
    */
    double _dErr_dXn;
    /*!
    * partial derivated of the error function over the propagation value
    */
    double _dErr_dYn;
    /*!
    * second partial derivated of the error function over the output value
    */
    double _d2Err_dXn2;
    /*!
    * second partial derivated of the error function over the propagation value
    */
    double _d2Err_dYn2;
};





class POP_EXPORTS NNLayer
{
public:
    /*!
     * \if DEV
     * \class pop::NNLayer
     * \ingroup NeuralNetwork
     * \brief connection with a neuron affected by a weight
     * \author Tariel Vincent
     * \endif
     *
     * A neuron collects all informations carried by connected neurons as follows  \f$Y^n_i = \sum_{j\in V_i} W^n_{j\rightarrow i}  X^{n-1}_j   \f$ where \f$V_i\f$ is the set of connected neurons and
     * activated its ouput value as follows   \f$ X^n_i = f_{act}(Y^n_i )\f$. Because the neuron is included in a layer, we add the letter n in the notation of the attributes
     * to facilate the algorithm undestanding.
    */
    /*!
    * type of layer
    */
    enum TypeLayer{
        INPUT=0, ///< input layer
        INPUTMATRIX=1, ///< input layer such that the input is organized in rows and columns (matrix) ->You must use this input if you use convolutional layer next \sa pop::NNLayerMatrix
        MATRIXCONVOLUTIONNAL=2, ///< convolutional layer with subscaling (Simard network)
        FULLYCONNECTED=3,  ///< fully connected layer
        MATRIXCONVOLUTIONNAL2=4, ///< convolutional layer
        MATRIXSUBSALE=5///< subscaling layer
    };
    /*!
    * type of layer
    */
    NNLayer(unsigned int nbr_neuron);
    virtual ~NNLayer();

    /*!
    * \param eta learning rate
    *
    * set this learning rate for all weights
    */
    void setLearningRate(double eta=0.01);
    /*!
    *
    * Front propagation of all neurons in this layer \sa NNNeuron::propagateFront()
    */
    void propagateFront();
    /*!
    *
    * Back first derivated propagation of all neurons in this layer  \sa NNNeuron::propagateBackFirstDerivate()
    */
    void propagateBackFirstDerivate();
    /*!
    *
    * Back second derivated propagation of all neurons in this layer  \sa NNNeuron::propagateBackSecondDerivate()
    */
    void propagateBackSecondDerivate();

    /*!
    *
    * Learning rule for all weights
    */
    void learningFirstDerivate();
    /*!
    *
    * init  all neurons in this layer  \sa NNNeuron::initPropagateBackFirstDerivate()
    */
    void initPropagateBackFirstDerivate();

    /*!
    *
    * init all neurons in this layer  \sa NNNeuron::initPropagateBackSecondDerivate()
    */
    void initPropagateBackSecondDerivate();
    /*!
    * vector of weights strored by this layer (different connections can share the same weight)
    */
    Vec<NNWeight> _weights;
    /*!
    * vector of neurons
    */
    Vec<NNNeuron> _neurons;
    /*!
    * layer type
    */
    TypeLayer _type;

};

class POP_EXPORTS NNLayerMatrix : public NNLayer
{
public:
    /*!
     * \if DEV
     * \class pop::NNLayerMatrix
     * \ingroup NeuralNetwork
     * \brief special layer when neurons are organised in matrices (maps)
     * \author Tariel Vincent
     * \endif
     *
     * In http://www.codeproject.com/Articles/16650/Neural-Network-for-Recognition-of-Handwritten-Digi#ActivationFunction , the tricky part is the code for building the neural network working for
     * a single topology of neural network. To allow the building of neural network with various topology, we use this class when the neuron layer is organised in collection of neuron matrices
    */
    /*!
    * \param nbr_map number of maps (pattern)
    * \param height height size of each map
    * \param width  width size of each map
    */
    NNLayerMatrix(unsigned int nbr_map,unsigned int height,unsigned int width );


    /*!
    * the neurons NNLayer::_neurons are organised in a collection  of matrices (maps)
    */
    Vec<MatN<2,NNNeuron*> > _neurons_matrix;

    void setLearningRate(double eta=0.01);
};


class POP_EXPORTS NNLayerMatrixConvolutionalPlusSubScaling : public NNLayerMatrix
{
    /*!
     * \if DEV
     * \class pop::NNLayerMatrixConvolutionalPlusSubScaling
     * \ingroup NeuralNetwork
     * \brief inner layer when neurons are organised in matrices with convolution and subscaling of the matrices of previous matrix layer
     * \author Tariel Vincent
     * \endif
     *
    */
    /*!
    * \param nbr_map number of maps (pattern)
    * \param height height size of each map
    * \param width  width size of each map
    */

public:
    NNLayerMatrixConvolutionalPlusSubScaling(unsigned int nbr_map,unsigned int height,unsigned int width );
    /*!
* size of the convolutionnal kernel in case of convolutional layer (use to save the data-structure in file)
*/
    int _size_convolutution;
    /*!
* sub-scaling factor in case of convolutional layer (use to save the data-structure in file)
*/
    int _sub_scaling;

};

class POP_EXPORTS NNLayerMatrixConvolutional : public NNLayerMatrix
{
public:
    /*!
     * \if DEV
     * \class pop::NNLayerMatrixConvolutional
     * \ingroup NeuralNetwork
     * \brief inner layer when neurons are organised in matrices with convolution of the matrices of previous matrix layer
     * \author Tariel Vincent
     * \endif
     *
    */
    /*!
    * \param nbr_map number of maps (pattern)
    * \param height height size of each map
    * \param width  width size of each map
    */

    NNLayerMatrixConvolutional(unsigned int nbr_map,unsigned int height,unsigned int width );
    /*!
    * size of the convolutionnal kernel in case of convolutional layer (use to save the data-structure in file)
    */
    int _size_convolutution;

};
class POP_EXPORTS NNLayerMatrixSubScale : public NNLayerMatrix
{
public:
    /*!
     * \if DEV
     * \class pop::NNLayerMatrixSubScale
     * \ingroup NeuralNetwork
     * \brief inner layer when neurons are organised in matrices with subscaling of the matrices of previous matrix layer
     * \author Tariel Vincent
     * \endif
     *
    */
    /*!
    * \param nbr_map number of maps (pattern)
    * \param height height size of each map
    * \param width  width size of each map
    */
    NNLayerMatrixSubScale(unsigned int nbr_map,unsigned int height,unsigned int width );
    /*!
    * sub-scaling factor in case of convolutional layer (use to save the data-structure in file)
    */
    int _sub_scaling;

};



class POP_EXPORTS NeuralNetworkFeedForward
{
public:
    /*!
     * \class pop::NeuralNetworkFeedForward
     * \ingroup NeuralNetwork
     * \brief neural network organised in feedforward topology
     * \author Tariel Vincent
     *
     *  The neurons are grouped in the following layers: One input layer, n-hidden processing layers and one output layer. Each neuron in one layer has only directed connections
     *  to the neurons of the next layer.
     */

    /*!
     * default constructor
     */
    NeuralNetworkFeedForward();
    NeuralNetworkFeedForward(const NeuralNetworkFeedForward& n);
    NeuralNetworkFeedForward & operator=(const NeuralNetworkFeedForward& n);


    virtual ~NeuralNetworkFeedForward();
    /*!
     * init the neural network
     */
    void init();

    /*!
     * \param  in input values
     * \param  out output values
     *
     * The outputs of the neurons of the input layer are updated with the input values \sa in, then the propagation and activation of the neurons of layer-by-layer until the output layer. We set the
     * the output values with the outputs of the neurons of the output layer.
     *
     */
    void propagateFront(const pop::VecF64& in , pop::VecF64 &out);

    /*!
     * \param  desired_output desired output value
     *
     *  In supervised learning algorithm, we want to find a function that best maps a set of inputs to its correct output. As explained by LeCun, in neural network, to find this function,
     *  we iterate a training procedure based of the back propagation of the error function. First we propagate one input generating a given output
     * \code
     * n.propagateFront(vin,vout);
     * \endcode
     * Then, in this method, we compare this given output with a desired output to define a mean square error for this output function following by the back propagation of this error function layer-by-layer
     * until the input layer.
     * \sa TrainingNeuralNetwork::trainingFirstDerivative
     *
     *
     */
    void propagateBackFirstDerivate(const pop::VecF64& desired_output);

    /*!
     *
     *  update the weight following the partial derivate of error function over the
     *
     * \sa TrainingNeuralNetwork::trainingFirstDerivative
     *
     *
     */
    void learningFirstDerivate();
    /*!
     * \param eta learning rate
     *
     *  set the learning rate of all weight with \sa eta
     *
     * \sa TrainingNeuralNetwork::trainingFirstDerivative
     *
     *
     */
    void setLearningRate(double eta=0.01);
    /*!
     *
     * Back propagation of the second derivated of the mean square error function layer-by-layer
     * until the input layer.
     *
     */
    void propagateBackSecondDerivate();

    //  construction

    /*!
     * \param nbr_neuron neuron number
     *
     * add input layer
     *
     */
    void addInputLayer(int nbr_neuron);
    /*!
     * \param nbr_neuron number of neurons
     * \param standart_deviation_weight initialiasisation of the weigh with random variable following a centered normal probability distribution with this standart deviation
     *
     *
     * add a fully connected layer
     *
     */
    void addLayerFullyConnected(unsigned int nbr_neuron,double standart_deviation_weight=1);
    /*!
     * \param height number of rows
     * \param width number of columns
     *
     *
     * add input layer with a matrix of neurons (the number of neuron is equal to height*width). You must use this input layer if you add convolutional layers after.
     *
     */
    void addInputLayerMatrix(unsigned int height,unsigned int width);
    /*!
     * \param nbr_map number of maps (matrices)
     * \param kernelsize size of the convolutionnal kernel
     * \param sub_scale_sampling sub scaling factor
     * \param standart_deviation_weight initialiasisation of the weigh with random variable following a centered normal probability distribution with this standart deviation
     *
     *
     * add input a convolutionnal layer with Feature maps and a sub scaling
     *
     */
    void addLayerConvolutionalPlusSubScaling( unsigned int nbr_map, unsigned int kernelsize=5/*5*5 convolutional pattern*/,unsigned int sub_scale_sampling=2,double standart_deviation_weight=1);

    /*!
     * \param nbr_map number of maps (matrices)
     * \param kernelsize size of the convolutionnal kernel
     * \param standart_deviation_weight initialiasisation of the weigh with random variable following a centered normal probability distribution with this standart deviation
     *
     *
     * add input a convolutionnal layer with Feature maps
     *
     */
    void addLayerConvolutional( unsigned int nbr_map, unsigned int kernelsize=5/*5*5 convolutional pattern*/,double standart_deviation_weight=1);
    /*!
     * \param nbr_map number of maps (matrices)
     * \param kernelsize size of the convolutionnal kernel
     * \param sub_scale_sampling sub scaling factor
     *
     *
     * add input a convolutionnal layer with Feature maps
     *
     */
    void addLayerSubScaling(unsigned int sub_scale_factor,double standart_deviation_weight=1);

    void addLayerIntegral(unsigned int nbr_integral,double standart_deviation_weight=1);


    /*!
    * \param file input file
    *
    * The saver attempts to write the neural network in the given file.
    */
    void save(const char * file)const;

    /*!
    * \param file input file
    *
    * The loader attempts to read the neural network in the given file.
    */
    void load(const char * file);
    /*!
    * \param doc XMLDocument
    *
    * The loader attempts to read the neural network in the given XML document
    */
    void load(XMLDocument &doc);

    /*!
    * \return vector of strings associated to the output layer
    *
    * each output neuron has a associated string (for instance, the first neuron, "0", the second "1", and so one for the numerical caracters). This accessor allows you to access/set this information
    */
    Vec<std::string>& label2String();

    /*!
    * \return vector of strings associated to the output layer
    *
    * each output neuron has a associated string (for instance, the first neuron, "0", the second "1", and so one for the numerical caracters). This accessor allows you to access this information
    */
    const Vec<std::string>& label2String()const;

    /*!
    * \return vector of layers
    *
    * Access the vector of layers
    */
    Vec<NNLayer*>& layers();
    /*!
    * \return vector of layers
    *
    * Access the vector of layers
    */
    const Vec<NNLayer*>& layers()const;

    /*!
    * \param input segmented matrix
    * \return the input value for the input layer a propagateFront
    *
    *  crop the image in a bounding box, scale the image to fit the domain of the input layer, scale the pixels values to the range [-1,1],
    */
    template<typename TypeScalarPixel>
    VecF64 inputMatrixToInputNeuron(const MatN<2,TypeScalarPixel> & matrix);



    /*!
    * vector of layers
    */
    Vec<NNLayer*> _layers;
    Vec<std::string> _label2string;
    //Debug variables
    bool _is_input_layer;
private:
};


struct POP_EXPORTS TrainingNeuralNetwork
{
    /*!
     * \class pop::TrainingNeuralNetwork
     * \ingroup NeuralNetwork
     * \brief train a neural network
     * \author Tariel Vincent
     *
     * This class contains the basic algorithm back propagation algorithm to train a neural network
     *
     *
    */
    //-------------------------------------
    //
    //! \name Training algorithm
    //@{
    //-------------------------------------
    /*!
     * \param n  layer neural network
     * \param trainingins  vector of training input
     * \param trainingouts  vector of training desired output
     * \param eta learning rate:  Wn <- Wn - eta*dErr_dWn
     * \param nbr_epoch number of iterations over the data set
     * \param display_error_classification display the classifcation error for each epoch in the standard output stream
     *
     * This algorithm trains the neural network with a classical back propagation algorithm (first order method).
     * In the following example, we train a neural network to reproduce a XOR gate. The neural network has one hidden fully connected layer with 3 neurons.
     * \code
    NeuralNetworkLayer n;
    n.addInputLayer(2);//2 scalar input
    n.addLayerFullyConnected(3);// 1 fully connected layer with 3 neurons
    n.addLayerFullyConnected(1);// 1 scalar output
    //create the training set
    // (-1,-1)->-1
    // ( 1,-1)-> 1
    // (-1, 1)-> 1
    // ( 1, 1)->-1
    Vec<VecF64> v_in(4,VecF64(2));//4 vector of two scalar values
    v_in(0)(0)=-1;v_in(0)(1)=-1; // (-1,-1)
    v_in(1)(0)= 1;v_in(1)(1)=-1; // ( 1,-1)
    v_in(2)(0)=-1;v_in(2)(1)= 1; // (-1, 1)
    v_in(3)(0)= 1;v_in(3)(1)= 1; // ( 1, 1)

    Vec<VecF64> v_out(4,VecF64(1));//4 vector of one scalar value
    v_out(0)(0)=-1;// -1
    v_out(1)(0)= 1;//  1
    v_out(2)(0)= 1;//  1
    v_out(3)(0)=-1;// -1
    //use the backprogation algorithm with first order method
    TrainingNeuralNetwork::trainingFirstDerivative(n,v_in,v_out,0.01,1000);
    //test the training
    for(int j=0;j<4;j++){
        VecF64 vout;
        n.propagateFront(v_in(j),vout);
        std::cout<<vout<<std::endl;// we obtain the expected value -1 , 1 , 1 , -1
    }
     * \endcode
    */

    static void trainingFirstDerivative(NeuralNetworkFeedForward&n,const Vec<VecF64>& trainingins,const Vec<VecF64>& trainingouts,double eta=0.001,unsigned int nbr_epoch=100,bool display_error_classification=false);
    static void trainingFirstDerivative(NeuralNetworkFeedForward&n,const Vec<VecF64>& trainingins,const Vec<VecF64>& trainingouts,const Vec<VecF64>& testins,const Vec<VecF64>& testouts,double eta=0.001,unsigned int nbr_epoch=20,bool display_error_classification=true);
    //@}
    //-------------------------------------
    //
    //! \name Some applications for optical character recognition
    //@{
    //-------------------------------------
    /*!
    * \brief Simard/LeCun neural network with MNIST training data base
    * \param path the folder path where you donwload the file data0.txt, data1.txt,... http://cis.jhu.edu/~sachin/digit/digit.html
    * \param n output neural network
    *
    * In the following example, we train a neural network for the recognition of the handwritten digits. As in the article of Simard and al "The Best Practices for Convolutional Neural Networks Applied to Visual Document Analysis"
    * the neural network is:
    * - input layer is 29*29 neurons,
    * - first layer is a convolutionnal one with 6 Feature Maps of size 5*5 kernel including a sub-scaling of factor 2,
    * - second layer is  a convolutionnal one with 50 Feature Maps of size 5*5 kernel including a sub-scaling of factor 2,
    * - third layer is  a fully connected one with 100 neurons,
    * - last layer is  a fully connected one with 10 neurons.
    *
    * We use the MNIST data set provided in this site  http://cis.jhu.edu/~sachin/digit/digit.html. We use 90% of this data set to train the neural network and 10% to evaluate the error rate.
   */
    static  void neuralNetworkForRecognitionForHandwrittenDigits(std::string path,NeuralNetworkFeedForward &n);




    //@}
};

template<typename TypeScalarPixel>
VecF64 NeuralNetworkFeedForward::inputMatrixToInputNeuron(const MatN<2,TypeScalarPixel> & img){

    NNLayer * layer = this->_layers[0];
    Vec2I32 domain;
    if(NNLayerMatrix * layermatrix = dynamic_cast<NNLayerMatrix*>(layer) ) {
        domain = layermatrix->_neurons_matrix[0].getDomain();
    }else{
        std::cerr<<"The input of this neural network is not neuron matrix!"<<std::endl;
        return VecF64();
    }

    //cropping
    pop::Vec2I32 xmin(NumericLimits<int>::maximumRange(),NumericLimits<int>::maximumRange()),xmax(0,0);
    ForEachDomain2D(x,img){
        if(img(x)!=0){
            xmin=minimum(xmin,x);
            xmax=maximum(xmax,x);
        }
    }
    Mat2F64 m = img(xmin,xmax+1);
    //downsampling the input matrix
    int index;
    double scale_factor;
    if(double(domain(0))/m.getDomain()(0)<double(domain(1))/m.getDomain()(1)){
        index = 0;
        scale_factor = double(domain(0))/m.getDomain()(0);
    }
    else{
        index = 1;
        scale_factor = double(domain(1))/m.getDomain()(1);
    }

    Mat2F64 mr = GeometricalTransformation::scale(m,Vec2F64(scale_factor,scale_factor),1);
    Mat2F64 mrf(domain);
    Vec2I32 trans(0,0);
    if(index==0){
        trans(0)=0;
        trans(1)=(domain(1)-mr.getDomain()(1))/2;
    }else{
        trans(0)=(domain(0)-mr.getDomain()(0))/2;
        trans(1)=0;
    }


    double maxi=pop::NumericLimits<double>::minimumRange();
    double mini=pop::NumericLimits<double>::maximumRange();

    ForEachDomain2D(xx,mr){
        maxi=std::max(maxi,mr(xx));
        mini=std::min(mini,mr(xx));
         mrf(xx+trans)=mr(xx);
    }
    double diff = (maxi-mini)/2.;
    ForEachDomain2D(xxx,mrf){
        mrf(xxx) = (mrf(xxx)-mini)/diff-1;
    }
    return VecF64(mrf);
}


//class POP_EXPORTS NeuralNetworkFeedForwardMerge
//{
//public:
//    void propagateFront(const pop::VecF64& in , pop::VecF64 &out);
//    void propagateBackFirstDerivate(const pop::VecF64& desired_output);
//    void learningFirstDerivate();
//    void setLearningRate(double eta=0.01);
//    void addLayerFullyConnected(int nbr_output_neuron,NeuralNetworkFeedForward * n1,NeuralNetworkFeedForward* n2);
//private:
//    NeuralNetworkFeedForward * n1;
//    NeuralNetworkFeedForward * n2;
//    NeuralNetworkFeedForward n;
//};

}
#endif // NEURALNETWORK_H

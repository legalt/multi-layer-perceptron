#include "ann/mlp.h"
#include "activation.h"
#include <ios>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace {
    double get_rand_value ( double min, double max )
    {
        double f = (double)rand() / RAND_MAX;
        return min + f * (max - min);
    }
};

namespace ann
{
    MLP::MLP ( std::vector<size_t> layers, std::vector<IActivation*> activations, double learningRate )
        : m_layers(layers)
        , m_activations(activations)
        , m_learningRate(learningRate)
    {
        #ifndef NODEJS
            if ( layers.size() != activations.size() )
                throw std::runtime_error("Count layers and activations must be one size");
        #endif // !NODEJS        

        initialize_indexes();
        initialize_weights();
    }

    MLP::MLP ( std::vector<size_t> layers, std::vector<IActivation*> activations, 
        double learningRate, std::vector<double> weights )
        : m_layers(layers)
        , m_activations(activations)
        , m_learningRate(learningRate)
        , m_weights(weights)
    {
        #ifndef NODEJS
            if ( layers.size() != activations.size() )
                throw std::runtime_error("Count layers and activations must be one size");
        #endif

        
        initialize_indexes();
    }

    MLP::~MLP () {
        for ( auto * act : m_activations )
            delete act;

        m_activations.clear();
    }

    void MLP::save ( const std::string & fileName ) {        
        if ( m_outputs.empty() )
        {
            #ifndef NODEJS
                throw std::runtime_error("Must be trained ann for save");
            #endif

            return;
        }
        
        
        std::ofstream fOut(fileName, std::ios::binary);

        if ( !fOut.is_open() ) {
            std::cerr << "Could not created file of backup\n";
            return;
        }

        fOut.write((char*)&m_learningRate, sizeof(double));
        size_t nLayers = m_layers.size();
        size_t nWeights = m_weights.size();
        size_t nActivations = m_activations.size();
        fOut.write((char*)&nLayers, sizeof(size_t));
        fOut.write((char*)&nWeights, sizeof(size_t));
        fOut.write((char*)&nActivations, sizeof(size_t));

        for ( auto activate : m_activations ) {
            size_t type = activate->getType();
            fOut.write((char*)&type, sizeof(size_t));
        }

        for ( size_t nLayerSize: m_layers ) {
            fOut.write((char*)&nLayerSize, sizeof(size_t));
        }

        for ( double weight: m_weights ) {
            fOut.write((char*)&weight, sizeof(double));
        }

        fOut.close();
    }    

    void MLP::setWeight ( size_t layerNo, size_t neuronNo, size_t weightNo, double weight ) {
        size_t index = m_wIndexedDB[layerNo];
        size_t pos = index + (neuronNo * m_layers[layerNo + 1]) + weightNo;

        m_weights[pos] = weight;
    }

    double MLP::getWeight ( size_t layerNo, size_t neuronNo, size_t weightNo ) {
        size_t index = m_wIndexedDB[layerNo];
        size_t pos = index + (neuronNo * m_layers[layerNo + 1]) + weightNo;
        
        return m_weights[pos];
    }

    std::vector<double> MLP::train ( std::vector<double> inputs )  {
        if ( inputs.size() != m_layers.front() )
        {
            #ifndef NODEJS
                std::stringstream errStream;            
                errStream << "Invalid size of training data! Expected: "
                        << (m_layers.front())
                        << ", real: " << (inputs.size());

            
                throw std::runtime_error(errStream.str());
            #endif

            return {};
        }

        std::vector<double> tLastLayerInputs = inputs;
        const double bias = 1.0;

        m_inputs.clear();
        m_outputs.clear();

        for ( auto input : inputs ) {
            m_inputs.push_back(input);
        }

        for ( size_t nLayer = 0; nLayer < m_layers.size() - 2; nLayer++ ) {
            size_t nNextLayer = m_layers[nLayer + 1];
            std::vector<double> tInputs_2;
            
            for ( size_t nWeight = 0; nWeight < nNextLayer; nWeight++ ) {
                double sum = 0.0;

                for ( size_t nNeuron = 0; nNeuron < m_layers[nLayer]; nNeuron++ ) {   
                    double weight = getWeight(nLayer, nNeuron, nWeight);
                    sum += weight * tLastLayerInputs[nNeuron];
                }

                sum = m_activations[nLayer]->activation(sum + bias);
                m_inputs.push_back(sum);
                tInputs_2.push_back(sum);
            }
            
            tLastLayerInputs = std::move(tInputs_2);
        }
        
        const size_t nOutputs = m_layers.back();
        const size_t nLayerSize = m_layers.size() - 1;
        const size_t nHiddenLayerSize = m_layers[m_layers.size() - 2];

        for ( size_t nOutput = 0; nOutput < nOutputs; nOutput++ ) {
            double sum = 0.0;

            for ( size_t nNeuron = 0; nNeuron < tLastLayerInputs.size(); nNeuron++ ) {                        
                sum += (tLastLayerInputs[nNeuron]) * getWeight(nLayerSize - 1, nNeuron, nOutput);                        
            }
            
            m_outputs.push_back(m_activations[nLayerSize]->activation(sum));
        }
        
        return m_outputs;
    }

    void MLP::backProp ( std::vector<double> desired ) {
        if ( desired.size() != m_layers.back() ) {
            #ifndef NODEJS
                std::stringstream errStream;
            
                errStream << "Invalid size of etalons data! Expected: "
                        << (m_layers.back())
                        << ", real: " << (desired.size());

                throw std::runtime_error(errStream.str());
            #endif
            
            return;
        }

        for ( size_t nOutput = 0; nOutput < m_outputs.size(); nOutput++ ) {
            const double output = m_outputs[nOutput];
            const double error = desired[nOutput] - output;
            double delta_weights = (error * m_activations.back()->deriavate(output));

            if ( delta_weights == 0 && output > 0 && desired[nOutput] <= 0 )
            {                
                // std::cout << "output: " << output << ", err: " << error
                // << " target: " << desired[nOutput]
                // << " delta_weights: " << delta_weights << " dx: " << m_activations.back()->deriavate(output) << '\n';
                // delta_weights = -0.1;
            }

            std::vector<double> hLayerErrors;

            int pos = m_layers.size() - 2;
            int inputPos = 0;

            while ( pos >= 0 ) {
                const size_t layerSize = m_layers[pos];

                if ( !hLayerErrors.empty() ) {
                    std::vector<double> newErrors(layerSize);
                    std::vector<double> inputs;
                    std::vector<double>::iterator inputIt = m_inputs.end() - (inputPos + layerSize);

                    std::for_each(inputIt, inputIt + layerSize + 1, [&]( double input ) {
                        inputs.push_back(input);
                    });

                    for ( size_t errIndex = 0; errIndex < hLayerErrors.size(); errIndex++ ) {
                        for ( size_t nNeuron = 0; nNeuron < layerSize; nNeuron++ ) {
                            const double oldWeight = getWeight(pos, nNeuron, errIndex);
                            const double output = m_activations[0]->activation(inputs[nNeuron] * oldWeight);
                            delta_weights = hLayerErrors[errIndex];
                            
                            const double newWeight = oldWeight + delta_weights * m_learningRate * inputs[nNeuron];
                            
                            newErrors[nNeuron] += (newWeight * delta_weights);
                            
                            setWeight(pos, nNeuron, errIndex, newWeight);
                        }
                    }

                    inputs.clear();
                    hLayerErrors.clear();
                    hLayerErrors = std::move(newErrors);
                } else {
                    std::vector<double> inputs(layerSize);
                    std::copy(m_inputs.end() - layerSize, m_inputs.end(), inputs.begin());

                    for ( size_t index = 0; index < layerSize; index++ ) {
                        double w = getWeight(pos, index, nOutput) + 
                                    inputs[index] * delta_weights * m_learningRate;                        
                        hLayerErrors.push_back(w * delta_weights);
                        setWeight(pos, index, nOutput, w);
                    }

                    inputPos += layerSize;
                }
                --pos;
            }
        }
    }
    
    void MLP::initialize_indexes () {
        m_nOutputs = m_layers.back();
        size_t nInputs = m_layers[0];
        m_wIndexedDB.push_back(0);
        size_t nWeights = 0;
        size_t nNeurons = m_layers.front();

        for ( size_t nLayer = 1; nLayer < m_layers.size(); nLayer++ ) {
            size_t index = m_wIndexedDB.back() + m_layers[nLayer - 1] * m_layers[nLayer];

            m_wIndexedDB.push_back(index);
            nWeights += index;
            nNeurons += m_layers[nLayer];
        }
    }

    void MLP::initialize_weights () {
        double variance;

        for ( size_t nLayer = 0; nLayer < m_layers.size() - 1; nLayer++ ) {
            size_t nNextLayer = m_layers[nLayer + 1];

            variance = 1.0 / sqrt(nNextLayer);

            for ( size_t nNeuron = 0; nNeuron < m_layers[nLayer]; nNeuron++ ) {

                for ( size_t nWeight = 0; nWeight < nNextLayer; nWeight++ ) {
                    m_weights.push_back(get_rand_value(-variance, variance));                            
                }
            }
        }
    }

    double MLP::getMse ( std::vector<double> inputs, std::vector<double> desired ) {
        double error = 0.0;
        auto outputs = train(inputs);

        for ( size_t index = 0; index < outputs.size(); index++ ) {
            double tError = desired[index] - outputs[index];

            error += (tError * tError);
        }

        return error / outputs.size();
    }
} // ann
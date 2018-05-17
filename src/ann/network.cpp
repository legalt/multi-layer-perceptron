#include "ann/network.h"
#include "ann/INeuron.h"
#include <memory>

namespace ann
{
    Network::Network( const std::vector<size_t> & layers, double learningRate, IActivation activation ):
            m_learningRate(learningRate),
            m_layers(layers),
            m_activation(activation)
        {
            init(m_layers);
        }
    
    void Network::init ( const std::vector<size_t> & layers )
    {
        if ( layers.size() <= 2 )
                throw std::runtime_error("Invalid size layers");

            size_t countNeurons = 0;
            int layerIndex = layers.size();

            for ( auto it = layers.rbegin(); it != layers.rend(); ++it )
            {
                size_t lastLayerSize = 0;

                if ( countNeurons > 0 ) lastLayerSize = countNeurons;

                countNeurons = (*it);

                if ( countNeurons <= 0 )
                    throw std::runtime_error("Invalid layers size, must be > 0");

                // for ( size_t index = 0; index < countNeurons; index++ )
                // {
                //     std::weak_ptr<INeuron> neuron = ann::createNeuron(layerIndex, index, m_activation, lastLayerSize);
                //     m_neurons.push_back(neuron.lock());

                //     if ( lastLayerSize > 0 )
                //     {
                //         size_t prevLayerIndex = (m_neurons.size() - (index + lastLayerSize)) + 1;

                //         for ( ; prevLayerIndex < (m_neurons.size() - index); prevLayerIndex++)
                //             (*m_neurons[prevLayerIndex]).addChild(&(*m_neurons.back()));
                //     }
                // }

                layerIndex--;
            }

            std::reverse(m_neurons.begin(), m_neurons.end());
    }

    std::vector<double> Network::train ( std::vector<double> inputs )
    {
        if ( m_layers[0] != inputs.size() )
            throw std::runtime_error("Invalid inputs size");

        for ( size_t index = 0; index < inputs.size(); index++ )
            m_neurons[index]->setInput(inputs[index]);

        std::vector<double> outputs;
        size_t countOutputs = m_layers.back();
    
        for ( size_t index = m_neurons.size() - countOutputs; index < m_neurons.size(); index++ )
            outputs.push_back(m_neurons[index]->getOutput());

        return outputs;
    }

    std::vector<double> Network::getErrors ( std::vector<double> outputs, std::vector<double> etalons )    
    {
        std::vector<double> errors;

        for ( size_t index = 0; index < outputs.size(); index++ )
            errors.push_back(outputs[index] - etalons[index]);

        return errors;
    }

    std::vector<double> Network::correctWeights ( std::vector<double> outputs, std::vector<double> etalons )
    {
        if ( m_layers.back() != etalons.size() )
            throw std::runtime_error("Invalid etalons size");

        std::vector<double> errors = getErrors(outputs, etalons);

        for ( size_t index = 0; index < outputs.size(); index++ )
        {
            double weights_delta = calculation_weights_delta<ActivationFn>(outputs[index], errors[index]);

            for ( size_t nIndex = (m_neurons.size() - outputs.size()); nIndex < m_neurons.size(); nIndex++ )
            {
                for ( auto neuron_ : (*m_neurons[nIndex]).getPrevLayer() )
                    (*neuron_).correctWeights(m_learningRate, weights_delta, (*m_neurons[nIndex]).getLayerNeuronIndex());
            }
        }

        return errors;
    }
};
#include "ann/network.h"
#include "ann/INeuron.h"
#include <memory>
#include <stdio.h>
#include <iostream>

namespace ann
{
    Network::Network( const std::vector<size_t> & layers, double learningRate, IActivation & activation ):
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

                for ( size_t index = 0; index < countNeurons; index++ )
                {
                    auto neuron = ann::createNeuron(layerIndex, index, m_activation, lastLayerSize);

                    if ( lastLayerSize > 0 )
                    {
                        size_t prevLayerIndex = (m_neurons.size() - (index + lastLayerSize));

                        for ( ; prevLayerIndex < (m_neurons.size() - index); prevLayerIndex++)
                            (*m_neurons[prevLayerIndex]).addChild(&(*neuron));
                    }

                    m_neurons.push_back(neuron);
                }

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

    std::vector<std::vector<double>> Network::train ( std::vector<std::vector<double>> samples )
    {
        std::vector<std::vector<double>> outputs;

        for ( std::vector<double> & inputs : samples )
            outputs.push_back(train(inputs));

        return outputs;
    }

    // handle only one vector of training data
    double Network::calculationMSE ( std::vector<double> inputs, std::vector<double> etalons )
    {
        double sampleErr = 0;
        std::vector<double> outputs = train(inputs);
        std::vector<double> errors = getErrors(outputs, etalons);

        for ( auto it = errors.begin(); it != errors.end(); ++it )
            sampleErr += (*it) * (*it);
        
        sampleErr /= outputs.size();

        return sampleErr;
    }

    // handle set training data
    double Network::calculationMSE ( std::vector<std::vector<double>> samples, std::vector<std::vector<double>> etalons )
    {
        double result = 0;
        size_t nSamples = samples.size();

        if ( etalons.size() != nSamples )
            throw std::runtime_error("Count samples must be equal count of etalons");

        for ( size_t sampleIndex = 0; sampleIndex < nSamples; sampleIndex++ )
            result += calculationMSE(samples[sampleIndex], etalons[sampleIndex]);

        return result / nSamples;
    }

    std::vector<double> Network::getErrors ( std::vector<double> outputs, std::vector<double> etalons )
    {
        std::vector<double> errors;

        for ( size_t index = 0; index < outputs.size(); index++ )
            errors.push_back(outputs[index] - etalons[index]);

        return errors;
    }

    void Network::correctWeights ( std::vector<double> outputs, std::vector<double> etalons )
    {
        if ( m_layers.back() != etalons.size() )
            throw std::runtime_error("Invalid etalons size");

        std::vector<double> errors = getErrors(outputs, etalons);

        for ( size_t index = 0; index < outputs.size(); index++ )
        {
            double weights_delta = calculation_weights_delta(outputs[index], errors[index], m_activation);
            size_t lastHiddenLayer = m_layers[m_layers.size() - 2];
            size_t nIndex = (m_neurons.size() - (outputs.size() + lastHiddenLayer));

            for ( ; nIndex < (m_neurons.size() - outputs.size()); nIndex++ )
                (*m_neurons[nIndex]).correctWeights(m_learningRate, weights_delta, (*m_neurons[nIndex]).getLayerNeuronIndex());
        }
    }

    void Network::correctWeights ( std::vector<std::vector<double>> outputs, std::vector<std::vector<double>> etalons )
    {
        size_t nOutputs = outputs.size();

        if ( etalons.size() != nOutputs )
            throw std::runtime_error("Count outputs must be equal count of etalons");

        for ( size_t indexOutput = 0; indexOutput < nOutputs; indexOutput++ )
            correctWeights(outputs[indexOutput], etalons[indexOutput]);
    }
};
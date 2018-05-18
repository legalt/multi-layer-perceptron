#include "ann/INeuron.h"
#include <stddef.h>
#include <iostream>
#include <memory>

namespace {
    double getRandomWeights ( double min, double max )
    {
        double f = (double)rand() / RAND_MAX;
        return min + f * (max - min);
    }

    class Neuron: public ann::INeuron
    {
        public:
            Neuron & operator=(const Neuron & obj) = delete;
            Neuron & operator=(Neuron && obj) = delete;
            Neuron ( Neuron && obj ) = delete;
            Neuron ( const Neuron & obj ) = delete;

            Neuron ( size_t layerIndex, size_t layerNeuronIndex, ann::IActivation & activation, size_t countWeights = 1 ):
                m_layerIndex(layerIndex),
                m_weights(countWeights),
                m_layerNeuronIndex(layerNeuronIndex),
                m_activation(activation)
            {
                generateWeights();
            }

            virtual size_t getLayerIndex () { return m_layerIndex; }
            virtual size_t getLayerNeuronIndex () { return m_layerNeuronIndex; }
            virtual double getWeight( size_t index ) { return m_weights[index];}
        
            virtual double getOutput( )
            {
                double output = 0.0;

                if ( m_childs.size() == 0 )
                    return m_input;

                for ( auto & child : m_childs )
                    output += child->getWeight(m_layerNeuronIndex) * child->getOutput();
            
                setInput(m_activation.activate(output));

                return getInput();
            }

            virtual void setInput ( double input ) { m_input = input;}

            virtual double getInput() { return m_input; }

            virtual void addChild ( ann::INeuron * neuron ) {
                m_childs.push_back(neuron);
            }

            virtual std::vector<ann::INeuron*> getPrevLayer() { return m_childs; };

            virtual void correctWeights ( double learningRate, double delta, size_t nNeuronIndex = 0 )
            {
                double error = 0.0;

                m_weights[nNeuronIndex] = m_weights[nNeuronIndex] - (m_input * delta * learningRate);    
                error = m_weights[nNeuronIndex] * delta;

                for ( auto & child : m_childs )
                {
                    double output = child->getOutput() * child->getWeight(m_layerNeuronIndex);
                    double weights_delta = error * m_activation.derivative(output);
                    child->correctWeights(learningRate, weights_delta, m_layerNeuronIndex);
                }
            }
        private:
            std::vector<double> m_weights; // weights for next layer
            std::vector<ann::INeuron*> m_childs; // prev layer
            double m_input = 0;
            size_t m_layerIndex;
            size_t m_layerNeuronIndex;
            ann::IActivation & m_activation;

            void generateWeights ()
            {
                srand((unsigned)time(0));
                
                for ( auto & weight: m_weights )
                    weight = -0.5 + (double)rand()/((double)RAND_MAX/(0.5 + 0.5));
            }
        };
};

namespace ann {
    std::shared_ptr<INeuron> createNeuron ( size_t layerIndex, size_t layerNeuronIndex, IActivation & activation, size_t countWeights )
    {
        return std::make_shared<Neuron>(layerIndex, layerNeuronIndex, activation, countWeights);
    }
};

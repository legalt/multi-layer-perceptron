#pragma once

#include "ann/INeuron.h"
#include <stddef.h>
#include <iostream>

namespace {
    double getRandomWeights ( double min, double max )
    {
	    double f = (double)rand() / RAND_MAX;
        return min + f * (max - min);
    }
};

namespace ann {

	template <typename ActivationFn>
    class Neuron: public INeuron
    {
    public:	
	    Neuron & operator=(const Neuron & obj) = delete;
	    Neuron & operator=(Neuron && obj) = delete;
	    Neuron ( Neuron && obj ) = delete;
	    Neuron ( const Neuron & obj ) = delete;

	    Neuron(size_t countWeights, int layerIndex, int layerNeuronIndex ):
		    m_layerIndex(layerIndex),
		    m_weights(countWeights),
		    m_layerNeuronIndex(layerNeuronIndex)
	    {
		    generateWeights();
	    }

	    Neuron (int layerIndex, int layerNeuronIndex): 
		    m_weights(1),
		    m_layerIndex(layerIndex),
		    m_layerNeuronIndex(layerNeuronIndex)
	    {
		    generateWeights();
	    }		

	    virtual int getLayerIndex () { return m_layerIndex; }
	    virtual int getLayerNeuronIndex () { return m_layerNeuronIndex; }
	    virtual double getWeight( size_t index ) { return m_weights[index];}
	
	    virtual double getOutput( )
	    {		
		    double output = 0.0;
		    // printIndex();

		    if ( m_childs.size() == 0 )
			    return m_input;

		    for ( auto & child : m_childs )
			    output += child->getWeight(m_layerNeuronIndex) * child->getOutput();
		
		    setInput(ActivationFn::activate(output));

		    /*std::cout << m_layerIndex << "\t\t" << m_layerNeuronIndex << "\t\t" 
			    << m_childs.size() <<  "\t\t" << getInput() << std::endl;*/

		    return getInput();
	    }

	    virtual void setInput ( double input ) { m_input = input;}

	    virtual double getInput() { return m_input; }

	    virtual void addChild ( INeuron * neuron ) { m_childs.push_back(neuron); }

	    virtual std::vector<INeuron*> getPrevLayer () { return m_childs; }

	    virtual void correctWeights ( double learningRate, double delta, size_t nNeuronIndex = 0 )
	    {
		    double error = 0;

		    m_weights[nNeuronIndex] = m_weights[nNeuronIndex] - (m_input * delta * learningRate);	
		    error = m_weights[nNeuronIndex] * delta;

		    for ( auto & child : m_childs )
		    {
			    double output = child->getOutput() * child->getWeight(m_layerNeuronIndex);
			    double weights_delta = error * ActivationFn::derivative(output);
			    child->correctWeights(learningRate, weights_delta, m_layerNeuronIndex);
		    }
	    }

	    virtual void printIndex ()
	    {
		    std::cout << m_layerIndex << "\t\t" << m_layerNeuronIndex << "\t\t" << m_childs.size() <<  "\n";
	    }	
    private:
	    std::vector<double> m_weights; // weights for next layer
	    std::vector<INeuron*> m_childs; // prev layer
	    double m_input = 0;
	    size_t m_layerIndex;
	    size_t m_layerNeuronIndex;

	    void generateWeights ()
	    {	
		    for ( auto & weight: m_weights )
			    weight = getRandomWeights(0.0, 1.0);
	    }
    };
};

#include "neuron.h"
#include <cmath>
#include <vector>
#include <memory>
#include <exception>
#include <stddef.h>
#include <algorithm>

namespace {
	// output - sigmoidFn(I1*W1..+..In * Wn)
	template <typename ActivationFn>
	double calculation_weights_delta ( double output, double error )
	{
		// e * sigm(x)dx
	    return error * ActivationFn::derivative(output);
	}
};

namespace ann
{
    struct Sigmoid {
	    static double activate ( double x )
	    {
	        return (1.0 / (1.0 + exp(-x)));
	    }

	    static double derivative ( double sigmFn )
	    {
	        return sigmFn * (1 - sigmFn);
	    }
    };

    template <typename ActivationFn>
    class Network
    {
    public:
	    Network () = delete;
	    Network ( const Network & ) = delete;
	    Network ( Network && ) = delete;
	    Network & operator= ( const Network & ) = delete;
	    Network & operator= ( Network && ) = delete;

	    Network ( const std::vector<size_t> & layers, double learningRate ):
		    m_learningRate(learningRate)
	    {
		    m_layers = layers;
		    init(m_layers);
	    }	

	    std::vector<std::shared_ptr<INeuron>> getNeurons () { return m_neurons; }
    private:
	    std::vector<std::shared_ptr<INeuron>> m_neurons;
	    std::vector<size_t> m_layers;
	    double m_learningRate;

	    void init ( const std::vector<size_t> & layers )
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
				    if ( lastLayerSize > 0 )
				    {
					    auto neuron = std::make_shared<Neuron<ActivationFn>>(lastLayerSize, layerIndex, index);

					    for ( size_t index2 = (m_neurons.size() - (index + lastLayerSize)); index2 < (m_neurons.size() - index); index2++)
						    (*m_neurons[index2]).addChild(&(*neuron));

					    m_neurons.push_back(neuron);
				    } else {
					    m_neurons.push_back(std::make_shared<Neuron<ActivationFn>>(layerIndex, index));
				    }				
			    }

			    layerIndex--;
		    }

		    std::reverse(m_neurons.begin(), m_neurons.end());
	    }

    public:
	    std::vector<double> train ( std::vector<double> inputs )
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

	    std::vector<double> getErrors ( std::vector<double> outputs, std::vector<double> etalons )	
	    {
		    std::vector<double> errors;

		    for ( size_t index = 0; index < outputs.size(); index++ )
	            errors.push_back(outputs[index] - etalons[index]);

		    return errors;
	    }

	    std::vector<double> correctWeights ( std::vector<double> outputs, std::vector<double> etalons )
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
};

#pragma once
#include <stddef.h>
#include <vector>
#include <memory>
#include "ann/IActivation.h"

namespace ann {
    class INeuron {
        public:
            virtual size_t getLayerIndex() = 0;
            virtual size_t getLayerNeuronIndex() = 0;
            virtual double getOutput() = 0;
            virtual void setInput(double) = 0;
            virtual double getInput() = 0;
            virtual void addChild(ann::INeuron*) = 0;
            virtual double getWeight(size_t neuronIndex) = 0;
            virtual std::vector<ann::INeuron*> getPrevLayer() = 0;
            virtual void correctWeights ( double, double, size_t = 0 ) = 0;
    };

	std::shared_ptr<INeuron> createNeuron ( 
		size_t layerIndex,
		size_t layerNeuronIndex,
		IActivation & activation,
		size_t countWeights);
};
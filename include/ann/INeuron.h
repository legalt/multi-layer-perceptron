#pragma once
#include <stddef.h>
#include <vector>

namespace ann {
	class INeuron {
	    public:
		    virtual int getLayerIndex() = 0;
		    virtual int getLayerNeuronIndex() = 0;
		    virtual double getOutput() = 0;	
		    virtual void setInput(double) = 0;
		    virtual double getInput() = 0;
		    virtual void addChild(INeuron *) = 0;
		    virtual double getWeight(size_t neuronIndex) = 0;
		    virtual void printIndex ()=0;
		    virtual std::vector<INeuron*> getPrevLayer() = 0;
		    virtual void correctWeights ( double, double, size_t = 0 ) = 0;
	};
};
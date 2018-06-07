#include "ann/network.h"
#include <vector>
#include <iostream>
#include <utility>

// values: (0, 1)
class Sigmoid: public ann::IActivation
{
	public:
		virtual double activate ( double x )
		{
			return (1.f / (1.f + exp(-x)));
		}

		virtual double derivative ( double x )
		{
			return x * (1 - x);
		}
};

// values (-1, 1)
class BipolarSigmoid: public ann::IActivation
{
    public:
        virtual double activate ( double x )
        {
            return (2 / (1 + exp(-x))) - 1;
        }

        virtual double derivative ( double x )
        {
            return (0.5 * (1 + x)) * (1 - x);
        }
};


int main ( int argc, char *argv[] )
{
    try {
        Sigmoid sig;
        ann::Network n({2,1}, 0.2, sig);        

		std::vector<std::vector<double>> trainSet = {
			{0, 0},
			{0, 1},
			{1, 0},
			{1, 1},
		};

		std::vector<std::vector<double>> trainEtalons = {
			{0},
			{1},
			{1},
			{0},
		};
        
        size_t epochs = 20000;
		double minError = 0.1;

		double error = 0;
        for ( size_t index = 0; index < epochs; index++ )
        {    
			error = n.calculationMSE(trainSet, trainEtalons);
			
			if ( error <= minError )
			{
				std::cout <<" 1123123\n";
				break;
			}

			n.correctWeights(trainSet, trainEtalons);
        }

		std::cout << "MSE Error: " << error << '\n';

		// n.safeWeights("/home/legalt/back");

		for ( size_t sampleIndex = 0; sampleIndex < trainSet.size(); sampleIndex++ )
		{
			std::vector<double> outputs = n.train(trainSet[sampleIndex]);
			std::cout << std::boolalpha << "Expected: " << (trainEtalons[sampleIndex][0] == 1) << " ";

			for ( auto out : outputs )
				std::cout << std::boolalpha << out << " ";

			std::cout << '\n';
		}		
    } catch ( std::exception & exception )
    {
        std::cerr << "Error: " << exception.what() << std::endl;
        return 1;
    }

    return 0;
}
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
			return (1.0 / (1.0 + exp(-x)));
		}

		virtual double derivative ( double sigmFn )
		{
			return sigmFn * (1.0 - sigmFn);
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
        ann::Network n({3,2,1}, 0.05, sig);
        double minError = 0.0001;

        std::vector<std::pair<std::vector<double>, double>> trainSet;
        trainSet.push_back(std::make_pair(std::vector<double>{0, 0, 1}, 1));
        trainSet.push_back(std::make_pair(std::vector<double>{0, 1, 0}, 0));
        trainSet.push_back(std::make_pair(std::vector<double>{0, 1, 1}, 0));
        trainSet.push_back(std::make_pair(std::vector<double>{1, 0, 0}, 1));
        trainSet.push_back(std::make_pair(std::vector<double>{1, 0, 1}, 1));
        trainSet.push_back(std::make_pair(std::vector<double>{1, 1, 0}, 1));
        trainSet.push_back(std::make_pair(std::vector<double>{1, 1, 1}, 0));

        size_t epochs = 50000;

        //TODO: need replace to network train (overloaded with epochs, with flag correct weights) method
        for ( size_t index = 0; index < epochs; index++ )
        {    
            for ( std::pair<std::vector<double>, double> set : trainSet )
            {
                std::vector<double> outputs_ = n.train(set.first);
                n.correctWeights(outputs_, {set.second});

                if ( outputs_[0] < minError || outputs_[0] > 1 )
                {
                    index = epochs;
                    break;
                }
            }
        }

        for ( std::pair<std::vector<double>, double> set : trainSet )
        {
            std::vector<double> outputs_ = n.train(set.first);
            auto errors = n.getErrors(outputs_, {set.second});
            std::cout << "Expected: " << set.second << " Errors:";

            for ( auto err : errors )
                std::cout << err << " ";

            std::cout << std::endl;
        }

        std::cout << std::endl;
    } catch ( std::exception & exception )
    {
        std::cerr << "Error: " << exception.what() << std::endl;
        return 1;
    }

    return 0;
}
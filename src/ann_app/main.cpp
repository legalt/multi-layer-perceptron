#include "ann/network.h"
#include <vector>
#include <iostream>
#include <utility>

int main ( int argc, char *argv[] )
{
    try {
        ann::Network<ann::Sigmoid> n({3,2,1}, 0.08);

        std::vector<std::pair<std::vector<double>, double>> trainSet;
        trainSet.push_back(std::make_pair(std::vector<double>{0, 0, 1}, 0));
        trainSet.push_back(std::make_pair(std::vector<double>{0, 1, 0}, 0));
        trainSet.push_back(std::make_pair(std::vector<double>{0, 1, 1}, 0));
        trainSet.push_back(std::make_pair(std::vector<double>{1, 0, 0}, 1));
        trainSet.push_back(std::make_pair(std::vector<double>{1, 0, 1}, 1));
        trainSet.push_back(std::make_pair(std::vector<double>{1, 1, 0}, 1));
        trainSet.push_back(std::make_pair(std::vector<double>{1, 1, 1}, 0));

        size_t epochs = 5000;

        for ( size_t index = 0; index < epochs; index++ )
        {    
            for ( std::pair<std::vector<double>, double> set : trainSet )
            {
                std::vector<double> outputs_ = n.train(set.first);
                n.correctWeights(outputs_, {set.second});                
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
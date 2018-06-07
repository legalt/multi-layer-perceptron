#include "ann/mlp.h"
#include <vector>
#include <iostream>
#include <utility>


int main () {
    // Ann::SigmoidActivation sig;
    // Ann::ReLuActivation relu;

    // std::vector<size_t> inputs{15,5,10};
    // std::vector<Ann::IActivation*> acts{&relu, &relu, &sig};

    // Ann::MLP mlp(inputs, acts, 0.05);

    std::vector<std::vector<double>> trainSet = {
        {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1},
        {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
        {1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1},
        {1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
        {1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1},
        {1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1},
        {1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}
    };

    // std::vector<std::vector<double>> etalons = {
    //     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
    // };

    // for ( int i = 0; i < 5000; i++ ) {
    //     for ( int nSample = 0; nSample < trainSet.size(); nSample++ ) {
    //         mlp.train(trainSet[nSample]);
    //         mlp.backProp(etalons[nSample]);
    //     }
    // }

    // mlp.save("./back");

    {
        auto mlp = ann::MLP::load("./back");
        for ( int i = 0; i < trainSet.size(); i++ ) {
            
            auto outs = mlp->train(trainSet[i]);
            for ( auto n: outs )
                std::cout << (n > 0.9 ? "true" : "false") << ' ';
            std::cout << '\n';
        }

        mlp.reset();
    }

    return 0;
};
#include <vector>
#include <iostream>
#include <algorithm>
#include <map>
#include <math.h>

#include "mnist/mnist_reader.hpp"
#include "ann/mlp.h"

int main ( int argc, char *argv[] ) {
    const char mnist_data [] = "./src/proj_mnist/mnist";
    mnist::MNIST_dataset<std::vector, std::vector<uint8_t>, uint8_t> dataset =
        mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>(mnist_data);

    std::cout << "loaded mnist db\n";

    std::vector<ann::IActivation*> atcs = {
        ann::getActivation(ann::RELU),
        ann::getActivation(ann::RELU),
        ann::getActivation(ann::SIG)
    };
    ann::MLP mlp({784, 400, 10}, atcs, 0.05);
    const size_t epochs = 100;
    std::cout << "inited network\n";

    std::vector<std::vector<double>> images(dataset.training_images.size());
    size_t index = 0;
    std::for_each(dataset.training_images.begin(), dataset.training_images.end(), [&]( std::vector<uint8_t> item ) {
        std::vector<double> image(item.size());

        for ( uint8_t pixel: item )
            image.push_back((double)pixel);

        images[index] = std::move(image);
        index++;
    });

    std::cout << "normalized data\n";

    std::map<uint8_t, std::vector<double>> etalons = {
        {'0', {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
        {'1', {0, 1, 0, 0, 0, 0, 0, 0, 0, 0}},
        {'2', {0, 0, 1, 0, 0, 0, 0, 0, 0, 0}},
        {'3', {0, 0, 0, 1, 0, 0, 0, 0, 0, 0}},
        {'4', {0, 0, 0, 0, 1, 0, 0, 0, 0, 0}},
        {'5', {0, 0, 0, 0, 0, 1, 0, 0, 0, 0}},
        {'6', {0, 0, 0, 0, 0, 0, 1, 0, 0, 0}},
        {'7', {0, 0, 0, 0, 0, 0, 0, 1, 0, 0}},
        {'8', {0, 0, 0, 0, 0, 0, 0, 0, 1, 0}},
        {'9', {0, 0, 0, 0, 0, 0, 0, 0, 0, 1}}
    };

    
    std::cout << "training\n";

    for ( size_t iter = 0; iter < epochs; iter++ ) {
        // for ( size_t nImage = 0; nImage < dataset.training_images.size(); nImage++ ) {
        for ( size_t nImage = 0; nImage < 21; nImage++ ) {
            std::cout << images[nImage].size() << std::endl;
            // mlp.train(images[nImage]);

            // uint8_t imgLabel = dataset.training_labels[nImage];
            
            // mlp.backProp(etalons[imgLabel]);
        }
        
        std::cout << "Progress: " << (round((epochs / iter) * 100)) << std::flush;
    }

    mlp.save("./mnist_weights");

    // std::cout << "Nbr of test images = " << dataset.test_images.size() << std::endl;
    // std::cout << "Nbr of test labels = " << dataset.test_labels.size() << std::endl;

    return 0;
}
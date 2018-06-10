#include <vector>
#include <iostream>
#include <algorithm>
#include <map>
#include <math.h>

#include "mnist/mnist_reader.hpp"
#include "ann/mlp.h"

void print ( std::vector<double> image, size_t width = 28, size_t height = 28  ) {
    for ( size_t i = 0; i < width; i++ ) {
	for ( size_t j = 0; j < height; j++ ) {
	    std::cout << (image[i+j]);
	    std::cout << ((image[i+j]) > 0.0 ? "*" : " ");
	}

	std::cout << '\n';
    }
}

int main ( int argc, char *argv[] ) {
    if ( argc < 2 ) {
	std::cerr << "enough params!\n";

	return -1;
    }

    const char mnist_data [] = "./src/proj_mnist/mnist";
    mnist::MNIST_dataset<std::vector, std::vector<uint8_t>, uint8_t> dataset =
        mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>(mnist_data);

    std::cout << "loaded mnist db\n";

    /*std::vector<ann::IActivation*> atcs = {
        ann::getActivation(ann::RELU),
        ann::getActivation(ann::RELU),
        ann::getActivation(ann::SIG)
    };
    ann::MLP mlp({784, 400, 10}, atcs, 0.05);
    const size_t epochs = 1;
    std::cout << "inited network\n";

    */std::vector<std::vector<double>> images(dataset.training_images.size());
    size_t index = 0;
    std::for_each(dataset.training_images.begin(), dataset.training_images.end(), [&]( std::vector<uint8_t> item ) {
        std::vector<double> image;

        for ( uint8_t pixel: item ) {
	    image.push_back((double)pixel);
	}
        images[index] = std::move(image);
        index++;
    });

    /*std::cout << "normalized data\n";

    std::vector<std::vector<double>> etalons = {
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
    };

    
    std::cout << "training\n";
    size_t setSize = dataset.training_images.size();
    for ( size_t iter = 0; iter < epochs; iter++ ) {
        for ( size_t nImage = 0; nImage < setSize; nImage++ ) {
            mlp.train(images[nImage]);

            size_t imgLabel = (size_t)(dataset.training_labels[nImage]);
            mlp.backProp(etalons[imgLabel]);
        }
    }

    mlp.save("./mnist_weights");
    */

    auto mlp = ann::MLP::load("./mnist_weights");
    auto outs = mlp->train(images[1]);

    for ( auto out: outs )
	std::cout << out << '\n';

    print(images[1]);
    
    // std::cout << "Nbr of test images = " << dataset.test_images.size() << std::endl;
    // std::cout << "Nbr of test labels = " << dataset.test_labels.size() << std::endl;

    return 0;
}
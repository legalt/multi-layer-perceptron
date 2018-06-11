#include <vector>
#include <iostream>
#include <algorithm>
#include <map>
#include <math.h>

#include "mnist/mnist_reader.hpp"
#include "ann/mlp.h"

void print ( std::vector<double> image, size_t width = 28, size_t height = 28  ) {
    int row = 0;

    for ( size_t i = 0; i < width * height; i++ ) {
        std::cout << ((image[i]) > 0 ? '*' : ' ');

        row++;
        
        if ( row == height ){
            row = 0;
            std::cout << '\n';
        }
    }
}

int main ( int argc, char *argv[] ) {
    if ( argc < 2 ) {
	    std::cerr << "enough params!\n";

	    return -1;
    }

    std::string command(argv[1]);
    const char mnist_data [] = "./src/proj_mnist/mnist";
    
    mnist::MNIST_dataset<std::vector, std::vector<uint8_t>, uint8_t> dataset =
        mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>(mnist_data);
    
    std::vector<std::vector<double>> images(dataset.training_images.size());

    std::cout << "loaded mnist db\n";
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

    {
        size_t index = 0;

        std::for_each(dataset.training_images.begin(), dataset.training_images.end(), [&]( std::vector<uint8_t> item ) {
            std::vector<double> image;

            for ( uint8_t pixel: item ) {
                double _pixel = (double)pixel;
                image.push_back((_pixel/255));
            }
            images[index] = std::move(image);
            index++;
        });

        std::cout << "normalized data\n";
    }

    if ( command == "l" ) {
        std::vector<ann::IActivation*> atcs = {
            ann::getActivation(ann::RELU),
            ann::getActivation(ann::RELU),
            ann::getActivation(ann::SIG)
        };
        ann::MLP mlp({784, 300, 10}, atcs, 0.05);
        const size_t epochs = 1;
        std::cout << "inited network\n";        

        
        std::cout << "training\n";
        size_t setSize = dataset.training_images.size();
        
        for ( size_t iter = 0; iter < epochs; iter++ ) {
            for ( size_t nImage = 0; nImage <= 10; nImage++ ) {
                mlp.train(images[nImage]);

                size_t imgLabel = (size_t)(dataset.training_labels[nImage]);
                mlp.backProp(etalons[imgLabel]);
                const double mse = mlp.getMse(images[nImage], etalons[imgLabel]);
                double progress = nImage;
                progress = round((progress / 100) * 100);

                std::cout << "\r" << "progress: " << progress << "% \t\tmse: " << mse << std::flush;
            }
        }

        auto outs = mlp.train(images[3]);
        print(images[3]);
        for ( auto out: outs )
            std::cout << (out > 0.3 ? 1 : 0) << "\t\t" << out << '\n';

        mlp.save("./mnist_weights");

        return 0;
    }

    size_t index = atoi(argv[1]);

    if ( index > 9 ) {
        std::cerr << "invalid index: " << index << '\n';

	    return -1;
    }

    auto mlp = ann::MLP::load("./mnist_weights");
    auto outs = mlp->train(images[index]);
    
    
    // for ( int i = 0; i < 1000; i++ ) {
    //     outs = mlp->train(images[index]);
    //     mlp->backProp(etalons[index]);
    //     std::cout << "\r" << "mse: " << (mlp->getMse(images[index], etalons[index])) << std::flush;
    // }    

    outs = mlp->train(images[index]);
    {
        size_t index = 0;
        for ( auto out: outs )
	        std::cout << (index++) << ": " << (out > 0.3 ? 1 : 0) << '\n';
    }
    print(images[index]);
    
    // std::cout << "Nbr of test images = " << dataset.test_images.size() << std::endl;
    // std::cout << "Nbr of test labels = " << dataset.test_labels.size() << std::endl;

    return 0;
}
#include <vector>
#include <iostream>
#include <algorithm>
#include <map>
#include <math.h>
#include <iomanip>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#include "mnist/mnist_reader.hpp"
#include "ann/mlp.h"

typedef std::vector<std::vector<double>> Samples;
typedef std::vector<std::vector<uint8_t>> Labels;


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

void printImage ( std::vector<double> image, size_t width = 28, size_t height = 28  ) {
    int row = 0;

    for ( size_t i = 0; i < width * height; i++ ) {
        std::cout << ((image[i]) > 0 ? '*' : ' ');

        row++;
        
        if ( row == height ) {
            row = 0;
            std::cout << '\n';
        }
    }
}

void printUsage () {
    std::cout << "ANN (Artificial neural network)\n"
        << "Used algorithms multi-layer perceptron by Rosenblat and \n"
        << "back propogation with gradient descent.\n"
        << "Using target - mnist database of handwritten digits\n\n"
        << "Usage:\n"
        << "  annAppMnist -l | --learning\n"        
        << "  annAppMnist -h | --help\n";
}

void printHelp () {
    std::cout << "Options:\n";
        std::cout << std::resetiosflags(std::ios::adjustfield);
        std::cout << std::setiosflags(std::ios::left);
        std::cout << std::setw(25) << "  -h --h" << "show help\n" 
                << std::setw(25) << "  -l --learning" << "learning network\n"
                << std::setw(25) << "  -r --recognize" << "recognize\n";
}

void trainNetwork ( size_t epochs, size_t nSamples, ann::MLP & network, Samples images, std::vector<uint8_t> labels ) {
    try {
        for ( size_t iter = 0; iter < epochs; iter++ ) {
            double res_mse = 0;

            for ( size_t nImage = 0; nImage < nSamples; nImage++ ) {
                network.train(images[nImage]);

                // std::cout << labels.size() << std::endl;
                size_t imgLabel = (size_t)(labels[nImage]);
                network.backProp(etalons[imgLabel]);
                
                const double mse = network.getMse(images[nImage], etalons[imgLabel]);                
                double total = round((iter / epochs) * 100);
                double progress = round(((double)nImage / (double)nSamples) * 100);

                std::cout << "\r"
                            << "epoch: " << iter
                            << "\tprogress: " << progress
                            << "\t\tmse: " << ((int)(mse * 100))
                            << std::flush;
                res_mse += mse;
            }

            std::cout << "\nepoch [" << iter  << "] mse: " << (int)((res_mse / 2) * 100) << '\n';
        }
    } catch ( boost::bad_lexical_cast & ex ) {
        std::cerr << __FUNCTION__ << " [" << __LINE__ << "]: " << ex.what();
        exit(-1);
    }
}

std::vector<std::vector<double>> loadDataBase ( std::vector<std::vector<uint8_t>> training_images ) {
    std::cout << "loaded mnist db\n";    
    
    std::vector<std::vector<double>> images(training_images.size());

    size_t index = 0;

    std::for_each(training_images.begin(), training_images.end(), [&]( std::vector<uint8_t> item ) {
        std::vector<double> image;

        for ( uint8_t pixel: item ) {
            double _pixel = (double)pixel;
            image.push_back((pixel > 1 ? 1 : 0));
        }

        images[index] = std::move(image);
        index++;
    });

    std::cout << "normalized data\n";

    return images;
}

void recognizeImage ( ann::MLP & network, std::vector<double> image, std::vector<double> etalon ) {
    auto outs = network.train(image);
            
    outs = network.train(image);

    {   
        std::cout << "\n\n#\t\t\treal\t\tetalon\n";
        size_t index = 0;        
        
        for ( auto out: outs ) {
            std::cout << std::boolalpha << (index) << ": " << std::setw(5) << (out > 0.3)            
                        << "\t\t" << std::setprecision(3) 
                        << out << std::fixed 
                        << "\t\t" << etalon[index]<< '\n';        

            index++;
        }
    }

    std::cout << "ERROR: " << ((int)(network.getMse(image, etalon) * 100)) << '%' << '\n';

    printImage(image);

}

void handle_command ( const char * arg ) {
    if ( boost::starts_with(arg, "-h") || boost::starts_with(arg, "--help") ) {
        printHelp();

        return;
    }

    if ( boost::starts_with(arg, "-l") || boost::starts_with(arg, "--learning") ) {
        std::string path;
        std::cout << "Folder with mnist database: ";
        std::cin >> path;

        if ( path.empty() || path.size() < 5 ) {
            std::cout << "using default db path\n";
            path = "./src/proj_mnist/mnist/";
        }

        mnist::MNIST_dataset<std::vector, std::vector<uint8_t>, uint8_t> dataset =
        mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>(path);

        if ( 0 == dataset.training_images.size() )
            exit(-1);
        
        const auto & images = loadDataBase(dataset.training_images);

        std::vector<ann::IActivation*> atcs = {
            ann::getActivation(ann::SIG),
            ann::getActivation(ann::SIG),
            ann::getActivation(ann::SIG)
        };
        ann::MLP mlp({784, 800, 10}, atcs, 0.006);
        std::cout << "inited network\n";
        
        std::cout << "training\n";
        size_t setSize = 5000;
        const size_t epochs = 1;

        trainNetwork(epochs, setSize, mlp, images, dataset.training_labels);

        size_t imgLabel = (size_t)(dataset.training_labels[0]);
        recognizeImage(mlp, images[0], etalons[imgLabel]);
        std::string backPath;
        std::cout << "\nBackup weight path: ";
        std::cin >> backPath;
        mlp.save(backPath);

        return;
    }

    if ( boost::starts_with(arg, "-r") || boost::starts_with(arg, "--recognize") ) {
        std::string backPath;
        std::cout << "Backup weight path: ";
        std::cin >> backPath;
        
        auto mlpObj = ann::MLP::load(backPath);
        std::shared_ptr<ann::MLP> mlp;

        if ( !mlpObj ) {
            return;
        }

        mlp = *mlpObj;

        std::string path;
        std::cout << "Folder with mnist database: ";
        std::cin >> path;

        if ( path.empty() || path.size() < 5 ) {
            std::cout << "using default db path\n";
            path = "./src/proj_mnist/mnist/";
        }

        mnist::MNIST_dataset<std::vector, std::vector<uint8_t>, uint8_t> dataset =
        mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>(path);

        const auto & images = loadDataBase(dataset.training_images);
        
        try {
            auto retry = [&]() {
                std::string index_;
                std::cout << "Image index: ";
                std::cin >> index_;
                size_t index = boost::lexical_cast<size_t>(index_);            
                size_t imgLabel = (size_t)(dataset.training_labels[index]);
                recognizeImage((*mlp), images[index], etalons[imgLabel]);
            };
            
            retry();

            while ( true ) {
                std::cout << "Repeat [y/n]?: ";
                std::string apply;
                std::cin >> apply;
                
                if ( apply == "y" ) {
                    retry();
                } else {
                    exit(0);
                }
            }
        } catch ( boost::bad_lexical_cast &e ) {
            std::cerr << "Invalid parameter: " << e.what() << '\n';
            return;
        }
    }
}

int main ( int argc, char *argv[] ) {
    if ( argc < 2 ) {
	    std::cerr << "Not enough params!\n";
        
        printUsage();
        printHelp();

	    return -1;
    }

    handle_command(argv[1]);

    return 0;
}
#include <cmath>
#include <vector>
#include <memory>
#include <exception>
#include <stddef.h>
#include <algorithm>
// #include "ann/IActivation.h"
#include "ann/INeuron.h"

namespace {
    // output - sigmoidFn(I1*W1..+..In * Wn)
    double calculation_weights_delta ( double output, double error, ann::IActivation & fn )
    {
        // e * sigm(x)dx
        return error * fn.derivative(output);
    }
};

namespace ann
{
    class Network
    {
    public:
        Network () = delete;
        Network ( const Network & ) = delete;
        Network ( Network && ) = delete;
        Network & operator= ( const Network & ) = delete;
        Network & operator= ( Network && ) = delete;

        Network ( const std::vector<size_t> & layers, double learningRate, IActivation & activation );

        inline std::vector<std::shared_ptr<INeuron>> getNeurons () { return m_neurons; }
    private:
        std::vector<std::shared_ptr<INeuron>> m_neurons;
        std::vector<size_t> m_layers;
        double m_learningRate;
        IActivation & m_activation;

        void init ( const std::vector<size_t> & layers );
    public:
        std::vector<double> train ( std::vector<double> inputs );

		std::vector<std::vector<double>> train ( std::vector<std::vector<double>> samples );

		//FOR ONE TRAIN DATA - sqrt(error) / outputCount
        double calculationMSE ( std::vector<double> inputs, std::vector<double> etalons );

		//FOR SET TRAIN DATA - sqrt(error) / outputCount
        double calculationMSE ( std::vector<std::vector<double>> samples, std::vector<std::vector<double>> etalons );

        std::vector<double> getErrors ( std::vector<double> outputs, std::vector<double> etalons );

        void correctWeights ( std::vector<double> outputs, std::vector<double> etalons);

		void correctWeights ( std::vector<std::vector<double>> outputs, std::vector<std::vector<double>> etalons);
    };
};

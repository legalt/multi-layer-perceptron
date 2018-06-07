#pragma once

#include <vector>
#include <cstddef>
#include <memory>
#include <fstream>

namespace ann
{
    const size_t SIG = 0;
    const size_t RELU = 1;

    class IActivation;

    IActivation * getActivation( size_t type );

    class MLP {
        public:
            MLP () = delete;
            MLP (const MLP&) = delete;
            MLP (MLP &&) = delete;

            explicit MLP ( std::vector<size_t> layers, std::vector<IActivation*> activations, double learningRate );

            explicit MLP ( std::vector<size_t> layers, std::vector<IActivation*> activations, 
                double learningRate, std::vector<double> weights );

            ~MLP ();

            void save ( const std::string & fileName );

            static std::shared_ptr<MLP> load ( const std::string & fileName ) {
                std::ifstream fIn(fileName, std::ios::binary);

                if ( !fIn.is_open() ) {
                    throw std::runtime_error("Could not open file of backup");
                }

                double learnRate;
                size_t layersSize;
                size_t weightsSize;
                size_t nActivations;
                fIn.read((char*)&learnRate, sizeof(double));
                fIn.read((char*)&layersSize, sizeof(size_t));
                fIn.read((char*)&weightsSize, sizeof(size_t));
                fIn.read((char*)&nActivations, sizeof(size_t));

                std::vector<size_t> activationTypes(nActivations);
                std::vector<size_t> layers(layersSize);
                std::vector<double> weights(weightsSize);                
                
                fIn.read((char*)&activationTypes[0], sizeof(size_t) * nActivations);
                fIn.read((char*)&layers[0], sizeof(size_t) * layersSize);
                fIn.read((char*)&weights[0], sizeof(double) * weightsSize);
                fIn.close();

                std::vector<IActivation*> acts;

                for ( auto type: activationTypes ) {
                    acts.push_back(getActivation(type));
                }
                
                return std::shared_ptr<MLP>(new MLP(layers, acts, learnRate, weights));
            }

            void setWeight ( size_t layerNo, size_t neuronNo, size_t weightNo, double weight );

            double getWeight ( size_t layerNo, size_t neuronNo, size_t weightNo );

            std::vector<double> train ( std::vector<double> inputs );

            void backProp ( std::vector<double> outputs );

        private:
            void initialize_indexes ();

            void initialize_weights ();
        
        protected:
            inline void setWeight(std::vector<double> weights) { m_weights = weights; }

        private:
            std::vector<size_t> m_layers;
            std::vector<double> m_weights;
            std::vector<size_t> m_wIndexedDB;
            std::vector<double> m_inputs;
            std::vector<double> m_outputs;
            std::vector<IActivation*> m_activations;
            size_t m_nOutputs;
            double m_learningRate;
    };
}
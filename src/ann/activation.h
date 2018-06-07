#include <cstddef>
#include <math.h>

namespace ann {
    class IActivation {
        public:
            virtual double activation ( double x ) =0;
            virtual double deriavate (double x ) =0;
            virtual size_t getType()=0;
            virtual ~IActivation() {}
    };

    class SigmoidActivation: public IActivation {
        public:
            inline double activation ( double x ) { return 1 / ( 1 + exp(-x)); }

            inline double deriavate ( double x ) { return x * (1.0 - x); }

            size_t getType() { return SIG; };   

            ~SigmoidActivation() {}         
    };

    class ReLuActivation: public IActivation {
        inline double activation ( double x ) { return (( x < 0 ) ? 0 : x); }

        inline double deriavate ( double x ) { return (( x < 0 ) ? 0 : 1); }

        size_t getType() { return RELU; };

        ~ReLuActivation() {}
    };

    IActivation * getActivation( size_t type ) {        
        switch ( type ) {
            case 1:
                return new ReLuActivation();
            case 0:
            default:
                return new SigmoidActivation();
        }
    }
};
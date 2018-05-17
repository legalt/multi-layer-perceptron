namespace ann {
    class IActivation 
    {
        public:
            virtual double activate ( double ) = 0;

            virtual double derivative ( double ) = 0;
    };
};
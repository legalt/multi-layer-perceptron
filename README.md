# Multilayer perceptron

Implemetation algorithm of neural network - multilayer perceptron with backpropogation error

## Using

```
// count neurons in layers (first or last item into vector it input or ouput layer)
const std::vector<size_t> layers{
  3, // input layer
  2, // hidden layer
  1  // output layer
};

ann::Neural<ActivationFunction> network(layers);

std::vector<float> outputs = network.train({1, 0, 1});

std::vector<float> errors = network.correctWeights({1});
```

## Description
[wiki](https://en.wikipedia.org/wiki/Multilayer_perceptron)

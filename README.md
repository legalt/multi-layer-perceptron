# Multilayer perceptron

Implemetation algorithm of neural network - multilayer perceptron with backpropogation error

## Using

```
// count neurons in layers (first or last item into vector it input or ouput layer)
const std::vector<size_t> layers{
  3, // input layer
  2, // hidden layer
  .....
  n + 1  // output layer
};
auto activationsByLayer = {
  ann::getActivation(ann::RELU),
  ann::getActivation(ann::RELU),
  ann::getActivation(ann::SIG)
};

ann::MLP mlp(layers, activationsByLayer, 0.05)

std::vector<double> outputs = mlp.train({1, 0, 1});
network.backProp({1});

// save options and weights of ANN
network.save("weights");

// load
auto mlp = ann::MLP::load("weights");
mlp->train({1, 0, 1});
```

## Description
[wiki](https://en.wikipedia.org/wiki/Multilayer_perceptron)

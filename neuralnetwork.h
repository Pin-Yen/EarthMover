#ifndef NEURALNETWORK_H_
#define NEURALNETWORK_H_

#include <stdlib.h>
#include <vector>

class NeuralNetwork {
 public:
  NeuralNetwork() { neurons_ = NULL; }
  ~NeuralNetwork();

  enum Type { INPUT, HIDDEN, OUTPUT };

  void init(int inputSize, const std::vector<LayerInf> &networkStruct);

  void train(double input[], int output[], int cycle,
             double rate, double error);

  void forward();

  // Each layer's information
  // type: this layer's neuron type
  // length: length of this layer
  struct LayerInf {
    Type type;
    int length;
  };

 protected:
  class Neuron;
  class InputNeuron;
  class HiddenNeuron;
  class OutputNeuron;

  Neuron* neuronArrayMaker(Type type, int length);

 private:
  Neuron** neurons_;
  std::vector<LayerInf> networkStruct_;
};

#endif  // NEURALNETWORK_H_

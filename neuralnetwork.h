#ifndef NEURALNETWORK_H_
#define NEURALNETWORK_H_

#include <stdlib.h>
#include <vector>

class NeuralNetwork {
 public:
  NeuralNetwork() { neurons_ = NULL; }
  ~NeuralNetwork();

  enum Type { INPUT, HIDDEN, OUTPUT };

  // Each layer's information.
  // type: this layer's neuron type
  // length: length of this layer
  struct LayerInf {
    Type type;
    int length;
  };

  // Training data.
  struct Data {
    double* input;
    int* output;
  };

  // Init all network.
  void init(int networkDepth, const LayerInf* networkStruct);

  // Training netowrk.
  void train(const Data data[], int dataAmount, int cycle, double rate);

  // Print network output for corresponding input.
  void predict(const double inputs[]);

 protected:
  class Neuron;
  class InputNeuron;
  class HiddenNeuron;
  class OutputNeuron;

  // Allocate a neuron array and return it
  // type: neuron's type, length: neuron array's length
  Neuron** neuronArrayMaker(Type type, int length);

 private:
  void forward(const double inputs[]);

  void back(const int expectedOutputs[]);

  void fix(double rate);

  Neuron*** neurons_;
  LayerInf* nnStruct_;
  int nnDepth_;
};

#endif  // NEURALNETWORK_H_

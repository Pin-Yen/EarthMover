#ifndef NEURALNETWORK_H_
#define NEURALNETWORK_H_

#include <stdlib.h>
#include <vector>

class NeuralNetwork {
 public:
  NeuralNetwork() { neurons_ = NULL; }
  ~NeuralNetwork();

  enum Type { NORMAL, OUTPUT };

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
  void init(int inputSize, int networkDepth, const LayerInf* networkStruct);

  // Training netowrk.
  void train(const Data data[], int dataAmount, int cycle,
             double rate, double allowError);

  // Get network output for corresponding input.
  void output(const double inputs[], double* output) const;

 protected:
  class Neuron;
  class OutputNeuron;

  // Allocate a neuron array and return it
  // type: neuron's type, length: neuron array's length
  Neuron* neuronArrayMaker(Type type, int length);

 private:
  void forward(const double inputs[], double** outputs) const;

  void back(const int expectedOutputs[],
            double** outputs, double** errors) const;

  void fix(const double inputs[], double** outputs,
           double** errors, double rate);

  Neuron** neurons_;
  LayerInf* networkStruct_;
  int networkDepth_;
  int inputSize_;
};

#endif  // NEURALNETWORK_H_

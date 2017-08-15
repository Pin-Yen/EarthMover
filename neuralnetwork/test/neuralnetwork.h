#ifndef NEURALNETWORK_H_
#define NEURALNETWORK_H_

#include <stdlib.h>
#include <vector>

class NeuralNetwork {
 public:
  NeuralNetwork() { neurons_ = NULL; }
  ~NeuralNetwork();

  enum Type { INPUT, HIDDEN, OUTPUT, CONVOLUTION };

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
  void init(int networkDepth, const LayerInf* networkStruct, int** args);

  // Training netowrk.
  void train(const Data data[], int dataAmount, int cycle,
             double allowError, int batchSize, double rate, double momentRate);

  // Print network output for corresponding input.
  void predict(const double inputs[]);

 protected:
  class Neuron;
  class InputNeuron;
  class HiddenNeuron;
  class OutputNeuron;
  class ConvolutionNeuron;

  // Allocate a neuron and return it
  // type: neuron's type.
  // args: args for neuron's constructor, each type of neuron may need
  // different kinds of args.
  Neuron* neuronMaker(Type type, const int* args);

 private:
  void clearFixValue();

  void forward(const double inputs[]);

  void back(const int expectedOutputs[]);

  void calculateFix(double rate, int batchSize);

  void updateMoment(double rate);

  void fix();

  Neuron*** neurons_;
  LayerInf* nnStruct_;
  int nnDepth_;
};

#endif  // NEURALNETWORK_H_

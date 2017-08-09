#include "neuralnetwork.h"

#include <vector>

NeuralNetwork::~NeuralNetwork() {
  if (neurons_ != NULL) {
    for (int i = 0, n = networkStruct_.size(); i < n; ++i) {
      delete neurons_[i];
    }
    delete neurons_;
  }
}

void NeuralNetwork::init(int inputSize, const std::vector<int> &networkStruct)
    : networkStruct_(networkStruct) {
  int size = networkStruct.size();
  neurons_ = new Neoron*[size];

  neurons_[0] = new Neuron(inputSize);
  for (int i = 1; i < size; ++i)
    neurons_[i] = new Neuron(networkStruct[i - 1]);
}

NeuralNetwork::Neuron::Neuron() {
  synapses_ = NULL;
}

NeuralNetwork::Neuron::Neuron(int upperSize) {
  synapses_ = new double[upperSize];
  for (int i = 0; i < upperSize; ++i)
    setSynapse(i, initValue());

  setGate(-initValue());
  setOutput(0.0);
}

NeuralNetwork::Neuron::~Neuron() {
  if (synapses_ != NULL) delete [] synapses_;
}

void NeuralNetwork::Neuron::forward(Neuron* upperNeurons, int upperSize) {
  double value = gate();
  for (int i = 0; i < upperSize; ++i)
    value += upperNeurons[i].output() * getSynapse(i);

  setOutput(activation(value));
}

double NeuralNetwork::Neuron::back(Neuron* lowerNeurons,
                                   double* lowerDifference,
                                   int lowerSize,
                                   int currentIndex) {
  double error = 0.0;
  for (int i = 0; i < lowerSize; ++i)
    error += lowerDifference[i] * lowerNeurons[i].getSynapse(currentIndex);

  return dActivation(error);
}

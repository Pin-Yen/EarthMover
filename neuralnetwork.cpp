#include "neuralnetwork.hpp"

NeuralNetwork::Neuron::Neuron(int upperSize) {
  synapses_ = new double[upperSize];
  for (int i = 0; i < upperSize; ++i)
    synapses_[i] = initValue();

  gate_ = initValue();
  output_ = 0.0;
}

NeuralNetwork::Neuron::~Neuron() {
  delete [] synapses_;
}

void NeuralNetwork::Neuron::forward(Neuron* upperNeurons, int upperSize) {
  double value = gate_;
  for (int i = 0; i < upperSize; ++i) {
    value += upperNeurons[i] * synapses_[i];
  }
  output_ = activation(value);
}

double NeuralNetwork::Neuron::backProp(Neuron* lowerNeurons,
                                       double* lowerDifference,
                                       int lowerSize) {
  double error = 0.0;
  for (int i = 0; i < lowerSize; ++i) {
    error += lowerDifference[i] * lowerNeurons.synapses_[i];
  }
  return dActivation(error);
}
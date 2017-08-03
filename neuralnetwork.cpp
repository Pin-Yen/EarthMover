#include "neuralnetwork.h"

NeuralNetwork::~NeuralNetwork() {
  if (neurons_ != NULL) {
    // TODO
  }
}

void NeuralNetwork::init(const int size[], int depth) {
  neurons_ = new Neuron*[depth];

  // TODO
}

NeuralNetwork::Neuron::Neuron() {
  synapses_ = NULL;
}

NeuralNetwork::Neuron::Neuron(int upperSize) {
  synapses_ = new double[upperSize];
  for (int i = 0; i < upperSize; ++i)
    setSynapse(i, initValue());

  setGate(initValue());
  setOutput(0.0);
}

NeuralNetwork::Neuron::~Neuron() {
  if (synapses_ != NULL) delete [] synapses_;
}

void NeuralNetwork::Neuron::forward(Neuron* upperNeurons, int upperSize) {
  double value = gate();
  for (int i = 0; i < upperSize; ++i) {
    value += upperNeurons[i].output() * getSynapse(i);
  }
  setOutput(activation(value));
}

double NeuralNetwork::Neuron::backProp(Neuron* lowerNeurons,
                                       double* lowerDifference,
                                       int lowerSize,
                                       int currentIndex) {
  double error = 0.0;
  for (int i = 0; i < lowerSize; ++i) {
    error += lowerDifference[i] * lowerNeurons[i].getSynapse(currentIndex);
  }
  return dActivation(error);
}

#include "neuron.h"

NeuralNetwork::Neuron::~HiddenNeuron() {
  if (synapses_ != NULL) delete [] synapses_;
}

void NeuralNetwork::HiddenNeuron::init(int upperSize) {
  synapses_ = new double[upperSize];
  for (int i = 0; i < upperSize; ++i)
    setSynapse(i, initValue());

  setGate(-initValue());
  setOutput(0.0);
}

void NeuralNetwork::HiddenNeuron::forward(Neuron* upperNeurons, int upperSize) {
  double value = gate();
  for (int i = 0; i < upperSize; ++i)
    value += upperNeurons[i].output() * getSynapse(i);

  setOutput(activation(value));
}

double NeuralNetwork::HiddenNeuron::back(Neuron* lowerNeurons,
                                         double* lowerDifference,
                                         int lowerSize,
                                         int currentIndex) {
  double error = 0.0;
  for (int i = 0; i < lowerSize; ++i)
    error += lowerDifference[i] * lowerNeurons[i].getSynapse(currentIndex);

  return dActivation(error);
}
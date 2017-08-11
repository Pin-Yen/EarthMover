#include "neuron.h"

NeuralNetwork::Neuron::~Neuron() {
  if (synapses_ != NULL) delete [] synapses_;
}

void NeuralNetwork::Neuron::init(int upperSize) {
  synapses_ = new double[upperSize];
  for (int i = 0; i < upperSize; ++i)
    setSynapse(i, initValue());

  setGate(-initValue());
}

double NeuralNetwork::Neuron::forward(const double upperOutputs[],
                                      int upperSize) const {
  double value = gate();
  for (int i = 0; i < upperSize; ++i)
    value += upperOutputs[i] * synapse(i);

  return activation(value);
}

double NeuralNetwork::Neuron::back(const Neuron lowerNeurons[],
                                   const double lowerErrors[],
                                   int lowerSize,
                                   double currentOutput,
                                   int currentIndex) const {
  double error = .0;
  for (int i = 0; i < lowerSize; ++i)
    error += lowerErrors[i] * lowerNeurons[i].synapse(currentIndex);

  return error * dActivation(currentOutput);
}

void NeuralNetwork::Neuron::fix(const double upperOutputs[],
                                int upperSize,
                                double currentError,
                                double rate) {
  fixGate(rate * currentError);
  for (int i = 0; i < upperSize; ++i)
    fixSynapse(i, rate * currentError * upperOutputs[i]);
}

double NeuralNetwork::OutputNeuron::back(double currentOutput,
                                         int expectedOutput) const {
  double error = expectedOutput - currentOutput;
  return error * dActivation(currentOutput);
}

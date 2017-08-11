#include "neuron.h"

NeuralNetwork::HiddenNeuron::~HiddenNeuron() {
  if (synapses_ != NULL) delete [] synapses_;
}

void NeuralNetwork::HiddenNeuron::init(int upperSize) {
  synapses_ = new double[upperSize];
  for (int i = 0; i < upperSize; ++i)
    setSynapse(i, initValue());

  setGate(-initValue());
}

void NeuralNetwork::HiddenNeuron::forward(Neuron** upperNeurons,
                                          int upperSize) {
  double value = gate();
  for (int i = 0; i < upperSize; ++i)
    value += upperNeurons[i]->output() * synapse(i);

  setOutput(activation(value));
}

void NeuralNetwork::HiddenNeuron::back(Neuron** lowerNeurons,
                                       int lowerSize,
                                       int currentIndex) {
  double error = .0;
  for (int i = 0; i < lowerSize; ++i)
    error += static_cast<HiddenNeuron*>(lowerNeurons[i])->error() *
        static_cast<HiddenNeuron*>(lowerNeurons[i])->synapse(currentIndex);

  setError(error * dActivation(output()));
}

void NeuralNetwork::HiddenNeuron::fix(Neuron** upperNeurons,
                                      int upperSize,
                                      double rate) {
  fixGate(rate * error());
  for (int i = 0; i < upperSize; ++i)
    fixSynapse(i, rate * error() * upperNeurons[i]->output());
}

void NeuralNetwork::OutputNeuron::back(int expectedOutput) {
  setError((expectedOutput - output()) * dActivation(output()));
}

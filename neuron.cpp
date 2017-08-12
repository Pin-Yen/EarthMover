#include "neuron.h"

// HiddenNeuron
NeuralNetwork::HiddenNeuron::~HiddenNeuron() {
  if (synapses_ != NULL) delete [] synapses_;
}

void NeuralNetwork::HiddenNeuron::init(int upperSize) {
  upperSize_ = upperSize;

  synapses_ = new double[upperSize];
  for (int i = 0; i < upperSize; ++i)
    setSynapse(i, initValue());

  fixSynapseValues_ = new double[upperSize];

  setGate(-initValue());
}

void NeuralNetwork::HiddenNeuron::forward(Neuron** upperNeurons) {
  double value = gate();
  for (int i = 0; i < upperSize_; ++i)
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

void NeuralNetwork::HiddenNeuron::calculateFix(Neuron** upperNeurons,
                                               double rate,
                                               int batchSize) {
  rate /= batchSize;
  changeFixGateValue(rate * error());
  for (int i = 0; i < upperSize_; ++i)
    changeFixSynapseValue(i, rate * error() * upperNeurons[i]->output());
}

void NeuralNetwork::HiddenNeuron::fix() {
  for (int i = 0; i < upperSize_; ++i) {
    fixSynapse(i);
  }
  fixGate();
}

void NeuralNetwork::HiddenNeuron::clearFixValue() {
  for (int i = 0; i < upperSize_; ++i) {
    fixSynapseValues_[i] = .0;
  }
  fixGateValue_ = .0;
}
// End of HiddenNeuron

// OutputNeuron
void NeuralNetwork::OutputNeuron::back(int expectedOutput) {
  setError((expectedOutput - output()) * dActivation(output()));
}

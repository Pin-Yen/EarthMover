#include "neuron.h"

// HiddenNeuron
NeuralNetwork::HiddenNeuron::HiddenNeuron(const int* args)
    : upperSize_(args[0]) {
  init(upperSize_);
}

NeuralNetwork::HiddenNeuron::~HiddenNeuron() {
  if (synapses_ != NULL) {
    delete [] synapses_;
    delete [] synapseFixValues_;
    delete [] synapseMoments_;
  }
}

void NeuralNetwork::HiddenNeuron::init(int synapseLength) {
  synapses_ = new double[synapseLength];
  for (int i = 0; i < synapseLength; ++i)
    setSynapse(i, initValue());

  synapseFixValues_ = new double[synapseLength];
  synapseMoments_ = new double[synapseLength];

  setBias(-initValue());
}

void NeuralNetwork::HiddenNeuron::forward(Neuron** upperNeurons) {
  double value = bias();
  for (int i = 0; i < upperSize(); ++i)
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
  changeBiasFixValue(rate * error());
  for (int i = 0; i < upperSize(); ++i)
    changeSynapseFixValue(i, rate * error() * upperNeurons[i]->output());
}

void NeuralNetwork::HiddenNeuron::fix() {
  for (int i = 0; i < upperSize(); ++i) {
    fixSynapse(i);
  }
  fixBias();
}

void NeuralNetwork::HiddenNeuron::updateMoment(double rate) {
  for (int i = 0; i < upperSize(); ++i) {
    updateSynapseMoment(i, rate);
  }
  updateBiasMoment(rate);
}

void NeuralNetwork::HiddenNeuron::clearFixValue() {
  for (int i = 0; i < upperSize(); ++i) {
    synapseFixValues_[i] = .0;
  }
  biasFixValue_ = .0;
}
// End of HiddenNeuron

// OutputNeuron
void NeuralNetwork::OutputNeuron::back(int expectedOutput) {
  setError((expectedOutput - output()) * dActivation(output()));
}
// End of outputNeuron

// Convolution neuron
NeuralNetwork::ConvolutionNeuron::ConvolutionNeuron(const int* args)
    : convolutionWidth_(args[1]) {
  setUpperSize(args[0]);
  init(args[0] * args[0]);
}

void NeuralNetwork::ConvolutionNeuron::forward(Neuron** upperNeurons) {
  double value = bias();
  //for (int i = 0; i < upperSize(); ++i)
  //  value += upperNeurons[i]->output() * synapse(i);

  setOutput(activation(value));
}


// End of convolution

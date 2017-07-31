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
  output_ = gate_;
  for (int i = 0; i < u; ++i) {
    output_ += upperNeurons[i] * synapses_[i];
  }
}

void NeuralNetwork::Neuron::backProp() {

}
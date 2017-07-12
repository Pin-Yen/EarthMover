#include "neuralnetwork.hpp"

NeuralNetwork::HiddenNeuron::HiddenNeuron(int upperSize) {
  synapses_.reserve(upperSize);
  for (Synapse* synapse : synapses_) synapse = new Synapse();
  gate_ = new Gate();
  output_ = new Output();
}

NeuralNetwork::HiddenNeuron::~HiddenNeuron() {
  for (Synapse* synapse : synapses_) delete synapse;
  delete gate_;
  delete output_;
}

void NeuralNetwork::HiddenNeuron::forward() {
  double value = -gate_->value;

  for (Synapse* synapse : synapses_)
    value += synapse->upperOutput->value * synapse->value;

  output_.value = activation(outputValue);
}
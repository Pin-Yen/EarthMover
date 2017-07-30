#include "neuralnetwork.hpp"

NeuralNetwork::HiddenNeuron::HiddenNeuron(int upperSize) {
  synapses_.reserve(upperSize);
  for (Synapse* synapse : synapses_) synapse = rand() / RAND_MAX - 0.5;
  gate_ = rand() / RAND_MAX - 0.5;
  output_ = 0.0;
}

void NeuralNetwork::HiddenNeuron::forward() {

}
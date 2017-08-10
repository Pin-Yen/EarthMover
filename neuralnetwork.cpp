#include "neuralnetwork.h"

#include <vector>

NeuralNetwork::~NeuralNetwork() {
  if (neurons_ != NULL) {
    for (int i = 0, n = networkStruct_.size(); i < n; ++i) {
      delete neurons_[i];
    }
    delete neurons_;
  }
}

void NeuralNetwork::init(int inputSize,
                         const std::vector<LayerInf> &networkStruct)
    : networkStruct_(networkStruct) {
  int size = networkStruct.size();
  neurons_ = new Neoron*[size];

  for (int i = 0; i < size; ++i) {
    neurons_[i] = neuronArrayMaker(networkStruct[i].type,
                                   networkStruct[i].length);
  }
  for (int i = 1; i < size; ++i) {
    for (int j = 0; j < networkStruct[i].length; ++j) {
      neurons_[i][j].init(networkStruct[i - 1].length)
    }
  }
}

void NeuralNetwork::train(double input[], int output[], int cycle,
           double rate, double error) {
  for (int i = 0; i < networkStruct_[0].length; ++i) {
    static_cast<InputNeuron>(neurons_[0][i]).output = input[i];
  }

  for (int i = 0; i < cycle; ++i) {
    forward();
    // TODO: back and fix
  }
}

void NeuralNetwork::forward() {
  for (int i = 1; i < networkStruct_.size(); ++i) {
    for (int j = 0; j < networkStruct_[i].length; ++j) {
      neurons_[i][j].forward(neurons_[i - 1], networkStruct_[i - 1].length);
    }
  }
}

Neuron* NeuralNetwork::neuronArrayMaker(Type type, int length) {
  switch (type) {
    case INPUT:
      return new InputNeuron[length];
    case HIDDEN:
      return new HiddenNeuron[length];
    case OUTPUT:
      return new OutputNeuron[length];
    default:
      return NULL;
  }
}
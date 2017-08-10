#include "neuralnetwork.h"

#include <iostream>
#include <vector>

#include "neuron.h"

NeuralNetwork::~NeuralNetwork() {
  if (neurons_ != NULL) {
    for (int i = 0, n = networkDepth_; i < n; ++i) {
      delete neurons_[i];
    }
    delete neurons_;
  }
}

void NeuralNetwork::init(int inputSize, int networkDepth,
                         const LayerInf* networkStruct) {
  inputSize_ = inputSize;
  networkDepth_ = networkDepth;

  networkStruct_ = new LayerInf[networkDepth_];
  for (int i = 0; i < networkDepth; ++i) {
    networkStruct_[i] = networkStruct[i];
  }

  // Allocate all neurons.
  neurons_ = new Neuron*[networkDepth];

  for (int i = 0; i < networkDepth; ++i) {
    neurons_[i] = neuronArrayMaker(networkStruct[i].type,
                                   networkStruct[i].length);
  }

  // Init all neuron in first layer (input layer).
  for (int i = 0; i < networkStruct[0].length; ++i) {
    neurons_[0][i].init(inputSize);
  }
  // Init all neuron except first layer (input layer).
  for (int i = 1; i < networkDepth; ++i) {
    for (int j = 0; j < networkStruct[i].length; ++j) {
      neurons_[i][j].init(networkStruct[i - 1].length);
    }
  }
}

void NeuralNetwork::train(const Data data[], int dataAmount, int cycle,
                          double rate, double allowError) {
  int dataIndex = 0;
  int outputIndex = networkDepth_ - 1;

  // Error array and output array for each neuron.
  double** errors = new double*[outputIndex];
  double** outputs = new double*[outputIndex];
  for (int i = 0; i < networkDepth_; ++i) {
    errors[i] = new double[networkStruct_[i].length];
    outputs[i] = new double[networkStruct_[i].length];
  }

  for (int i = 0; i < cycle; ++i) {
    // Forward prop and back prop.
    forward(data[dataIndex].input, outputs);
    back(data[dataIndex].output, outputs, errors);

    // Calculate error, break if less then allow value.
    if (i % 1111 == 0) {
      double outputError = .0;
      for (int i = 0; i < networkStruct_[outputIndex].length; ++i)
        outputError += abs(errors[outputIndex][i]);

      std::cout << "cycle " << i << ", error: " << outputError << std::endl;
      //if (outputError <= allowError) break;
    }

    // Fix the neuron's weight.
    fix(data[dataIndex].input, outputs, errors, rate);

    // To next data.
    ++dataIndex;
    if (dataIndex == dataAmount) dataIndex = 0;
  }

  // Delete error and output array.
  for (int i = 0; i < networkDepth_; ++i) {
    delete [] errors[i];
    delete [] outputs[i];
  }

  delete [] errors;
  delete [] outputs;
}

void NeuralNetwork::output(const double inputs[], double* output) const {
  int outputIndex = networkDepth_ - 1;

  double** outputs = new double*[outputIndex];
  for (int i = 0; i < networkDepth_; ++i) {
    outputs[i] = new double[networkStruct_[i].length];
  }

  // Forward prop.
  forward(inputs, outputs);

  // Copy output.
  for (int i = 0; i < networkStruct_[outputIndex].length; ++i) {
    output[i] = outputs[outputIndex][i];
  }

  // Delete output array.
  for (int i = 0; i < networkDepth_; ++i)
    delete [] outputs[i];
  delete [] outputs;
}

void NeuralNetwork::forward(const double inputs[], double** outputs) const {
  // First layer
  for (int i = 0; i < networkStruct_[0].length; ++i)
    outputs[0][i] = neurons_[0][i].forward(inputs, inputSize_);

  // Other layer
  for (int i = 1; i < networkDepth_; ++i) {
    for (int j = 0; j < networkStruct_[i].length; ++j) {
      outputs[i][j] = neurons_[i][j].forward(outputs[i - 1],
                                             networkStruct_[i - 1].length);
    }
  }
}

void NeuralNetwork::back(const int expectedOutputs[],
                         double** outputs, double** errors) const {
  int outputIndex = networkDepth_ - 1;
  for (int i = 0; i < networkStruct_[outputIndex].length; ++i) {
    errors[outputIndex][i] = static_cast<OutputNeuron&>(
        neurons_[outputIndex][i]).back(outputs[outputIndex][i],
                                       expectedOutputs[i]);
    //std::cout << "E::" << errors[outputIndex][i] << std::endl;
  }
  for (int i = 0; i < outputIndex; ++i) {
    for (int j = 0; j < networkStruct_[i].length; ++j) {
      errors[i][j] = neurons_[i][j].back(neurons_[i + 1], errors[i + 1],
          networkStruct_[i + 1].length, outputs[i][j], j);
    }
  }
}

void NeuralNetwork::fix(const double inputs[],
                        double** outputs,
                        double** errors,
                        double rate) {
  for (int i = 0; i < networkStruct_[0].length; ++i) {
    neurons_[0][i].fix(inputs, inputSize_, errors[0][i], rate);
  }
  for (int i = 1; i < networkDepth_; ++i) {
    for (int j = 0; j < networkStruct_[i].length; ++j) {
      neurons_[i][j].fix(outputs[i - 1], networkStruct_[i - 1].length,
                         errors[i - 1][j], rate);
    }
  }
}

NeuralNetwork::Neuron* NeuralNetwork::neuronArrayMaker(Type type, int length) {
  switch (type) {
    case NORMAL:
      return new Neuron[length];
    case OUTPUT:
      return new OutputNeuron[length];
    default:
      return NULL;
  }
}
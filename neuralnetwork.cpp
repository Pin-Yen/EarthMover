#include "neuralnetwork.h"

#include <iostream>
#include <vector>

#include "neuron.h"

NeuralNetwork::~NeuralNetwork() {
  if (neurons_ != NULL) {
    for (int i = 0, n = nnDepth_; i < n; ++i) {
      for (int j = 0; j < nnStruct_[i].length; ++j) {
        delete neurons_[i][j];
      }
      delete neurons_[i];
    }
    delete neurons_;
    delete [] nnStruct_;
  }
}

void NeuralNetwork::init(int nnDepth, const LayerInf* nnStruct, int** args) {
  nnDepth_ = nnDepth;

  nnStruct_ = new LayerInf[nnDepth_];
  for (int i = 0; i < nnDepth_; ++i) {
    nnStruct_[i] = nnStruct[i];
  }

  // Allocate all neurons.
  neurons_ = new Neuron**[nnDepth_];

  for (int i = 0; i < nnDepth_; ++i) {
    int length = nnStruct_[i].length;
    neurons_[i] = new Neuron*[length];
    for (int j = 0; j < length; ++j) {
      neurons_[i][j] = neuronMaker(nnStruct_[i].type, args[i]);
    }
  }
}

void NeuralNetwork::train(const Data data[], int dataAmount,
                          int cycle, double allowError,
                          int batchSize, double rate, double momentRate) {
  const int PRINT_ERROR_CYCLE = 10000;
  int dataIndex = 0;
  int outputIndex = nnDepth_ - 1;

  double outputError = .0;

  clearFixValue();
  for (int i = 0; i < cycle; ++i) {
    updateMoment(momentRate);
    clearFixValue();

    for (int b = 0; b < batchSize; ++b) {
      // Forward prop and back prop.
      forward(data[dataIndex].input);
      back(data[dataIndex].output);

      calculateFix(rate, batchSize);

      // Calculate error.
      if (i % PRINT_ERROR_CYCLE == 0) {
        for (int i = 0; i < nnStruct_[outputIndex].length; ++i) {
          outputError += abs(data[dataIndex].output[i] -
              static_cast<OutputNeuron*>(neurons_[outputIndex][i])->output());
        }
      }

      // To next data.
      ++dataIndex;
      if (dataIndex == dataAmount) dataIndex = 0;
    }

    if (i % PRINT_ERROR_CYCLE == 0) {
      outputError /= batchSize;
      std::cout << "cycle " << i << ", error: " << outputError << std::endl;
      if (outputError <= allowError) break;
      outputError = .0;
    }

    // Fix the neuron's weight.
    fix();
  }
}

void NeuralNetwork::predict(const double inputs[]) {
  forward(inputs);

  // Print result.
  std::cout << "input: [" << static_cast<int>(inputs[0]);
  for (int i = 1; i < nnStruct_[0].length; ++i) {
    std::cout << ", " << static_cast<int>(inputs[i]);
  }
  std::cout << "], output: ["
      << static_cast<OutputNeuron*>(neurons_[nnDepth_ - 1][0])->output();
  for (int i = 1; i < nnStruct_[nnDepth_ - 1].length; ++i) {
    std::cout << ", "
        << static_cast<OutputNeuron*>(neurons_[nnDepth_ - 1][i])->output();
  }
  std::cout << "]" << std::endl;
}

void NeuralNetwork::clearFixValue() {
  for (int i = 1; i < nnDepth_; ++i) {
    for (int j = 0; j < nnStruct_[i].length; ++j) {
      neurons_[i][j]->clearFixValue();
    }
  }
}

void NeuralNetwork::forward(const double inputs[]) {
  // Input layer.
  for (int i = 0; i < nnStruct_[0].length; ++i)
    static_cast<InputNeuron*>(neurons_[0][i])->setOutput(inputs[i]);

  // Other layer
  for (int i = 1; i < nnDepth_; ++i) {
    for (int j = 0; j < nnStruct_[i].length; ++j) {
      neurons_[i][j]->forward(neurons_[i - 1]);
    }
  }
}

void NeuralNetwork::back(const int expectedOutputs[]) {
  int outputIndex = nnDepth_ - 1;
  // Output layer.
  for (int i = 0; i < nnStruct_[outputIndex].length; ++i) {
    static_cast<OutputNeuron*>(neurons_[outputIndex][i])->back(
        expectedOutputs[i]);
  }
  // Other layer.
  for (int i = outputIndex - 1; i > 0; --i) {
    for (int j = 0; j < nnStruct_[i].length; ++j) {
      neurons_[i][j]->back(neurons_[i + 1],
                           nnStruct_[i + 1].length, j);
    }
  }
}

void NeuralNetwork::calculateFix(double rate, int batchSize) {
  for (int i = 1; i < nnDepth_; ++i) {
    for (int j = 0; j < nnStruct_[i].length; ++j) {
      neurons_[i][j]->calculateFix(
          neurons_[i - 1], rate, batchSize);
    }
  }
}

void NeuralNetwork::updateMoment(double rate) {
  for (int i = 1; i < nnDepth_; ++i) {
    for (int j = 0; j < nnStruct_[i].length; ++j) {
      neurons_[i][j]->updateMoment(rate);
    }
  }
}

void NeuralNetwork::fix() {
  for (int i = 1; i < nnDepth_; ++i) {
    for (int j = 0; j < nnStruct_[i].length; ++j) {
      neurons_[i][j]->fix();
    }
  }
}

NeuralNetwork::Neuron* NeuralNetwork::neuronMaker(Type type, const int* args) {
  switch (type) {
    case INPUT:
      return new InputNeuron();
    case HIDDEN:
      return new HiddenNeuron(args);
    case OUTPUT:
      return new OutputNeuron(args);
    case CONVOLUTION:
      return new ConvolutionNeuron(args);
    default:
      return NULL;
  }
}
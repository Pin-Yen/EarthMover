#include <iostream>

#include "neuralnetwork.cpp"
#include "neuron.cpp"

int main() {
  NeuralNetwork network;

  NeuralNetwork::LayerInf networkStruct[3];
  networkStruct[0].type = NeuralNetwork::NORMAL;
  networkStruct[0].length = 20;
  networkStruct[1].type = NeuralNetwork::OUTPUT;
  networkStruct[1].length = 1;

  network.init(2, 2, networkStruct);

  NeuralNetwork::Data data[4];
  for (int i = 0; i < 4; ++i) {
    data[i].input = new double[2];
    data[i].output = new int[1];
  }
  data[0].input[0] = 0; data[0].input[1] = 0; data[0].output[0] = 0;
  data[1].input[0] = 1; data[1].input[1] = 0; data[1].output[0] = 1;
  data[2].input[0] = 0; data[2].input[1] = 1; data[2].output[0] = 1;
  data[3].input[0] = 1; data[3].input[1] = 1; data[3].output[0] = 0;

  network.train(data, 4, 10000, 1, .00001);

  double output[1];
  for (int i = 0; i < 4; ++i) {
    network.output(data[i].input, output);
    std::cout << i << ": " << output[0] << std::endl;
  }
}
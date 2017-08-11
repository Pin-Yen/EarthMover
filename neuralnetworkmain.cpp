#include <iostream>
#include <iomanip>

#include "neuralnetwork.cpp"
#include "neuron.cpp"

int main() {
  NeuralNetwork network;

  NeuralNetwork::LayerInf networkStruct[4];
  networkStruct[0].type = NeuralNetwork::INPUT;
  networkStruct[0].length = 2;
  networkStruct[1].type = NeuralNetwork::HIDDEN;
  networkStruct[1].length = 10;
  networkStruct[2].type = NeuralNetwork::HIDDEN;
  networkStruct[2].length = 5;
  networkStruct[3].type = NeuralNetwork::OUTPUT;
  networkStruct[3].length = 1;

  network.init(4, networkStruct);

  NeuralNetwork::Data data[4];

  // XOR data
  double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
  int output[4][1] = {{0}, {1}, {1}, {0}};
  for (int i = 0; i < 4; ++i) {
    data[i].input = input[i];
    data[i].output = output[i];
  }

  network.train(data, 4, 100000, 1);
  std::cout << std::fixed << std::setprecision(3);
  for (int i = 0; i < 4; ++i) {
    network.predict(data[i].input);
  }
}
#include <iostream>
#include <iomanip>

#include "neuralnetwork.cpp"
#include "neuron.cpp"

int main() {
  NeuralNetwork network;

  NeuralNetwork::LayerInf networkStruct[3];
  networkStruct[0].type = NeuralNetwork::INPUT;
  networkStruct[0].length = 2;
  networkStruct[1].type = NeuralNetwork::HIDDEN;
  networkStruct[1].length = 10;
  networkStruct[2].type = NeuralNetwork::OUTPUT;
  networkStruct[2].length = 2;

  network.init(3, networkStruct);

  NeuralNetwork::Data data[4];

  // XOR data
  double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
  int output[4][2] = {{0, 1}, {1, 0}, {1, 0}, {0, 1}};
  for (int i = 0; i < 4; ++i) {
    data[i].input = input[i];
    data[i].output = output[i];
  }

  std::cout << std::fixed << std::setprecision(6);
  // Train network,
  // args is: data, dataAmount, cycle, allowError, batchSize, rate.
  network.train(data, 4, 1000000, .0005, 4, 5.0);

  std::cout << std::fixed << std::setprecision(3);
  for (int i = 0; i < 4; ++i) {
    network.predict(data[i].input);
  }
}
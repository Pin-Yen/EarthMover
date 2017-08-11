#include <iostream>
#include <iomanip>

#include "neuralnetwork.cpp"
#include "neuron.cpp"

int main() {
  NeuralNetwork network;

  NeuralNetwork::LayerInf networkStruct[3];
  networkStruct[0].type = NeuralNetwork::NORMAL;
  networkStruct[0].length = 10;
  networkStruct[1].type = NeuralNetwork::NORMAL;
  networkStruct[1].length = 5;
  networkStruct[2].type = NeuralNetwork::OUTPUT;
  networkStruct[2].length = 1;

  network.init(2, 3, networkStruct);

  NeuralNetwork::Data data[4];

  // XOR data
  double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
  int output[4][1] = {{0}, {1}, {1}, {0}};
  for (int i = 0; i < 4; ++i) {
    data[i].input = input[i];
    data[i].output = output[i];
  }

  network.train(data, 4, 100000, 1);

  double networkOutput[1];
  std::cout << std::fixed << std::setprecision(3) << "result:\n";
  for (int i = 0; i < 4; ++i) {
    network.output(data[i].input, networkOutput);
    std::cout << "input: [" << static_cast<int>(data[i].input[0])
              << ", " << static_cast<int>(data[i].input[1])
              << "], output: [" << networkOutput[0] << "]" << std::endl;
  }
}
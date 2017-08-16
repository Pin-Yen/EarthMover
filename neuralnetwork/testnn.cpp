#include <fstream>
#include <iostream>
#include <iomanip>

#include "../lib/tiny-dnn/tiny_dnn/tiny_dnn.h"
using namespace tiny_dnn;

int main() {
  network<sequential> nn;

  nn.load("nn");
  std::cout << "load nn" << std::endl;

  std::vector<vec_t> input_data;

  std::vector<vec_t> desired_out;

  std::ifstream file("cnndata.txt");

  int dataIndex;
  while (file >> dataIndex) {
    vec_t in;
    vec_t out;
    for (int i = 0; i < 900; ++i) {
      int d;
      file >> d;
      in.push_back(d);
    }
    for (int i = 0; i < 225; ++i) {
      int d;
      file >> d;
      out.push_back(d);
    }
    input_data.push_back(in);
    desired_out.push_back(out);
  }
  std::cout << "Reading " << dataIndex << " datas" << std::endl;
  file.close();

  double loss = nn.get_loss<cross_entropy>(input_data, desired_out);
          std::cout << "cross entropy loss: " << loss << std::endl;

  loss = nn.get_loss<mse>(input_data, desired_out);
          std::cout << "mse loss: " << loss << std::endl;

  std::cout << std::fixed << std::setprecision(1);
  for (int i = 0; i < dataIndex; i+= 10) {
    vec_t result = nn.predict(input_data[i]);

    std::cout << i << "-out:" << std::endl;
    for (int r = 0; r < 15; ++r) {
      for (int c = 0; c < 15; ++c) {
        std::cout << abs(result[r * 15 + c]) << " ";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "desired out:\n";
    for (int r = 0; r < 15; ++r) {
      for (int c = 0; c < 15; ++c) {
        std::cout << abs(desired_out[i][r * 15 + c]) << " ";
      }
      std::cout << std::endl;
    }
    std::cout << "----------------------" << std::endl;
  }
}
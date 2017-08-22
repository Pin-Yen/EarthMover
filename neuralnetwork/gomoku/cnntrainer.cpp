#include <fstream>
#include <iostream>
#include <iomanip>

#include "../../lib/tiny-dnn/tiny_dnn/tiny_dnn.h"
using namespace tiny_dnn;

int main() {
  char mode = ' ';
  do {
    std::cout << "Keep training origin network? y/n";
    std::cin >> mode;
  } while (mode != 'N' && mode != 'n' && mode != 'Y' && mode != 'y');

  network<sequential> nn;
  if (mode == 'N' || mode == 'n') {
    nn << convolutional_layer<leaky_relu>(15, 15, 5, 3, 16, padding::same)
       << convolutional_layer<leaky_relu>(15, 15, 3, 16, 16, padding::same)
       << convolutional_layer<leaky_relu>(15, 15, 3, 16, 16, padding::same)
       << convolutional_layer<leaky_relu>(15, 15, 3, 16, 16, padding::same)
       << convolutional_layer<leaky_relu>(15, 15, 3, 16, 16, padding::same)
       << convolutional_layer<leaky_relu>(15, 15, 3, 16, 16, padding::same)
       << convolutional_layer<softmax>(15, 15, 3, 16, 1, padding::same);
  } else {
    nn.load("nn");
    std::cout << "load nn" << std::endl;
  }

  size_t batch_size, epochs;

  std::cout << "batch size: ";
  std::cin >> batch_size;
  std::cout << "epoch: ";
  std::cin >> epochs;

  adagrad opt;

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
  std::cout << "Finish reading " << dataIndex << " datas." << std::endl;
  file.close();

  for (int i = 0; i < nn.depth(); ++i) {
    std::cout << "layer " << i << std::endl;
    std::cout << "layer type: " << nn[i]->layer_type() << std::endl;
    std::cout << "input: " << nn[i]->in_data_size()
              << "(" << nn[i]->in_data_shape() << ")" << std::endl;
    std::cout << "output: " << nn[i]->out_data_size()
              << "(" << nn[i]->out_data_shape() << ")" << std::endl;
    std::cout << "=================================" << std::endl;
  }

  int count = 0;

  nn.fit<mse>(opt, input_data, desired_out, batch_size, epochs,
      // called for each mini-batch
      [&]() {
        //
      },
      // called for each epoch
      [&]() {
        if (count % 10 == 0) {
          double loss = nn.get_loss<mse>(input_data, desired_out);
          std::cout << "Epochs " << count
                    << ", mse loss: " << loss << std::endl;

          if (count % 50 == 0 && count > 0) {
            nn.save("nn");
            std::cout << "saved nn" << std::endl;
          }
        }

        ++count;
      });

  nn.save("nn");
  std::cout << "saved nn" << std::endl;

  image<> img = nn.at<convolutional_layer<leaky_relu>>(0).weight_to_image();
  img.write("layer0.png");
}
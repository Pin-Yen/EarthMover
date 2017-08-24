#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

#include "../../lib/tiny-dnn/tiny_dnn/tiny_dnn.h"
using namespace tiny_dnn;
using namespace tiny_dnn::layers;

bool checkArgs(int argc, char const *argv[]);

int loadData(const std::string fileName,
             std::vector<vec_t> *input,
             std::vector<vec_t> *output);

int main(int argc, char const *argv[]) {
  // Check the rationality of args.
  if (!checkArgs(argc, argv)) return 1;

  // Get args.
  char mode = argv[1][0];
  int batchSize = std::stoi(argv[2]);
  int epochSize = std::stoi(argv[3]);

  std::vector<vec_t> input, output;
  std::cout << "Loading training data...\n";
  int trainingAmount = loadData("cnndata.txt", &input, &output);

  std::vector<vec_t> testInput, testOutput;
  std::cout << "Loading testing data...\n";
  int testingAmount = loadData("cnntestdata.txt", &testInput, &testOutput);

  // Create network.
  network<sequential> nn;
  if (mode == 'N' || mode == 'n') {
    std::cout << "Creating new network... ";
    nn << conv<leaky_relu>(19, 19, 5, 3, 16, padding::same)
       << batch_norm(19 * 19, 16)
       << conv<leaky_relu>(19, 19, 3, 16, 16, padding::same)
       << batch_norm(19 * 19, 16)
       << conv<leaky_relu>(19, 19, 3, 16, 16, padding::same)
       << batch_norm(19 * 19, 16)
       << conv<leaky_relu>(19, 19, 3, 16, 16, padding::same)
       << batch_norm(19 * 19, 16)
       << conv<leaky_relu>(19, 19, 3, 16, 16, padding::same)
       << batch_norm(19 * 19, 16)
       << conv<leaky_relu>(19, 19, 3, 16, 16, padding::same)
       << batch_norm(19 * 19, 16)
       << conv<leaky_relu>(19, 19, 3, 16, 16, padding::same)
       << batch_norm(19 * 19, 16)
       << conv<leaky_relu>(19, 19, 3, 16, 16, padding::same)
       << batch_norm(19 * 19, 16)
       << conv<softmax>(19, 19, 3, 16, 1, padding::same);
    std::cout << "done" << std::endl;
  } else {
    std::cout << "Loading origin network.... ";
    nn.load("nn");
    std::cout << "done" << std::endl;
  }

  // Print network structure.
  std::cout << "Network structure:\n";
  for (int i = 0; i < nn.depth(); ++i) {
    std::cout << "layer " << i
              << ", type: " << nn[i]->layer_type() << "\n"
              << "input: " << nn[i]->in_data_size()
              << "(" << nn[i]->in_data_shape() << ")"
              << ", output: " << nn[i]->out_data_size()
              << "(" << nn[i]->out_data_shape() << ")"
              << std::endl;
  }

  int count = 0;
  adagrad opt;
  nn.fit<mse>(opt, input, output, batchSize, epochSize,
      // called for each mini-batch
      [&]() {
        //
      },
      // called for each epoch
      [&]() {
        std::cout << "Epoch " << ++count;
        int correct = 0;
        for (int i = 0; i < trainingAmount; i += 10) {
          if (output[i][nn.predict_label(input[i])] == 1)
            ++correct;
        }
        std::cout << "\ncorrect rate in 10 precent of training data: "
                  << static_cast<double>(correct) / (trainingAmount / 10);
        correct = 0;
        for (int i = 0; i < testingAmount; ++i) {
          if (testOutput[i][nn.predict_label(testInput[i])] == 1)
            ++correct;
        }
        std::cout << "\ncorrect rate in testing data: "
                  << static_cast<double>(correct) / testingAmount
                  << std::endl;

        if (count % 10 == 0 && count > 0) {
          nn.save("nn");
          std::cout << "saved nn" << std::endl;
        }
      });

  nn.save("nn");
  std::cout << "saved nn" << std::endl;

  // visualize activations of recent input
  //image<> img = nn[nn.depth() - 1]->output_to_image();
  //img.write("output.png");

  image<> img = nn.at<conv<leaky_relu>>(0).weight_to_image();
  img.write("layer0.png");
}

bool checkArgs(int argc, char const *argv[]) {
  const std::string usage =
      "Usage ./cnntrainer keep_training batch_size epoch_size";

  if (argc != 4) {
    std::cout << usage << std::endl;
    return false;
  }

  if (static_cast<std::string>(argv[1]).length() != 1) {
    std::cout << usage
              << "\nkeep_training should be 'y' or 'n' " << std::endl;
    return false;
  }

  if (argv[1][0] != 'Y' && argv[1][0] != 'y' &&
      argv[1][0] != 'N' && argv[1][0] != 'n') {
    std::cout << usage
              << "\nkeep_training should be 'y' or 'n' " << std::endl;
    return false;
  }

  int value = std::stoi(argv[2]);
  if (value <= 0) {
    std::cout << usage
              << "\nbatch must be grater then 0"<< std::endl;
    return false;
  }

  value = std::stoi(argv[3]);
  if (value <= 0) {
    std::cout << usage
              << "\nepoch must be grater then 0"<< std::endl;
    return false;
  }

  return true;
}

int loadData(const std::string fileName,
             std::vector<vec_t> *input,
             std::vector<vec_t> *output) {
  std::ifstream file(fileName);
  int count;
  while (file >> count) {
    std::cout << "reading the " << count << "th data.\r" << std::flush;
    vec_t in;
    vec_t out;
    for (int i = 0; i < 1083; ++i) {
      int d;
      file >> d;
      in.push_back(d);
    }
    for (int i = 0; i < 361; ++i) {
      int d;
      file >> d;
      out.push_back(d);
    }
    input->push_back(in);
    output->push_back(out);
  }
  std::cout << "Finish reading " << count << " data." << std::endl;
  file.close();

  return count;
}
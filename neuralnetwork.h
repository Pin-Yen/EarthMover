#ifndef NEURALNETWORK_H_
#define NEURALNETWORK_H_

#include <vector>
#include <iostream>

class NeuralNetwork {
 public:
  NeuralNetwork();
  ~NeuralNetwork();

 protected:
  class Neuron {
   public:
    Neuron();
    Neuron(int upperSize);
    ~Neuron();

    void forward(Neuron* upperNeurons, int upperSize);

    double backProp(Neuron* lowerNeurons, int lowerSize);

   protected:
    double initValue() {
      return static_cast<double>(rand()) / RAND_MAX;
    }

   private:
    double activation(double input) { return input > 0.0 ? input : 0.0; }
    double dActivation(double input) { return input > 0.0 ? 1.0 : 0.0; }

    double* synapses_;
    double gate_;
    double output_;
  };
};

#endif  // NEURALNETWORK_H_
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

    void forward();

    void backProp();

   protected:
    double initValue() {
      return static_cast<double>(rand()) / RAND_MAX - 0.5;
    }

   private:
    double* synapses_;
    double gate_;
    double output_;
  };
};

#endif  // NEURALNETWORK_H_
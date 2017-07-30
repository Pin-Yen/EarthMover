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
    ~Neuron();

    virtual void forward() = 0;

    virtual void backProp() = 0;
  };

  class HiddenNeuron : public Neuron {
   public:
    HiddenNeuron(int upperSize);

    ~HiddenNeuron();

    virtual void forward() override;

    virtual void backProp() override;

    double activation(double input);

    double dActivation(double input);

   private:
    std::vector<double*> synapses_;
    double gate_;
    double output_;
  };
};

#endif  // NEURALNETWORK_H_
#ifndef NEURALNETWORK_H_
#define NEURALNETWORK_H_

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

    double backProp(Neuron* lowerNeurons, double* lowerDifference,
                    int lowerSize, int currentIndex);

   protected:
    double initValue() {
      return static_cast<double>(rand()) / RAND_MAX;
    }

    void setSynapes(int index, double value) { synapses_[index] = value; }
    void setGate(double value) { gate_ = value; }
    void setOutput(double value) { output_ = value; }

    double synapse(int index) { return synapses_[index]; }
    double gate() { return gate_; }
    double output() { return output_; }

   private:
    double activation(double input) { return input > 0.0 ? input : 0.0; }
    double dActivation(double input) { return input > 0.0 ? 1.0 : 0.0; }

    double* synapses_;
    double gate_;
    double output_;
  };

  class OutputNeuron : public Neuron {
   public:
    OutputNeuron() : Neuron();
    OutputNeuron(int upperSize) : Neuron(upperSize);
    ~OutputNeuron();

    double output() { return Neuron::output(); }
  };
};

#endif  // NEURALNETWORK_H_

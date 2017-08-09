#ifndef NEURALNETWORK_H_
#define NEURALNETWORK_H_

#include <iostream>
#include <vector>

class NeuralNetwork {
 public:
  NeuralNetwork() { neurons_ = NULL; }
  ~NeuralNetwork();

  void init(int inputSize, const std::vector<int> &networkStruct);

  void train();

 protected:
  // base neuron
  class Neuron {
   public:
    Neuron();
    Neuron(int upperSize);
    ~Neuron();

    // forwer propagation, and store the output
    void forward(Neuron* upperNeurons, int upperSize);

    // back propagation, and return the error
    double back(Neuron* lowerNeurons, double* lowerError,
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
    OutputNeuron() : Neuron() {}
    OutputNeuron(int upperSize) : Neuron(upperSize) {}
    ~OutputNeuron();

    // override back propagation
    double back(double expectedOutput) { return value() - expectedOutput; }

    double output() { return Neuron::output(); }
  };

 private:
  Neuron** neurons_;
  std::vector<int> networkStruct_;
};

#endif  // NEURALNETWORK_H_

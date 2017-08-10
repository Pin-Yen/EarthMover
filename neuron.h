// Neuron inheritance structure
//
// Neuron -------- OutputNeuron

#ifndef NEURON_H_
#define NEURON_H_

#include <math.h>
#include <stdlib.h>

#include "neuralnetwork.h"

// Base neuron.
// Contains dynamic synapse array, gate.
// Activation using ReLU.
class NeuralNetwork::Neuron {
 public:
  Neuron() { synapses_ = NULL; }
  ~Neuron();

  // Init neuron.
  virtual void init(int upperSize);

  // Forwer propagation, and return the output.
  virtual double forward(const double upperOutputs[], int upperSize) const;

  // Back propagation, and return the error.
  virtual double back(const Neuron lowerNeurons[],
                      const double lowerErrors[],
                      int lowerSize,
                      double currentOutput,
                      int currentIndex) const;

  // Fix the neuron's weight.
  virtual void fix(const double upperOutputs[],
                   int upperSize,
                   double currentError,
                   double rate);

 protected:
  double initValue() {
    return static_cast<double>(rand()) / RAND_MAX - .5;
  }

  void fixGate(double value) { gate_ += value; }
  void fixSynapse(int index, double value) { synapses_[index] += value; }

  // setter
  void setSynapse(int index, double value) { synapses_[index] = value; }
  void setGate(double value) { gate_ = value; }

  // getter
  double synapse(int index) const { return synapses_[index]; }
  double gate() const { return gate_; }

 private:
  virtual double activation(double input) const {
    return input > .0 ? input : .0;
  }
  virtual double dActivation(double input) const {
    return input > .0 ? 1.0 : .0;
  }

  double* synapses_;
  double gate_;
};

// Output neuron.
// Activation using sigmoid to fix the output range from 0 to 1.
class NeuralNetwork::OutputNeuron final : public NeuralNetwork::Neuron {
 public:
  OutputNeuron() : Neuron() {}
  ~OutputNeuron() {}

  // Back propagation, and return the error.
  virtual double back(double currentOutput, int expectedOutput) const final;

 private:
  virtual double activation(double input) const final {
    return 1 / (1 + exp(-input));
  }
  virtual double dActivation(double input) const final {
    return input * (1 - input);
  }
};

#endif  // NEURON_H_

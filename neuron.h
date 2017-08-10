// Neuron inheritance structure
//
// (base neuron, +output) (+synapses and gate)  (+public output getter)
//       Neuron -------------- HiddenNeuron -------- OutputNeuron
//                   |
//                   |   (+public output setter)
//                   --------- InputNeuron
//
#ifndef NEURON_H_
#define NEURON_H_

#include "neuralnetwork.h"
#include <math.h>
#include <stdlib.h>

// base neuron
class Neuron {
 public:
  Neuron() {}
  ~Neuron() {}

  // init neuron
  virtual void init(int upperSize) = 0;

  // forwer propagation, and store the output
  virtual void forward(Neuron* upperNeurons, int upperSize) = 0;

  // back propagation, and return the error
  virtual double back(Neuron* lowerNeurons, double* lowerError,
                      int lowerSize, int currentIndex) = 0;

 protected:
  double initValue() {
    return static_cast<double>(rand()) / RAND_MAX;
  }

  // setter
  void setOutput(double value) { output_ = value; }

  // getter
  double output() { return output_; }

 private:
  double output_;
};

// Input neuron, contains public output setter.
class InputNeuron final : public Neuron {
 public:
  InputNeuron() {}
  ~InputNeuron() {}

  void setOutput(double value) { Neuron::setOutput(value); }
};

// Hidden neuron, contains dynamic synapse array, gate.
// Activation using ReLU.
class HiddenNeuron : public Neuron {
 public:
  HiddenNeuron() { synapses_ = NULL; }
  ~HiddenNeuron();

  // init neuron
  virtual void init(int upperSize) final;

  // forwer propagation, and store the output
  virtual void forward(Neuron* upperNeurons, int upperSize) final;

  // back propagation, and return the error
  virtual double back(Neuron* lowerNeurons, double* lowerError,
                      int lowerSize, int currentIndex) override;

 protected:
  // setter
  void setSynapes(int index, double value) { synapses_[index] = value; }
  void setGate(double value) { gate_ = value; }

  // getter
  double synapse(int index) { return synapses_[index]; }
  double gate() { return gate_; }

 private:
  virtual double activation(double input) { return input > .0 ? input : .0; }
  virtual double dActivation(double input) { return input > .0 ? 1.0 : .0; }

  double* synapses_;
  double gate_;
};

// Output neuron, contains public output getter.
// Activation using sigmoid to fix the output range from 0 to 1.
class OutputNeuron final : public HiddenNeuron {
 public:
  OutputNeuron() : HiddenNeuron() {}
  ~OutputNeuron() {}

  // back propagation
  virtual double back(double expectedOutput) final {
    return value() - expectedOutput;
  }

  double output() { return Neuron::output(); }

 private:
  virtual double activation(double input) {
    return 1 / (1 + exp(-input));
  }
  virtual double dActivation(double input) {
    return input * (1 - input);
  }
};

#endif  // NEURON_H_

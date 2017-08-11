// Neuron inheritance structure
//
// (basic abstract neuron)   (+synapses and gate)  (+public output getter)
//         Neuron -------------- HiddenNeuron -------- OutputNeuron
//                     |
//                     |    (+public output setter)
//                     --------- InputNeuron
//

#ifndef NEURON_H_
#define NEURON_H_

#include "neuralnetwork.h"

#include <math.h>
#include <stdlib.h>

// base neuron
class NeuralNetwork::Neuron {
 public:
  Neuron() {}
  ~Neuron() {}

  // Init neuron.
  virtual void init(int upperSize) = 0;

  // Forwer propagation.
  virtual void forward(Neuron** upperNeurons, int upperSize) = 0;

  // Back propagation.
  virtual void back(Neuron** lowerNeurons,
                    int lowerSize,
                    int currentIndex) = 0;

  virtual void fix(Neuron** upperNeurons, int upperSize,
                   double rate) = 0;

  // Getter.
  double output() const { return output_; }

 protected:
  double initValue() const {
    return static_cast<double>(rand()) / RAND_MAX - .5;
  }

  // Setter.
  void setOutput(double value) { output_ = value; }

 private:
  double output_;
};

// Input neuron, contains public output setter.
class NeuralNetwork::InputNeuron final : public NeuralNetwork::Neuron {
 public:
  InputNeuron() {}
  ~InputNeuron() {}

  virtual void init(int upperSize) final { std::cout << "I" << std::endl;}
  virtual void forward(Neuron** upperNeurons, int upperSize) final {}
  virtual void back(Neuron** lowerNeurons,
                    int lowerSize,
                    int currentIndex) final {}
  virtual void fix(Neuron** upperNeurons, int upperSize,
                   double rate) final {}

  void setOutput(double value) { Neuron::setOutput(value); }
};

// Hidden neuron, contains dynamic synapse array, gate.
// Activation using ReLU.
class NeuralNetwork::HiddenNeuron : public NeuralNetwork::Neuron {
 public:
  HiddenNeuron() { synapses_ = NULL; std::cout << synapses_ << std::endl;}
  ~HiddenNeuron();

  // init neuron
  virtual void init(int upperSize) final;

  // forwer propagation, and store the output
  virtual void forward(Neuron** upperNeurons, int upperSize) final;

  // back propagation, and store the error
  virtual void back(Neuron** lowerNeurons,
                    int lowerSize,
                    int currentIndex) override;

  virtual void fix(Neuron** upperNeurons, int upperSize, double rate) final;

 protected:
  void fixGate(double value) { gate_ += value; }
  void fixSynapse(int index, double value) { synapses_[index] += value; }

  // setter
  void setSynapse(int index, double value) { synapses_[index] = value; }
  void setGate(double value) { gate_ = value; }
  void setError(double value) { error_ = value; }

  // getter
  double synapse(int index) const { return synapses_[index]; }
  double gate() const { return gate_; }
  double error() const { return error_; }

 private:
  virtual double activation(double input) const {
    return input > .0 ? input : .0;
  }
  virtual double dActivation(double input) const {
    return input > .0 ? 1.0 : .0;
  }
 public:
  double* synapses_;
  double gate_;
  double error_;
};

// Output neuron, contains public output getter.
// Activation using sigmoid to fix the output range from 0 to 1.
class NeuralNetwork::OutputNeuron final : public NeuralNetwork::HiddenNeuron {
 public:
  OutputNeuron() : HiddenNeuron() {}
  ~OutputNeuron() {}

  // back propagation
  virtual void back(int expectedOutput) final;

  double error() const { return HiddenNeuron::error(); }

 private:
  virtual double activation(double input) const {
    return 1 / (1 + exp(-input));
  }
  virtual double dActivation(double input) const {
    return input * (1 - input);
  }
};

#endif  // NEURON_H_

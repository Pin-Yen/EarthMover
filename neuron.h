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
  virtual ~Neuron() {}

  // Init neuron.
  virtual void init(int upperSize) = 0;

  // Forwer propagation.
  virtual void forward(Neuron** upperNeurons) = 0;

  // Back propagation.
  virtual void back(Neuron** lowerNeurons,
                    int lowerSize,
                    int currentIndex) = 0;

  virtual void calculateFix(Neuron** upperNeurons,
                            double rate, int batchSize) = 0;

  virtual void fix() = 0;

  virtual void clearFixValue() = 0;

  virtual void updateMoment(double rate) = 0;

  // Getter.
  double output() const { return output_; }

 protected:
  double initValue() const {
    return static_cast<double>(rand()) / RAND_MAX - .5;
  }

  // Setter.
  void setOutput(double val) { output_ = val; }

 private:
  double output_;
};

// Input neuron, contains public output setter.
// Input neuron don't need any calculate method,
// it's only function is to store the input.
class NeuralNetwork::InputNeuron final : public NeuralNetwork::Neuron {
 public:
  InputNeuron() {}
  virtual ~InputNeuron() {}

  // setter
  void setOutput(double val) { Neuron::setOutput(val); }

 private:
  virtual void init(int) final {}
  virtual void forward(Neuron**) final {}
  virtual void back(Neuron**, int, int) final {}
  virtual void calculateFix(Neuron**, double, int) final {}
  virtual void fix() final {}
  virtual void clearFixValue() final {}
  virtual void updateMoment(double) final {};
};

// Hidden neuron, contains dynamic synapse array, gate.
// Activation using ReLU.
class NeuralNetwork::HiddenNeuron : public NeuralNetwork::Neuron {
 public:
  HiddenNeuron() { synapses_ = NULL; }
  virtual ~HiddenNeuron();

  // init neuron
  virtual void init(int upperSize) final;

  // forwer propagation, and store the output
  virtual void forward(Neuron** upperNeurons) final;

  // back propagation, and store the error
  virtual void back(Neuron** lowerNeurons,
                    int lowerSize,
                    int currentIndex) override;

  virtual void calculateFix(Neuron** upperNeurons,
                            double rate, int batchSize) override;

  virtual void fix() final;

  virtual void clearFixValue() override;

  virtual void updateMoment(double rate) override;

 protected:
  // Fix synapse and gate by fixValue and moment.
  void fixSynapse(int index) {
    synapseFixValues_[index] += synapseMoments_[index];
    synapses_[index] += synapseFixValues_[index];
  }
  void fixGate() {
    gateFixValue_ += gateMoments_;
    gate_ += gateFixValue_;
  }

  // Change synapses and gate's fix value.
  void changeSynapseFixValue(int index, double val) {
    synapseFixValues_[index] += val;
  }
  void changeGateFixValue(double val) {
    gateFixValue_ += val;
  }

  // Update moment.
  void updateSynapseMoment(int index, double rate) {
    synapseMoments_[index] = synapseFixValues_[index] * rate;
  }
  void updateGateMoment(double rate) { gateMoments_ = gateFixValue_ * rate; }

  // setter
  void setSynapse(int index, double val) { synapses_[index] = val; }
  void setGate(double val) { gate_ = val; }
  void setError(double val) { error_ = val; }

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
  double* synapseMoments_;
  double* synapseFixValues_;
  double gate_;
  double gateMoments_;
  double gateFixValue_;
  double error_;

  int upperSize_;
};

// Output neuron, contains public output getter.
// Activation using sigmoid to fix the output range from 0 to 1.
class NeuralNetwork::OutputNeuron final : public NeuralNetwork::HiddenNeuron {
 public:
  OutputNeuron() : HiddenNeuron() {}
  virtual ~OutputNeuron() {}

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

// Neuron inheritance structure
//
// (basic abstract neuron)   (+synapses and bias)  (+public output getter)
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
  virtual void forward(Neuron**) final {}
  virtual void back(Neuron**, int, int) final {}
  virtual void calculateFix(Neuron**, double, int) final {}
  virtual void fix() final {}
  virtual void clearFixValue() final {}
  virtual void updateMoment(double) final {};
};

// Hidden neuron, contains dynamic synapse array and bias.
// Activation using ReLU.
class NeuralNetwork::HiddenNeuron : public NeuralNetwork::Neuron {
 public:
  HiddenNeuron() {}
  HiddenNeuron(const int* args);
  virtual ~HiddenNeuron();

  // forwer propagation, and store the output
  virtual void forward(Neuron** upperNeurons) override;

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
  // Init neuron,
  // call by this neuron's constructor or derived class' constructor.
  void init(int synapseLength);

  // Fix by fixValue and moment.
  void fixSynapse(int index) {
    synapseFixValues_[index] += synapseMoments_[index];
    synapses_[index] += synapseFixValues_[index];
  }
  void fixBias() {
    biasFixValue_ += biasMoments_;
    bias_ += biasFixValue_;
  }

  // Change fix value.
  void changeSynapseFixValue(int index, double val) {
    synapseFixValues_[index] += val;
  }
  void changeBiasFixValue(double val) {
    biasFixValue_ += val;
  }

  // Update moment.
  void updateSynapseMoment(int index, double rate) {
    synapseMoments_[index] = synapseFixValues_[index] * rate;
  }
  void updateBiasMoment(double rate) { biasMoments_ = biasFixValue_ * rate; }

  // setter
  void setSynapse(int index, double val) { synapses_[index] = val; }
  void setBias(double val) { bias_ = val; }
  void setError(double val) { error_ = val; }
  void setUpperSize(int val) { upperSize_ = val; }

  // getter
  double synapse(int index) const { return synapses_[index]; }
  double bias() const { return bias_; }
  double error() const { return error_; }
  int upperSize() const { return upperSize_; }

 private:
  virtual double activation(double input) const {
    return input > .0 ? input : .0;
  }
  virtual double dActivation(double input) const {
    return input > .0 ? 1.0 : .0;
  }

  double* synapses_;
  double* synapseMoments_;
  double* synapseFixValues_;
  double bias_;
  double biasMoments_;
  double biasFixValue_;
  double error_;

  int upperSize_;
};

// Output neuron, contains public output getter.
// Activation using sigmoid to fix the output range from 0 to 1.
class NeuralNetwork::OutputNeuron final : public NeuralNetwork::HiddenNeuron {
 public:
  OutputNeuron() {}
  OutputNeuron(const int* args) : HiddenNeuron(args) {}
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

// Convolution Neuron
class NeuralNetwork::ConvolutionNeuron : public NeuralNetwork::HiddenNeuron {
 public:
  ConvolutionNeuron() {}
  ConvolutionNeuron(const int* args);
  ~ConvolutionNeuron() {};

 private:
  int convolutionWidth_;
};

#endif  // NEURON_H_

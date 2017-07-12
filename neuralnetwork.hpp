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

    struct Output;
    struct Synapse {
      Synapse() { value = rand() / RAND_MAX - 0.5; }

      double value;

      Output* upperOutput;
    };

    struct Gate {
      Gate() { value = rand() / RAND_MAX - 0.5; }

      double value;
    };

    struct Output {
      Output() {}
      double value;

      std::vector<Synapse*> lowerSynapse;
    };

    virtual void forward() = 0;

    virtual void backProp() = 0;

    virtual double activation(double input) = 0;

    virtual double dActivation(double input) = 0;
  };

  class HiddenNeuron : public Neuron {
   public:
    HiddenNeuron(int upperSize);

    ~HiddenNeuron();

    virtual void forward() override;

    virtual void backProp() override;

    virtual double activation(double input) override;

    virtual double dActivation(double input) override;

   private:
    std::vector<Synapse*> synapses_;
    Gate* gate_;
    Output* output_;
  };
};
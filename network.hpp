#include <vector>
#include <iostream>

class Network {
 public:
  Network();
  ~Network();

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

    struct Output{
      double value;

      Synapse* lowerSynapse[];
    };

    virtual void forward() = 0;

    virtual void backProp() = 0;
  };

  class HiddenNeuron : public Neuron {
   public:
    HiddenNeuron(int upperSize) {
      synapses_.reserve(upperSize);
      for (Synapse* synapse : synapses_) synapse = new Synapse();
      gate_ = new Gate();
      output_ = new Output();
    }

    ~HiddenNeuron() {
      for (Synapse* synapse : synapses_) delete synapse;
      delete gate_;
      delete output_;
    }

   private:
    std::vector<Synapse*> synapses_;
    Gate* gate_;
    Output* output_;
  };
};
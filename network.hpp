class Network {
 public:
  Network();
  ~Network();

 protected:
  class Neuron {
   public:
    Neuron();
    ~Neuron();

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
    Neuron(int upperSize) {
      for (int i = 0; i < upperSize; ++i)
        synapse_[i] = new Synapse();

      gate_ = new Gate();

      output_ = new Output();
    }

    ~Neuron() {
      delete [] synapse_;
      delete gate_;
      delete output_;
    }

   private:
    Synapse* synapse_[];
    Gate* gate_;
    Output* output_;
  }
};
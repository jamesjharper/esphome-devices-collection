#pragma once


namespace esphome {
namespace output {

class RoutedDestination {
    AggregatedXyOutput* dest_aggregator = 0;
    XyOutput* dest_output = 0;

    float percentage = 1.0f;
    SourceHandle source_handle = 0;

    public:

    void set_destination_aggregator(AggregatedXyOutput* dest_aggregator) {
      this->dest_aggregator = dest_aggregator;     
    }

    void set_destination_output(XyOutput* dest_output) {
      this->dest_output = dest_output;     
    }

    void set_percentage(float percentage) {
      this->percentage = percentage;
    }

    void init() {
      if(this->dest_aggregator) 
        this->source_handle = dest_aggregator->add_source(this->percentage);
    }

    void write_state_to_destination(float X, float Y, float Z) {
      if(this->dest_aggregator) {
        this->dest_aggregator->write_source_state(this->source_handle, X, Y, Z);
      }

      if(this->dest_output) {
        this->dest_output->write_state(
          X * this->percentage, 
          Y * this->percentage,  
          Z * this->percentage);
      }
    }
};



class XyOutputSplitter : 
  public XyOutput {

  std::vector<SplitterDestination> destinations;

  public:

  void add_destination(SplitterDestination dest) { 
    dest.init();
    this->destinations.push_back(dest);
  }

  void set_state(float X, float Y, float Z) override {
    for (auto& dest : this->destinations) {
      dest.write_state_to_destination(X, Y, Z);
    }
  }
};

}  // namespace output
}  // namespace esphome

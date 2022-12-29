#pragma once

#include "esphome/components/multi_sink_float_output/multi_sink_float_output.h"

namespace esphome {
namespace output {

class SinkTarget {
    MultiSinkOutput* target_sink = NULL;
    float aggregate_percentage = 1.0f;
    SourceHandle source_handle = 0;

    public:

    void set_target(MultiSinkOutput* target_sink) {
      this->target_sink = target_sink;     
    }

    void set_aggregate_percentage(float aggregate_percentage) {
      this->aggregate_percentage = aggregate_percentage;
    }

    void init() {
      if(this->target_sink) 
        this->source_handle = target_sink->add_source(this->aggregate_percentage);
    }

    void write_state_to_target(float value) {
      if(!this->target_sink) 
        return;

      this->target_sink->write_source_state(value, this->source_handle);
    }
};

class MultiSinkSourceFloatOutput : 
  public esphome::Component, 
  public esphome::output::FloatOutput,
  public MultiSinkOutput {

  struct SinkRef {
    MultiSinkOutput* target_sink;
    SourceHandle source_handle;
  };

  FloatValueAggregator aggregated_value;
  std::vector<SinkTarget> targets;

  public:

  void add_sink(SinkTarget target) { 
    target.init();
    this->targets.push_back(target);
  }

  SourceHandle add_source(float aggregate_percentage) override { 
    return this->aggregated_value.add_source(aggregate_percentage);
  }

  void write_source_state(float value, SourceHandle handle) override { 
    this->aggregated_value.update_source_state(value, handle);
    this->apply_state();
  }

  void write_state(float value) override {
    this->aggregated_value.update_value(value);
    this->apply_state();
  }

  void apply_state() {
    auto value = this->aggregated_value.get_aggregate_value();
    for (auto& target : this->targets) {
      target.write_state_to_target(value);
    }
  }
};

}  // namespace output
}  // namespace esphome

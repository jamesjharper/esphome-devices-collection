#pragma once

#include "esphome/components/multi_sink_float_output/multi_sink_float_output.h"

namespace esphome {
namespace output {

class SinkTarget {
    MultiSinkOutput* target_sink = NULL;
    float max_attenuation = 1.0f;
    SourceHandle source_handle = 0;

    public:

    void set_target(MultiSinkOutput* target_sink) {
      this->target_sink = target_sink;
    }

    void set_target_source_handle(SourceHandle source_handle) {
      this->source_handle = source_handle;
    }

    void set_max_attenuation(float max_attenuation) {
      this->max_attenuation = max_attenuation;
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
  std::vector<SinkRef> targets;

  public:

  void add_sink(SinkTarget* target_sink]) { 
    assert (target_sink != NULL);
    auto handle = target_sink->add_source(max_attenuation);
    this->targets.push_back(SinkRef {
      target_sink: target_sink, 
      source_handle: handle
    });
  }

  void add_sink(MultiSinkOutput* target_sink, float max_attenuation = 1.0f) { 
    assert (target_sink != NULL);
    auto handle = target_sink->add_source(max_attenuation);
    auto target = SinkTarget();
    target.set_target(target_sink);
    target.set_target_source_handle(handle);

    this->targets.push_back(SinkTarget {
      target_sink: target_sink, 
      source_handle: handle
    });
  }

  SourceHandle add_source(float max_attenuation) override { 
    return this->aggregated_value.add_source(max_attenuation);
  }

  void write_source_state(float attenuation, SourceHandle handle) override { 
    this->aggregated_value.update_source_state(attenuation, handle);
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

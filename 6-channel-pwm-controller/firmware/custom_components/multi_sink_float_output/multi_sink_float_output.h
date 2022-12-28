#pragma once
#include "esphome/core/log.h"
#include "esphome/core/optional.h"

namespace esphome {
namespace output {

using SourceHandle = std::size_t;

class FloatValueAggregator {

  struct SourceState {
    float value;
    optional<float> max_attenuation;
  };

  float value;
  float aggregate_value;
  std::vector<SourceState> source_states;

  public:

  FloatValueAggregator() : 
    value(1.0f),
    aggregate_value(1.0f),
    source_states(0) {
  }

  SourceHandle add_source(float max_attenuation) { 
    this->source_states.push_back(SourceState {
      value: 1.0f,
      max_attenuation: (max_attenuation >= 1.0f)? nullopt : optional<float>{ max_attenuation }
    });

    this->calculate_aggregate_value();
    return this->source_states.size() -  1; // return index as a handle
  }

  inline float get_aggregate_value() { 
    return this->aggregate_value;
  }

  void update_value(float value) { 
    this->value = value;
    this->calculate_aggregate_value();
  }

  void update_source_state(float attenuation, SourceHandle handle) { 
    this->source_states.at(handle).value = attenuation;
    this->calculate_aggregate_value();
  }

  private:

  void calculate_aggregate_value() { 
    if(this->source_states.empty()) {
      this->aggregate_value = this->value;
      return;
    }

    float attenuation = 0.0f;
    for (auto& n : this->source_states) {
      if (n.max_attenuation.has_value()) {
        attenuation = std::max(attenuation, (*n.max_attenuation) * n.value);
      } else {
        attenuation = std::max(attenuation, n.value);
      }
    }
    this->aggregate_value = this->value * attenuation;
  }
};

class MultiSinkOutput {
  public:
  virtual SourceHandle add_source(float max_attenuation) = 0;
  virtual void write_source_state(float attenuation, SourceHandle handle) = 0;
};



class MultiSinkFloatOutput : 
  public esphome::Component, 
  public esphome::output::FloatOutput,
  public MultiSinkOutput 
{

  esphome::output::FloatOutput* output;
  FloatValueAggregator aggregated_value;

  public:

  SourceHandle add_source(float max_attenuation) override { 
    return this->aggregated_value.add_source(max_attenuation);
  }

  void set_output(esphome::output::FloatOutput* output) { 
    this->output = output;
  }

  void write_source_state(float source_state, SourceHandle handle) override { 
    this->aggregated_value.update_source_state(source_state, handle);
    this->apply_state();
  }

  void write_state(float value) override {
    this->aggregated_value.update_value(value);
    this->apply_state();
  }

  void apply_state() {
    if(this->output) {
      // Call set level to insure min max values are observed 
      //ESP_LOGD("CALIBRATE_OUTPUT", "Set value to %f", this->aggregated_value.get_aggregate_value());
      this->output->set_level(this->aggregated_value.get_aggregate_value()); 
    }
  }
};
}  // namespace output
}  // namespace esphome

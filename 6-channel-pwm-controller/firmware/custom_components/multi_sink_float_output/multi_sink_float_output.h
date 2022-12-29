#pragma once
#include "esphome/core/log.h"
#include "esphome/core/optional.h"
//#include "esphome/components/ledc/ledc_output.h"

namespace esphome {
namespace output {

using SourceHandle = std::size_t;



static const char *const TAG = "ledc.output";
static const int MAX_RES_BITS = 20;
float ledc_max_frequency_for_bit_depth(uint8_t bit_depth) { return 80e6f / float(1 << bit_depth); }

float ledc_min_frequency_for_bit_depth(uint8_t bit_depth, bool low_frequency) {
  const float max_div_num = ((1 << MAX_RES_BITS) - 1) / (low_frequency ? 32.0f : 256.0f);
  return 80e6f / (max_div_num * float(1 << bit_depth));
}

optional<uint8_t> ledc_bit_depth_for_frequency(float frequency) {
  for (int i = MAX_RES_BITS; i >= 1; i--) {
    const float min_frequency = ledc_min_frequency_for_bit_depth(i, (frequency < 100));
    const float max_frequency = ledc_max_frequency_for_bit_depth(i);
    if (min_frequency <= frequency && frequency <= max_frequency) {
      return i;
    }
  }
  return {};
}

class FloatValueAggregator {

  struct SourceState {
    float value;
    float aggregate_percentage;
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

  SourceHandle add_source(float aggregate_percentage) { 
    this->source_states.push_back(SourceState {
      value: 1.0f,
      aggregate_percentage: aggregate_percentage
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

  void update_source_state(float value, SourceHandle handle) { 
    this->source_states.at(handle).value = value;
    this->calculate_aggregate_value();
  }

  void update_source_aggregate_percentage(float aggregate_percentage, SourceHandle handle) { 
    this->source_states.at(handle).aggregate_percentage = aggregate_percentage;
    this->calculate_aggregate_value();
  }

  private:

  void calculate_aggregate_value() { 
    if(this->source_states.empty()) {
      this->aggregate_value = this->value;
      return;
    }

    float aggregate = 0.0f;
    for (auto& n : this->source_states) {
      aggregate = std::max(aggregate, n.aggregate_percentage * n.value);
      //aggregate += n.aggregate_percentage * n.value;
    }
    this->aggregate_value = this->value * aggregate;
  }
};

class MultiSinkOutput {
  public:
  virtual SourceHandle add_source(float aggregate_percentage) = 0;
  virtual void write_source_state(float value, SourceHandle handle) = 0;
};


class MultiSinkFloatOutput : 
  public esphome::Component, 
  public esphome::output::FloatOutput,
  public MultiSinkOutput 
{

  esphome::output::FloatOutput* output;
  FloatValueAggregator aggregated_value;

  float previous_value =-1.0;
  int current_bit_depth =-1.0;

  public:

  SourceHandle add_source(float aggregate_percentage) override { 
    return this->aggregated_value.add_source(aggregate_percentage);
  }

  void set_output(esphome::output::FloatOutput* output) { 
    this->output = output;
  }

  void write_source_aggregate_percentage(float aggregate_percentage, SourceHandle handle) { 
    this->aggregated_value.update_source_aggregate_percentage(aggregate_percentage, handle);
    this->apply_state();
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

    auto new_value = this->aggregated_value.get_aggregate_value();
    if(new_value != this->previous_value) {
      if(this->output) {

        /*auto bit_depth = 0;
        if(new_value <= 0.003f) {

          bit_depth = ledc_bit_depth_for_frequency(1220.0f).value_or(8);

          if(current_bit_depth != bit_depth) {
            ((esphome::ledc::LEDCOutput*)this->output)->update_frequency(1220.0f);
            this->current_bit_depth = bit_depth;
          }
        }
        else if(new_value <= 0.1f) {
          
          bit_depth = ledc_bit_depth_for_frequency(4882.0f).value_or(8);

          if(this->current_bit_depth != bit_depth) {
            ((esphome::ledc::LEDCOutput*)this->output)->update_frequency(4882.0f);
            this->current_bit_depth = bit_depth;
          }
        } else {
          
          bit_depth = ledc_bit_depth_for_frequency(9765.0f).value_or(8);

          if(this->current_bit_depth != bit_depth) {
            ((esphome::ledc::LEDCOutput*)this->output)->update_frequency(9765.0f);
            this->current_bit_depth = bit_depth;
          }
        }


        // Call set level to insure min max values are observed
        if(this->id == 1) {
          

          const uint32_t max_duty = (uint32_t(1) << bit_depth) - 1;
          const float duty_rounded = roundf(new_value * max_duty);
          auto duty = static_cast<uint32_t>(duty_rounded);

          ESP_LOGD("CALIBRATE_OUTPUT", "[%i] Set value to %f [%i@%i bits]", this->id, new_value, duty, bit_depth);
        }*/
        //ESP_LOGD("CALIBRATE_OUTPUT", "Set value to %f ", new_value);
        this->output->set_level(new_value); 
      }
    }

    this->previous_value = new_value;
  }
};
}  // namespace output
}  // namespace esphome

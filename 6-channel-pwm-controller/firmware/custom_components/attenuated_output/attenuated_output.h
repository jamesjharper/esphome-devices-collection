#pragma once
#include "attenuated_output.h"
#include "esphome/core/optional.h"
#include "esphome/core/log.h"
#include "esphome/components/ledc/ledc_output.h"

namespace esphome {
namespace output {

using AttenuationHandle = std::size_t;

class FloatValueAttenuator {

  struct Attenuation {
    float attenuation;
    optional<float> max_attenuation;
  };

  float value;
  float attenuated_value;
  std::vector<Attenuation> attenuations;

  public:

  FloatValueAttenuator() : 
    value(1.0f),
    attenuated_value(1.0f),
    attenuations(0) {
    this->attenuations.reserve(2); 
  }

  AttenuationHandle register_attenuation(float max_attenuation) { 

    this->attenuations.push_back(Attenuation {
      attenuation: 1.0f,
      max_attenuation: (max_attenuation >= 1.0f)? nullopt : optional<float>{ max_attenuation }
    });

    this->calculate_attenuated_value();
    return this->attenuations.size() -  1; // return index as a handle
  }

  inline float get_attenuated_value() { 
    return this->attenuated_value;
  }

  void update_value(float value) { 
    this->value = value;
    this->calculate_attenuated_value();
  }

  void update_attenuation(float attenuation, AttenuationHandle handle) { 
    this->attenuations.at(handle).attenuation = attenuation;
    this->calculate_attenuated_value();
  }

  private:

  void calculate_attenuated_value() { 
    if(this->attenuations.empty()) {
      this->attenuated_value = this->value;
      return;
    }

    float attenuation = 0.0f;
    for (auto& n : this->attenuations) {
      if (n.max_attenuation.has_value()) {
        attenuation = std::max(attenuation, (*n.max_attenuation) * n.attenuation);
      } else {
        attenuation = std::max(attenuation, n.attenuation);
      }
    }
    this->attenuated_value = this->value * attenuation;
  }
};

class AttenuatedOutput {
  public:
  virtual AttenuationHandle register_attenuation(float max_attenuation) = 0;
  virtual void write_attenuation_state(float attenuation, AttenuationHandle handle) = 0;
};


class AttenuatedFloatOutput : 
  public esphome::Component, 
  public esphome::output::FloatOutput,
  public AttenuatedOutput 
{

  esphome::output::FloatOutput* output;
  FloatValueAttenuator attenuated_value;

  public:

  AttenuatedFloatOutput(esphome::output::FloatOutput* output) : 
    output(output),
    attenuated_value()
  {
  }

   AttenuatedFloatOutput() : 
    output(0),
    attenuated_value()
  {
  }

  AttenuationHandle register_attenuation(float max_attenuation) override { 
    return this->attenuated_value.register_attenuation(max_attenuation);
  }

  void set_output(esphome::output::FloatOutput* output) { 
    this->output = output;
  }

  void write_attenuation_state(float attenuation, AttenuationHandle handle) override { 
    this->attenuated_value.update_attenuation(attenuation, handle);
    this->apply_state();
  }

  void write_state(float value) override {
    this->attenuated_value.update_value(value);
    this->apply_state();
  }

  void apply_state() {
    if(this->output) {
      // Call set level to insure min max values are observed 
      //ESP_LOGD("CALIBRATE_OUTPUT", "Set value to %f", this->attenuated_value.get_attenuated_value());
      this->output->set_level(this->attenuated_value.get_attenuated_value()); 
    }
  }
};

class OutputAttenuator : 
  public esphome::Component, 
  public esphome::output::FloatOutput,
  public AttenuatedOutput {

  struct TargetOutput {
    AttenuatedOutput* target;
    AttenuationHandle attenuation_handle;
    std::vector<float> max_attenuation;
  };

  FloatValueAttenuator attenuated_value;
  std::vector<TargetOutput> targets;

  public:

  OutputAttenuator() : 
    attenuated_value(),
    targets(0) {
  }

  void attach(AttenuatedOutput* target, float max_attenuation = 1.0f) { 

    assert (target!=NULL);
    auto handle = target->register_attenuation(max_attenuation);
    this->targets.push_back(TargetOutput {
      target: target, 
      attenuation_handle: handle
    });
  }

  AttenuationHandle register_attenuation(float max_attenuation) override { 
    return this->attenuated_value.register_attenuation(max_attenuation);
  }


  void write_attenuation_state(float attenuation, AttenuationHandle handle) override { 
    this->attenuated_value.update_attenuation(attenuation, handle);
    this->apply_state();
  }

  void write_state(float value) override {
    this->attenuated_value.update_value(value);
    this->apply_state();
  }

  void apply_state() {
    for (auto& item : this->targets) {
      item.target->write_attenuation_state(this->attenuated_value.get_attenuated_value(), item.attenuation_handle);
    }
  }
};

}  // namespace output
}  // namespace esphome

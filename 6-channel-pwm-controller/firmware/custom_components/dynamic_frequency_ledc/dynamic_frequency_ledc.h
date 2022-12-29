#pragma once
#include "esphome/core/log.h"
#include "esphome/core/optional.h"
//include "esphome/components/ledc/ledc_output.h"
#include "esphome/components/ledc_ext/ledc_output.h"

#ifdef USE_ARDUINO
#include <esp32-hal-ledc.h>
#endif
namespace esphome {
namespace output {

class DynamicFrequencyLedc : 
  public esphome::Component, 
  public esphome::output::FloatOutput
{

  esphome::ledc_ext::LEDCOutput* ledc = NULL;
  float low_freq = 1220.0f;
  float high_freq = 9765.0;
  float transition = 0.1;
  float max_power_high = 0.0025;
  float max_power_low = 0.001;
  bool is_high_freq = false;

  public:

  void set_ledc(esphome::ledc_ext::LEDCOutput* ledc) { 
    this->ledc = ledc;
  }

  void set_low_frequency(float low_freq) { 
    this->low_freq = low_freq;
  }

  void set_high_frequency(float low_freq) { 
    this->high_freq = high_freq;
  }

  void set_transition_duty(float transition) { 
    this->transition = transition;
  }


  void write_state(float value) override {
      if(!ledc)
        return;

      if(value > this->transition) {
        if(!this->is_high_freq) {
          
          this->ledc->set_min_power(this->max_power_high);
          this->ledc->update_frequency(this->high_freq);
          this->ledc->set_level(value);
          this->is_high_freq = true;
        } else {
          this->ledc->set_level(value);
        }

      } else {
        if(this->is_high_freq) {
          this->ledc->set_min_power(this->max_power_low);
          this->ledc->update_frequency(this->low_freq);
          this->ledc->set_level(value);
          this->is_high_freq = false;
        } else {
          this->ledc->set_level(value);
        }
      }

      
    
      
  }

  
};
}  // namespace output
}  // namespace esphome

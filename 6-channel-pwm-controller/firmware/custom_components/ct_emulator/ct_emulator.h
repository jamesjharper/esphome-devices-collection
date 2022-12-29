#pragma once

#include "esphome/core/component.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/light/light_output.h"

#include "esphome/components/ct_emulator/black_body.h"

namespace esphome {
namespace ct_emulator {


class ColourTemperatureEmulatorLightOutput : public light::LightOutput {
 public:
  void set_red(output::FloatOutput *red) { this->_red = red; }
  void set_green(output::FloatOutput *green) { this->_green = green; }
  void set_blue(output::FloatOutput *blue) { this->_blue = blue; }
  void set_warm_white(output::FloatOutput *warm_white) { this->_warm_white = warm_white; }
  void set_cold_white(output::FloatOutput *cold_white) { this->_cold_white = cold_white; }
  void set_mid_white(output::FloatOutput *mid_white) { this->_mid_white = mid_white; }

  void set_cold_white_temperature(float cold_white_temperature) { 
    this->_cold_white_temperature = cold_white_temperature;
    if(this->_emulated_cold_white_temperature == 0.0f) {
       this->_emulated_cold_white_temperature = cold_white_temperature;
    } 
  }

  void set_warm_white_temperature(float warm_white_temperature) { 
    this->_warm_white_temperature = warm_white_temperature; 
    if(this->_emulated_warm_white_temperature == 0.0f) {
       this->_emulated_warm_white_temperature = warm_white_temperature;
    }
  }

  void set_mid_white_temperature(float mid_white_temperature) { 
    this->_mid_white_temperature = mid_white_temperature; 
  }

  void set_emulated_cold_white_temperature(float emulated_cold_white_temperature) { 
    this->_emulated_cold_white_temperature = emulated_cold_white_temperature; 
    this->_emulated_colour_temperature = true;
  }
  void set_emulated_warm_white_temperature(float emulated_warm_white_temperature) { 
    this->_emulated_warm_white_temperature = emulated_warm_white_temperature; 
    this->_emulated_colour_temperature = true;
  }

  void set_constant_brightness(bool constant_brightness) { 
    this->_constant_brightness = constant_brightness; 
  }

  float get_emulated_cold_white_temperature() {
    if(this->_emulated_cold_white_temperature == 0.0f) {
      return this->_cold_white_temperature;
    } else {
      return this->_emulated_cold_white_temperature;
    }
  }

  float get_emulated_warm_white_temperature() {
    if(this->_emulated_warm_white_temperature == 0.0f) {
      return this->_warm_white_temperature;
    } else {
      return this->_emulated_warm_white_temperature;
    }
  }

  light::LightTraits get_traits() override {
    auto traits = light::LightTraits();

    traits.set_supported_color_modes({light::ColorMode::COLOR_TEMPERATURE});
    traits.set_min_mireds(this->get_emulated_cold_white_temperature());
    traits.set_max_mireds(this->get_emulated_warm_white_temperature());

    return traits;
  }


  void write_state(light::LightState *state) override {

    auto brightness = state->current_values.get_brightness() * state->current_values.get_state();
    auto target_mireds = state->current_values.get_color_temperature();
    auto gamma = state->get_gamma_correct();

    if(this->has_rgb_output()) {
      this->write_rgb_state(brightness, gamma, target_mireds);
    }
    
    if(this->has_cwww_output()) {
       this->write_cwww_emulated_state(
        brightness, 
        gamma,
        target_mireds,
        this->get_emulated_cold_white_temperature(),
        this->get_emulated_warm_white_temperature()
      );
    } 
    
    if (this->has_mw_output()) {
      this->write_mw_emulated_state(
        brightness, 
        gamma,
        target_mireds,
        this->get_emulated_cold_white_temperature(),
        this->get_emulated_warm_white_temperature()
      );
    }
  }

  void write_rgb_state(float brightness, float gamma, float target_mireds) {

    float red, green, blue;

    color_science::black_body_rgb(
      target_mireds, 
      brightness,
      gamma,
      &red, 
      &green, 
      &blue
    );

    if (this->_red)
      this->_red->set_level(red);

    if (this->_green)
      this->_green->set_level(green);

    if (this->_blue) 
      this->_blue->set_level(blue);
  }

  void write_mw_emulated_state(float brightness, float gamma, float target_mireds, float min_mireds, float max_mireds) {

    float mw;
    color_science::black_body_to_mw(
        target_mireds, 
        brightness, 
        gamma,
        min_mireds, 
        max_mireds, 
        this->_mid_white_temperature, 
        false,
        &mw
      );

    //ESP_LOGW("wer", "brightness %f, gamma %f, target_mireds %f, min_mireds %f, max_mireds %f, mw %f", brightness, gamma, target_mireds, min_mireds, max_mireds, mw);
    this->_mid_white->set_level(mw);
  }

  void write_cwww_emulated_state(float brightness, float gamma, float target_mireds, float min_mireds, float max_mireds) {

    float cw, ww;

    color_science::black_body_to_mw(
        target_mireds, 
        brightness, 
        gamma,
        min_mireds, 
        this->_warm_white_temperature, 
        this->_cold_white_temperature,
        this->_constant_brightness,
        &cw
      );

    color_science::black_body_to_mw(
        target_mireds, 
        brightness, 
        gamma,
        this->_cold_white_temperature, 
        max_mireds, 
        this->_warm_white_temperature, 
        this->_constant_brightness,
        &ww
      );

    //ESP_LOGW("wer", "brightness %f, gamma %f, target_mireds %f, min_mireds %f, max_mireds %f, cw %f, ww %f", brightness, gamma, target_mireds, min_mireds, max_mireds, cw, ww);
    this->_cold_white->set_level(cw);
    this->_warm_white->set_level(ww);
  }


 protected:

  bool has_rgb_output() {
    return this->_red || this->_green || this->_blue;
  }


  bool has_cwww_output() {
    return this->_warm_white && this->_cold_white;
  }

  bool has_mw_output() {
    return this->_mid_white;
  }

  output::FloatOutput *_red = NULL;
  output::FloatOutput *_green = NULL;
  output::FloatOutput *_blue = NULL;
  output::FloatOutput *_warm_white = NULL;
  output::FloatOutput *_cold_white = NULL;
  output::FloatOutput *_mid_white = NULL;

  float _cold_white_temperature{0};
  float _warm_white_temperature{0};
  float _mid_white_temperature{0};

  float _emulated_cold_white_temperature{0};
  float _emulated_warm_white_temperature{0};

  bool _constant_brightness{false};
  bool _emulated_colour_temperature{false};
  bool _color_interlock_{false};
};



}  // namespace rgbw
}  // namespace esphome

#pragma once

#include "esphome/core/component.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/light/light_output.h"

#include "esphome/components/ct_emulator/black_body.h"
#include "esphome/components/ct_emulator/white_point.h"
#include "esphome/components/ct_emulator/colour_profile.h"

namespace esphome {
namespace ct_emulator {

using namespace esphome::cms;
using namespace esphome::cms::math;
#define clamp_1( in) clamp(in, 0.0f, 1.0f)

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
    this->update_wb();
  }

  void set_warm_white_temperature(float warm_white_temperature) { 
    this->_warm_white_temperature = warm_white_temperature; 
    if(this->_emulated_warm_white_temperature == 0.0f) {
       this->_emulated_warm_white_temperature = warm_white_temperature;
    }
    this->update_wb();
  }

  void set_mid_white_temperature(float mid_white_temperature) { 
    this->_mid_white_temperature = mid_white_temperature; 
    this->update_wb();
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

    //traits.set_supported_color_modes({light::ColorMode::COLOR_TEMPERATURE});
    
    //traits.set_supported_color_modes({ light::ColorMode::RGB, light::ColorMode::COLOR_TEMPERATURE});
    
   traits.set_supported_color_modes(
     
      {(light::ColorMode)((uint8_t)(
        light::ColorCapability::ON_OFF | 
        light::ColorCapability::BRIGHTNESS | 
        light::ColorCapability::RGB |
        light::ColorCapability::WHITE | 
        light::ColorCapability::COLOR_TEMPERATURE
      ))}
    );
    

    traits.set_min_mireds(this->get_emulated_cold_white_temperature());
    traits.set_max_mireds(this->get_emulated_warm_white_temperature());

    return traits;
  }


  void write_state(light::LightState *state) override {

    auto brightness = state->current_values.get_brightness() * state->current_values.get_state();
    auto colour_brightness = state->current_values.get_color_brightness() * state->current_values.get_state();
    auto target_mireds = state->current_values.get_color_temperature();

    auto red = state->current_values.get_red() * colour_brightness;

    auto green = state->current_values.get_green() * colour_brightness;
    auto blue = state->current_values.get_blue() * colour_brightness;

    auto gamma = state->get_gamma_correct();


    ESP_LOGW("test", "mireds %f,  r = %f,  g = %f, b = %f", target_mireds, red, green, blue);


    if(this->has_rgb_output()) {
      this->write_rgb_state(brightness, gamma, target_mireds, red, green, blue);
    }
    
    if(this->has_cwww_output() || this->has_mw_output()) {
       this->write_cwww_emulated_state(
        brightness, 
        gamma,
        target_mireds,
        this->get_emulated_cold_white_temperature(),
        this->get_emulated_warm_white_temperature(),
        red, green, blue
      );
    } 
    
  }

  void write_rgb_state(float brightness, float gamma, float target_mireds, float r,float g, float b ) {

    // convert source RGB to 
    // Caculature XZ for 
    auto k =  1000000.0f / target_mireds;
    bfloat2	pcs_wb_xy;
    white_point::compute_white_point_chromaticities_approx_mireds(target_mireds, pcs_wb_xy);

    auto source_rgb = bfloat4( r, g, b, 1.0f );
    bfloat4	pcs_colour_xyz;
    this->source_profile.RGB2XYZ(source_rgb, pcs_colour_xyz);

    // float4x4 whiteBalanceMatrix = float4x4::Identity;
    //this->rgb_device_colour_profile.white_balance_XYZ_matrix(wb_xy, (float3x3&)whiteBalanceMatrix);
   // XYZ = XYZ * whiteBalanceMatrix;
   // ESP_LOGW("test", "mireds %f (%f K),  x = %f,  y = %f", target_mireds, k, xy.x, xy.y);

    
    bfloat4	target_rgb;
    this->rgb_device_colour_profile.XYZ2RGB(pcs_colour_xyz, target_rgb);

    bfloat4	target_wb;
    this->rgb_device_colour_profile.xyY2RGB(pcs_wb_xy, brightness, target_wb);

    ESP_LOGW("test", "mireds %f (%f K),  r = %f,  g = %f, b = %f", target_mireds, k, clamp_1(target_rgb.x), clamp_1(target_rgb.y), clamp_1(target_rgb.z));


    if (this->_red)
      this->_red->set_level(clamp_1(target_rgb.x * target_wb.x));

    if (this->_green)
      this->_green->set_level(clamp_1(target_rgb.y * target_wb.y));

    if (this->_blue) 
      this->_blue->set_level(clamp_1(target_rgb.z * target_wb.z));
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

  void write_cwww_emulated_state(float brightness, float gamma, float target_mireds, float min_mireds, float max_mireds, float r,float g, float b ) {

    auto source_rgb = bfloat4( r, g, b, 1.0f );
    bfloat4	pcs_colour_xyz;
    this->source_profile.RGB2XYZ(source_rgb, pcs_colour_xyz);


    auto k =  1000000.0f / target_mireds;
    bfloat2	pcs_wb_xy;
    white_point::compute_white_point_chromaticities_approx_mireds(target_mireds, pcs_wb_xy);


    bfloat4	target_rgb;
    this->rgb_device_colour_profile.XYZ2RGB(pcs_colour_xyz, target_rgb);

    bfloat4	target_wb;
    this->rgb_device_colour_profile.xyY2RGB(pcs_wb_xy, brightness, target_wb);



   // ESP_LOGW("test", "mireds %f (%f K),  x = %f,  y = %f", target_mireds, k, xy.x, xy.y);

  //  bfloat4	cwwww;
   // this->w_device_colour_profile.xyY2RGB(xy, brightness, cwwww);

   // ESP_LOGW("test", "mireds %f (%f K),  ww = %f,  wm = %f, cw = %f", target_mireds, k, clamp_1(cwwww.y), clamp_1(cwwww.x), clamp_1(cwwww.z));

    if (this->has_cwww_output()) {
      this->_warm_white->set_level(target_wb.y * target_rgb.y);
      this->_cold_white->set_level(target_wb.z * target_rgb.z);
    }
    if (this->has_mw_output()) {
      this->_mid_white->set_level(target_wb.x * target_rgb.x);
    }



  }


  /*ColourTemperatureEmulatorLightOutput() : light::LightOutput() {

  }*/

  void update_wb() {
     this->rgb_device_colour_profile.set_chromas(esphome::cms::Chromaticities::AdobeRGB_D50);
    this->source_profile.set_chromas(esphome::cms::Chromaticities::AdobeRGB_D50);

    bfloat2	xy_ww, xy_mw, xy_cw;
    if(this->_warm_white_temperature == 0.0f) {
      white_point::compute_white_point_chromaticities_approx_kelvin(_warm_white_temperature, xy_ww);
    }
     
    if(this->_mid_white_temperature == 0.0f) {
      white_point::compute_white_point_chromaticities_approx_kelvin(_mid_white_temperature, xy_mw);
    }
    
    if(this->_cold_white_temperature == 0.0f) {
      white_point::compute_white_point_chromaticities_approx_kelvin(_cold_white_temperature, xy_cw);
    }

    this->w_device_colour_profile.set_chromas(
      esphome::cms::Chromaticities(  
        xy_ww, 
        xy_mw,
        xy_cw, 
        esphome::cms::Chromaticities::ILLUMINANT_D50 
      )
    );
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

  esphome::cms::ColorProfile source_profile;
  esphome::cms::ColorProfile rgb_device_colour_profile;
  esphome::cms::ColorProfile w_device_colour_profile;

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

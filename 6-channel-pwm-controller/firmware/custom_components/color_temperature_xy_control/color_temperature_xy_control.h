#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/color_output/color_output.h"

namespace esphome {
namespace light {

class ColorTemperatureXyControl : public LightOutput {
    protected:

    esphome::output::ColorOutput* _primary_output_device;
    float min_mireds, max_mireds;

    public:

    void set_cold_white_color_temperature(float mireds) { 
        this->min_mireds = mireds;
    }

    void set_warm_white_color_temperature(float mireds) { 
        this->max_mireds = mireds;
    }

    void set_output_device(esphome::output::ColorOutput* output) { 
        this->_primary_output_device = output; 
    }  

    LightTraits get_traits() override {
        auto traits = light::LightTraits();
        traits.set_supported_color_modes(
            {
                light::ColorMode::COLOR_TEMPERATURE
            }
        );

        traits.set_min_mireds(this->min_mireds);
        traits.set_max_mireds(this->max_mireds);
        return traits;
    }

    void write_state(LightState *state) override {

        auto colour_brightness = state->current_values.get_brightness() * state->current_values.get_state();
        this->_primary_output_device->set_brightness(colour_brightness);

         auto cct = state->current_values.get_color_temperature();

        this->_primary_output_device->set_white_balance_mireds(cct);
        this->_primary_output_device->apply();

    }
};

}  // namespace light
}  // namespace esphome

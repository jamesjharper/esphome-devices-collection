#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/color_output/color_output.h"

namespace esphome {
namespace light {


class RgbXyControl : public LightOutput {
    protected:

    esphome::output::ColorOutput* _output;
    public:


    void set_output_device(esphome::output::ColorOutput* output) { 
        this->_output = output; 
    }  

    LightTraits get_traits() override {
        auto traits = light::LightTraits();
        traits.set_supported_color_modes({light::ColorMode::RGB});
        return traits;
    }

    void write_state(LightState *state) override {

        auto colour_brightness = state->current_values.get_brightness() * state->current_values.get_state();
        this->_output->set_saturation(colour_brightness);

        this->_output->set_rgb(
            state->current_values.get_red(),
            state->current_values.get_green(),
            state->current_values.get_blue()
        );

        this->_output->apply();

    }
};

}  // namespace light
}  // namespace esphome

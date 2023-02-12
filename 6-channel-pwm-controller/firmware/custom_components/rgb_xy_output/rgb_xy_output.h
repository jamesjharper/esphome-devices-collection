#pragma once

#include "esphome/components/output/float_output.h"
#include "esphome/core/component.h"
#include "esphome/components/color_output/color_output.h"

namespace esphome {
namespace output {


using namespace esphome::profiles;

#define clamp_1(in) std::min(std::max(in, 0.0f ), 1.0f)


class RgbXyOutput : public Component, public XyOutput {
    protected:
    RgbChromaTransform _rgb_profile_transform;
    
    output::FloatOutput *_x = NULL; // Red or warm white
    output::FloatOutput *_y = NULL; // Green or mid white
    output::FloatOutput *_z = NULL; // Blue or cold white

    public:

    void set_state(float X, float Y, float Z) override {

        auto rgb = this->_rgb_profile_transform.XYZ_to_RGB(XYZ_Cie1931(X, Y, Z));

        auto max = std::max(std::max(rgb.r, rgb.g), rgb.b);
        if (max > 1.0f) {
            rgb.r /= max;
            rgb.g /= max;
            rgb.b /= max;
        }

        //ESP_LOGD("RGB", "RGB [%f,%f,%f]", rgb[0], rgb[1], rgb[2]);

        if(this->_x)
            this->_x->set_level(clamp_1(rgb.r));
            
        if(this->_y)
            this->_y->set_level(clamp_1(rgb.g));

        if(this->_z)
            this->_z->set_level(clamp_1(rgb.b));
    }


    void set_color_profile(RgbProfile *profile) { 
        this->_rgb_profile_transform = profile->get_chroma_transform();
    }

    void set_red_output(output::FloatOutput *red) { 
        this->_x = red;
    }

    void set_green_output(output::FloatOutput *green) {
        this->_y = green; 
    }

    void set_blue_output(output::FloatOutput *blue) { 
        this->_z = blue; 
    }

    void set_warm_white_output(output::FloatOutput *warm_white) { 
        this->_x = warm_white; 
    }

    void set_mid_white_output(output::FloatOutput *mid_white) { 
        this->_y = mid_white; 
    }

    void set_cold_white_output(output::FloatOutput *cold_white) { 
        this->_z = cold_white; 
    }
};
}  // namespace color_calibrated_output
}  // namespace esphome

#pragma once

#include "esphome/components/output/float_output.h"
#include "esphome/core/component.h"
#include "esphome/components/enable_cms/rgb_profile.h"
#include "esphome/components/enable_cms/outputs.h"

namespace esphome {
namespace output {

using namespace esphome::profiles;
using namespace esphome::cms;

#define clamp_1(in) std::min(std::max(in, 0.0f ), 1.0f)

class ColorTransform {
    RgbChromaTransform _chroma_transform;
    float _brightness = 1.0f; 
    float _saturation = 1.0f; 

    public:

    void set_brightness(float brightness) { 
        this->_brightness = brightness;
    }

    void set_saturation(float saturation) { 
        this->_saturation = saturation;
    }

    XYZ_Cie1931 RGB_to_XYZ(RGB rgb) {
        // TODO: do this transform in native XYZ
        if(this->_saturation != 1.0f) {
            auto hsl = rgb.as_hsl();
            hsl.s *= this->_saturation;
            rgb = hsl.as_rgb();
        }

        if(this->_brightness != 1.0f) {
            rgb.r *= this->_brightness;
            rgb.g *= this->_brightness;
            rgb.b *= this->_brightness;
        }

       // ESP_LOGD("ColorProfile", "rgb[%f,%f,%f]", rgb_t[0], rgb_t[1], rgb_t[2]);
        return this->_chroma_transform.RGB_to_XYZ(rgb);
    }
};

class RoutedDestination {
    XyOutput* _output = 0;

    RoutingMethod method;
    SourceHandle _source_handle = 0;

    public:

    void set_output(XyOutput* dest_output) {
      this->_output = dest_output;     
    }

    void route_xyz_percentage(float percentage) {
      this->method = RoutingMethod::XYZ_percentage(percentage);
    }

    void init() {
        this->_source_handle = this->_output->create_source(); 
    }

    void set_state(float X, float Y, float Z) {
        auto t_xyz = this->method.route(XYZ_Cie1931(X, Y, Z));
        this->_output->set_source_state(this->_source_handle, t_xyz.X, t_xyz.Y, t_xyz.Z);
    }
};

class ColorOutput : public Component {
    protected:

    RgbChromaTransform _chroma_transform;

    float _brightness = 1.0f; 
    float _saturation = 1.0f; 

    RGB _rgb;
    std::vector<RoutedDestination> _routed_destinations; 

    public:

    void add_routed_destination(RoutedDestination& output) { 
        this->_routed_destinations.push_back(output);
        output.init();
    }

    void set_source_color_profile(RgbProfile *profile) { 
        this->_chroma_transform = profile->get_chroma_transform();
    }

    void set_brightness(float brightness) { 
        this->_brightness = brightness;
    }

    void set_saturation(float saturation) { 
        this->_saturation = saturation;
    }

    void set_rgb(float r, float g, float b) { 
        this->_rgb = RGB(r, g, b);
    }

    void set_white_balance_mireds(float mireds) { 
        this->_chroma_transform.set_white_point(Cct::from_mireds(mireds).uv.as_xy_cie1931());
    }

    XYZ_Cie1931 transform_to_xyz() {

        RGB rgb = this->_rgb;
        if(this->_saturation != 1.0f) {
            auto hsl = rgb.as_hsl();
            hsl.s *= this->_saturation;
            rgb = hsl.as_rgb();
        }

        if(this->_brightness != 1.0f) {
            rgb.r *= this->_brightness;
            rgb.g *= this->_brightness;
            rgb.b *= this->_brightness;
        }

       // ESP_LOGD("ColorProfile", "rgb[%f,%f,%f]", rgb_t[0], rgb_t[1], rgb_t[2]);
        return this->_chroma_transform.RGB_to_XYZ(rgb);
    }

    void apply() {
        auto XYZ = this->transform_to_xyz();
        for (auto dest : this->_routed_destinations)
            dest.set_state(XYZ.X, XYZ.Y, XYZ.Z);
    }
};
}  // namespace color_calibrated_output
}  // namespace esphome

#pragma once
#include "esphome/core/component.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/enable_cms/chroma.h"
#include "esphome/components/enable_cms/cwww_profile.h"

namespace esphome {
namespace output {

using namespace esphome::profiles;

class CwwwXyOutput : public Component, public XyOutput {
    protected:
    CwWwChromaTransform _cwww_profile_transform;
    
    output::FloatOutput *_warm_white = NULL;
    output::FloatOutput *_cold_white = NULL;

    public:

    void set_state(float X, float Y, float Z) override {
        auto XYZ = XYZ_Cie1931(X, Y, Z);
        auto cwww = this->_cwww_profile_transform.XYZ_to_CwWw(XYZ);

        if(this->_cold_white)
            this->_cold_white->set_level(cwww.cw);

        if(this->_warm_white)
            this->_warm_white->set_level(cwww.ww);
    }


    void set_profile(CwwwProfile *profile) { 
        this->_cwww_profile_transform = profile->get_chroma_transform();
    }

    void set_warm_white_output(output::FloatOutput *warm_white) { 
        this->_warm_white = warm_white; 
    }

    void set_cold_white_output(output::FloatOutput *cold_white) { 
        this->_cold_white = cold_white; 
    }
};



}  // namespace output
}  // namespace esphome

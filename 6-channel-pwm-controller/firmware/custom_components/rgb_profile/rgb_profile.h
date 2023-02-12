#pragma once

#include "esphome/core/component.h"
#include "esphome/components/enable_cms/color_spaces.h"
#include "esphome/components/enable_cms/matrices.h"
#include "esphome/components/enable_cms/chroma.h"

namespace esphome {
namespace profiles {

using namespace esphome::cms;
using namespace esphome::cms::matrices;

class RgbProfile : public Component {
    protected:

    RgbChromaTransform _chroma_transform;

    public:

    void set_gamma(float g) { 
        this->_chroma_transform.set_gamma(g);
    }

    void set_red_xy(float x, float y) { 
        this->_chroma_transform.set_red(Xy_Cie1931(x,y));
    }

    void set_green_xy(float x, float y) { 
        this->_chroma_transform.set_green(Xy_Cie1931(x,y));
    }

    void set_blue_xy(float x, float y) { 
        this->_chroma_transform.set_blue(Xy_Cie1931(x,y));
    }

    void set_white_point_xy(float x, float y) { 
        this->_chroma_transform.set_white_point(Xy_Cie1931(x,y));
    }

    void set_white_point_cct(float mireds) { 
        this->_chroma_transform.set_white_point(Cct::from_mireds(mireds).uv.as_xy_cie1931());
    }

    RgbChromaTransform get_chroma_transform() {
        return this->_chroma_transform;
    }
};

}  // namespace profiles
}  // namespace esphome

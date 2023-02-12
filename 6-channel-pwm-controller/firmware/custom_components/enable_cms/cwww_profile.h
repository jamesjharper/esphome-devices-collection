#pragma once

#include "esphome/core/component.h"
#include "esphome/components/enable_cms/color_spaces.h"
#include "esphome/components/enable_cms/chroma.h"

namespace esphome {
namespace profiles {

using namespace esphome::cms;

class CwwwProfile : public Component {
    protected:

    CwWwChromaTransform _chroma_transform;
    float _gamma = 0.0f;

    public:

  void set_gamma(float g) { 
        this->_chroma_transform.set_gamma(g);
    }

    void set_tint_duv_impurity(float duv) { 
        this->_chroma_transform.set_green_tint_duv_impurity(duv);
        this->_chroma_transform.set_purple_tint_duv_impurity(duv);
    }

    void set_green_tint_duv_impurity(float duv) { 
        this->_chroma_transform.set_green_tint_duv_impurity(duv);
    }

    void set_purple_tint_duv_impurity(float duv) { 
        this->_chroma_transform.set_purple_tint_duv_impurity(duv);
    }

    void set_white_point_cct(float mireds) { 
        this->_chroma_transform.set_white_point(Cct::from_mireds(mireds));
    }

    void set_warm_white_cct(float mireds) { 
        this->_chroma_transform.set_warm_white(Cct::from_mireds(mireds));
    }

    void set_cold_white_cct(float mireds) { 
        this->_chroma_transform.set_cold_white(Cct::from_mireds(mireds));
    }

    void set_cct_impurity(float mireds) { 
        this->_chroma_transform.set_red_cct_impurity(mireds);
        this->_chroma_transform.set_blue_cct_impurity(mireds);
    }

    void set_red_cct_impurity(float mireds) { 
        this->_chroma_transform.set_red_cct_impurity(mireds);
    }

    void set_blue_cct_impurity(float mireds) { 
        this->_chroma_transform.set_blue_cct_impurity(mireds);
    }

    CwWwChromaTransform get_chroma_transform() {
        return this->_chroma_transform;
    }
};

}  // namespace profiles
}  // namespace esphome

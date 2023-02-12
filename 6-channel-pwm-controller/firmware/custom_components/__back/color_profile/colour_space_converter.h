#pragma once
#include "esphome/components/color_profile/math.h"

//https://github.com/Patapom/GodComplex/blob/master/Packages/ImageUtilityLib/ColorProfile.h
namespace esphome {
namespace color_profile {
namespace cms {

    using namespace math;

    class ColourSpaceConverter {
        public:
        virtual void XYZ2RGB( const bfloat3& _XYZ, bfloat3& _RGB ) const;
        virtual void RGB2XYZ( const bfloat3& _RGB, bfloat3& _XYZ ) const;

        // Converts between gamma- and linear-space RGB
	    virtual void GammaRGB2LinearRGB( const bfloat3& _gammaRGB, bfloat3& _linearRGB ) const;
        virtual void LinearRGB2GammaRGB( const bfloat3& _linearRGB, bfloat3& _gammaRGB ) const;
    };


	class ColourSpaceConverter_CalibratedDevice_NoGamma : public ColourSpaceConverter {
		float3x3	m_RGB2XYZ;
		float3x3	m_XYZ2RGB;

		public:
		ColourSpaceConverter_CalibratedDevice_NoGamma( const float3x3& _RGB2XYZ, const float3x3& _XYZ2RGB ) {
			this->m_RGB2XYZ = _RGB2XYZ;
			this->m_XYZ2RGB = _XYZ2RGB;
		}

		void XYZ2RGB( const bfloat3& _XYZ, bfloat3& _RGB ) const override {
			// Transform into RGB
			_RGB =  _XYZ * this->m_XYZ2RGB;
		}

		void RGB2XYZ( const bfloat3& _RGB, bfloat3& _XYZ ) const override {
			// Transform into XYZ
			_XYZ = _RGB * this->m_RGB2XYZ;
		}

		void GammaRGB2LinearRGB( const bfloat3& _gammaRGB, bfloat3& _linearRGB ) const override {
			_linearRGB = _gammaRGB;
		}

		void LinearRGB2GammaRGB( const bfloat3& _linearRGB, bfloat3& _gammaRGB ) const override {
			_gammaRGB = _linearRGB;
		}
	};
}; // cms
}; // device_color_profile
}; // esphome

#pragma once
#include "esphome/components/ct_emulator/agnostic.h"
#include "esphome/components/ct_emulator/math.h"

//https://github.com/Patapom/GodComplex/blob/master/Packages/ImageUtilityLib/ColorProfile.h
namespace esphome {
namespace cms {

    using namespace math;

    class ColourSpaceConverter {
        public:
        virtual void XYZ2RGB( const bfloat4& _XYZ, bfloat4& _RGB ) const;
        virtual void RGB2XYZ( const bfloat4& _RGB, bfloat4& _XYZ ) const;

        // Converts between gamma- and linear-space RGB
	    virtual void GammaRGB2LinearRGB( const bfloat4& _gammaRGB, bfloat4& _linearRGB ) const;
        virtual void LinearRGB2GammaRGB( const bfloat4& _linearRGB, bfloat4& _gammaRGB ) const;
    };


	class ColourSpaceConverter_CalibratedDevice_NoGamma : public ColourSpaceConverter {
		float3x3	m_RGB2XYZ;
		float3x3	m_XYZ2RGB;

		public:
		ColourSpaceConverter_CalibratedDevice_NoGamma( const float3x3& _RGB2XYZ, const float3x3& _XYZ2RGB ) {
			this->m_RGB2XYZ = _RGB2XYZ;
			this->m_XYZ2RGB = _XYZ2RGB;
		}

		void XYZ2RGB( const bfloat4& _XYZ, bfloat4& _RGB ) const override {
			// Transform into RGB
			((bfloat3&) _RGB) = ((bfloat3&) _XYZ) * this->m_XYZ2RGB;
			_RGB.w = _XYZ.w;
		}

		void RGB2XYZ( const bfloat4& _RGB, bfloat4& _XYZ ) const override {
			// Transform into XYZ
			((bfloat3&) _XYZ) = ((bfloat3&) _RGB) * this->m_RGB2XYZ;
			_XYZ.w = _RGB.w;
		}

		void GammaRGB2LinearRGB( const bfloat4& _gammaRGB, bfloat4& _linearRGB ) const override {
			_linearRGB = _gammaRGB;
		}

		void LinearRGB2GammaRGB( const bfloat4& _linearRGB, bfloat4& _gammaRGB ) const override {
			_gammaRGB = _linearRGB;
		}
	};
}; // cms
}; // esphome

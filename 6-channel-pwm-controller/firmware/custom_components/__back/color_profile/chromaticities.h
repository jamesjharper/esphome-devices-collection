
#pragma once
#include "esphome/components/color_profile/math.h"

namespace esphome {
namespace color_profile {
namespace cms {

    using namespace math;

    /// <summary>
    /// Describes the Red, Green, Blue and White Point chromaticities of a simple/standard color profile
    /// </summary>
    struct	Chromaticities {
        
		static const bfloat2 ILLUMINANT_A;	// Incandescent, tungsten
		static const bfloat2 ILLUMINANT_D50;	// Daylight, Horizon
		static const bfloat2 ILLUMINANT_D55;	// Mid-Morning, Mid-Afternoon
		static const bfloat2 ILLUMINANT_D65;	// Daylight, Noon, Overcast (sRGB reference illuminant)
		static const bfloat2 ILLUMINANT_E;	// Reference

        static const Chromaticities	Empty;
        static const Chromaticities	sRGB;
        static const Chromaticities	AdobeRGB_D50;
        static const Chromaticities	AdobeRGB_D65;
        static const Chromaticities	ProPhoto;
        static const Chromaticities	Radiance;


        bfloat2 R, G, B, W;

        Chromaticities() {

        }

        Chromaticities( const Chromaticities& _other ) {
            R = _other.R;
            G = _other.G;
            B = _other.B;
            W = _other.W;
        }

        Chromaticities( const bfloat2& r, const bfloat2& g, const bfloat2& b, const bfloat2& w ) {
            R = r;
            G = g;
            B = b;
            W = w;
        }

        Chromaticities( float xr, float yr, float xg, float yg, float xb, float yb, float xw, float yw ) {
            R.Set( xr, yr );
            G.Set( xg, yg );
            B.Set( xb, yb );
            W.Set( xw, yw );
        }

        
        void build_transform_matrix() {
        }
        

        /// <summary>
        /// Attempts to recognize the current chromaticities as a standard profile
        /// </summary>
        /// <returns></returns>
        /*STANDARD_PROFILE FindRecognizedChromaticity() const {
            if ( Equals( sRGB ) )
                return STANDARD_PROFILE::sRGB;
            if ( Equals( AdobeRGB_D65 ) )
                return STANDARD_PROFILE::ADOBE_RGB_D65;
            if ( Equals( AdobeRGB_D50 ) )
                return STANDARD_PROFILE::ADOBE_RGB_D50;
            if ( Equals( ProPhoto ) )
                return STANDARD_PROFILE::PRO_PHOTO;
            if ( Equals( Radiance ) )
                return STANDARD_PROFILE::RADIANCE;

            // Ensure the profile is valid
            return R.x != 0.0f && R.y != 0.0f && G.x != 0.0f && G.y != 0.0f && B.x != 0.0f && B.y != 0.0f && W.x != 0.0f && W.y != 0.0f ? STANDARD_PROFILE::CUSTOM : STANDARD_PROFILE::INVALID;
        }*/

        bool Equals( const Chromaticities& other, float _eps=1e-3f ) const {
            return R.Almost( other.R, _eps )
                && G.Almost( other.G, _eps )
                && B.Almost( other.B, _eps )
                && W.Almost( other.W, _eps );
        }
    };


}; // cms
}; // color_profile
}; // esphome
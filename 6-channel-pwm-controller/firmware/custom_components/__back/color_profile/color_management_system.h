#pragma once

#include "esphome/components/color_profile/math.h"
#include "esphome/components/color_profile/chromaticities.h"
#include "esphome/components/color_profile/colour_space_converter.h"
#include "esphome/components/color_profile/white_point.h"

#define SAFE_DELETE( a ) if ( (a) != NULL ) { delete (a); (a) = NULL; }

namespace esphome {
namespace color_profile {
namespace cms {

	using namespace math;

	class ColorManagementSystem {

		Chromaticities		m_chromaticities;
		//GAMMA_CURVE			m_gammaCurve;
		//float				m_gamma;

		float3x3			m_RGB2XYZ;
		float3x3			m_XYZ2RGB;

		ColourSpaceConverter*	m_internalConverter;
 	    public:
        /// <summary>
        /// Converts from XYZ to xyY
        /// </summary>
        /// <param name="_XYZ"></param>
        /// <returns></returns>
        static void	XYZ2xyY( const bfloat3& _XYZ, bfloat3& _xyY ) {
            float InvSum = _XYZ.x + _XYZ.y + _XYZ.z;
            InvSum = InvSum > 1e-8f ? 1.0f / InvSum : 0.0f;
            _xyY.Set( _XYZ.x * InvSum, _XYZ.y * InvSum, _XYZ.y );
        }

        /// <summary>
        /// Converts from xyY to XYZ
        /// </summary>
        /// <param name="_xyY"></param>
        /// <returns></returns>
        static void	xyY2XYZ( const bfloat3& _xyY, bfloat3& _XYZ ) {
            float Y_y = _xyY.y > 1e-8f ? _xyY.z / _xyY.y : 0.0f;
            _XYZ.Set( _xyY.x * Y_y, _xyY.z, (1.0f - _xyY.x - _xyY.y) * Y_y );
        }

        static void	xy2uv( const bfloat2& xy, bfloat2& uv ) {
            uv.Set(
                (4.0 * xy.x) / (-2.0 * xy.x + (12.0 * xy.y) + 3.0),
                (6.0 * xy.y) / (-2.0 * xy.x + (12.0 * xy.y) + 3.0)
            );
        }

        static void	uv2xy( const bfloat2& uv, bfloat2& xy ) {
            xy.Set(
                (9.0f * uv.x) / (6.0f * uv.x - (16.0f * uv.y) + 12.0f),
                (2.0f * uv.y) / (3.0f * uv.x - (8.0f * uv.y) + 6.0f)
            );
        }


		Chromaticities& GetChromas() { 
            return this->m_chromaticities; 
        }

		const Chromaticities& GetChromas() const { 
            return this->m_chromaticities; 
        }

		void set_chromas( Chromaticities value )	{
            this->m_chromaticities = value;
			this->BuildTransformFromChroma();
		}

        void set_white_point( bfloat2 wp )	{
            this->m_chromaticities.W = wp;
			this->BuildTransformFromChroma();
		}

        ColorManagementSystem() :
			m_chromaticities( Chromaticities::Empty ),
			//, m_gammaCurve( GAMMA_CURVE::STANDARD )
			//, m_gamma( 1.0f )
			m_RGB2XYZ( float3x3::Identity ),
		    m_XYZ2RGB( float3x3::Identity ),
			m_internalConverter( nullptr ) 
        {
        }


        void BuildTransformFromChroma() {
            bfloat3	xyz_R( this->m_chromaticities.R.x, this->m_chromaticities.R.y, 1.0f - this->m_chromaticities.R.x - this->m_chromaticities.R.y );
            bfloat3	xyz_G( this->m_chromaticities.G.x, this->m_chromaticities.G.y, 1.0f - this->m_chromaticities.G.x - this->m_chromaticities.G.y );
            bfloat3	xyz_B( this->m_chromaticities.B.x, this->m_chromaticities.B.y, 1.0f - this->m_chromaticities.B.x - this->m_chromaticities.B.y );
            bfloat3	XYZ_W;
            ColorManagementSystem::xyY2XYZ( bfloat3( this->m_chromaticities.W.x, this->m_chromaticities.W.y, 1.0f ), XYZ_W );

            // Build the xyz->RGB matrix
            this->m_XYZ2RGB.r[0] = xyz_R;
            this->m_XYZ2RGB.r[1] = xyz_G;
            this->m_XYZ2RGB.r[2] = xyz_B;
            this->m_XYZ2RGB.Invert();

            // Knowing the XYZ of the white point, we retrieve the scale factor for each axis x, y and z that will help us get X, Y and Z
            bfloat3	sum_RGB = XYZ_W * this->m_XYZ2RGB;

            // Finally, we can retrieve the RGB->XYZ transform
            this->m_RGB2XYZ.r[0] = sum_RGB.x * xyz_R;
            this->m_RGB2XYZ.r[1] = sum_RGB.y * xyz_G;
            this->m_RGB2XYZ.r[2] = sum_RGB.z * xyz_B;

            // And the XYZ->RGB transform is simply the existing xyz->RGB matrix scaled by the reciprocal of the sum
            bfloat3	recSum_RGB( 1.0f / sum_RGB.x, 1.0f / sum_RGB.y, 1.0f / sum_RGB.z );
            (bfloat3&) this->m_XYZ2RGB.r[0] = (bfloat3&) this->m_XYZ2RGB.r[0] * recSum_RGB;
            (bfloat3&) this->m_XYZ2RGB.r[1] = (bfloat3&) this->m_XYZ2RGB.r[1] * recSum_RGB;
            (bfloat3&) this->m_XYZ2RGB.r[2] = (bfloat3&) this->m_XYZ2RGB.r[2] * recSum_RGB;

            // 
            SAFE_DELETE( this->m_internalConverter );
            this->m_internalConverter = new ColourSpaceConverter_CalibratedDevice_NoGamma( this->m_RGB2XYZ,  this->m_XYZ2RGB );

            // ============= Attempt to recognize a standard profile ============= 
            /*STANDARD_PROFILE	recognizedChromaticity = m_chromaticities.FindRecognizedChromaticity();

            if ( _checkGammaCurveOverride ) {
                // Also ensure the gamma ramp is correct before assigning a standard profile
                bool	bIsGammaCorrect = true;
                switch ( recognizedChromaticity ) {
                    case STANDARD_PROFILE::sRGB:			bIsGammaCorrect = EnsureGamma( GAMMA_CURVE::sRGB, GAMMA_EXPONENT_sRGB ); break;
                    case STANDARD_PROFILE::ADOBE_RGB_D50:	bIsGammaCorrect = EnsureGamma( GAMMA_CURVE::STANDARD, GAMMA_EXPONENT_ADOBE ); break;
                    case STANDARD_PROFILE::ADOBE_RGB_D65:	bIsGammaCorrect = EnsureGamma( GAMMA_CURVE::STANDARD, GAMMA_EXPONENT_ADOBE ); break;
                    case STANDARD_PROFILE::PRO_PHOTO:		bIsGammaCorrect = EnsureGamma( GAMMA_CURVE::PRO_PHOTO, GAMMA_EXPONENT_PRO_PHOTO ); break;
                    case STANDARD_PROFILE::RADIANCE:		bIsGammaCorrect = EnsureGamma( GAMMA_CURVE::STANDARD, 1.0f ); break;
                }

                if ( !bIsGammaCorrect ) {
                    recognizedChromaticity = STANDARD_PROFILE::CUSTOM;	// A non-standard gamma curves fails our pre-defined design...
                } 
            }

            // ============= Assign the internal converter depending on the profile =============
            SAFE_DELETE( m_internalConverter );

            switch ( recognizedChromaticity ) {
                case STANDARD_PROFILE::sRGB:
                    m_gammaCurve = GAMMA_CURVE::sRGB;
                    m_gamma = GAMMA_EXPONENT_sRGB;
                    m_internalConverter = new InternalColorConverter_sRGB();
                    break;

                case STANDARD_PROFILE::ADOBE_RGB_D50:
                    m_gammaCurve = GAMMA_CURVE::STANDARD;
                    m_gamma = GAMMA_EXPONENT_ADOBE;
                    m_internalConverter = new InternalColorConverter_AdobeRGB_D50();
                    break;

                case STANDARD_PROFILE::ADOBE_RGB_D65:
                    m_gammaCurve = GAMMA_CURVE::STANDARD;
                    m_gamma = GAMMA_EXPONENT_ADOBE;
                    m_internalConverter = new InternalColorConverter_AdobeRGB_D65();
                    break;

                case STANDARD_PROFILE::PRO_PHOTO:
                    m_gammaCurve = GAMMA_CURVE::PRO_PHOTO;
                    m_gamma = GAMMA_EXPONENT_PRO_PHOTO;
                    m_internalConverter = new InternalColorConverter_ProPhoto();
                    break;

                case STANDARD_PROFILE::RADIANCE:
                    m_gammaCurve = GAMMA_CURVE::STANDARD;
                    m_gamma = 1.0f;
                    m_internalConverter = new InternalColorConverter_Radiance();
                    break;

            default:	// Switch to one of our generic converters
                switch ( m_gammaCurve ) {
                    case GAMMA_CURVE::sRGB:
                        m_internalConverter = new InternalColorConverter_Generic_sRGBGamma( m_RGB2XYZ, m_XYZ2RGB );
                        break;

                    case GAMMA_CURVE::PRO_PHOTO:
                        m_internalConverter = new InternalColorConverter_Generic_ProPhoto( m_RGB2XYZ, m_XYZ2RGB );
                        break;

                        case GAMMA_CURVE::STANDARD:
                        if ( fabs( m_gamma - 1.0f ) < 1e-3f ) {
                            m_internalConverter = new InternalColorConverter_Generic_NoGamma( m_RGB2XYZ, m_XYZ2RGB );
                        }
                        else {
                            m_internalConverter = new InternalColorConverter_Generic_StandardGamma( m_RGB2XYZ, m_XYZ2RGB, m_gamma );
                        }
                    
                    break;
                }
                break;
            }*/
        }

        // Computes the XYZ matrix to perform white balancing between 2 white points assuming the R,G,B chromaticities are the same for the input and output profiles
        // Re-using the equations from http://wiki.nuaj.net/index.php/Color_Transforms#XYZ_Matrices we know that to compute the RGB->XYZ matrix for the input profile
        //	we need to find { Sigma_R, Sigma_G, Sigma_B } in order to scale the xyz_R, xyz_G, xyz_B vectors into XYZ_R, XYZ_G and XYZ_B respectively.
        //
        // We thus obtain the (RGB_in -> XYZ_in) matrix:
        //	| XYZ_R_in | = | Sigma_R_in * xyz_R |
        //	| XYZ_G_in | = | Sigma_G_in * xyz_G |
        //	| XYZ_B_in | = | Sigma_B_in * xyz_B |
        //
        // In the same maner, we can obtain the (RGB_out -> XYZ_out) matrix:
        //	| XYZ_R_out | = | Sigma_R_out * xyz_R |
        //	| XYZ_G_out | = | Sigma_G_out * xyz_G |
        //	| XYZ_B_out | = | Sigma_B_out * xyz_B |
        //
        // The matrix we're after is simply the (XYZ_in -> RGB_in) * (RGB_out -> XYZ_out) matrix...
        //
        static void	compute_white_balance_XYZ_Matrix( const bfloat2& _xyR, const bfloat2& _xyG, const bfloat2& _xyB, const bfloat2& _whitePointIn, const bfloat2& _whitePointOut, float3x3& _whiteBalanceMatrix ) {
                bfloat3	xyz_R( _xyR.x, _xyR.y, 1.0f - _xyR.x - _xyR.y );
                bfloat3	xyz_G( _xyG.x, _xyG.y, 1.0f - _xyG.x - _xyG.y );
                bfloat3	xyz_B( _xyB.x, _xyB.y, 1.0f - _xyB.x - _xyB.y );

                bfloat3	XYZ_W_in;
                xyY2XYZ( bfloat3( _whitePointIn.x, _whitePointIn.y, 1.0f ), XYZ_W_in );

                bfloat3	XYZ_W_out;
                xyY2XYZ( bfloat3( _whitePointOut.x, _whitePointOut.y, 1.0f ), XYZ_W_out );

                // Build xyz matrix and its inverse (common to both input and output)
                float3x3	M_xyz;
                M_xyz.r[0].Set( xyz_R.x, xyz_R.y, xyz_R.z );
                M_xyz.r[1].Set( xyz_G.x, xyz_G.y, xyz_G.z );
                M_xyz.r[2].Set( xyz_B.x, xyz_B.y, xyz_B.z );
                float3x3	M_inv_xyz = M_xyz.Inverse();

                // Retrieve the sigmas for in and out white points
                bfloat3	Sum_RGB_in = XYZ_W_in * M_inv_xyz;
                bfloat3	Sum_RGB_out = XYZ_W_out * M_inv_xyz;

                // Perform the XYZ_in^-1 * XYZ_out product
                M_xyz.Scale( Sum_RGB_out );
                bfloat3	rec_Sum_RGB_in( 1.0f / Sum_RGB_in.x, 1.0f / Sum_RGB_in.y, 1.0f / Sum_RGB_in.z );
                M_inv_xyz.r[0] = rec_Sum_RGB_in * M_inv_xyz.r[0];
                M_inv_xyz.r[1] = rec_Sum_RGB_in * M_inv_xyz.r[1];
                M_inv_xyz.r[2] = rec_Sum_RGB_in * M_inv_xyz.r[2];

                _whiteBalanceMatrix = M_inv_xyz * M_xyz;
        } 

        void white_balance_XYZ_matrix(const bfloat2& _whitePointOut, float3x3& _whiteBalanceMatrix ) {
            ColorManagementSystem::compute_white_balance_XYZ_Matrix(
                //this->m_chromaticities.R, this->m_chromaticities.G, this->m_chromaticities.B, this->m_chromaticities.W, _whitePointOut, _whiteBalanceMatrix 
                this->m_chromaticities.R, this->m_chromaticities.G, this->m_chromaticities.B, this->m_chromaticities.W, _whitePointOut, _whiteBalanceMatrix 
            );
        }

        /// <summary>
        /// Converts a CIEXYZ color to a RGB color
        /// </summary>
        /// <param name="_XYZ"></param>
        /// <returns></returns>
        void XYZ2RGB( const bfloat3& _XYZ, bfloat3& _RGB ) const {
            this->m_internalConverter->XYZ2RGB( _XYZ, _RGB );
        }

        void xyY2RGB( const bfloat2& _xy,const float brightness, bfloat3& _RGB ) const {
            bfloat3	XYZ;
            bfloat3	xyY = bfloat3(_xy.x, _xy.y, brightness);
            ColorManagementSystem::xyY2XYZ(xyY, XYZ);
            this->m_internalConverter->XYZ2RGB(XYZ, _RGB);
        }

        /// <summary>
        /// Converts a RGB color to a CIEXYZ color
        /// </summary>
        /// <param name="_RGB"></param>
        /// <returns></returns>
        void RGB2XYZ( const bfloat3& _RGB, bfloat3& _XYZ ) const {
            this->m_internalConverter->RGB2XYZ( _RGB, _XYZ );
        }

        void RGB2xyY( const bfloat3& _RGB, bfloat3& xyY ) const {
            bfloat3	XYZ;
            this->m_internalConverter->RGB2XYZ( _RGB, XYZ );
            ColorManagementSystem::XYZ2xyY(XYZ, xyY);
        }

        void GammaRGB2LinearRGB( const bfloat3& _gammaRGB, bfloat3& _linearRGB ) const {
            this->m_internalConverter->GammaRGB2LinearRGB( _gammaRGB, _linearRGB );
        }
        void LinearRGB2GammaRGB( const bfloat3& _linearRGB, bfloat3& _gammaRGB ) const {
            this->m_internalConverter->LinearRGB2GammaRGB( _linearRGB, _gammaRGB );
        }

    };

}; // cms
}; // colour_profile
}; // esphome
#pragma once

#include "esphome/core/optional.h"
#include "esphome/components/enable_cms/matrices.h"
#include "esphome/components/enable_cms/color_spaces.h"


namespace esphome {
namespace cms {

    template<class T> inline T clamp( const T& x, const T& min, const T& max )	{ return std::min( std::max( min, x ), max ); }

    using namespace esphome::cms::matrices;

    /// <summary>
    /// Describes the Red, Green, Blue and White Point chromaticities of a color profile
    /// </summary>
    struct Chroma {
        
	/*	static const bfloat2 ILLUMINANT_A;	// Incandescent, tungsten
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

        */


        Cie2dColorSpace R, G, B, W;


        Chroma() : 
            R(Xy_Cie1931()), G(Xy_Cie1931()), B(Xy_Cie1931()), W(Xy_Cie1931())  
        {
        }

        Chroma( const Cie2dColorSpace& r, const Cie2dColorSpace& g, const Cie2dColorSpace& b, const Cie2dColorSpace& w ) : 
            R(r), G(g), B(b), W(w)  
        {
        }

        Chroma( const Xy_Cie1931& r, const Xy_Cie1931& g, const Xy_Cie1931& b, const Xy_Cie1931& w ) : 
            R(r), G(g), B(b), W(w)  
        {
        }

        Chroma( const Uv_Cie1976& r, const Uv_Cie1976& g, const Uv_Cie1976& b, const Uv_Cie1976& w ) : 
            R(r), G(g), B(b), W(w)  
        {
        }  

    };


   /// <summary>
    /// Describes the Red, Green, Blue and White Point chromaticities of a color profile
    /// </summary>
    class RgbChromaTransform {
        // 
        // https://gist.github.com/dbr/24cfd1033c2d59f263e3#file-rgb_to_xyz_matrix-py-L181
        // 
        // Great learning resources can be found here
        // https://patapom.com/blog/Colorimetry/ColorTransforms/
	    float _gamma = 0.0f;
        Cie2dColorSpace _r, _g, _b, _w;
        optional<Vec3> _wp_scale, _wp_scale_inv;
        optional<Matrix3x3> _XYZ2RGB_d, _XYZ2RGB_inv_d;
        optional<Matrix3x3> _XYZ2RGB, _RGB2XYZ;

        public:

        RgbChromaTransform() :
            _r(Xy_Cie1931()),
            _g(Xy_Cie1931()),
            _b(Xy_Cie1931()),
            _w(Xy_Cie1931()) {
        }

        void set_gamma( float g) {
            this->_gamma = g;
        }


        void set_red( Cie2dColorSpace r) {
            this->_r = r;
            this->reset_chroma();
        }

        void set_green( Cie2dColorSpace g) {
            this->_g = g;
            this->reset_chroma();
        }

        void set_blue( Cie2dColorSpace b) {
            this->_b = b;
            this->reset_chroma();
        }

        void set_white_point( Cie2dColorSpace w) {
            this->_w = w;
            this->reset_scale_vec();
        }
    
        XYZ_Cie1931 RGB_to_XYZ(RGB rgb) {
            auto rgb_decomp = rgb.gamma_decompressed(this->_gamma);
            auto XYZ = RGB_2_Cie1931XYZ_transform_matrix() * Vec3(rgb_decomp.r, rgb_decomp.g, rgb_decomp.b);
            return XYZ_Cie1931(XYZ.x, XYZ.y, XYZ.z);
        }

        RGB XYZ_to_RGB(XYZ_Cie1931 XYZ) {
            auto rgb = Cie1931_2_XYZ_transform_matrix() * Vec3(XYZ.X, XYZ.Y, XYZ.Z);
            return RGB(rgb.x, rgb.y, rgb.z).gamma_compressed(this->_gamma);
        }

        Matrix3x3& RGB_2_Cie1931XYZ_transform_matrix() {
            if (!this->_RGB2XYZ.has_value()) {
                auto wp_scale = this->wp_scale_vector();
                auto XYZ2RGB_d = this->XYZ2RGB_d();
                this->_RGB2XYZ = Matrix3x3(
                    XYZ2RGB_d(0,0) * wp_scale.x, XYZ2RGB_d(1,0) * wp_scale.y, XYZ2RGB_d(2,0) * wp_scale.z,
                    XYZ2RGB_d(0,1) * wp_scale.x, XYZ2RGB_d(1,1) * wp_scale.y, XYZ2RGB_d(2,1) * wp_scale.z,
                    XYZ2RGB_d(0,2) * wp_scale.x, XYZ2RGB_d(1,2) * wp_scale.y, XYZ2RGB_d(2,2) * wp_scale.z
                );
            }
            return this->_RGB2XYZ.value();
        }


        Matrix3x3& Cie1931_2_XYZ_transform_matrix() {
            if (!this->_XYZ2RGB.has_value()) {
                auto wp_scale_inv = this->wp_scale_vector_inv();
                auto XYZ2RGB_inv_d = this->XYZ2RGB_inv_d();
                this->_XYZ2RGB = Matrix3x3(     
                    XYZ2RGB_inv_d(0,0) * wp_scale_inv.x, XYZ2RGB_inv_d(1,0) * wp_scale_inv.x, XYZ2RGB_inv_d(2,0) * wp_scale_inv.x,
                    XYZ2RGB_inv_d(0,1) * wp_scale_inv.y, XYZ2RGB_inv_d(1,1) * wp_scale_inv.y, XYZ2RGB_inv_d(2,1) * wp_scale_inv.y,
                    XYZ2RGB_inv_d(0,2) * wp_scale_inv.z, XYZ2RGB_inv_d(1,2) * wp_scale_inv.z, XYZ2RGB_inv_d(2,2) * wp_scale_inv.z
                );
            }
            return this->_XYZ2RGB.value();
        }

        protected: 

        void reset_chroma() {
            this->_XYZ2RGB_d.reset();
            this->_XYZ2RGB_inv_d.reset();
            this->reset_scale_vec();
        }

        void reset_transform_matrix() {
            this->_XYZ2RGB.reset();
            this->_RGB2XYZ.reset();
        }

        void reset_scale_vec() {
            this->_wp_scale.reset();
            this->_wp_scale_inv.reset();
            this->reset_transform_matrix();
        }

        Vec3& wp_scale_vector() {
            if (!this->_wp_scale.has_value()) {
                auto w_XYZ = this->_w.as_xy_cie1931().as_XYZ_cie1931(1.0f);
                this->_wp_scale = Vec3( w_XYZ.X, w_XYZ.Y, w_XYZ.Z ) * this->XYZ2RGB_inv_d();
            }
            return this->_wp_scale.value();
        }

        Vec3& wp_scale_vector_inv() {
            if (!this->_wp_scale_inv.has_value()) {
                auto wp_scale = this->wp_scale_vector();
                this->_wp_scale_inv = Vec3( 1.0f / wp_scale.x, 1.0f / wp_scale.y, 1.0f / wp_scale.z );
            }
            return this->_wp_scale_inv.value();
        }

        Matrix3x3& XYZ2RGB_d() {
            if (!this->_XYZ2RGB_d.has_value()) {
                auto r = _r.as_xy_cie1931();
                auto g = _g.as_xy_cie1931();
                auto b = _b.as_xy_cie1931();

                // For each primary, get the "missing" z (where x+y+z = 1, so z = 1-x-y)
                this->_XYZ2RGB_d = Matrix3x3(
                    r.x, r.y, 1.0f - r.x - r.y, 
                    g.x, g.y, 1.0f - g.x - g.y, 
                    b.x, b.y, 1.0f - b.x - b.y 
                );
            }
            return this->_XYZ2RGB_d.value();
        }

         Matrix3x3& XYZ2RGB_inv_d() {
            if (!this->_XYZ2RGB_inv_d.has_value()) {
                this->_XYZ2RGB_inv_d = this->XYZ2RGB_d().inverse();
            }
            return this->_XYZ2RGB_inv_d.value();
         }
    };

    class CwWwChromaTransform {
        protected:
        float _gamma = 0.00f;
        float _duv_area_limit = 0.09f;
        float _green_tint_duv_impurity = 0.05f;
        float _purple_tint_duv_impurity= 0.05f;
        float _red_cct_mireds_impurity = 20.0f;
        float _blue_cct_mireds_impurity= 20.0f;

        float _warm_white_k;
        float _cold_white_k; 
        optional<float> _white_point_k; 
        std::vector<Uv_Cie1960> _duv_area;

        public:
        
        CwWwChromaTransform() {
            this->build_duv_quadrants();
        }

        void set_gamma( float g) {
            this->_gamma = g;
        }

        void set_warm_white(Cct cct) {
            this->_warm_white_k = cct.kelvin;
        }

        void set_cold_white(Cct cct) {
            this->_cold_white_k = cct.kelvin;
        }

        void set_white_point(Cct cct) {
            this->_white_point_k = cct.kelvin;
        }

        void set_green_tint_duv_impurity(float duv) {
            this->_green_tint_duv_impurity = duv;
        }

        void set_purple_tint_duv_impurity(float duv) {
            this->_purple_tint_duv_impurity = duv;
        }

        void set_red_cct_impurity(float mireds) { 
            this->_red_cct_mireds_impurity = mireds;
        }

        void set_blue_cct_impurity(float mireds) { 
            this->_blue_cct_mireds_impurity = mireds;
        }

        CwWw XYZ_to_CwWw(XYZ_Cie1931 XYZ) {

            auto t_xyY = XYZ.as_xyY_cie1931();
            auto t_vu = Xy_Cie1931(t_xyY.x, t_xyY.y).as_uv_cie1960();

            if(!this->has_duv(this->_duv_area, t_vu)) { 
                return { 0.0f , 0.0f };
            }

            auto k = t_xyY.cct_kelvin_approx();
            auto duv = t_vu.duv_approx();

            float acceptable_tint = duv > 0 ?
                clamp((this->_green_tint_duv_impurity - duv) / this->_green_tint_duv_impurity, 0.0f, 1.0f) :
                clamp((this->_purple_tint_duv_impurity + duv) / this->_purple_tint_duv_impurity, 0.0f, 1.0f);
    
            float lightness = acceptable_tint * t_xyY.Y;
            float wp_k = this->white_point();

            if(k < wp_k) {
                return CwWw {
                    cw: (1 - ((wp_k - k) / (wp_k - this->_warm_white_k))) * lightness, 
                    ww: lightness
                }.gamma_compressed(this->_gamma);
            } else {
                return CwWw { 
                    cw: lightness, 
                    ww: (1 - ((k - wp_k) / (this->_cold_white_k - wp_k))) * lightness
                }.gamma_compressed(this->_gamma);
            }
        }

        protected:
        
        bool has_duv(std::vector<Uv_Cie1960>& vert, Uv_Cie1960& point) {
            int i, j, nvert = vert.size();
            bool c = false;
            
            for(i = 0, j = nvert - 1; i < nvert; j = i++) {
                if( ( (vert[i].v >= point.v ) != (vert[j].v >= point.v) ) &&
                    (point.u <= (vert[j].u - vert[i].u) * (point.v - vert[i].v) / (vert[j].v - vert[i].v) + vert[i].u)
                )
                c = !c;
            }
            
            return c;
        }

        std::vector<Cct> quadrants() {
            return {
                Cct::from_kelvin(1000),
                Cct::from_kelvin(1600),
                Cct::from_kelvin(2000),
                Cct::from_kelvin(2500),
                Cct::from_kelvin(3000),
                Cct::from_kelvin(4000),
                Cct::from_kelvin(5000),
                Cct::from_kelvin(6000),
                Cct::from_kelvin(7000),
                Cct::from_kelvin(8000),
                Cct::from_kelvin(10000),
            };
        }


        void build_duv_quadrants() {
            std::vector<Uv_Cie1960> _green_duv;
            std::vector<Uv_Cie1960> _purple_duv;
            auto bins = this->quadrants();
            for (auto cct : bins) {
                auto max_green_tint = cct.delta_uv_cie1960( this->_duv_area_limit);
                auto max_purple_tint = cct.delta_uv_cie1960(-this->_duv_area_limit);

                _green_duv.push_back(max_green_tint);
                _purple_duv.push_back(max_purple_tint);

            }
            this->_duv_area = _green_duv;
            this->_duv_area.insert(this->_duv_area.end(), _purple_duv.rbegin(), _purple_duv.rend());
        }


        float& white_point() {
            if (!this->_white_point_k.has_value()) {
                this->_white_point_k = (this->_warm_white_k + this->_cold_white_k) / 2;
            }
            return this->_white_point_k.value();
        }
    };


}; // cms
}; // esphome
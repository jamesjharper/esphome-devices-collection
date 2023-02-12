#pragma once

#include "esphome/core/component.h"
#include "esphome/components/color_profile/color_management_system.h"

namespace esphome {
namespace color_profile {

using namespace esphome::color_profile::math;
using namespace esphome::color_profile::cms;

#define clamp_1(in) std::min(std::max(in, 0.0f ), 1.0f)

namespace setup_priority {
    const float CALCULATE_CHROMA = esphome::setup_priority::HARDWARE;
}


class OutputColourProfileTransform {
    protected:
   
    float3x3 _XYZ2RGB;  
    bfloat2 _white_balance;

    float _red_gamma;
    float _green_gamma;  
    float _blue_gamma;

    public:
    cms::Chromaticities _chroma;

    
    OutputColourProfileTransform() {
        this->_XYZ2RGB = float3x3::Identity;
    }

    OutputColourProfileTransform(
        cms::Chromaticities chroma,
        float red_gamma,
        float green_gamma,
        float blue_gamma  
    ) {
        this->_XYZ2RGB = float3x3::Identity;
        this->_chroma = chroma;
        this->_white_balance = chroma.W;
        this->_red_gamma = red_gamma;
        this->_green_gamma = green_gamma;
        this->_blue_gamma = blue_gamma;
        OutputColourProfileTransform::build_transform_matrix();


        ESP_LOGD("_XYZ2RGB", "[%.2f, %.2f, %.2f]", 
            this->_XYZ2RGB.r[0][0], this->_XYZ2RGB.r[0][1], this->_XYZ2RGB.r[0][2]
        );
        ESP_LOGD("_XYZ2RGB", "[%.2f, %.2f, %.2f]", 
            this->_XYZ2RGB.r[1][0], this->_XYZ2RGB.r[1][1], this->_XYZ2RGB.r[1][2]
        );  
        ESP_LOGD("_XYZ2RGB", "[%.2f, %.2f, %.2f]", 
            this->_XYZ2RGB.r[2][0], this->_XYZ2RGB.r[2][1], this->_XYZ2RGB.r[2][2]
        );
        ESP_LOGD("_XYZ2RGB", "");

    }

    void set_white_point_kelvin(float k) {
        cms::compute_white_point_chromaticities_approx_kelvin(k, this->_white_balance);
        OutputColourProfileTransform::build_transform_matrix();
    }

    void set_white_point_mireds(float mireds) {
        cms::compute_white_point_chromaticities_approx_mireds(mireds, this->_white_balance);
        OutputColourProfileTransform::build_transform_matrix();
    }

    void set_white_point_xy(float x, float y) {
        this->_white_balance = bfloat2(x,y);
        this->_chroma.W = bfloat2(x,y);
        OutputColourProfileTransform::build_transform_matrix();
    }

    std::array<float, 3> transform_XYZ2RGB(float x, float y, float z) {
        bfloat3 xyz_t = this->_XYZ2RGB * bfloat3(x, y, z);
        return this->apply_gamma({xyz_t.x, xyz_t.y, xyz_t.z});
    }

    std::array<float, 3> transform_xyY2RGB(float x, float y, float Y) {
        bfloat3	XYZ;
        ColorManagementSystem::xyY2XYZ(bfloat3(x,y,Y), XYZ);
        return transform_XYZ2RGB(XYZ[0], XYZ[1], XYZ[2]);
    }

    protected:


    std::array<float, 3> apply_gamma(std::array<float, 3> rgb) {
        rgb[0] = apply_gamma(rgb[0], this->_red_gamma);
        rgb[1] = apply_gamma(rgb[1], this->_green_gamma);
        rgb[2] = apply_gamma(rgb[2], this->_blue_gamma);
        return rgb;
    }

    static float apply_gamma(float value, float gamma) {
        if (value <= 0.0f)
            return 0.0f;
        if (gamma <= 0.0f)
            return value;

        return powf(value, gamma);
    }


    /*void build_transform_matrix() {
        bfloat3	xyz_R( this->_chroma.R.x, this->_chroma.R.y, 1.0f - this->_chroma.R.x - this->_chroma.R.y );
        bfloat3	xyz_G( this->_chroma.G.x, this->_chroma.G.y, 1.0f - this->_chroma.G.x - this->_chroma.G.y );
        bfloat3	xyz_B( this->_chroma.B.x, this->_chroma.B.y, 1.0f - this->_chroma.B.x - this->_chroma.B.y );

        bfloat3	XYZ_W_in;
        ColorManagementSystem::xyY2XYZ( bfloat3( this->_chroma.W.x, this->_chroma.W.y, 1.0f ), XYZ_W_in );

        bfloat3	XYZ_W_out;
        ColorManagementSystem::xyY2XYZ( bfloat3( this->_white_balance.x, this->_white_balance.y, 1.0f ), XYZ_W_out );

        // Build xyz matrix and its inverse (common to both input and output)
        float3x3 M_xyz;
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

        this->_XYZ2RGB = M_inv_xyz * M_xyz;
    }*/
    
    void build_transform_matrix() {
        bfloat3	xyz_R( this->_chroma.R.x, this->_chroma.R.y, 1.0f - this->_chroma.R.x - this->_chroma.R.y );
        bfloat3	xyz_G( this->_chroma.G.x, this->_chroma.G.y, 1.0f - this->_chroma.G.x - this->_chroma.G.y );
        bfloat3	xyz_B( this->_chroma.B.x, this->_chroma.B.y, 1.0f - this->_chroma.B.x - this->_chroma.B.y );
        bfloat3	XYZ_W;
        ColorManagementSystem::xyY2XYZ( bfloat3( this->_chroma.W.x, this->_chroma.W.y, 1.0f ), XYZ_W );

        // Build the xyz->RGB matrix
        this->_XYZ2RGB.r[0] = xyz_R;
        this->_XYZ2RGB.r[1] = xyz_G;
        this->_XYZ2RGB.r[2] = xyz_B;
        this->_XYZ2RGB.Invert();

        // Knowing the XYZ of the white point, we retrieve the scale factor for each axis x, y and z that will help us get X, Y and Z
        bfloat3	sum_RGB = XYZ_W * this->_XYZ2RGB;

        // Finally, we can retrieve the RGB->XYZ transform
       // RGB2XYZ.r[0] = sum_RGB.x * xyz_R;
        //RGB2XYZ.r[1] = sum_RGB.y * xyz_G;
        //RGB2XYZ.r[2] = sum_RGB.z * xyz_B;

        // And the XYZ->RGB transform is simply the existing xyz->RGB matrix scaled by the reciprocal of the sum
        bfloat3	recSum_RGB( 1.0f / sum_RGB.x, 1.0f / sum_RGB.y, 1.0f / sum_RGB.z );
        this->_XYZ2RGB.r[0] = this->_XYZ2RGB.r[0] * recSum_RGB;
        this->_XYZ2RGB.r[1] = this->_XYZ2RGB.r[1] * recSum_RGB;
        this->_XYZ2RGB.r[2] = this->_XYZ2RGB.r[2] * recSum_RGB;


        std::swap( this->_XYZ2RGB.r[0][1],this->_XYZ2RGB.r[1][0]);  
        std::swap( this->_XYZ2RGB.r[2][1],this->_XYZ2RGB.r[1][2]);
        std::swap( this->_XYZ2RGB.r[2][0],this->_XYZ2RGB.r[0][2]);

    }
};

class OutputCctProfileTransform {
    protected:
   
    std::vector<bfloat2> _duv_area;
   // std::vector<bfloat2> _negative_duv_area;
    float _duv_area_limit = 0.05f;
    float _positive_duv_limit = 0.05f;
    float _negative_duv_limit = 0.05f;
    bfloat2 _white_balance;
    cms::Chromaticities _chroma;
    public:
    

    
    OutputCctProfileTransform() {
    }

    OutputCctProfileTransform(cms::Chromaticities chroma) {
        this->_chroma = chroma;
        this->_white_balance = chroma.W;
        OutputCctProfileTransform::build_duv_quadrants();
    }



    bool is_within_duv_area(std::vector<bfloat2>& vert, bfloat2 point) {
        int i, j, nvert = vert.size();
        bool c = false;
        
        for(i = 0, j = nvert - 1; i < nvert; j = i++) {
            if( ( (vert[i].y >= point.y ) != (vert[j].y >= point.y) ) &&
                (point.x <= (vert[j].x - vert[i].x) * (point.y - vert[i].y) / (vert[j].y - vert[i].y) + vert[i].x)
            )
            c = !c;
        }
        
        return c;
    }

    /*std::array<float, 3> transform_XYZ2CWWW(float x, float y, float z) {
        bfloat3 xyz_t = this->_XYZ2RGB * bfloat3(x, y, z);
        return {xyz_t.x, xyz_t.y, xyz_t.z};
    }*/

    std::array<float,2> transform_xyY2CWWW(float x, float y, float Y) {
        if(!is_within_duv_area(this->_duv_area, bfloat2(x,y))) { 
            return {0.0f , 0.0f };
        }
        
        auto t = compute_white_point_kelvin_approx(bfloat2(x,y));
        auto duv = compute_white_point_duv_approx(bfloat2(x,y));

        auto ww_t = compute_white_point_kelvin_approx(this->_chroma.R);
        auto cw_t = compute_white_point_kelvin_approx(this->_chroma.B);
        auto wb_t = compute_white_point_kelvin_approx(this->_chroma.W);

        float p = duv > 0 ?
            clamp_1((_positive_duv_limit - duv) / _positive_duv_limit) :
            clamp_1((_negative_duv_limit + duv) / _negative_duv_limit);
  

        ESP_LOGD("CCT", "p %f", p);
        if(t < wb_t) {
            // cold white
            return {(1 - ((wb_t - t) / (wb_t - ww_t))) * p, p};
        } else {
            // warm white
            return { p, (1 - ((t - wb_t) / (cw_t - wb_t))) * p};
        }
    }

    protected:

    std::vector<float> quadrants() {
        return {
            1600,
            2000,
            2500,
            3000,
            4000,
            5000,
            6000,
            8000,
            10000,
        };
    }


    void build_duv_quadrants() {

        std::vector<bfloat2> _negative_duv;

        for (float k : this->quadrants()) {


            
            bfloat2 xy_p, xy_n;
            this->duv_cct_to_xy(k, 
                this->_duv_area_limit, 
                this->_duv_area_limit, 
                xy_p, xy_n
            );

            this->_duv_area.push_back(xy_p);
            _negative_duv.push_back(xy_n);

           
        }

        this->_duv_area.insert(this->_duv_area.end(), _negative_duv.rbegin(), _negative_duv.rend());

    }


    void duv_cct_to_xy(float kelvin, float duv_p, float duv_n, bfloat2& xy_p,  bfloat2& xy_n) {

        bfloat2 t0_xt;
        cms::compute_white_point_chromaticities_approx_kelvin(kelvin, t0_xt);

        bfloat2 t1_xt;
        cms::compute_white_point_chromaticities_approx_kelvin(kelvin + 0.1f, t1_xt);

        bfloat2 t0_uv;
        ColorManagementSystem::xy2uv(t0_xt, t0_uv);

        bfloat2 t1_uv;
        ColorManagementSystem::xy2uv(t1_xt, t1_uv);

        auto du = t0_uv.x - t1_uv.x;
        auto dv = t0_uv.y - t1_uv.y;

        auto du2 = du * du;
        auto dv2 = dv * dv;
        auto Lfp = sqrt(du2 + dv2);

        // calculate uv point for delta uv in the positive direction 
        auto up_p = t0_uv.x - (duv_p * (dv / Lfp));
        auto vp_p = (t0_uv.y + (duv_p * (du / Lfp))) * 1.5;

        // calculate uv point for delta uv in the negative direction 
        auto up_n = t0_uv.x - (-duv_n * (dv / Lfp));
        auto vp_n = (t0_uv.y + (-duv_n * (du / Lfp))) * 1.5;

        // convert uv points to xy
        ColorManagementSystem::uv2xy(bfloat2(up_p, vp_p), xy_p);
        ColorManagementSystem::uv2xy(bfloat2(up_n, vp_n), xy_n);
         ESP_LOGD("CCT", "duv %f k -> [%f,%f]/[%f,%f]/[%f,%f]", kelvin,  xy_p[0], xy_p[1], t0_xt[0], t0_xt[1], xy_n[0], xy_n[1]);
    }


};


class InputColourProfileTransform {
    protected:
    cms::Chromaticities _chroma;
    float3x3 _RGB2XYZ;  
    float3x3 _XYZ2RGB;  

    public:

    InputColourProfileTransform() {
        this->_RGB2XYZ = float3x3::Identity;
    }

    InputColourProfileTransform(cms::Chromaticities chroma) {
        this->_RGB2XYZ = float3x3::Identity;
        this->_chroma = chroma;

        InputColourProfileTransform::build_transform_matrix();

        ESP_LOGD("_RGB2XYZ", "[%.2f, %.2f, %.2f]", 
            this->_RGB2XYZ.r[0][0], this->_RGB2XYZ.r[0][1], this->_RGB2XYZ.r[0][2]
        );
        ESP_LOGD("_RGB2XYZ", "[%.2f, %.2f, %.2f]", 
            this->_RGB2XYZ.r[1][0], this->_RGB2XYZ.r[1][1], this->_RGB2XYZ.r[1][2]
        );  
        ESP_LOGD("_RGB2XYZ", "[%.2f, %.2f, %.2f]", 
            this->_RGB2XYZ.r[2][0], this->_RGB2XYZ.r[2][1], this->_RGB2XYZ.r[2][2]
        );
        ESP_LOGD("_RGB2XYZ", "");
    }


    void set_white_point_kelvin(float k) {
        cms::compute_white_point_chromaticities_approx_kelvin(k, this->_chroma.W);
        InputColourProfileTransform::build_transform_matrix();
    }

    void set_white_point_mireds(float mireds) {
        cms::compute_white_point_chromaticities_approx_mireds(mireds, this->_chroma.W);
        InputColourProfileTransform::build_transform_matrix();
    }

    void set_white_point(float x, float y) {
        this->_chroma.W = bfloat2(x,y);
        InputColourProfileTransform::build_transform_matrix();
    }

    std::array<float, 3> transform_RGB2XYZ(float r, float g, float b) {
        bfloat3 xyz_t = this->_RGB2XYZ * bfloat3(r, g, b);
        return {xyz_t.x, xyz_t.y, xyz_t.z};
    }


    std::array<float, 3> transform_RGB2xyY(float r, float g, float b) {
        bfloat3 xyz_t = this->_RGB2XYZ * bfloat3(r, g, b);
        bfloat3	xyy;
        ColorManagementSystem::xyY2XYZ(xyz_t, xyy);
        return transform_RGB2XYZ(xyy[0], xyy[1], xyy[2]);
    }

    protected:

    void build_transform_matrix() {
        bfloat3	xyz_R( this->_chroma.R.x, this->_chroma.R.y, 1.0f - this->_chroma.R.x - this->_chroma.R.y );
        bfloat3	xyz_G( this->_chroma.G.x, this->_chroma.G.y, 1.0f - this->_chroma.G.x - this->_chroma.G.y );
        bfloat3	xyz_B( this->_chroma.B.x, this->_chroma.B.y, 1.0f - this->_chroma.B.x - this->_chroma.B.y );

        bfloat3	XYZ_W;
        ColorManagementSystem::xyY2XYZ( bfloat3( this->_chroma.W.x, this->_chroma.W.y, 1.0f ), XYZ_W );

        // Build the xyz->RGB matrix
        //float3x3 XYZ2RGB;  
        this->_XYZ2RGB.r[0] = xyz_R;
        this->_XYZ2RGB.r[1] = xyz_G;
        this->_XYZ2RGB.r[2] = xyz_B;
        this->_XYZ2RGB.Invert();

        // Knowing the XYZ of the white point, we retrieve the scale factor for each axis x, y and z that will help us get X, Y and Z
        bfloat3	sum_RGB = XYZ_W * this->_XYZ2RGB;

        // Finally, we can retrieve the RGB->XYZ transform
        this->_RGB2XYZ.r[0] = sum_RGB.x * xyz_R;
        this->_RGB2XYZ.r[1] = sum_RGB.y * xyz_G;
        this->_RGB2XYZ.r[2] = sum_RGB.z * xyz_B;

        std::swap(this->_RGB2XYZ.r[0][1],this->_RGB2XYZ.r[1][0]);  
        std::swap( this->_RGB2XYZ.r[2][1],this->_RGB2XYZ.r[1][2]);
        std::swap( this->_RGB2XYZ.r[2][0],this->_RGB2XYZ.r[0][2]);

    }
};


class ColorProfile : public Component {
    protected:

    bfloat2	_white_point; 
    bfloat2	_x_xy = bfloat2(1.0f, 1.0f); // Red or warm white
    bfloat2	_y_xy = bfloat2(0.0f, 1.0f); // Green or mid white
    bfloat2	_z_xy = bfloat2(0.0f, 0.0f); // Blue or cold white


    float _red_gamma = 0.0f; // disable gamma adjustment
    float _green_gamma = 0.0f; // disable gamma adjustment
    float _blue_gamma = 0.0f; // disable gamma adjustment

    public:

    void set_gamma(float g) { 
        this->_red_gamma = g;
        this->_green_gamma = g;
        this->_blue_gamma = g;
    }

    void set_red_gamma(float g) { 
        this->_red_gamma = g;
    }

    void set_green_gamma(float g) { 
        this->_green_gamma = g;
    }

    void set_blue_gamma(float g) { 
        this->_blue_gamma = g;
    }

    void set_white_point_xy(std::array<float, 2> xy) {  
        this->_white_point.Set( xy[0], xy[1]);
    }

    void set_red_xy(std::array<float, 2> xy) {  
        this->_x_xy.Set( xy[0], xy[1]);
    }

    void set_green_xy(std::array<float, 2> xy) {  
        this->_y_xy.Set( xy[0], xy[1]);
    }

    void set_blue_xy(std::array<float, 2> xy) {  
        this->_z_xy.Set( xy[0], xy[1]);
    }

    void set_warm_white_xy(std::array<float, 2> xy) {  
        this->_x_xy.Set( xy[0], xy[1]);
    }
   
    void set_mid_white_xy(std::array<float, 2> xy) {  
        this->_y_xy.Set( xy[0], xy[1]);
    }

    void set_cold_white_xy(std::array<float, 2> xy) {  
        this->_z_xy.Set( xy[0], xy[1]);
    }

    void set_white_point_ct(float mireds) { 
        cms::compute_white_point_chromaticities_approx_mireds(mireds, this->_white_point);
    }

    void set_warm_white_ct(float mireds) { 
        cms::compute_white_point_chromaticities_approx_mireds(mireds, this->_x_xy);
    }

    void set_mid_white_ct(float mireds) { 
        cms::compute_white_point_chromaticities_approx_mireds(mireds, this->_y_xy);
    }

    void set_cold_white_ct(float mireds) { 
        cms::compute_white_point_chromaticities_approx_mireds(mireds, this->_z_xy);
    }

    InputColourProfileTransform get_input_transform() {
        return InputColourProfileTransform(
            cms::Chromaticities(  
                this->_x_xy, 
                this->_y_xy,
                this->_z_xy, 
                this->_white_point 
            )
        );
    }

    OutputColourProfileTransform get_output_transform() {
        return OutputColourProfileTransform(
            cms::Chromaticities(  
                this->_x_xy, 
                this->_y_xy,
                this->_z_xy, 
                this->_white_point
            ),
            this->_red_gamma,
            this->_green_gamma,
            this->_blue_gamma
        );
    }

     OutputCctProfileTransform get_cct_output_transform() {
        return OutputCctProfileTransform(
            cms::Chromaticities(  
                this->_x_xy, 
                this->_y_xy,
                this->_z_xy, 
                this->_white_point 
            )
        );
    }
    

    void setup() override {
        ESP_LOGD("ColorProfile", "Using colour profile (CIE 1931) x(%f,%f) y(%f,%f) z(%f,%f) w(%f,%f)", 
            this->_x_xy.x, this->_x_xy.y,
            this->_y_xy.x, this->_y_xy.y,
            this->_z_xy.x, this->_z_xy.y,
            this->_white_point.x, this->_white_point.y
        );
    }

    float get_setup_priority() const override { 
      // Must run before any device set its IO state
      return setup_priority::CALCULATE_CHROMA; 
    }
};

}  // namespace color_profile
}  // namespace esphome

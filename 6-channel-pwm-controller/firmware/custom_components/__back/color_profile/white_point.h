#pragma once
#include "esphome/components/color_profile/math.h"

namespace esphome {
namespace color_profile {
namespace cms {

    using namespace esphome::color_profile::math;
//https://www.colour-science.org/api/0.3.1/html/_modules/colour/temperature/cct.html
	void compute_white_point_chromaticities_approx_kelvin( float _blackBodyTemperatureKelvin, bfloat2& _whitePointChromaticities ) {
		/*double t = _blackBodyTemperatureKelvin;
		double t2 = t * t;
		double t3 = t * t2;

		double x = t < 4000.0f ? 
			0.2661239 * (1e9 / t3) - 0.2343580 * (1e6/t2) + 0.8776956 * (1e3/t) + 0.179910 : 
			-3.0258469 * (1e9 / t3) + 2.1070379 * (1e6/ t2) + 0.2226347 * (1e3/t) + 0.240390;

		double x2 = x * x;
		double x3 = x * x2;

		double y = 0;

		if (t < 2222.0f) {
			y = (-1.1063814 * x3) - (1.34811020 * x2) + (2.18555832 * x) - 0.20219683;
		} else if (t < 4000.f) {
			y = (-0.9549476 * x3) - (1.37418593 * x2) + (2.09137015 *x) - 0.16748867;
		} else {
			y = (+3.0817580 * x3) - (5.87338670 * x2) + (3.75112997 *x) - 0.37001483;
		}    

		_whitePointChromaticities.Set( float(x), float(y) );*/


		double t = _blackBodyTemperatureKelvin;
		double t2 = t * t;
		auto u = (0.860117757 + (0.000154118254 * t) + (0.000000128641212 * t2)) /
				(1.0 + (0.000842420235 * t) + (0.000000708145163 * t2));

		auto v = (0.317398726 + (0.0000422806245 * t) + (0.0000000420481691 * t2)) /
				(1.0 - (0.0000289741816 * t) + (0.000000161456053 * t2));



		_whitePointChromaticities.Set(
			(3.0f * u) / (2.0f * u - (8.0f * v) + 4.0f),
			(2.0f * v) / (2.0f * u - (8.0f * v) + 4.0f)
		);

	}

	void compute_white_point_chromaticities_approx_mireds( float _blackBodyTemperatureMireds, bfloat2& _whitePointChromaticities ) {
		auto temperature_k = 1000000.0f / _blackBodyTemperatureMireds;
    	compute_white_point_chromaticities_approx_kelvin(temperature_k, _whitePointChromaticities);
	}


	float compute_white_point_kelvin_approx(bfloat2 xy ) {
		/*float n = (xy.x -0.3320f) / (0.1858f - xy.y);
		float n2 = n * n;
		float n3 = n * n2;
		return (437.0f * n3) + (3601.0f * n2) + (6861.0f * n) + 5517.0f;*/

		double c = ((double)xy.x - 0.3320) / ((double)0.1858 - xy.y);

	//	c = (xyy[0] - 0.332) / (0.1858 - xyy[1])
		return ((449.0 * c +3525.0)* c + 6823.3)* c + 5520.33;
	}

	float compute_white_point_mireds_approx(bfloat2 xy ) {
		return 1000000.0f / compute_white_point_kelvin_approx(xy);
	}

	void compute_white_point_xy_approx(bfloat2 xy, bfloat2& wb_xy ) {
		// Find the colour temperature of the given xy color
		auto k = compute_white_point_kelvin_approx(xy);

		//ESP_LOGD("ColorProfile", "%fk", k);
		// convert colour temperature to xy, removing the hue of the original color
		compute_white_point_chromaticities_approx_kelvin(k, wb_xy);
	}


	float Q_rsqrt( float number ) {
		//https://en.wikipedia.org/wiki/Fast_inverse_square_root
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y  = number;
		i  = * ( long * ) &y;                       // evil floating point bit level hacking
		i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
		y  = * ( float * ) &i;
		y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
	//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

		return y;
	}

	// Calculate the duv, the distance from the black body curve.  
	//https://www.waveformlighting.com/tech/calculate-duv-from-cie-1931-xy-coordinates/
	/*float compute_white_point_duv_approx(bfloat2 xy ) {
		double u = (4.0 * xy.x) / (-2.0 * xy.x + (12.0 * xy.y) + 3.0);
		double v = (6.0 * xy.y) / (-2.0 * xy.x + (12.0 * xy.y) + 3.0);

		auto vd2 = (v - 0.24) * (v - 0.24);
 		auto ud2 = (u - 0.292) * (u - 0.292);
		auto Lfp = sqrt(ud2 + vd2);
		//auto Lfp = Q_rsqrt(ud2 + vd2);
		auto a = acos((u-0.292) / Lfp);

		auto a2 = a * a;
		auto a3 = a * a2;
		auto a4 = a * a3;
		auto a5 = a * a4;
		auto a6 = a * a5;

		auto Lbb = (-0.00616793 * a6) + (0.0893944 * a5) + (-0.5179722 * a4) + (1.5317403 * a3) + (-2.4243787 * a2) + (1.925865 * a) + -0.471106;
		return Lfp - Lbb;
	}*/


		// Calculate the duv, the distance from the black body curve.  
	//https://cormusa.org/wp-content/uploads/2018/04/CORM_2011_Calculation_of_CCT_and_Duv_and_Practical_Conversion_Formulae.pdf
	float compute_white_point_duv_approx(bfloat2 xy ) {
		double u = (4.0 * xy.x) / (-2.0 * xy.x + (12.0 * xy.y) + 3.0);
		double v = (6.0 * xy.y) / (-2.0 * xy.x + (12.0 * xy.y) + 3.0);

		auto vd2 = (v - 0.24) * (v - 0.24);
 		auto ud2 = (u - 0.292) * (u - 0.292);
		auto Lfp = sqrt(ud2 + vd2);


		//auto Lfp = Q_rsqrt(ud2 + vd2);
		auto a = acos((u-0.292) / Lfp);

		auto a2 = a * a;
		auto a3 = a * a2;
		auto a4 = a * a3;
		auto a5 = a * a4;
		auto a6 = a * a5;

		auto k6 =-0.00616793f;
		auto k5 =0.0893944f;
		auto k4 =-0.5179722f;
		auto k3 =1.5317403f;
		auto k2 =-2.4243787f;
		auto k1 =1.925865f;
		auto k0 =-0.471106f;


		auto Lbb = (k6 * a6) + (k5 * a5) + (k4 * a4) + (k3 * a3) + (k2 * a2) + (k1 * a) + k0;
		return Lfp - Lbb;
	}
	
}; // cms
}; // color_profile
}; // esphome
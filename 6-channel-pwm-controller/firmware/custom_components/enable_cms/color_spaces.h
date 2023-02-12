#pragma once
#include "esphome/core/optional.h"
#include <math.h>

namespace esphome {
namespace cms {

	template<class T> inline T min3( const T& a, const T& b, const T& c )	{ return std::min( std::min( a, b ), c ); }
	template<class T> inline T max3( const T& a, const T& b, const T& c )	{ return std::max( std::max( a, b ), c ); }

	struct Uv_Cie1976;
	struct Uv_Cie1960;
	struct xyY_Cie1931;

	struct RGB;
	struct HSL;

    static float gamma_compression(float value, float gamma) {
        if (value <= 0.0f)
            return 0.0f;
        if (gamma <= 0.0f)
            return value;

        return powf(value, gamma);
    }

     static float gamma_decompression(float value, float gamma) {
        if (value <= 0.0f)
            return 0.0f;
        if (gamma <= 0.0f)
            return value;

        return powf(value, 1.0f / gamma);
    }

	struct RGB {
		float r;
		float g;
		float b;

		RGB() : r(0.0), g(0.0), b(0.0) {};
		RGB(float r, float g, float b) : r(g), g(g), b(b) {};

		RGB gamma_compressed(float gamma) {
			return RGB(
				gamma_compression(this->r, gamma),
				gamma_compression(this->g, gamma),
				gamma_compression(this->b, gamma)
			);
		}

		RGB gamma_decompressed(float gamma) {
			return RGB(
				gamma_decompression(this->r, gamma),
				gamma_decompression(this->g, gamma),
				gamma_decompression(this->b, gamma)
			);
		}

		HSL as_hsl();
	};

	struct HSL {
		float h;
		float s;
		float l;

		HSL() : h(0.0), s(0.0), l(0.0) {};
		HSL(float h, float s, float l) : h(h), s(s), l(l) {};

		RGB as_rgb();

		private:

		float hue2rgb(float p, float q, float t);
	};

	struct CwWw {
		float cw;
		float ww;

		CwWw() : cw(0.0), ww(0.0) {};
		CwWw(float cw, float ww) : cw(cw), ww(ww) {};

		CwWw gamma_compressed(float gamma) {
			return CwWw( 
				gamma_compression(this->cw, gamma), 
				gamma_compression(this->ww, gamma)
			);
		}

		CwWw gamma_decompressed(float gamma) {
			return CwWw(
				gamma_decompression(this->cw, gamma),
				gamma_decompression(this->ww, gamma)
			);
		}
	};

	struct XYZ_Cie1931 {
		float X;
		float Y;
		float Z;

		XYZ_Cie1931() : X(0.0), Y(0.0), Z(0.0) {};
		XYZ_Cie1931(float X, float Y, float Z) : X(X), Y(Y), Z(Z) {};

		xyY_Cie1931 as_xyY_cie1931();
	};

	struct xyY_Cie1931 {
		float x;
		float y;
		float Y;

		xyY_Cie1931() : x(0.0), y(0.0), Y(0.0) {};
		xyY_Cie1931(float x, float y, float Y) : x(x), y(y), Y(Y) {};

		float cct_kelvin_approx();
		XYZ_Cie1931 as_XYZ_cie1931();
	};

	struct Xy_Cie1931 {
		float x;
		float y;

		Xy_Cie1931() : x(0.0), y(0.0) {};
		Xy_Cie1931(float x, float y) : x(x), y(y) {};

		Uv_Cie1960 as_uv_cie1960();
		Uv_Cie1976 as_uv_cie1976();
		xyY_Cie1931 as_xyY_cie1931(float Y);
		XYZ_Cie1931 as_XYZ_cie1931(float Y);

		float cct_kelvin_approx();
	};

	struct Uv_Cie1960 {
		float u;
		float v;

		Uv_Cie1960() : u(0.0), v(0.0) {};
		Uv_Cie1960(float u, float v) : u(u), v(v) {};

		Xy_Cie1931 as_xy_cie1931();
		Uv_Cie1976 as_uv_cie1976();

		float cct_kelvin_approx();

		float duv_approx();
	};


	struct Uv_Cie1976 {
		float u;
		float v;

		Uv_Cie1976() : u(0.0), v(0.0) {};
		Uv_Cie1976(float u, float v) : u(u), v(v) {};

		Xy_Cie1931 as_xy_cie1931();
		Uv_Cie1960 as_uv_cie1960();
	};


	class Cct {
		float sin_t;
		float cos_t;
		bool calc_theta = true;
		
		public:
		Uv_Cie1960 uv;

		float kelvin;

		Cct() {};
		Cct(float kelvin, Uv_Cie1960 xy) : uv(xy), kelvin(kelvin) {};

		static Cct from_mireds(float m);

		static Cct from_kelvin(float k);
	
		Uv_Cie1960 delta_uv_cie1960(float delta_uv);
	};


	class Cie2dColorSpace
	{
		enum {
			CIE_1931, 
			CIE_1976
		};

		optional<Xy_Cie1931> xy;
		optional<Uv_Cie1976> uvt;

		public:
		Cie2dColorSpace(const Xy_Cie1931& xy) : xy(xy) {};

		Cie2dColorSpace(const Uv_Cie1976& uvt) : uvt(uvt) {};


		Xy_Cie1931 as_xy_cie1931()
		{
			if (this->xy.has_value())
				return (*this->xy);
			this->xy = (*this->uvt).as_xy_cie1931();
			return (*this->xy);
		}

		Uv_Cie1976 as_uv_cie1976()
		{
			if (this->uvt.has_value())
				return (*this->uvt);
			this->uvt = (*this->xy).as_uv_cie1976();
			return (*this->uvt);
		}
	};
} // cms
} // esphome
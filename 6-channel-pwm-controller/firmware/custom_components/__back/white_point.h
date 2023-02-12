#pragma once
#include "esphome/components/ct_emulator/math.h"

// https://github.com/Patapom/GodComplex/blob/9cc9bb29d203cd971449da1eb06315c4d48d234c/Packages/ImageUtilityLib/ColorMatchingFunctions.cpp
#include <vector>
 
namespace esphome {
namespace cms {
namespace white_point {

    using namespace math;
	using namespace std;

	struct ColorMatchingLUT {

		static const float CMF_WAVELENGTH_START;
		static const float CMF_WAVELENGTH_END;
		static const float CMF_WAVELENGTH_STEP;
		static const float CMF_WAVELENGTH_RCP_STEP;
		static float ms_colorMatchingFunctions[];
	};

	// According to https://en.wikipedia.org/wiki/Black-body_radiation#Planck.27s_law_of_black-body_radiation
	// Planck's law states that[30]
	// 
	//	I(nu,T) = [(2.h.nu^3) / c²] * 1 / [e^(h.nu/(k.T)) - 1]
	//
	// Where:
	//	I(nu,T) is the power (the energy per unit time) radiated per unit area of emitting surface in the normal direction per unit solid angle per unit frequency by a black body at temperature T, also known as spectral radiance;
	//	h is the Planck constant;
	//	c is the speed of light in a vacuum;
	//	k is the Boltzmann constant;
	//	nu is the frequency of the electromagnetic radiation;
	//	T is the absolute temperature of the body.
	//
	double compute_black_body_radiation_power( float _blackBodyTemperature, float _wavelength );

	void integrate_spectral_power_distribution_into_XYZ( U32 _wavelengthsCount, float _wavelengthStart, float _wavelengthStep, std::vector<double> _spectralPowerDistibution, bfloat3& _XYZ );

	std::vector<double> build_spectral_power_distribution_for_black_body( float _blackBodyTemperature, U32 _wavelengthsCount, float _wavelengthStart, float _wavelengthStep);

	void compute_white_point_chromaticities_kelvin( float _blackBodyTemperatureKelvin, bfloat2& _whitePointChromaticities );

	void compute_white_point_chromaticities_mireds( float _blackBodyTemperatureMireds, bfloat2& _whitePointChromaticities );

	// The x chromaticity is given by:
	//	x = 0.244063 + 0.09911 * (10^3 / T) + 2.9678 * (10^6 / T^2) - 4.6070 * (10^9 / T^3)		for 4000K < T < 7000K
	//	x = 0.237040 + 0.24748 * (10^3 / T) + 1.9018 * (10^6 / T^2) - 2.0064 * (10^9 / T^3)		for 7000K <= T < 25000K
	//
	// The y chromaticity is infered from x by the relation:
	//	y = -0.275 + 2.870 x - 3 x^2
	//
	void compute_white_point_chromaticities_approx_kelvin( float _blackBodyTemperatureKelvin, bfloat2& _whitePointChromaticities );

	// The x chromaticity is given by:
	//	x = 0.244063 + 0.09911 * (10^3 / T) + 2.9678 * (10^6 / T^2) - 4.6070 * (10^9 / T^3)		for 4000K < T < 7000K
	//	x = 0.237040 + 0.24748 * (10^3 / T) + 1.9018 * (10^6 / T^2) - 2.0064 * (10^9 / T^3)		for 7000K <= T < 25000K
	//
	// The y chromaticity is infered from x by the relation:
	//	y = -0.275 + 2.870 x - 3 x^2
	//
	void compute_white_point_chromaticities_approx_mireds( float _blackBodyTemperatureMireds, bfloat2& _whitePointChromaticities );
}; // white_point
}; // cms
}; // esphome
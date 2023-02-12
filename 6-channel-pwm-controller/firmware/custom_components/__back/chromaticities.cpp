
#include "esphome/components/ct_emulator/chromaticities.h"

using namespace esphome::cms;

const bfloat2 Chromaticities::ILLUMINANT_A( 0.44757f, 0.40745f );	// Incandescent, tungsten
const bfloat2 Chromaticities::ILLUMINANT_D50( 0.34567f, 0.35850f );	// Daylight, Horizon
const bfloat2 Chromaticities::ILLUMINANT_D55( 0.33242f, 0.34743f );	// Mid-Morning, Mid-Afternoon
const bfloat2 Chromaticities::ILLUMINANT_D65( 0.31271f, 0.32902f );	// Daylight, Noon, Overcast (sRGB reference illuminant)
const bfloat2 Chromaticities::ILLUMINANT_E( 1/3.0f, 1/3.0f );		// Reference

// Standard chromaticities
const Chromaticities Chromaticities::Chromaticities::Empty;
const Chromaticities Chromaticities::Chromaticities::sRGB           ( bfloat2( 0.6400f, 0.3300f ), bfloat2( 0.3000f, 0.6000f ), bfloat2( 0.1500f, 0.0600f ), ILLUMINANT_D65 );
const Chromaticities Chromaticities::Chromaticities::AdobeRGB_D50   ( bfloat2( 0.6400f, 0.3300f ), bfloat2( 0.2100f, 0.7100f ), bfloat2( 0.1500f, 0.0600f ), ILLUMINANT_D50 );
const Chromaticities Chromaticities::Chromaticities::AdobeRGB_D65   ( bfloat2( 0.6400f, 0.3300f ), bfloat2( 0.2100f, 0.7100f ), bfloat2( 0.1500f, 0.0600f ), ILLUMINANT_D65 );
const Chromaticities Chromaticities::Chromaticities::ProPhoto       ( bfloat2( 0.7347f, 0.2653f ), bfloat2( 0.1596f, 0.8404f ), bfloat2( 0.0366f, 0.0001f ), ILLUMINANT_D50 );
const Chromaticities Chromaticities::Chromaticities::Radiance       ( bfloat2( 0.6400f, 0.3300f ), bfloat2( 0.2900f, 0.6000f ), bfloat2( 0.1500f, 0.0600f ), ILLUMINANT_E );

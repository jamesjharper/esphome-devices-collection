#pragma once
#include <math.h>
#include "esphome/core/helpers.h"

namespace color_science {
#define clamp( in, low, high ) std::min(std::max(in, low ), high)
#define clamp_1( in) clamp(in, 0.0f, 1.0f)

void black_body_to_cwww(
    float target_mired,
    float brightness, 
    float gamma,  
    float min_mireds, 
    float max_mireds,
    bool constant_brightness,
    float* out_cw, 
    float* out_ww
) {
    auto mired_range = max_mireds - min_mireds;
    auto cold = ((max_mireds - target_mired) / mired_range);
    auto warm = 1.0f - cold;

    const float white_level = esphome::gamma_correct(brightness, gamma);
    const float cw_level = esphome::gamma_correct(cold, gamma);
    const float ww_level = esphome::gamma_correct(warm, gamma);

    if (!constant_brightness) {
        *out_cw = clamp_1(white_level * cw_level);
        *out_ww = clamp_1(white_level * ww_level);
    } else {
        // Just multiplying by cw_level / (cw_level + ww_level) would divide out the brightness information from the
        // cold_white and warm_white settings (i.e. cw=0.8, ww=0.4 would be identical to cw=0.4, ww=0.2), which breaks
        // transitions. Use the highest value as the brightness for the white channels (the alternative, using cw+ww/2,
        // reduces to cw/2 and ww/2, which would still limit brightness to 100% of a single channel, but isn't very
        // useful in all other aspects -- that behaviour can also be achieved by limiting the output power).
        const float sum = cw_level > 0 || ww_level > 0 ? cw_level + ww_level : 1;  // Don't divide by zero.
        *out_cw = clamp_1(white_level * std::max(cw_level, ww_level) * cw_level / sum);
        *out_ww = clamp_1(white_level * std::max(cw_level, ww_level) * ww_level / sum);
    }
}

void black_body_to_mw(
    float target_mired,
    float brightness,  
    float gamma,  
    float min_mireds, 
    float max_mireds,   
    float output_mireds,
    bool constant_brightness,
    float* out_w
) {
    float cw, ww;
    if (target_mired >= output_mireds) {
        black_body_to_cwww(target_mired, brightness, gamma, output_mireds, max_mireds, constant_brightness, &cw, &ww);
        *out_w = cw;
    } else {
        black_body_to_cwww(target_mired, brightness, gamma, min_mireds, output_mireds, constant_brightness, &cw, &ww);
        *out_w = ww;
    }
}

void black_body_rgb(
    float target_mired, 
    float brightness,
    float gamma, 
    float* out_red, 
    float* out_green, 
    float* out_blue 
)
{
    // Special thanks Roger Lew
    // https://stackoverflow.com/questions/7229895/display-temperature-as-a-color-with-c
    auto temperature_k = 1000000.0f / target_mired;
    auto x = (temperature_k / 1000.0f);
    auto x2 = x * x;
    auto x3 = x2 * x;
    auto x4 = x3 * x;
    auto x5 = x4 * x;

    float r, g, b = 0.0f;

    // red
    if (temperature_k <= 6600) 
        r = 1.0f;
    else
        r = 0.0002889f * x5 - 0.01258f * x4 + 0.2148f * x3 - 1.776f * x2 + 6.907f * x - 8.723f;

    // green
    if (temperature_k <= 6600)
        g = -4.593e-05f * x5 + 0.001424f * x4 - 0.01489f * x3 + 0.0498f * x2 + 0.1669f * x - 0.1653f;
    else
        g = -1.308e-07f * x5 + 1.745e-05f * x4 - 0.0009116f * x3 + 0.02348f * x2 - 0.3048f * x + 2.159f;

    // blue
    if (temperature_k <= 2000.0f)
        b = 0.0f;
    else if (temperature_k < 6600.0f)
        b = 1.764e-05f * x5 + 0.0003575f * x4 - 0.01554f * x3 + 0.1549f * x2 - 0.3682f * x + 0.2386f;
    else
        b = 1.0f;
    ;
    *out_red = clamp_1(esphome::gamma_correct(r * brightness, gamma));
    *out_green = clamp_1(esphome::gamma_correct(g * brightness, gamma));
    *out_blue = clamp_1(esphome::gamma_correct(b * brightness, gamma));
}
}

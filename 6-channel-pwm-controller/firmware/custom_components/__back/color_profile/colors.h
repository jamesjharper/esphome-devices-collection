#pragma once

namespace esphome {
namespace color_profile {
namespace colors {

    //https://gist.github.com/ciembor/1494530
    #define MIN(a,b) ((a)<(b)?(a):(b))
    #define MAX(a,b) ((a)>(b)?(a):(b))


    std::array<float, 3> rgb2hsl(std::array<float, 3> rgb) {
    
        std::array<float, 3> hsl;
       
        float r = rgb[0];
        float g = rgb[1];
        float b = rgb[2];

        float max = MAX(MAX(rgb[0],rgb[1]),rgb[2]);
        float min = MIN(MIN(rgb[0],rgb[1]),rgb[2]);

        float h = (max + min) / 2;
        float s = h;
        float l = h;

        if (max == min) {
            h = s = 0; // achromatic
        }
        else {
            float d = max - min;
            s = (l > 0.5) ? d / (2 - max - min) : d / (max + min);
            
            if (max == r) {
                h = (g - b) / d + (g < b ? 6 : 0);
            }
            else if (max == g) {
                h = (b - r) / d + 2;
            }
            else if (max == b) {
                h = (r - g) / d + 4;
            }
            
            h /= 6;
        }

        return {h, s, l};
    
    }

    float hue2rgb(float p, float q, float t) {

        if (t < 0) 
            t += 1;
        if (t > 1) 
            t -= 1;
        if (t < 1./6) 
            return p + (q - p) * 6 * t;
        if (t < 1./2) 
            return q;
        if (t < 2./3)   
            return p + (q - p) * (2./3 - t) * 6;
            
        return p;
    
    }

    ////////////////////////////////////////////////////////////////////////

    /*
    * Converts an HSL color value to RGB. Conversion formula
    * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
    * Assumes h, s, and l are contained in the set [0, 1] and
    * returns RGB in the set [0, 255].
    */
    std::array<float, 3> hsl2rgb(std::array<float, 3> hsl) {

       
        float h = hsl[0];
        float s = hsl[1];
        float l = hsl[2];
        float r = l;
        float g = l;
        float b = l;
        
        if(0 != s) {
            float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
            float p = 2 * l - q;
            r = hue2rgb(p, q, h + 1./3);
            g = hue2rgb(p, q, h);
            b = hue2rgb(p, q, h - 1./3);
        }

        return {r,g,b};

    }

    std::array<float, 3> rgb_saturation(std::array<float, 3> rgb, float saturation) {
        auto hsl = colors::rgb2hsl(rgb);
        hsl[1] *= saturation;
        return hsl2rgb(hsl);
    }

     std::array<float, 3> rgb_brightness(std::array<float, 3> rgb, float brightness) {
        rgb[0] *= brightness;
        rgb[1] *= brightness;
        rgb[2] *= brightness;
        return rgb;
    }
}  // namespace colors
}  // namespace color_profile
}  // namespace esphome
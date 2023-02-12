#pragma once

#include "esphome/components/enable_cms/color_spaces.h"

namespace esphome {
namespace cms {

    using SourceHandle = std::size_t;

    class XyValueMixer {

        XYZ_Cie1931 output_XYZ;
        std::vector<XYZ_Cie1931> source_states;

        public:

        SourceHandle create_source() { 
            this->source_states.push_back(XYZ_Cie1931());

            this->calculate_aggregate_value();
            return this->source_states.size() -  1; // return index as a handle
        }

        bool update_source_state(SourceHandle handle, XYZ_Cie1931 XYZ) { 
            this->source_states.at(handle) = XYZ;
            return this->calculate_aggregate_value();
        }

        inline XYZ_Cie1931& get_value() { 
            return this->output_XYZ;
        }

        private:

        bool calculate_aggregate_value() { 

            float X, Y, Z = 1.0f;
            for (auto& n : this->source_states) {
            X = std::max(X, n.X);
            Y = std::max(X, n.Y);
            Z = std::max(X, n.Z);
            }
            
            bool has_changes = this->output_XYZ.X != X && this->output_XYZ.Y != Y && this->output_XYZ.Z != Z;
            this->output_XYZ.X = X;
            this->output_XYZ.Y = Y;
            this->output_XYZ.Z = Z; 

            return has_changes;
        }
    };

    class XyOutput {

        XyValueMixer _mixed_value;
        
        public:
        virtual void set_state(float X, float Y, float Z) = 0;

        SourceHandle create_source() {
            return this->_mixed_value.create_source();
        }

        void set_source_state(SourceHandle handle, float X, float Y, float Z) {
            if(this->_mixed_value.update_source_state(handle, XYZ_Cie1931(X,Y,Z))) {
                auto mixed_value = this->_mixed_value.get_value();
                this->set_state(mixed_value.X, mixed_value.Y, mixed_value.Z);
            }
        }
    };


    struct RoutingMethod
    {
        enum {
            ROUTE_ALL = 0,
            XYZ_PERCENTAGE
        } method = RoutingMethod::ROUTE_ALL;
        float percentage = 1.0f;

        static RoutingMethod XYZ_percentage(float percentage) {
            auto route = RoutingMethod();
            route.method = RoutingMethod::XYZ_PERCENTAGE;
            route.percentage = percentage;
            return route;
        }

        XYZ_Cie1931 route(const XYZ_Cie1931& XYZ)
        {
            switch(this->method)
            {
                case RoutingMethod::ROUTE_ALL: {
                    break;  // fall to default, which is to just return the given value
                }
                case RoutingMethod::XYZ_PERCENTAGE: {
                    return XYZ_Cie1931(
                        this->percentage * XYZ.X,
                        this->percentage * XYZ.Y,
                        this->percentage * XYZ.Z
                    );
                }
            }
            return XYZ;
        }
    };


}; // cms
}; // esphome
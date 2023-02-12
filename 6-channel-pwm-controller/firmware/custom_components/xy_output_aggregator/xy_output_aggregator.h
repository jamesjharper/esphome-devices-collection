#pragma once
#include "esphome/core/log.h"
#include "esphome/core/optional.h"
#include "esphome/components/enable_cms/color_spaces.h"
#include "esphome/components/enable_cms/outputs.h"

namespace esphome {
namespace output {

using namespace esphome::cms;

class XyOutputAggregator : public XyOutput
{

  class XyValueAggregator {

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

  std::vector<XyOutput*> outputs;
  XyValueAggregator aggregated_value;

  public:


  void set_state(float X, float Y, float Z) override {
    for (auto& output : this->outputs) {
      output->set_state(X, Y, Z);
    }
  }

  SourceHandle create_source() {
      return this->aggregated_value.create_source();
  }

  void set_source_state(SourceHandle handle, float X, float Y, float Z) {
      if(this->aggregated_value.update_source_state(handle, XYZ_Cie1931(X,Y,Z))) {
        auto new_value = this->aggregated_value.get_value();
        this->set_state(new_value.X, new_value.Y, new_value.Z);
      }
  }
};
}  // namespace output
}  // namespace esphome

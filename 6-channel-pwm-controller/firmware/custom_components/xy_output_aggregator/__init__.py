import esphome.codegen as cg
from esphome.components import output

multi_sink_float_output_ns = cg.esphome_ns.namespace('output')
MultiSinkOutput = multi_sink_float_output_ns.class_("MultiSinkOutput")
MultiSinkFloatOutput = multi_sink_float_output_ns.class_("MultiSinkFloatOutput", output.FloatOutput, cg.Component, MultiSinkOutput)
import esphome.codegen as cg
from esphome.components import output

multi_sink_source_float_output_ns = cg.esphome_ns.namespace('output')
MultiSinkSourceFloatOutput = multi_sink_source_float_output_ns.class_('MultiSinkSourceFloatOutput', output.FloatOutput, cg.Component)
SinkTarget = multi_sink_source_float_output_ns.class_('SinkTarget', cg.EntityBase)

# TODO: Find a way to ref def from other file
MultiSinkOutput = multi_sink_source_float_output_ns.class_("MultiSinkOutput")
MultiSinkFloatOutput = multi_sink_source_float_output_ns.class_("MultiSinkFloatOutput", output.FloatOutput, cg.Component, MultiSinkOutput)

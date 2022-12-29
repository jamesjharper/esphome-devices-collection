import esphome.codegen as cg
from esphome.components import output

dynamic_frequency_ledc_ns = cg.esphome_ns.namespace('output')
DynamicFrequencyLedc = dynamic_frequency_ledc_ns.class_("DynamicFrequencyLedc", output.FloatOutput, cg.Component)
from esphome import pins, automation
import esphome.codegen as cg
from esphome.components import output


ledc_ns = cg.esphome_ns.namespace("ledc_ext")
LEDCOutput = ledc_ns.class_("LEDCOutput", output.FloatOutput, cg.Component)
SetFrequencyAction = ledc_ns.class_("SetFrequencyAction", automation.Action)


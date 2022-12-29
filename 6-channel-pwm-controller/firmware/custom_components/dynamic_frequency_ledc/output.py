import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
#from esphome.components.ledc.output import LEDCOutput
from esphome.const import CONF_ID

from . import dynamic_frequency_ledc_ns, DynamicFrequencyLedc
from ..ledc_ext import LEDCOutput

#CONF_HIGH_OUTPUT_ID = "high_output_id"
#CONF_LOW_HIGH_OUTPUT_ID = "low_output_id"

CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend({ 
    cv.Required(CONF_ID): cv.declare_id(DynamicFrequencyLedc),
    cv.Required("ledc"): cv.use_id(LEDCOutput),
    cv.Optional("low_frequency", default="1220Hz"): cv.frequency,
    cv.Optional("high_frequency", default="4882Hz"): cv.frequency,
    cv.Optional("transition_duty", default="2%"): cv.percentage,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await output.register_output(var, config)

    ledc = await cg.get_variable(config["ledc"])
    cg.add(var.set_ledc(ledc))

    low_frequency = config["low_frequency"]
    cg.add(var.set_low_frequency(low_frequency))

    high_frequency = config["low_frequency"]
    cg.add(var.set_high_frequency(high_frequency))

    transition_duty = config["transition_duty"]
    cg.add(var.set_transition_duty(transition_duty))

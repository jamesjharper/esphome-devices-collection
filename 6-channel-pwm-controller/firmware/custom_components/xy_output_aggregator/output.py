import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
from esphome.const import CONF_ID

from . import multi_sink_float_output_ns, MultiSinkOutput,  MultiSinkFloatOutput

CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend({ 
    cv.Required(CONF_ID): cv.declare_id(MultiSinkFloatOutput),
    cv.Required("output_id"): cv.use_id(output.FloatOutput),
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await output.register_output(var, config)

    out = await cg.get_variable(config["output_id"])
    cg.add(var.set_output(out))
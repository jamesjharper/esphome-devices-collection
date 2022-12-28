import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
from esphome.const import CONF_ID


multi_sink_float_output_ns = cg.esphome_ns.namespace('output')
multi_sink_float_output = multi_sink_float_output_ns.class_("MultiSinkFloatOutput", output.FloatOutput, cg.Component)

CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend({
    cv.Required(CONF_ID): cv.declare_id(multi_sink_float_output),
    cv.Required("output_id"): cv.use_id(output.FloatOutput),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield output.register_output(var, config)

    out = yield cg.get_variable(config["output_id"])
    cg.add(var.set_output(out))

    yield cg.register_component(var, config)



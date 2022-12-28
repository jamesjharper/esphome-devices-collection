import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
from esphome.const import CONF_ID


multi_sink_source_float_output_ns = cg.esphome_ns.namespace('output')
multi_sink_source_float_output = multi_sink_source_float_output_ns.class_('MultiSinkSourceFloatOutput', output.FloatOutput, cg.Component)

sink_target_ns = cg.esphome_ns.namespace('output')
sink_target = sink_target_ns.class_('SinkTarget')


CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend({
    cv.Required(CONF_ID): cv.declare_id(multi_sink_source_float_output),
    cv.Optional("sinks"): cv.ensure_list(
            cv.Schema({
                cv.GenerateID(): cv.declare_id(sink_target),

            }).extend(cv.ENTITY_BASE_SCHEMA)
        ),
}).extend(cv.COMPONENT_SCHEMA)





def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield output.register_output(var, config)

    #out = yield cg.get_variable(config["output_id"])
    #cg.add(var.set_output(out))

    for i, conf in enumerate(config.get("sinks", [])):
        #sink_def = cg.Pvariable(conf[CONF_ID], var.get_component(i))

        #  void add_sink(MultiSinkOutput* target_sink, float max_attenuation = 1.0f) { 
        cg.add(var.add_sink(conf))
        #await cg.register_component(comp, conf)

    yield cg.register_component(var, config)
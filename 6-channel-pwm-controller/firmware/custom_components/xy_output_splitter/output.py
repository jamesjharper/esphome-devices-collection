import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
from esphome.const import CONF_ID

from . import multi_sink_source_float_output_ns, MultiSinkSourceFloatOutput, SinkTarget
from ..multi_sink_float_output import MultiSinkOutput,  MultiSinkFloatOutput

CONF_SINKS = "sinks"
CONF_OUTPUT_ID = "output_id"
CONF_AGG_PERCENT = "percentage"

CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend({
    cv.Required(CONF_ID): cv.declare_id(MultiSinkSourceFloatOutput),
    cv.Optional(CONF_SINKS): cv.ensure_list(
            cv.Schema({
                cv.GenerateID(): cv.declare_id(SinkTarget),
                cv.Required(CONF_OUTPUT_ID): cv.use_id(MultiSinkFloatOutput),
                cv.Required(CONF_AGG_PERCENT): cv.percentage,
            }).extend(cv.ENTITY_BASE_SCHEMA)
        ),
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await output.register_output(var, config)

    if CONF_SINKS in config:
        for sink in config[CONF_SINKS]:
            snk = cg.new_variable(sink[CONF_ID], SinkTarget())

            target = await cg.get_variable(sink[CONF_OUTPUT_ID])
            cg.add(snk.set_target(target))

            agg_percent = sink[CONF_AGG_PERCENT]
            cg.add(snk.set_aggregate_percentage(agg_percent))

            cg.add(var.add_sink(snk))

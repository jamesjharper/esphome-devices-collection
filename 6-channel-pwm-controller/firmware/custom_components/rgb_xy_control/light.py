import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light, output
from esphome.const import (CONF_OUTPUT_ID, CONF_ID)
from ..color_output.output import ColorOutput

CONF_OUTPUT_DEVICE = "output"
CONF_ADDITIONAL_OUTPUT_DEVICES = "additional_outputs"

light_ns = cg.esphome_ns.namespace("light")
RgbXyControl = light_ns.class_("RgbXyControl", light.LightOutput, cg.Component)

CONFIG_SCHEMA = light.RGB_LIGHT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(RgbXyControl),
        cv.Required(CONF_OUTPUT_DEVICE): cv.use_id(ColorOutput),
        cv.Optional(CONF_ADDITIONAL_OUTPUT_DEVICES): cv.ensure_list(
            cv.Schema({
                cv.Optional(CONF_ID): cv.use_id(ColorOutput)
            }).extend(cv.ENTITY_BASE_SCHEMA)
        ),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    await light.register_light(var, config)

    if CONF_OUTPUT_DEVICE in config:
        output = await cg.get_variable(config[CONF_OUTPUT_DEVICE])
        cg.add(var.set_output_device(output))


    if CONF_ADDITIONAL_OUTPUT_DEVICES in config:  
        for device in config[CONF_ADDITIONAL_OUTPUT_DEVICES]:
            output = await cg.get_variable(device[CONF_ID])
            cg.add(var.add_additional_output_device(output))

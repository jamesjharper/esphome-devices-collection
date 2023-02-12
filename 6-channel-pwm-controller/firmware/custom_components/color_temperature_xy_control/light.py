import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light, output
from esphome.const import (CONF_OUTPUT_ID, CONF_ID)
from ..color_output.output import ColorOutput

CONF_CMS_OUTPUT_DEVICE = "output"
CONF_CMS_WARM_WHITE_COLOR_TEMPERATURE = "warm_white"
CONF_CMS_COLD_WHITE_COLOR_TEMPERATURE = "cold_white"

light_ns = cg.esphome_ns.namespace("light")
ColorTemperatureXyControl = light_ns.class_("ColorTemperatureXyControl", light.LightOutput)


CONFIG_SCHEMA = light.RGB_LIGHT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(ColorTemperatureXyControl),
        cv.Required(CONF_CMS_OUTPUT_DEVICE): cv.use_id(ColorOutput),
        cv.Required(CONF_CMS_COLD_WHITE_COLOR_TEMPERATURE): cv.color_temperature,
        cv.Required(CONF_CMS_WARM_WHITE_COLOR_TEMPERATURE): cv.color_temperature,
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    await light.register_light(var, config)

    output = await cg.get_variable(config[CONF_CMS_OUTPUT_DEVICE])
    cg.add(var.set_output_device(output))

    cg.add(var.set_cold_white_color_temperature(config[CONF_CMS_COLD_WHITE_COLOR_TEMPERATURE]))
    cg.add(var.set_warm_white_color_temperature(config[CONF_CMS_WARM_WHITE_COLOR_TEMPERATURE]))


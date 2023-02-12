import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output

from esphome.const import CONF_ID
from ..enable_cms import XyOutput

from ..enable_cms import (RGB_PROFILE_CONFIG_SCHEMA, RgbProfile, to_rgb_profile_code)
from ..enable_cms import (CONF_CMS_RGB_COLOR_PROFILE_ID, CONF_CMS_RGB_COLOR_PROFILE, CONF_CMS_RED_OUTPUT_ID, CONF_CMS_GREEN_OUTPUT_ID, CONF_CMS_BLUE_OUTPUT_ID)

output_ns = cg.esphome_ns.namespace("output")
RgbXyOutput = output_ns.class_("RgbXyOutput", cg.Component, XyOutput)

CONFIG_SCHEMA = cv.Schema({ 
        cv.Required(CONF_ID): cv.declare_id(RgbXyOutput),
        cv.Optional(CONF_CMS_RED_OUTPUT_ID): cv.use_id(output.FloatOutput),
        cv.Optional(CONF_CMS_GREEN_OUTPUT_ID): cv.use_id(output.FloatOutput),
        cv.Optional(CONF_CMS_BLUE_OUTPUT_ID): cv.use_id(output.FloatOutput),
        cv.Optional(CONF_CMS_RGB_COLOR_PROFILE_ID): cv.use_id(RgbProfile),
        cv.Optional(CONF_CMS_RGB_COLOR_PROFILE): RGB_PROFILE_CONFIG_SCHEMA,
    },
    cv.has_exactly_one_key(CONF_CMS_RGB_COLOR_PROFILE, CONF_CMS_RGB_COLOR_PROFILE_ID)
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
   
    # Color Profile
    if CONF_CMS_RGB_COLOR_PROFILE_ID in config:     
        color_profile = await cg.get_variable(config[CONF_CMS_RGB_COLOR_PROFILE_ID])
        cg.add(var.set_color_profile(color_profile))

    if CONF_CMS_RGB_COLOR_PROFILE in config:
        await to_rgb_profile_code(config[CONF_CMS_RGB_COLOR_PROFILE])
        inline_profile = await cg.get_variable(config[CONF_CMS_RGB_COLOR_PROFILE][CONF_ID])
        cg.add(var.set_color_profile(inline_profile))

    # RGB
    if CONF_CMS_RED_OUTPUT_ID in config:     
        red_output = await cg.get_variable(config[CONF_CMS_RED_OUTPUT_ID])
        cg.add(var.set_red_output(red_output))

    if CONF_CMS_GREEN_OUTPUT_ID in config:     
        green_output = await cg.get_variable(config[CONF_CMS_GREEN_OUTPUT_ID])
        cg.add(var.set_green_output(green_output))

    if CONF_CMS_BLUE_OUTPUT_ID in config:     
        blue_output = await cg.get_variable(config[CONF_CMS_BLUE_OUTPUT_ID])
        cg.add(var.set_blue_output(blue_output))

    await cg.register_component(var, config)

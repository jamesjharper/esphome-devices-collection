import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output

from esphome.const import CONF_ID
from ..enable_cms import XyOutput
from ..enable_cms import (CWWW_PROFILE_CONFIG_SCHEMA, CwwwProfile, to_cwww_profile_code)

from ..enable_cms import (CONF_CMS_COLD_WHITE_OUTPUT_ID, CONF_CMS_WARM_WHITE_OUTPUT_ID, CONF_CMS_CWWW_COLOR_PROFILE_ID, CONF_CMS_CWWW_COLOR_PROFILE)

output_ns = cg.esphome_ns.namespace("output")
CwwwXyOutput = output_ns.class_("CwwwXyOutput", cg.Component, XyOutput)


CONF_CMS_CWWW_COLOR_PROFILE_ID = "cwww_profile_id"
CONF_CMS_CWWW_COLOR_PROFILE = "cwww_profile"


CONFIG_SCHEMA = cv.Schema({ 
        cv.Required(CONF_ID): cv.declare_id(CwwwXyOutput),
        cv.Optional(CONF_CMS_COLD_WHITE_OUTPUT_ID): cv.use_id(output.FloatOutput),
        cv.Optional(CONF_CMS_WARM_WHITE_OUTPUT_ID): cv.use_id(output.FloatOutput),
        cv.Optional(CONF_CMS_CWWW_COLOR_PROFILE_ID): cv.use_id(CwwwProfile),
        cv.Optional(CONF_CMS_CWWW_COLOR_PROFILE): CWWW_PROFILE_CONFIG_SCHEMA,
    },
    cv.has_exactly_one_key(CONF_CMS_CWWW_COLOR_PROFILE, CONF_CMS_CWWW_COLOR_PROFILE_ID)
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
   
    # Color Profile
    if CONF_CMS_CWWW_COLOR_PROFILE_ID in config:     
        profile = await cg.get_variable(config[CONF_CMS_CWWW_COLOR_PROFILE_ID])
        cg.add(var.set_profile(profile))

    if CONF_CMS_CWWW_COLOR_PROFILE in config:
        await to_cwww_profile_code(config[CONF_CMS_CWWW_COLOR_PROFILE])
        inline_profile = await cg.get_variable(config[CONF_CMS_CWWW_COLOR_PROFILE][CONF_ID])
        cg.add(var.set_profile(inline_profile))

    if CONF_CMS_COLD_WHITE_OUTPUT_ID in config:     
        cold_white_output = await cg.get_variable(config[CONF_CMS_COLD_WHITE_OUTPUT_ID])
        cg.add(var.set_cold_white_output(cold_white_output))

    if CONF_CMS_WARM_WHITE_OUTPUT_ID in config:     
        warm_white_output = await cg.get_variable(config[CONF_CMS_WARM_WHITE_OUTPUT_ID])
        cg.add(var.set_warm_white_output(warm_white_output))

    await cg.register_component(var, config)

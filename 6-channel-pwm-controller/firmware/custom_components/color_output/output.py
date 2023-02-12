import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light, output
from esphome.const import (CONF_OUTPUT_ID, CONF_ID)
#from ..rgb_profile.output import RgbProfile
from ..enable_cms import XyOutput
from ..enable_cms import (RGB_PROFILE_CONFIG_SCHEMA, RgbProfile, to_rgb_profile_code)
from ..enable_cms import (CONF_CMS_OUTPUT_DEVICES, CONF_CMS_XY_OUTPUT_ID, CONF_CMS_XYZ_ROUTE_PERCENT, CONF_CMS_SOURCE_COLOR_PROFILE_ID, CONF_CMS_SOURCE_COLOR_PROFILE)

output_ns = cg.esphome_ns.namespace("output")
RoutedDestination = output_ns.class_("RoutedDestination")
ColorOutput = output_ns.class_("ColorOutput", cg.Component)

CONFIG_SCHEMA = cv.Schema({ 
    cv.GenerateID(CONF_ID): cv.declare_id(ColorOutput),
    cv.Optional(CONF_CMS_SOURCE_COLOR_PROFILE_ID): cv.use_id(RgbProfile),
    cv.Optional(CONF_CMS_SOURCE_COLOR_PROFILE): RGB_PROFILE_CONFIG_SCHEMA,
    cv.Required(CONF_CMS_OUTPUT_DEVICES): cv.ensure_list(
        cv.Schema({
            cv.GenerateID(): cv.declare_id(RoutedDestination),
            cv.Required(CONF_CMS_XY_OUTPUT_ID): cv.use_id(XyOutput),
            cv.Optional(CONF_CMS_XYZ_ROUTE_PERCENT): cv.percentage,
        }).extend(cv.ENTITY_BASE_SCHEMA)
    )},
    cv.has_exactly_one_key(CONF_CMS_SOURCE_COLOR_PROFILE_ID, CONF_CMS_SOURCE_COLOR_PROFILE)
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if CONF_CMS_SOURCE_COLOR_PROFILE_ID in config:
        profile = await cg.get_variable(config[CONF_CMS_SOURCE_COLOR_PROFILE_ID])
        cg.add(var.set_source_color_profile(profile))

    if CONF_CMS_SOURCE_COLOR_PROFILE in config:
        await to_rgb_profile_code(config[CONF_CMS_SOURCE_COLOR_PROFILE])
        inline_profile = await cg.get_variable(config[CONF_CMS_SOURCE_COLOR_PROFILE][CONF_ID])
        cg.add(var.set_source_color_profile(inline_profile))

    for routes in config[CONF_CMS_OUTPUT_DEVICES]:
        route = cg.new_variable(routes[CONF_ID], RoutedDestination())

        target = await cg.get_variable(routes[CONF_CMS_XY_OUTPUT_ID])
        cg.add(route.set_output(target))

        if CONF_CMS_XYZ_ROUTE_PERCENT in routes:
            percentage = routes[CONF_CMS_XYZ_ROUTE_PERCENT]
            cg.add(route.route_xyz_percentage(percentage))

        cg.add(var.add_routed_destination(route))
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import CONF_ID

enable_cms_ns = cg.esphome_ns.namespace("cms")
output_ns = cg.esphome_ns.namespace("output")
profile_ns = cg.esphome_ns.namespace("profiles")
XyOutput = output_ns.class_("XyOutput")




# routing
CONF_CMS_OUTPUT_DEVICES = "routes"
CONF_CMS_XY_OUTPUT_ID = "xy_output"
CONF_CMS_XYZ_ROUTE_PERCENT = "xyz_route_percentage"

# Color profiles - general
CONF_CMS_GAMMA = "gamma"
CONF_CMS_WHITE_POINT_XY = "white_point_xy"
CONF_CMS_WHITE_POINT_CCT = "white_point_cct"

# Color profiles - RGB
CONF_CMS_SOURCE_COLOR_PROFILE_ID = "source_color_profile_id"
CONF_CMS_SOURCE_COLOR_PROFILE = "source_color_profile"

CONF_CMS_RGB_COLOR_PROFILE_ID = "rgb_profile_id"
CONF_CMS_RGB_COLOR_PROFILE = "rgb_profile"

CONF_CMS_RED_XY = "red_xy"
CONF_CMS_GREEN_XY = "green_xy"
CONF_CMS_BLUE_XY = "blue_xy"

# Color profiles - CWWW
CONF_CMS_CWWW_COLOR_PROFILE_ID = "cwww_profile_id"
CONF_CMS_CWWW_COLOR_PROFILE = "cwww_profile"

CONF_CMS_WHITE_POINT_COLOR_TEMPERATURE = "white_point"
CONF_CMS_COLD_WHITE_COLOR_TEMPERATURE = "cold_white"
CONF_CMS_WARM_WHITE_COLOR_TEMPERATURE = "warm_white"

CONF_CMS_TINT_IMPURITY = "tint_impurity"

CONF_CMS_GREEN_TINT_IMPURITY = "green_tint_impurity"
CONF_CMS_PURPLE_TINT_IMPURITY = "purple_tint_impurity"

CONF_CMS_CCT_IMPURITY = "cct_impurity"
CONF_CMS_RED_CCT_IMPURITY = "red_cct_impurity"
CONF_CMS_BLUE_CCT_IMPURITY = "blue_cct_impurity"

# output devices
CONF_CMS_RED_OUTPUT_ID = "red"
CONF_CMS_GREEN_OUTPUT_ID = "green"
CONF_CMS_BLUE_OUTPUT_ID = "blue"
CONF_CMS_WARM_WHITE_OUTPUT_ID = "warm_white"
CONF_CMS_MID_WHITE_OUTPUT_ID = "mid_white"
CONF_CMS_COLD_WHITE_OUTPUT_ID = "cold_white"

CONFIG_SCHEMA = cv.Schema({ 
}).extend(cv.COMPONENT_SCHEMA)

def cie_xy(value):
    if isinstance(value, list):
        if len(value) != 2:
            raise cv.Invalid(f"CIE xy must have a length of two, not {len(value)}")
        try:
            x, y = float(value[0]), float(value[1])
        except ValueError:
            # pylint: disable=raise-missing-from
            raise cv.Invalid("CIE xy coordinates must be decimals")

        if x < 0:
            raise cv.Invalid("CIE x must at least be greater the 0.0")
        if x > 0.75:
            raise cv.Invalid("CIE x must at most be less the 0.75")

        if y < 0:
            raise cv.Invalid("CIE y must at least be greater the 0.0")
        if y > 0.85:
            raise cv.Invalid("CIE y must at most be less the 0.85")

        return [x, y]

    raise cv.Invalid(
        "Invalid value '{}' for CIE xy. Only x,y is allowed."
    )

def duv(value):
    if isinstance(duv, (int, float)):

        if x < 0:
            raise cv.Invalid("Delta uv must be greater then 0.0")
        if x > 0.5:
            raise cv.Invalid("Delta uv must be less then 0.5")

        return duv

    raise cv.Invalid(
        "Invalid value '{}' for Delta uv. Only values between 0.0 and 0.5 is allowed."
    )

# RGB Profile Common 
RgbProfile = profile_ns.class_("RgbProfile", cg.Component)

RGB_PROFILE_CONFIG_SCHEMA = cv.Schema({ 
    cv.GenerateID(CONF_ID): cv.declare_id(RgbProfile),
    cv.Required(CONF_CMS_RED_XY): cie_xy,
    cv.Required(CONF_CMS_GREEN_XY): cie_xy,
    cv.Required(CONF_CMS_BLUE_XY): cie_xy,
    cv.Optional(CONF_CMS_WHITE_POINT_XY): cie_xy,
    cv.Optional(CONF_CMS_WHITE_POINT_CCT): cv.color_temperature,
    cv.Optional(CONF_CMS_GAMMA): cv.positive_float,

}).extend(cv.COMPONENT_SCHEMA)


async def to_rgb_profile_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
   
    if CONF_CMS_GAMMA in config:
        g = config[CONF_CMS_GAMMA]
        cg.add(var.set_gamma(g))

    if CONF_CMS_RED_XY in config:
        xy = config[CONF_CMS_RED_XY]
        cg.add(var.set_red_xy(xy[0], xy[1]))

    if CONF_CMS_GREEN_XY in config:
        xy = config[CONF_CMS_GREEN_XY]
        cg.add(var.set_green_xy(xy[0], xy[1]))

    if CONF_CMS_BLUE_XY in config:
        xy = config[CONF_CMS_BLUE_XY]
        cg.add(var.set_blue_xy(xy[0], xy[1]))

    if CONF_CMS_WHITE_POINT_CCT in config:
        k = config[CONF_CMS_WHITE_POINT_CCT]
        cg.add(var.set_white_point_cct(k))

    if CONF_CMS_WHITE_POINT_XY in config:
        xy = config[CONF_CMS_WHITE_POINT_XY]
        cg.add(var.set_white_point_xy(xy[0], xy[1]))
    
    await cg.register_component(var, config)
    

# CWWW Profile Common 
CwwwProfile = profile_ns.class_("CwwwProfile", cg.Component)

CWWW_PROFILE_CONFIG_SCHEMA  = cv.Schema({ 
    cv.GenerateID(CONF_ID): cv.declare_id(CwwwProfile),

    cv.Optional(CONF_CMS_TINT_IMPURITY): duv,
    cv.Optional(CONF_CMS_GREEN_TINT_IMPURITY): duv,
    cv.Optional(CONF_CMS_PURPLE_TINT_IMPURITY): duv,
    cv.Optional(CONF_CMS_CCT_IMPURITY): cv.color_temperature,
    cv.Optional(CONF_CMS_RED_CCT_IMPURITY): cv.color_temperature,
    cv.Optional(CONF_CMS_BLUE_CCT_IMPURITY): cv.color_temperature,

    cv.Optional(CONF_CMS_WHITE_POINT_COLOR_TEMPERATURE): cv.color_temperature,
    cv.Required(CONF_CMS_COLD_WHITE_COLOR_TEMPERATURE): cv.color_temperature,
    cv.Required(CONF_CMS_WARM_WHITE_COLOR_TEMPERATURE): cv.color_temperature,
    cv.Optional(CONF_CMS_GAMMA): cv.positive_float

}).extend(cv.COMPONENT_SCHEMA)

async def to_cwww_profile_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
   
    if CONF_CMS_GAMMA in config:
        g = config[CONF_CMS_GAMMA]
        cg.add(var.set_gamma(g))

    if CONF_CMS_WHITE_POINT_COLOR_TEMPERATURE in config:
        wp_cct = config[CONF_CMS_WHITE_POINT_COLOR_TEMPERATURE]
        cg.add(var.set_white_point_cct(wp_cct))

    if CONF_CMS_COLD_WHITE_COLOR_TEMPERATURE in config:
        cw_cct = config[CONF_CMS_COLD_WHITE_COLOR_TEMPERATURE]
        cg.add(var.set_cold_white_cct(cw_cct))

    if CONF_CMS_WARM_WHITE_COLOR_TEMPERATURE in config:
        ww_cct = config[CONF_CMS_WARM_WHITE_COLOR_TEMPERATURE]
        cg.add(var.set_warm_white_cct(ww_cct))

    if CONF_CMS_TINT_IMPURITY in config:
        imp = config[CONF_CMS_TINT_IMPURITY]
        cg.add(var.set_tint_duv_impurity(imp))

    if CONF_CMS_GREEN_TINT_IMPURITY in config:
        imp = config[CONF_CMS_GREEN_TINT_IMPURITY]
        cg.add(var.set_green_tint_duv_impurity(imp))

    if CONF_CMS_PURPLE_TINT_IMPURITY in config:
        imp = config[CONF_CMS_PURPLE_TINT_IMPURITY]
        cg.add(var.set_purple_tint_duv_impurity(imp))

    await cg.register_component(var, config)
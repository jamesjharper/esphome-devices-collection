import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import CONF_ID

color_profile_ns = cg.esphome_ns.namespace("color_profile")
ColorProfile = color_profile_ns.class_("ColorProfile", cg.Component)

CONF_RED_XY = "red_xy"
CONF_GREEN_XY = "green_xy"
CONF_BLUE_XY = "blue_xy"


CONF_GAMMA = "gamma"

CONF_WHITE_POINT_XY = "white_point_xy"
CONF_WHITE_POINT_COLOR_TEMPERATURE = "white_point"

CONF_COLD_WHITE_XY = "cold_white_xy"
CONF_COLD_WHITE_COLOR_TEMPERATURE = "cold_white"

CONF_MID_WHITE_XY = "mid_white_xy"
CONF_MID_WHITE_COLOR_TEMPERATURE = "mid_white"

CONF_WARM_WHITE_XY = "warm_white_xy"
CONF_WARM_WHITE_COLOR_TEMPERATURE = "warm_white"

def cie_xy(value):
    if isinstance(value, list):
        if len(value) != 2:
            raise Invalid(f"CIE xy must have a length of two, not {len(value)}")
        try:
            x, y = float(value[0]), float(value[1])
        except ValueError:
            # pylint: disable=raise-missing-from
            raise Invalid("CIE xy coordinates must be decimals")

        if x < 0:
            raise Invalid("CIE x must at least be greater the 0.0")
        if x > 0.75:
            raise Invalid("CIE x must at most be less the 0.75")

        if y < 0:
            raise Invalid("CIE y must at least be greater the 0.0")
        if y > 0.85:
            raise Invalid("CIE y must at most be less the 0.85")

        return [x, y]

    raise Invalid(
        "Invalid value '{}' for CIE xy. Only x,y is allowed."
    )

CONFIG_SCHEMA = cv.Schema({ 
    cv.Required(CONF_ID): cv.declare_id(ColorProfile),

    cv.Optional(CONF_RED_XY): cie_xy,
    cv.Optional(CONF_GREEN_XY): cie_xy,
    cv.Optional(CONF_BLUE_XY): cie_xy,

    cv.Optional(CONF_GAMMA, default=0.0): cv.positive_float, # zero disables gamma adjustment

    cv.Optional(CONF_WHITE_POINT_XY): cie_xy,
    cv.Optional(CONF_WHITE_POINT_COLOR_TEMPERATURE): cv.color_temperature,

    cv.Optional(CONF_COLD_WHITE_XY): cie_xy,
    cv.Optional(CONF_COLD_WHITE_COLOR_TEMPERATURE): cv.color_temperature,

    cv.Optional(CONF_MID_WHITE_XY): cie_xy,
    cv.Optional(CONF_MID_WHITE_COLOR_TEMPERATURE): cv.color_temperature,

    cv.Optional(CONF_WARM_WHITE_XY): cie_xy,
    cv.Optional(CONF_WARM_WHITE_COLOR_TEMPERATURE): cv.color_temperature,

}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
   
    # gamma
    if CONF_GAMMA in config:
        g = config[CONF_GAMMA]
        cg.add(var.set_gamma(g))

    # RGB
    if CONF_BLUE_XY in config:
        blue = config[CONF_BLUE_XY]
        cg.add(var.set_blue_xy(blue))

    if CONF_RED_XY in config:
        red = config[CONF_RED_XY]
        cg.add(var.set_red_xy(red))

    if CONF_GREEN_XY in config:
        green = config[CONF_GREEN_XY]
        cg.add(var.set_green_xy(green))

    if CONF_BLUE_XY in config:
        blue = config[CONF_BLUE_XY]
        cg.add(var.set_blue_xy(blue))

    # White point
    if CONF_WHITE_POINT_XY in config:
        wp = config[CONF_WHITE_POINT_XY]
        cg.add(var.set_white_point_xy(wp))
    elif CONF_WHITE_POINT_COLOR_TEMPERATURE in config:
        wp_ct = config[CONF_WHITE_POINT_COLOR_TEMPERATURE]
        cg.add(var.set_white_point_ct(wp_ct))

    # CW + MW + WW
    if CONF_COLD_WHITE_XY in config:
        cw_xy = config[CONF_COLD_WHITE_XY]
        cg.add(var.set_cold_white_xy(cw_xy))
    elif CONF_COLD_WHITE_COLOR_TEMPERATURE in config:
        cw_ct = config[CONF_COLD_WHITE_COLOR_TEMPERATURE]
        cg.add(var.set_cold_white_ct(cw_ct))

    if CONF_MID_WHITE_XY in config:
        mw_xy = config[CONF_MID_WHITE_XY]
        cg.add(var.set_mid_white_xy(mw_xy))
    elif CONF_MID_WHITE_COLOR_TEMPERATURE in config:
        mw_ct = config[CONF_MID_WHITE_COLOR_TEMPERATURE]
        cg.add(var.set_mid_white_ct(mw_ct))

    if CONF_WARM_WHITE_XY in config:
        ww_xy = config[CONF_WARM_WHITE_XY]
        cg.add(var.set_warm_white_xy(ww_xy))
    elif CONF_WARM_WHITE_COLOR_TEMPERATURE in config:
        ww_ct = config[CONF_WARM_WHITE_COLOR_TEMPERATURE]
        cg.add(var.set_warm_white_ct(ww_ct))
    
    await cg.register_component(var, config)

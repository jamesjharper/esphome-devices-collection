import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light, output
from esphome.const import (
    CONF_BLUE, 
    CONF_GREEN, 
    CONF_RED, 
    CONF_OUTPUT_ID, 
    CONF_WARM_WHITE,
    CONF_COLD_WHITE, 
    CONF_COLD_WHITE_COLOR_TEMPERATURE,
    CONF_WARM_WHITE_COLOR_TEMPERATURE,
    CONF_CONSTANT_BRIGHTNESS
)

CONF_MID_WHITE = "mid_white"
CONF_MID_WHITE_COLOR_TEMPERATURE = "mid_white_color_temperature"

CONF_EMULATED_WARM_WHITE_COLOR_TEMPERATURE = "emulated_warm_white_color_temperature"
CONF_EMULATED_COLD_WHITE_COLOR_TEMPERATURE = "emulated_cold_white_color_temperature"

ct_emulator_ns = cg.esphome_ns.namespace("ct_emulator")
ColourTemperatureEmulatorLightOutput = ct_emulator_ns.class_("ColourTemperatureEmulatorLightOutput", light.LightOutput)

CONFIG_SCHEMA = light.RGB_LIGHT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(ColourTemperatureEmulatorLightOutput),
        cv.Optional(CONF_RED): cv.use_id(output.FloatOutput),
        cv.Optional(CONF_GREEN): cv.use_id(output.FloatOutput),
        cv.Optional(CONF_BLUE): cv.use_id(output.FloatOutput),
        cv.Optional(CONF_WARM_WHITE): cv.use_id(output.FloatOutput),
        cv.Optional(CONF_MID_WHITE): cv.use_id(output.FloatOutput),
        cv.Optional(CONF_COLD_WHITE): cv.use_id(output.FloatOutput),
        cv.Optional(CONF_COLD_WHITE_COLOR_TEMPERATURE): cv.color_temperature,
        cv.Optional(CONF_MID_WHITE_COLOR_TEMPERATURE): cv.color_temperature,
        cv.Optional(CONF_WARM_WHITE_COLOR_TEMPERATURE): cv.color_temperature,
        cv.Optional(CONF_EMULATED_WARM_WHITE_COLOR_TEMPERATURE): cv.color_temperature,
        cv.Optional(CONF_EMULATED_COLD_WHITE_COLOR_TEMPERATURE): cv.color_temperature,
        cv.Optional(CONF_CONSTANT_BRIGHTNESS, default=False): cv.boolean,
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    await light.register_light(var, config)

    if CONF_RED in config:
        red = await cg.get_variable(config[CONF_RED])
        cg.add(var.set_red(red))

    if CONF_GREEN in config:  
        green = await cg.get_variable(config[CONF_GREEN])
        cg.add(var.set_green(green))

    if CONF_BLUE in config:     
        blue = await cg.get_variable(config[CONF_BLUE])
        cg.add(var.set_blue(blue))

    if CONF_WARM_WHITE in config:     
        warm_white = await cg.get_variable(config[CONF_WARM_WHITE])
        cg.add(var.set_warm_white(warm_white)) 

    if CONF_MID_WHITE in config:     
        mid_white = await cg.get_variable(config[CONF_MID_WHITE])
        cg.add(var.set_mid_white(mid_white)) 

    if CONF_COLD_WHITE in config:     
        cold_white = await cg.get_variable(config[CONF_COLD_WHITE])
        cg.add(var.set_cold_white(cold_white))

    if CONF_COLD_WHITE_COLOR_TEMPERATURE in config:
        cg.add(
            var.set_cold_white_temperature(config[CONF_COLD_WHITE_COLOR_TEMPERATURE])
        )

    if CONF_MID_WHITE_COLOR_TEMPERATURE in config:
        cg.add(
            var.set_mid_white_temperature(config[CONF_MID_WHITE_COLOR_TEMPERATURE])
        )
    
    if CONF_WARM_WHITE_COLOR_TEMPERATURE in config:
        cg.add(
            var.set_warm_white_temperature(config[CONF_WARM_WHITE_COLOR_TEMPERATURE])
        )

    if CONF_EMULATED_WARM_WHITE_COLOR_TEMPERATURE in config:
        cg.add(
            var.set_emulated_warm_white_temperature(config[CONF_EMULATED_WARM_WHITE_COLOR_TEMPERATURE])
        )
        
    if CONF_EMULATED_COLD_WHITE_COLOR_TEMPERATURE in config:
        cg.add(
            var.set_emulated_cold_white_temperature(config[CONF_EMULATED_COLD_WHITE_COLOR_TEMPERATURE])
        )
from ..enable_cms import (RGB_PROFILE_CONFIG_SCHEMA, to_rgb_profile_code)

DEPENDENCIES = ["enable_cms"]
CONFIG_SCHEMA = RGB_PROFILE_CONFIG_SCHEMA

async def to_code(config):
    await to_rgb_profile_code(config)

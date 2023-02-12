from ..enable_cms import (CWWW_PROFILE_CONFIG_SCHEMA, to_cwww_profile_code)

DEPENDENCIES = ["enable_cms"]
CONFIG_SCHEMA = CWWW_PROFILE_CONFIG_SCHEMA


async def to_code(config):
    await to_cwww_profile_code(config)

"""LIPC wire identifiers and helpers for x52ctl.

Keep numeric values in sync with include/libx52/x52d_ipc.h and the daemon
registry (daemon/config_registry.json / generated config-defs).
"""

from enum import IntEnum


class IpcRequest(IntEnum):
    """Framed IPC request opcodes (lipc_header.request)."""

    CONFIG_LOAD = 0x01
    CONFIG_RELOAD = 0x02
    CONFIG_RESET = 0x03
    CONFIG_CLEAR = 0x04
    CONFIG_SAVE = 0x05
    CONFIG_DUMP = 0x06
    CONFIG_SET = 0x07
    CONFIG_GET = 0x08
    LOGGING_SHOW = 0x11
    LOGGING_SET = 0x12


class ConfigClearTarget(IntEnum):
    """lipc_header.index for CONFIG_CLEAR."""

    STATE = 1
    SYSCONF = 2


class IpcPush(IntEnum):
    """Server push request ids (tid == 0)."""

    DEVICE_STATE = 0x8001


# Same sentinel as module-map.h / x52d_ipc logging selectors.
X52D_MOD_GLOBAL = 0xFF


def device_state_pack_usb(vendor_id: int, product_id: int) -> int:
    """Pack 16-bit USB ids into lipc_header.value lower 32 bits (host order)."""
    return int(((vendor_id & 0xFFFF) << 16) | (product_id & 0xFFFF))

# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
"""x52ctl: CLI for x52d using framed LIPC (libx52dcomm)."""

from __future__ import annotations

import argparse
import errno
import os
import shlex
import sys

_pkg = os.path.dirname(os.path.abspath(__file__))
if _pkg not in sys.path:
    sys.path.insert(0, _pkg)
from ctypes import (
    CDLL,
    POINTER,
    Structure,
    byref,
    c_char_p,
    c_int,
    c_size_t,
    c_uint16,
    c_uint32,
    c_uint64,
    c_void_p,
    cast,
    create_string_buffer,
    get_errno,
)

import module_defs
from command_defs import ConfigClearTarget, IpcRequest, X52D_MOD_GLOBAL

LIPC_OK = 0


class LIPCHeader(Structure):
    _fields_ = [
        ('magic', c_uint32),
        ('version', c_uint16),
        ('request', c_uint16),
        ('status', c_uint16),
        ('index', c_uint16),
        ('tid', c_uint32),
        ('length', c_uint32),
        ('checksum', c_uint32),
        ('value', c_uint64),
    ]


def _load_comm_lib() -> CDLL:
    path = os.environ.get('X52DCOMM_LIB')
    names = (path, 'libx52dcomm.so.1', 'libx52dcomm.so')
    last: OSError | None = None
    for cand in names:
        if not cand:
            continue
        try:
            return CDLL(cand)
        except OSError as exc:  # pragma: no cover
            last = exc
    raise OSError(last or 'cannot load libx52dcomm')


_LIB = None


def _lib() -> CDLL:
    global _LIB
    if _LIB is None:
        _LIB = _load_comm_lib()
        _LIB.x52d_dial_ipc.argtypes = [c_char_p]
        _LIB.x52d_dial_ipc.restype = c_int
        _LIB.x52d_ipc_call.argtypes = [
            c_int,
            c_uint16,
            c_uint16,
            c_uint64,
            c_void_p,
            c_size_t,
            POINTER(LIPCHeader),
            c_void_p,
            c_size_t,
            POINTER(c_size_t),
        ]
        _LIB.x52d_ipc_call.restype = c_int
    return _LIB


def _nul_out(parts: list[str]) -> bytes:
    return b'\0'.join(p.encode('utf-8') for p in parts) + b'\0'


def _ipc_call(
    fd: int,
    request: int,
    index: int,
    value: int,
    payload: bytes | None,
    reply_cap: int = 512 * 1024,
) -> tuple[int, LIPCHeader, bytes]:
    pl = payload or b''
    hdr = LIPCHeader()
    rbuf = create_string_buffer(reply_cap)
    rlen = c_size_t(0)
    if pl:
        pl_buf = create_string_buffer(pl, len(pl))
        pl_ptr = cast(pl_buf, c_void_p)
        pl_len = len(pl)
    else:
        pl_ptr = None
        pl_len = 0
    st = _lib().x52d_ipc_call(
        fd,
        request,
        index,
        value,
        pl_ptr,
        pl_len,
        byref(hdr),
        rbuf,
        reply_cap,
        byref(rlen),
    )
    raw = bytes(rbuf[: rlen.value])
    if int(st) != LIPC_OK:
        return int(st), hdr, raw
    # lipc_client_call returns LIPC_OK when a reply frame arrived; check wire status.
    if int(hdr.status) != LIPC_OK:
        return int(hdr.status), hdr, raw
    return LIPC_OK, hdr, raw


def _errno_from_strerror(msg: str) -> int | None:
    for code in range(1, 200):
        try:
            if os.strerror(code) == msg:
                return code
        except OSError:
            break
    return None


def _format_config_set_err(sec: str, opt: str, val: str, srv: str) -> str:
    n = _errno_from_strerror(srv)
    if n is not None:
        return f"Error {n} setting '{sec}.{opt}'='{val}': {srv}"
    return srv


def _map_config_get_err(sec: str, opt: str, srv: str) -> str:
    if 'configuration key not found' in srv or srv == 'configuration key not found':
        return f"Error getting '{sec}.{opt}'"
    return srv


def _import_config_defs():
    import config_defs as cd  # pylint: disable=import-outside-toplevel

    return cd


def _config_wire(sec: str, opt: str):
    cd = _import_config_defs()
    sec_id = getattr(cd.Section, sec.upper()).value
    opt_cls = getattr(cd, cd.Section(sec_id).name)
    opt_id = getattr(opt_cls, opt.upper()).value
    return sec_id, opt_id


def _lookup_module(name: str) -> int | None:
    try:
        return module_defs.Module[name.upper()].value
    except KeyError:
        return None


def _lookup_level(name: str) -> int | None:
    key = name.lower()
    for lvl in module_defs.LogLevel:
        lbl = 'default' if lvl == module_defs.LogLevel.NOTSET else lvl.name.lower()
        if lbl == key:
            return int(lvl.value)
    return None


def _level_to_wire(level: int) -> int:
    return level & ((1 << 64) - 1)


def _open_ipc(sock_path: str | None) -> int:
    arg = sock_path.encode('utf-8') if sock_path else None
    fd = _lib().x52d_dial_ipc(arg)
    if fd < 0:
        err = get_errno()
        raise OSError(err, os.strerror(err))
    return fd


def cmd_config_load(fd: int, path: str) -> bytes:
    if not path:
        return _nul_out(['ERR', "Invalid file '' for 'config load' command"])
    try:
        if not os.path.exists(path) or not os.access(path, os.R_OK):
            return _nul_out(['ERR', f"Invalid file '{path}' for 'config load' command"])
    except OSError:
        return _nul_out(['ERR', f"Invalid file '{path}' for 'config load' command"])
    st, _hdr, pl = _ipc_call(fd, IpcRequest.CONFIG_LOAD, 0, 0, os.fsencode(path))
    if st != LIPC_OK:
        return _nul_out(['ERR', os.fsdecode(pl) if pl else f'IPC error {st}'])
    return _nul_out(['OK', 'config', 'load', path])


def cmd_config_reload(fd: int) -> bytes:
    st, _h, pl = _ipc_call(fd, IpcRequest.CONFIG_RELOAD, 0, 0, None)
    if st != LIPC_OK:
        return _nul_out(['ERR', pl.decode('utf-8', 'replace') if pl else f'IPC error {st}'])
    return _nul_out(['OK', 'config', 'reload'])


def cmd_config_reset(fd: int) -> bytes:
    st, _h, pl = _ipc_call(fd, IpcRequest.CONFIG_RESET, 0, 0, None)
    if st != LIPC_OK:
        return _nul_out(['ERR', pl.decode('utf-8', 'replace') if pl else f'IPC error {st}'])
    return _nul_out(['OK', 'config', 'reset'])


def cmd_config_clear(fd: int, target: str) -> bytes:
    tmap = {'state': ConfigClearTarget.STATE, 'sysconf': ConfigClearTarget.SYSCONF}
    if target not in tmap:
        return _nul_out(['ERR', f'unknown clear target {target!r}'])
    st, _h, pl = _ipc_call(fd, IpcRequest.CONFIG_CLEAR, int(tmap[target]), 0, None)
    if st != LIPC_OK:
        return _nul_out(['ERR', pl.decode('utf-8', 'replace') if pl else f'IPC error {st}'])
    return _nul_out(['OK', 'config', 'clear', target])


def cmd_config_save(fd: int) -> bytes:
    st, _h, pl = _ipc_call(fd, IpcRequest.CONFIG_SAVE, 0, 0, None)
    if st != LIPC_OK:
        return _nul_out(['ERR', pl.decode('utf-8', 'replace') if pl else f'IPC error {st}'])
    return _nul_out(['OK', 'config', 'save'])


def cmd_config_dump(fd: int, out_path: str) -> bytes:
    if not out_path:
        return _nul_out(['ERR', "Invalid file '' for 'config dump' command"])
    parent = os.path.dirname(os.path.abspath(out_path)) or '/'
    if not os.path.isdir(parent):
        return _nul_out(['ERR', f"Invalid file '{out_path}' for 'config dump' command"])
    st, _h, pl = _ipc_call(fd, IpcRequest.CONFIG_DUMP, 0, 0, None)
    if st != LIPC_OK:
        return _nul_out(['ERR', pl.decode('utf-8', 'replace') if pl else f'IPC error {st}'])
    try:
        with open(out_path, 'wb') as out_f:
            out_f.write(pl)
    except OSError as exc:
        return _nul_out(['ERR', str(exc)])
    return _nul_out(['OK', 'config', 'dump', out_path])


def cmd_config_get(fd: int, sec: str, opt: str) -> bytes:
    try:
        sid, oid = _config_wire(sec, opt)
    except (AttributeError, KeyError, ValueError):
        return _nul_out(['ERR', _map_config_get_err(sec, opt, 'configuration key not found')])
    st, _h, pl = _ipc_call(fd, IpcRequest.CONFIG_GET, sid, oid, None)
    if st != LIPC_OK:
        msg = pl.decode('utf-8', 'replace') if pl else ''
        return _nul_out(['ERR', _map_config_get_err(sec, opt, msg)])
    val = pl.decode('utf-8', 'replace')
    return _nul_out(['DATA', sec, opt, val])


def cmd_config_set(fd: int, sec: str, opt: str, val: str) -> bytes:
    try:
        sid, oid = _config_wire(sec, opt)
    except (AttributeError, KeyError, ValueError):
        n = errno.EINVAL
        return _nul_out(['ERR', _format_config_set_err(sec, opt, val, os.strerror(n))])
    st, _h, pl = _ipc_call(fd, IpcRequest.CONFIG_SET, sid, oid, val.encode('utf-8'))
    if st != LIPC_OK:
        msg = pl.decode('utf-8', 'replace') if pl else ''
        return _nul_out(['ERR', _format_config_set_err(sec, opt, val, msg)])
    return _nul_out(['OK', 'config', 'set', sec, opt, val])


def cmd_logging_show(fd: int, module: str | None) -> bytes:
    if module is None:
        idx = X52D_MOD_GLOBAL
        label = 'global'
    else:
        mid = _lookup_module(module)
        if mid is None:
            return _nul_out(['ERR', f"Invalid module '{module}'"])
        idx = mid
        label = module.lower()
    st, _h, pl = _ipc_call(fd, IpcRequest.LOGGING_SHOW, idx, 0, None)
    if st != LIPC_OK:
        return _nul_out(['ERR', pl.decode('utf-8', 'replace') if pl else f'IPC error {st}'])
    lvl_name = pl.decode('utf-8', 'replace').strip()
    return _nul_out(['DATA', label, lvl_name])


def cmd_logging_set(fd: int, module: str | None, level_name: str) -> bytes:
    level = _lookup_level(level_name)
    if level is None:
        return _nul_out(['ERR', f"Unknown level '{level_name}' for 'logging set' command"])
    if module is None:
        if level == int(module_defs.LogLevel.NOTSET.value):
            return _nul_out(["ERR", "'default' level is not valid without a module"])
        st, _h, pl = _ipc_call(
            fd, IpcRequest.LOGGING_SET, X52D_MOD_GLOBAL, _level_to_wire(level), None
        )
        if st != LIPC_OK:
            return _nul_out(['ERR', pl.decode('utf-8', 'replace') if pl else f'IPC error {st}'])
        return _nul_out(['OK', 'logging', 'set', level_name.lower()])
    mid = _lookup_module(module)
    if mid is None:
        return _nul_out(['ERR', f"Invalid module '{module}'"])
    st, _h, pl = _ipc_call(fd, IpcRequest.LOGGING_SET, mid, _level_to_wire(level), None)
    if st != LIPC_OK:
        return _nul_out(['ERR', pl.decode('utf-8', 'replace') if pl else f'IPC error {st}'])
    return _nul_out(['OK', 'logging', 'set', module.lower(), level_name.lower()])


def _legacy_argc_errors(argv: list[str]) -> bytes | None:
    if not argv:
        return None
    if argv[0] == 'config':
        if len(argv) < 2:
            return _nul_out(['ERR', "Insufficient arguments for 'config' command"])
        sub = argv[1]
        if sub == 'load':
            if len(argv) != 3:
                return _nul_out(
                    [
                        'ERR',
                        f"Unexpected arguments for 'config load' command; got {len(argv)}, expected 3",
                    ]
                )
        elif sub == 'reload':
            if len(argv) != 2:
                return _nul_out(
                    [
                        'ERR',
                        f"Unexpected arguments for 'config reload' command; got {len(argv)}, expected 2",
                    ]
                )
        elif sub == 'dump':
            if len(argv) != 3:
                return _nul_out(
                    [
                        'ERR',
                        f"Unexpected arguments for 'config dump' command; got {len(argv)}, expected 3",
                    ]
                )
        elif sub == 'save':
            if len(argv) != 2:
                return _nul_out(
                    [
                        'ERR',
                        f"Unexpected arguments for 'config save' command; got {len(argv)}, expected 2",
                    ]
                )
        elif sub == 'clear':
            if len(argv) != 3:
                return _nul_out(
                    [
                        'ERR',
                        f"Unexpected arguments for 'config clear' command; got {len(argv)}, expected 3",
                    ]
                )
        elif sub == 'get':
            if len(argv) != 4:
                return _nul_out(
                    [
                        'ERR',
                        f"Unexpected arguments for 'config get' command; got {len(argv)}, expected 4",
                    ]
                )
        elif sub == 'set':
            if len(argv) != 5:
                return _nul_out(
                    [
                        'ERR',
                        f"Unexpected arguments for 'config set' command; got {len(argv)}, expected 5",
                    ]
                )
        elif sub == '':
            return _nul_out(['ERR', "Unknown subcommand '' for 'config' command"])
        elif sub not in ('load', 'reload', 'reset', 'save', 'dump', 'clear', 'get', 'set'):
            return _nul_out(['ERR', f"Unknown subcommand '{sub}' for 'config' command"])
    if argv[0] == 'logging':
        if len(argv) < 2:
            return _nul_out(['ERR', "Insufficient arguments for 'logging' command"])
        sub = argv[1]
        if sub not in ('show', 'set'):
            return _nul_out(['ERR', f"Unknown subcommand '{sub}' for 'logging' command"])
        if sub == 'show' and len(argv) > 3:
            return _nul_out(
                [
                    'ERR',
                    f"Unexpected arguments for 'logging show' command; got {len(argv)}, expected 2 or 3",
                ]
            )
        if sub == 'set':
            if len(argv) < 3:
                return _nul_out(
                    [
                        'ERR',
                        "Unexpected arguments for 'logging set' command; got 2, expected 3 or 4",
                    ]
                )
            if len(argv) > 4:
                return _nul_out(
                    [
                        'ERR',
                        f"Unexpected arguments for 'logging set' command; got {len(argv)}, expected 3 or 4",
                    ]
                )
    return None


def _build_cmd_parser() -> argparse.ArgumentParser:
    """Parse `config` / `logging` subcommands; trailing tokens go in `extras` where listed."""
    kw: dict = {'prog': 'x52ctl', 'add_help': False}
    if sys.version_info >= (3, 9):
        kw['exit_on_error'] = False
    p = argparse.ArgumentParser(**kw)
    sub = p.add_subparsers(dest='cmd', required=True)

    pc = sub.add_parser('config')
    pcs = pc.add_subparsers(dest='sub', required=True)

    pr = pcs.add_parser('reload')
    pr.add_argument('extras', nargs='*', default=[])

    prs = pcs.add_parser('reset')
    prs.add_argument('extras', nargs='*', default=[])

    psa = pcs.add_parser('save')
    psa.add_argument('extras', nargs='*', default=[])

    pld = pcs.add_parser('load')
    pld.add_argument('path')
    pld.add_argument('extras', nargs='*', default=[])

    pcl = pcs.add_parser('clear')
    pcl.add_argument('target', choices=('state', 'sysconf'))
    pcl.add_argument('extras', nargs='*', default=[])

    pdu = pcs.add_parser('dump')
    pdu.add_argument('path')
    pdu.add_argument('extras', nargs='*', default=[])

    pge = pcs.add_parser('get')
    pge.add_argument('section')
    pge.add_argument('option')
    pge.add_argument('extras', nargs='*', default=[])

    pst = pcs.add_parser('set')
    pst.add_argument('section')
    pst.add_argument('option')
    pst.add_argument('value')
    pst.add_argument('extras', nargs='*', default=[])

    pl = sub.add_parser('logging')
    pls = pl.add_subparsers(dest='sub', required=True)

    psh = pls.add_parser('show')
    psh.add_argument('module', nargs='?', default=None)
    psh.add_argument('extras', nargs='*', default=[])

    pset = pls.add_parser('set')
    pset.add_argument('tail', nargs='+', metavar='ARG')

    return p


_CMD_PARSER: argparse.ArgumentParser | None = None

_CMD_PARSE_EXC: tuple[type[BaseException], ...] = (SystemExit,)
if sys.version_info >= (3, 9):
    _CMD_PARSE_EXC += (argparse.ArgumentError,)


def _cmd_parser() -> argparse.ArgumentParser:
    global _CMD_PARSER
    if _CMD_PARSER is None:
        _CMD_PARSER = _build_cmd_parser()
    return _CMD_PARSER


def _dispatch_ns(fd: int, ns: argparse.Namespace) -> bytes:
    if ns.cmd == 'config':
        sub = ns.sub
        if sub == 'reload':
            return cmd_config_reload(fd)
        if sub == 'reset':
            return cmd_config_reset(fd)
        if sub == 'save':
            return cmd_config_save(fd)
        if sub == 'load':
            return cmd_config_load(fd, ns.path)
        if sub == 'clear':
            return cmd_config_clear(fd, ns.target)
        if sub == 'dump':
            return cmd_config_dump(fd, ns.path)
        if sub == 'get':
            return cmd_config_get(fd, ns.section, ns.option)
        if sub == 'set':
            return cmd_config_set(fd, ns.section, ns.option, ns.value)
    if ns.cmd == 'logging':
        if ns.sub == 'show':
            return cmd_logging_show(fd, ns.module)
        if ns.sub == 'set':
            tail = ns.tail
            if len(tail) == 1:
                return cmd_logging_set(fd, None, tail[0])
            return cmd_logging_set(fd, tail[0], tail[1])
    return _nul_out(['ERR', f"Unknown command '{ns.cmd}'"])


def run_line(fd: int, argv: list[str]) -> bytes:
    if not argv:
        return b''
    pre = _legacy_argc_errors(argv)
    if pre:
        return pre
    try:
        ns = _cmd_parser().parse_args(argv)
    except _CMD_PARSE_EXC:
        return _nul_out(['ERR', f"Unknown command '{argv[0]}'"])
    if ns.cmd == 'logging' and ns.sub == 'set' and len(ns.tail) > 2:
        return _nul_out(
            [
                'ERR',
                f"Unexpected arguments for 'logging set' command; got {len(argv)}, expected 3 or 4",
            ]
        )
    return _dispatch_ns(fd, ns)


def run_interactive(fd: int) -> int:
    sys.stdout.write('> ')
    sys.stdout.flush()
    for line in sys.stdin:
        if line.strip().lower() == 'quit':
            break
        parts = shlex.split(line, comments=False)
        if not parts:
            sys.stdout.write('\n> ')
            sys.stdout.flush()
            continue
        out = run_line(fd, parts)
        sys.stdout.buffer.write(out)
        sys.stdout.write('\n> ')
        sys.stdout.flush()
    return 0


def _build_main_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(prog='x52ctl', add_help=True)
    p.add_argument(
        '-s',
        '--socket',
        metavar='PATH',
        default=None,
        help='path to the daemon framed IPC socket (default: compiled-in path)',
    )
    p.add_argument(
        '-i',
        '--interactive',
        action='store_true',
        help='read commands from stdin until EOF or quit',
    )
    p.add_argument(
        'args',
        nargs='*',
        help='config … or logging … (see subcommands)',
    )
    return p


def main() -> int:
    m = _build_main_parser()
    a = m.parse_args()

    try:
        fd = _open_ipc(a.socket)
    except OSError as exc:
        print(f'x52d_dial_ipc: {exc.strerror}', file=sys.stderr)
        return 1

    try:
        if a.interactive:
            if a.args:
                print(
                    'Running in interactive mode, ignoring extra arguments',
                    file=sys.stderr,
                )
            return run_interactive(fd)
        if not a.args:
            m.print_usage(file=sys.stderr)
            return 2
        out = run_line(fd, a.args)
        sys.stdout.buffer.write(out)
    finally:
        try:
            os.close(fd)
        except OSError:
            pass
    return 0

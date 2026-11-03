"""Map Skyrim Address Library IDs <-> addresses (format follows Code/client/VersionDb.h).

  python Tools/Scripts/addrlib.py 39004 0x1406cf760   ID -> address, address -> ID
  python Tools/Scripts/addrlib.py -v 1.7.104 39004    other game version (default 1.6.1170, the main IDB)

Addresses are 0x140000000 + offset, as in the IDBs; the 0x is optional for 9+ digit addresses.
An address inside a function or object resolves to the closest ID below it, e.g. "39004+0x1a".
-v also takes a path to a .bin. Bins are read from <game>/Data/SKSE/Plugins; the game dir comes
from SKYRIM_SE_DIR or the Steam registry key.
"""

import bisect
import os
import struct
import sys

BASE = 0x140000000

# Format 2 delta codes: kind -> (stored value, sign). Sign 0 means the value replaces the
# previous one, otherwise it's added/subtracted. Kind 1 is "previous + 1" with nothing stored.
DELTAS = {0: ("<Q", 0), 2: ("<B", 1), 3: ("<B", -1), 4: ("<H", 1), 5: ("<H", -1), 6: ("<H", 0), 7: ("<I", 0)}


def find_bin(version):
    if version.endswith(".bin"):
        return version
    game = os.environ.get("SKYRIM_SE_DIR")
    if not game:
        try:
            import winreg
            key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r"SOFTWARE\WOW6432Node\Bethesda Softworks\Skyrim Special Edition")
            game = winreg.QueryValueEx(key, "installed path")[0]
        except (ImportError, OSError):
            sys.exit("Skyrim SE not found: set SKYRIM_SE_DIR or pass -v <path to .bin>")
    parts = (version.split(".") + ["0"] * 4)[:4]
    path = os.path.join(game, "Data", "SKSE", "Plugins", f"versionlib-{'-'.join(parts)}.bin")
    if not os.path.isfile(path):
        sys.exit(f"{path} not found (is Address Library for {version} installed?)")
    return path


def delta(d, p, kind, prev):
    """Decodes one format 2 value at d[p]. Returns (value, next p)."""
    if kind == 1:
        return prev + 1, p
    fmt, sign = DELTAS[kind]
    v = struct.unpack_from(fmt, d, p)[0]
    return prev + sign * v if sign else v, p + struct.calcsize(fmt)


def load(path):
    """Returns {id: offset}."""
    with open(path, "rb") as f:
        d = f.read()
    fmt = struct.unpack_from("<i", d)[0]  # then 4 x i32 game version

    if fmt == 5:  # char name[64], u64 ptr size, u32 count, then u32 offset per ID (0 = none)
        count = struct.unpack_from("<I", d, 92)[0]
        return {i: o for i, o in enumerate(struct.unpack_from(f"<{count}I", d, 96)) if o}

    if fmt != 2:
        sys.exit(f"unsupported format {fmt}")

    # i32 name length + name, i32 ptr size, i32 count, then records: a type byte whose low
    # nibble is the ID's delta kind and high nibble the offset's. High bit 8 means the offset
    # delta is in units of ptr size.
    p = 24 + struct.unpack_from("<i", d, 20)[0]
    ptr, count = struct.unpack_from("<ii", d, p)
    p += 8
    out = {}
    vid = off = 0
    for _ in range(count):
        t = d[p]
        vid, p = delta(d, p + 1, t & 0xF, vid)
        scale = ptr if t & 0x80 else 1
        off, p = delta(d, p, (t >> 4) & 7, off // scale)
        off *= scale
        out[vid] = off
    return out


def main():
    args = sys.argv[1:]
    version = "1.6.1170"
    if args[:1] == ["-v"]:
        version, args = args[1], args[2:]
    if not args or args[0] in ("-h", "--help"):
        sys.exit(__doc__)
    ids = load(find_bin(version))
    rev = sorted((o, i) for i, o in ids.items())

    for arg in args:
        if not (arg.lower().startswith("0x") or len(arg) >= 9):
            o = ids.get(int(arg))
            print(f"{arg} -> {o + BASE:#x}" if o is not None else f"{arg} -> not found")
            continue
        target = int(arg, 16) - BASE
        n = bisect.bisect_right(rev, (target, float("inf"))) - 1
        if n < 0 or target - rev[n][0] > 0x1000000:  # outside the image (largest real gap is 0x1000000)
            print(f"{arg} -> not found")
        else:
            o, i = rev[n]
            print(f"{arg} -> {i}" + (f"+{target - o:#x}" if target != o else ""))


if __name__ == "__main__":
    main()

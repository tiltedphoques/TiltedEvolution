"""Read the PE fields needed by the Fallout 4 offset tools."""

import struct
from pathlib import Path


class Image:
    def __init__(self, path):
        self.data = Path(path).read_bytes()
        if self.data[:2] != b"MZ":
            raise ValueError("Not a PE image")
        pe = struct.unpack_from("<I", self.data, 0x3C)[0]
        if self.data[pe:pe + 4] != b"PE\0\0":
            raise ValueError("Invalid PE signature")
        machine, count, self.timestamp = struct.unpack_from("<HHI", self.data, pe + 4)
        optional_size = struct.unpack_from("<H", self.data, pe + 20)[0]
        optional = pe + 24
        if machine != 0x8664 or struct.unpack_from("<H", self.data, optional)[0] != 0x20B:
            raise ValueError("Expected an x64 PE image")
        self.size = struct.unpack_from("<I", self.data, optional + 56)[0]
        self.sections = []
        for index in range(count):
            offset = optional + optional_size + index * 40
            name, size, rva, raw_size, raw = struct.unpack_from("<8sIIII", self.data, offset)
            flags = struct.unpack_from("<I", self.data, offset + 36)[0]
            self.sections.append((name.rstrip(b"\0").decode(), rva, size, raw, raw_size, flags))

    def section(self, rva):
        return next((s for s in self.sections if s[1] <= rva < s[1] + max(s[2], s[4])), None)

    def offset(self, rva):
        section = self.section(rva)
        if section is None or rva - section[1] >= section[4]:
            raise ValueError(f"RVA {rva:#x} has no file backing")
        return section[3] + rva - section[1]

    def executable(self, rva):
        section = self.section(rva)
        return section is not None and bool(section[5] & 0x20000000)

    def text(self):
        section = next(s for s in self.sections if s[0] == ".text")
        return section[1], self.data[section[3]:section[3] + min(section[2], section[4])]


def load_library(path, image=None):
    data = Path(path).read_bytes()
    if len(data) < 8:
        raise ValueError("Truncated address library")
    count, = struct.unpack_from("<Q", data)
    if not count or len(data) != 8 + count * 16:
        raise ValueError("Invalid address library record count")
    result = {}
    for identifier, rva in struct.iter_unpack("<QQ", data[8:]):
        if identifier in result:
            raise ValueError(f"Duplicate address ID {identifier}")
        if rva and image is not None and (rva >= image.size or image.section(rva) is None):
            raise ValueError(f"ID {identifier} is outside the image: {rva:#x}")
        result[identifier] = rva
    return result

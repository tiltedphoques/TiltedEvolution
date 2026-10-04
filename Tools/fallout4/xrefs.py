#!/usr/bin/env python3
"""Find RIP-relative references or direct calls to a Fallout 4 address library ID.

Requires capstone. Reports the nearest preceding function ID as context, not
as a verified containing function. Multiple IDs can share the same address.
"""

import argparse
import bisect
import struct
from collections import defaultdict
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_GRP_CALL, CS_GRP_JUMP
from capstone.x86 import X86_OP_MEM, X86_OP_IMM, X86_REG_RIP

from pe import Image, load_library


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable', type=Path)
    parser.add_argument('library', type=Path)
    parser.add_argument('id', type=int)
    args = parser.parse_args()
    image = Image(args.executable)
    ids = load_library(args.library, image)
    target = ids[args.id]
    by_address = defaultdict(list)
    for identifier, rva in ids.items():
        if image.executable(rva):
            by_address[rva].append(identifier)
    starts = sorted(by_address)
    base, code = image.text()
    decoder = Cs(CS_ARCH_X86, CS_MODE_64)
    decoder.detail = True
    seen = set()
    for offset in range(len(code) - 4):
        displacement, = struct.unpack_from('<i', code, offset)
        if base + offset + 4 + displacement != target:
            continue
        for beginning in range(max(0, offset - 11), offset + 1):
            instruction = next(decoder.disasm(code[beginning:offset + 5], base + beginning, count=1), None)
            if instruction is None or instruction.address in seen:
                continue
            referenced = any(
                (op.type == X86_OP_MEM and op.mem.base == X86_REG_RIP and
                 instruction.address + instruction.size + op.mem.disp == target) or
                (op.type == X86_OP_IMM and op.imm == target and
                 (instruction.group(CS_GRP_CALL) or instruction.group(CS_GRP_JUMP)))
                for op in instruction.operands)
            if not referenced:
                continue
            seen.add(instruction.address)
            index = bisect.bisect_right(starts, instruction.address) - 1
            nearest = ','.join(map(str, by_address[starts[index]])) if index >= 0 else '?'
            print(f'{instruction.address:#x}\t{nearest}\t{instruction.mnemonic} {instruction.op_str}')


if __name__ == '__main__':
    main()

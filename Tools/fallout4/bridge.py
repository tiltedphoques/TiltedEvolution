#!/usr/bin/env python3
"""Map selected reference PDB functions to IDs in a newer address library.

The public-symbol TSV has offsets relative to the reference .text section.
Requires capstone. Only unique instruction matches at target library addresses
are reported. RIP displacements and relative control-flow immediates are masked.
"""

import argparse
import re
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_GRP_JUMP, CS_GRP_CALL
from capstone.x86 import X86_OP_MEM, X86_REG_RIP

from pe import Image, load_library


def signature(data, image=None):
    decoder = Cs(CS_ARCH_X86, CS_MODE_64)
    decoder.detail = True
    pattern = []
    for instruction in decoder.disasm(data, 0):
        mask = set()
        if any(op.type == X86_OP_MEM and (op.mem.base == X86_REG_RIP or
               (image is not None and op.mem.disp >= 0x100000 and image.section(op.mem.disp)))
               for op in instruction.operands):
            mask.update(range(instruction.disp_offset, instruction.disp_offset + instruction.disp_size))
        if instruction.group(CS_GRP_CALL) or instruction.group(CS_GRP_JUMP):
            mask.update(range(instruction.imm_offset, instruction.imm_offset + instruction.imm_size))
        pattern.extend(None if i in mask else byte for i, byte in enumerate(instruction.bytes))
        if instruction.mnemonic.startswith('ret') or instruction.mnemonic == 'int3' or len(pattern) >= 96:
            break
    return pattern


def match_function(source, target, starts, image=None):
    pattern = signature(source, image)
    if len(pattern) < 16:
        return None
    anchors = []
    current = []
    start = 0
    for index, byte in enumerate(pattern + [None]):
        if byte is None:
            if current:
                anchors.append((start, bytes(current)))
            current = []
        else:
            if not current:
                start = index
            current.append(byte)
    offset, anchor = max(anchors, key=lambda item: len(item[1]), default=(0, b''))
    if len(anchor) < 4:
        return None
    matches = []
    index = target.find(anchor)
    while index >= 0:
        candidate = index - offset
        if candidate in starts and candidate + len(pattern) <= len(target):
            if all(byte is None or target[candidate + i] == byte for i, byte in enumerate(pattern)):
                matches.append(candidate)
                if len(matches) > 1:
                    return None
        index = target.find(anchor, index + 1)
    return matches[0] if matches else None


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('reference', type=Path)
    parser.add_argument('target', type=Path)
    parser.add_argument('symbols', type=Path)
    parser.add_argument('library', type=Path)
    parser.add_argument('pattern', help='Regex selecting mangled public symbols')
    args = parser.parse_args()
    reference = Image(args.reference)
    target = Image(args.target)
    reference_base, reference_text = reference.text()
    target_base, target_text = target.text()
    ids = load_library(args.library, target)
    starts = {}
    for identifier, rva in ids.items():
        if target_base <= rva < target_base + len(target_text):
            starts.setdefault(rva - target_base, []).append(identifier)
    selected = re.compile(args.pattern)
    for line in args.symbols.read_text().splitlines():
        relative, name = line.split('\t', 1)
        if not selected.search(name):
            continue
        offset = int(relative, 16)
        result = match_function(reference_text[offset:offset + 128], target_text, starts, reference)
        if result is None:
            print(f'MISSING\t{name}')
        else:
            print(f"{','.join(map(str, starts[result]))}\t{target_base + result:#x}\t{name}")


if __name__ == '__main__':
    main()

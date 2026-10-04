#!/usr/bin/env python3
"""Verify recorded Fallout 4 bindings against the executable and address library."""

import argparse
import csv
import hashlib
from pathlib import Path

from pe import Image, load_library


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable', type=Path)
    parser.add_argument('library', type=Path)
    parser.add_argument('bindings', type=Path, nargs='?', default=Path(__file__).with_name('bindings.tsv'))
    args = parser.parse_args()
    image = Image(args.executable)
    ids = load_library(args.library, image)
    digest = hashlib.sha256(image.data).hexdigest()
    count = 0
    with args.bindings.open() as stream:
        for record in csv.DictReader(stream, delimiter='\t'):
            identifier = int(record['id'])
            rva = int(record['rva'], 16)
            if digest != record['image_sha256']:
                raise ValueError('Executable does not match the verified patch')
            if ids.get(identifier) != rva:
                raise ValueError(f"{record['symbol']}: ID {identifier} does not resolve to {rva:#x}")
            if record['kind'] == 'function' and not image.executable(rva):
                raise ValueError(f"{record['symbol']}: function ID resolves outside executable code")
            if record['bytes']:
                expected = bytes.fromhex(record['bytes'])
                offset = image.offset(rva)
                if image.data[offset:offset + len(expected)] != expected:
                    raise ValueError(f"{record['symbol']}: recorded instructions do not match")
            count += 1
    print(f'Verified {count} bindings against {digest}')


if __name__ == '__main__':
    main()

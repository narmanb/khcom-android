import struct
import unittest
from unittest.mock import patch

from strip_rom_data import object_symbols, reloc_words


class StripRomDataTest(unittest.TestCase):
    def test_shared_library_symbol_in_both_tables_is_not_ambiguous(self):
        output = """
Symbol table '.dynsym':
  1: 00590000 4096 OBJECT GLOBAL DEFAULT 19 table
Symbol table '.symtab':
  2: 00590000 4096 OBJECT GLOBAL DEFAULT 19 table
  3: 00591000 4 OBJECT LOCAL DEFAULT 19 local
  4: 00592000 4 OBJECT LOCAL DEFAULT 19 local
"""
        with patch('strip_rom_data.subprocess.check_output', return_value=output):
            self.assertEqual(object_symbols('unused', 'readelf'),
                             {'table': (0x590000, 4096, 19)})

    def test_arm_codec_relocation_words_are_preserved(self):
        types = (2, 3, 28, 29, 38, 58, 40)
        data = b''.join(struct.pack('<II', 0x1000 + 4*i, t)
                        for i, t in enumerate(types))
        sections = {'.rel.romtext': {'offset': 0, 'size': len(data)}}
        self.assertEqual(reloc_words(data, sections, '.romtext'),
                         set(range(0x1000, 0x1018, 4)))

    def test_unknown_relocation_still_fails(self):
        data = struct.pack('<II', 0x1000, 255)
        with self.assertRaises(SystemExit):
            reloc_words(data, {'.rel.romtext': {'offset': 0, 'size': 8}}, '.romtext')


if __name__ == '__main__':
    unittest.main()

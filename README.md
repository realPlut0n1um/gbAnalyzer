# gbAnalyzer
Analyzes Gameboy ROMS, Currently only supports Header analysis. Will add support for Disassembler later...

### [EXAMPLE OUTPUT]
<pre>
$ ./gbAnalyzer -h game_and_watch_gallery.gb<br />

[LOCATION] (0x0104 -> 0x0133)<br />
- NINTENDO_CHECKSUM: CEED6666CC0D000B03730083000C000D0008111F8889000EDCCC6EE6DDDDD999BBBB67636E0EECCCDDDC999FBBB9333E<br />

[LOCATION] (0x0134 -> 0x143)<br />
- TITLE: G&W GALLERY<br />

[LOCATION] (0x0146)<br />
- SGB SUPPORT: True<br />

[LOCATION] (0x0145)<br />
- LICENSEE: Nintendo<br />

[LOCATION] = (0x0147)<br />
- HARDWARE TYPE: MBC1+RAM+BATTERY<br />

[LOCATION] = (0x0148)<br />
- ROM SIZE: 256 KByte<br />
- BANKS: 16<br />

[LOCATION] = (0x014A)<br />
- DESTINATION CODE: Non-Japanese<br />
</pre>

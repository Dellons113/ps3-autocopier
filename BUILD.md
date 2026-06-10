BUILD INSTRUCTIONS

This file describes how to install a PS3 toolchain and build the project into .elf/.sprx/.self artifacts.

1) Install ps3toolchain (recommended)
   - See: https://github.com/ps3dev/ps3toolchain
   - Typical steps (on Linux):
     git clone https://github.com/ps3dev/ps3toolchain.git
     cd ps3toolchain
     ./toolchain.sh
   - After installation, ensure the toolchain bin directory is in your PATH (or set PS3DEV in the Makefile).

2) Verify required tools are available in PATH:
   - ppu-lv2-gcc (or set CROSS_COMPILE so that $(CC) points to your cross-compiler)
   - prxgen or sprxlinker (for converting ELF -> SPRX)
   - make_fself (optional; for SPRX -> SELF)

3) Build
   - From the project root run:
       make
   - Output:
       wm_restorer.elf
       wm_restorer.sprx   (if prxgen/sprxlinker present)
       wm_restorer.self   (if make_fself present)

4) Notes
   - If your loader requires a specific entrypoint name (module_start, plugin_init, etc.), edit main.c or sprx_plugin.c to match.
   - For advanced cases you may need a linker script (.lds). Tell me which loader you target (webMAN, multiman, custom) and I can add a suitable linker script or linker flags.

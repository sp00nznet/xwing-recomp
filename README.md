# X-Wing recomp

A static recompilation of **Star Wars: X-Wing** (the 1998 Windows 95 edition,
as sold on Steam) to a native 32-bit Windows program. The game's x86 code is
lifted to C by pcrecomp and runs on its native32 runtime. DirectDraw is
replaced by a software implementation, so the game's 640x480 8-bit
fullscreen mode runs in a window and the desktop is never touched.

You supply your own copy of the game. Nothing from it is in this repo.

## Status

**Early.** Boots, plays the whole intro (logos, crawl, the Star Destroyer
cinematic) and reaches pilot registration. Keys and clicks work, from the
player or from a `--keys` script.

![Intro](docs/screenshots/intro.png)
![Intro cinematic](docs/screenshots/intro_cinematic.png)
![Pilot registration](docs/screenshots/pilot_register.png)

## Step by step

1. Copy the Steam folder `STAR WARS X-Wing/remastered` into `game/`.
2. The Steam executable is wrapped in SteamStub DRM. Unwrap your copy with
   [Steamless](https://github.com/atom0s/Steamless) and put the result at
   `work/xwing95.exe`:
   ```
   Steamless.CLI.exe game\XWING95.EXE
   copy game\XWING95.EXE.unpacked.exe work\xwing95.exe
   ```
3. Catalogue and lift it:
   ```
   py -3 ..\pcrecomp\tools\disasm\disasm32.py work\xwing95.exe -o work\functions.json
   py -3 run_lift.py --all
   ```
4. Build: `build.cmd` (Visual Studio 2022, x86 tools).
5. Run: `build\xwingrecomp.exe --run`

## How it differs from the original

| What | Why |
|---|---|
| DirectDraw is softdd (`src/runtime/softdd.c`) | the game sets an 8-bit fullscreen mode; softdd draws it in a window |
| A virtual joystick when none is plugged in | the game refuses to start without one |
| DirectInput devices are background, non-exclusive | Acquire fails otherwise while another window is in front |

## Test switches

`--keys "10:esc,20:click:271:327,..."` sends keys (esc, enter, space, arrows,
letters, f1-f10) and clicks in game pixels at the given seconds; while it runs
the real keyboard and mouse are ignored. `--watchdog S` ends the run after S seconds and answers message boxes itself.
`--shot S file.bmp` writes the frame presented after S seconds (only the
game's own frame buffer, never the screen). `--sound` unmutes.

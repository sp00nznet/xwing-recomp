# Roadmap

## Remaster
- [x] Direct3D 11 presenter: resizable, fullscreen, scaling modes, settings menu
- [x] High-resolution 3D from the game's 3D hardware mode, MSAA, texture filtering
- [x] HD texture replacement and dumping (mods\textures, mods\dump)
- [x] Mod file overlay (mods\files)
- [x] Pad, mouse and arrow-key flight
- [ ] Widescreen (Hor+): find the flight engine's projection and clip width. The
      screen size at 0x4CEFA4/0x4CEFA8 is read by 23 functions, most of them 2D;
      gpu.c already renders a wider 3D target when told the gain.
- [ ] OPT conversion (X-Wing vs. TIE Fighter's version 2, XWA's) to X-Wing's version 1, for upgraded craft
- [ ] Concourse palette: the room behind the Options dialog shows in the dialog's colours
- [x] Firing with the virtual joystick (laser sounds play on fire)
- [ ] Check on a real GPU (netlab): so far tested over RDP

## Game
- [x] Sound: effects and music play (measured with --sound-log; not yet listened to)
- [ ] A whole mission flown; the campaign (Tour of Duty)
- [ ] TIE Fighter (TIE95.EXE, same engine) on this host

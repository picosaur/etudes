# Etudes
Sandbox for myself

## Ubuntu
### Dependencies (install via apt)
```
libglfw3-dev
libfreetype-dev
```

### WASM
```
emcmake cmake ../imhello/
emmake make
emrun *.html
```
### ArrayFire
- Problem: devices are available only with sudo
  - Solution: add user to the following groups: `render`, `video` 
```
sudo usermod -a -G video $LOGNAME
sudo usermod -a -G render $LOGNAME
```


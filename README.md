# Etudes
Sandbox for myself

## Devtools
```
apt-utils
build-essential
git
make
ninja-build
cmake
pkg-config
llvm-17
llvm-17-dev
clang-17
libclang-17-dev
libclang-cpp17-dev
libclang-cpp17
clinfo
python3-dev
libpython3-dev
ocl-icd-libopencl1
ocl-icd-libopencl1
ocl-icd-dev
ocl-icd-opencl-dev
zlib1g
zlib1g-dev
libxml2-dev
libhwloc-dev
dialog
```

## Dependencies (install via apt)
```
libglfw3-dev
libfreetype-dev
```

## WASM
```
emcmake cmake ../imhello/
emmake make
emrun *.html
```
## ArrayFire
- Problem: devices are available only with sudo
  - Solution: add user to the following groups: `render`, `video` 
```
sudo usermod -a -G video $LOGNAME
sudo usermod -a -G render $LOGNAME
```


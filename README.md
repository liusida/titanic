# titanic

Trying to use Titan to do experiments.

This project gets around `vcpkg` and includes Titan project as part of it, so it is possible to touch the source code of Titan when needed and rebuild all.

The folder `titan` is cloned from `https://github.com/ja3067/Titan`, we can `cd titan; git pull;` to keep it up to date.

You can either use `cmake` to build the project, or I choose Qt5 Creator as my IDE.

## cmake

```bash
titanic$ mkdir build
titanic/build$ cd build
titanic/build$ cmake ..
```

## prerequisites

```bash
$ sudo apt install libglfw3-dev libglew-dev libglm-dev freeglut3-dev xorg-dev libglu1-mesa-dev
```

Other install notes:

I use Alien 17 with GPU GTX 1070

Ubuntu 19.10 (to avoid confliction, use safe graphic mode, and allow third part source. need set a MOK password and enter it after reboot.)

nVidia Driver 435.21 (use Ubuntu's default, ubuntu-drivers install)

CUDA 10.1 + Patch (Don't install the graphic card driver in cuda)

(Titan requires at least CUDA 10.1, and CUDA 10.2 requires at least nVidia driver 440, so CUDA 10.1 is the only choice.)

add those to .bashrc

```bash
$ export CUDA_PATH=/usr/local/cuda
$ export CUDA_BIN_PATH=/usr/local/cuda/bin
```

install gcc-8 g++-8, replace /usr/bin/gcc

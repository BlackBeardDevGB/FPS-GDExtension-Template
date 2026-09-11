#     FPS-GDExtension-Template

This repository serves as a resource for how an FPS character might be implemented in Godot using C++.
This project can be built for Windows and Web, however Linux and Mac will need to be implemented.

## Requirements
* Experience using GDExtension and C++

## Contents
* Source Files for an FPS character including jump + crouch.
* A playable in-editor level

You can build the project with the following command for windows debug:

```shell
scons
```
Or for release:

```shell
scons target=template_release
```

To build for web, use the following:
```shell
cd emsdk
```
```shell
./emsdk activate latest
```
```shell
cd ..
```
```shell
scons platform=web target=template_release threads=no 
```
```shell
scons platform=web target=template_release threads=yes
```

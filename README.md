
# GRASS FFTools

A fork of the fabulous Grass GIS software, to allow docker GUI installation in Linux and Windows, with ready to use FFSTool Addons (Factum Foundation Surface Tools) to process Lucida and photogrammetry depthmaps produced by Factum Foundation and related projects. 

## Grass GIS

Source project: https://github.com/OSGeo/grass

## Release Image: GUI support and FFTools Addon installed

The build is intented to provide Grass GUI with FFTools Addon already imported and ready to be used.
Dockerfile: `Dockerfile_ubuntu_fftools`

* Build
```
docker build -f docker/ubuntu/Dockerfile_ubuntu_fftools -t grass-fftools:stable-ubuntu .
```
* Run
```
docker run -it --user=$(id -u $USER):$(id -g $USER) --env="DISPLAY" --workdir="/home/$USER" --volume="/home/$USER:/home/$USER" --volume="/etc/group:/etc/group:ro" --volume="/etc/passwd:/etc/passwd:ro" --volume="/etc/shadow:/etc/shadow:ro" --volume="/etc/sudoers.d:/etc/sudoers.d:ro" --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" grass-fftools:stable-ubuntu grass --gui
```

## Development Image: GUI support and GRASS_ADDON_PATH set to local repo folder `fftools` 

This build provide Grass GUI with the ´fftools´ folder set as the ADDONs path, to develop on local folder and update the module code in real time while using Grass.
Once the modules are validated and tested, release build can be done for integration and delivery to users via Dockerhub (see above) 
Dockerfile: `Dockerfile_ubuntu_fftools_dev`

* Build
```
docker build -f docker/ubuntu/Dockerfile_ubuntu_fftools_dev -t grass-fftools-dev:stable-ubuntu .
```
* Run
```
docker run -it --user=$(id -u $USER):$(id -g $USER) --env="DISPLAY" --workdir="/home/$USER" --volume="/home/$USER:/home/$USER" --volume="/etc/group:/etc/group:ro" --volume="/etc/passwd:/etc/passwd:ro" --volume="/etc/shadow:/etc/shadow:ro" --volume="/etc/sudoers.d:/etc/sudoers.d:ro" --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" grass-fftools-dev:stable-ubuntu grass --gui
```

## Pull Image and run on Windows

### TODO: Upload docker image to dockerhub

### Steps
1. Install Docker
2- Install choco: https://chocolatey.org/install
3. Install VcXsrv with choco: Using administrator Powershell `choco install vcxsrv`
4. Run Xlaunch and configure it as shown in https://dev.to/darksmile92/run-gui-app-in-linux-docker-container-on-windows-host-4kde
5. Download/Build Grass FFTools image
6. Set Display env var: `set-variable -name DISPLAY -value 10.11.128.118:0.0` (get IP for Docker with ifconfig)
6. Run it `docker run -ti --rm -e DISPLAY=$DISPLAY firefox`

### References: 

* [Running Ubuntu dockers on Windows hosts](https://ubuntu.com/tutorials/windows-ubuntu-hyperv-containers#1-overview) 
* [Running GUI Linux Docker on Windows host](https://dev.to/darksmile92/run-gui-app-in-linux-docker-container-on-windows-host-4kde)

## Addon library

References: 
* [Grass development wiki](https://grasswiki.osgeo.org/wiki/Development)
* [Grass Compile and Install Addons wiki](https://grasswiki.osgeo.org/wiki/Compile_and_Install#Addons)
* [Official Grass Addons repository](https://github.com/OSGeo/grass-addons)

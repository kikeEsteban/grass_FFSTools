# Docker GRASS GIS (Ubuntu Linux)

## Release Image: GUI and FFTools Addon installed

Dockerfile: `Dockerfile_ubuntu_fftools`

* Build
```
docker build -f docker/ubuntu/Dockerfile_ubuntu_fftools -t grass-fftools:stable-ubuntu .
```
* Run
```
docker run -it --user=$(id -u $USER):$(id -g $USER) --env="DISPLAY" --workdir="/home/$USER" --volume="/home/$USER:/home/$USER" --volume="/etc/group:/etc/group:ro" --volume="/etc/passwd:/etc/passwd:ro" --volume="/etc/shadow:/etc/shadow:ro" --volume="/etc/sudoers.d:/etc/sudoers.d:ro" --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" grass-fftools:stable-ubuntu grass --gui
```

## Development: GUI and GRASS_ADDON_PATH set to local repo dir fftools 

Dockerfile: `Dockerfile_ubuntu_fftools_dev`

* Build
```
docker build -f docker/ubuntu/Dockerfile_ubuntu_fftools_dev -t grass-fftools-dev:stable-ubuntu .
```
* Run
```
docker run -it --user=$(id -u $USER):$(id -g $USER) --env="DISPLAY" --workdir="/home/$USER" --volume="/home/$USER:/home/$USER" --volume="/etc/group:/etc/group:ro" --volume="/etc/passwd:/etc/passwd:ro" --volume="/etc/shadow:/etc/shadow:ro" --volume="/etc/sudoers.d:/etc/sudoers.d:ro" --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" grass-fftools-dev:stable-ubuntu grass --gui
```
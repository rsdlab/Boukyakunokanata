#!/bin/bash

CONTAINER_NAME="folium_20221024"
CONTAINER_CMD=""
DOCKER_ENV=""

USER_ID=$(id -u)
CMDNAME=$(basename $0)

IMAGE_NAME="folium_20221024:latest"

XAUTH="/tmp/.docker.xauth"

HOST_WS=$(dirname $(dirname $(readlink -f $0)))/shared_dir

DOCKER_VOLUME="${DOCKER_VOLUME} -v ${XAUTH}:${XAUTH}:rw"
DOCKER_VOLUME="${DOCKER_VOLUME} -v ${HOST_WS}:/home/folium:rw"
# DOCKER_VOLUME="${DOCKER_VOLUME} -v /etc/passwd:/etc/passwd:rw"

DOCKER_ENV="-e XAUTHORITY=${XAUTH}"
DOCKER_ENV="${DOCKER_ENV} -e DISPLAY=$DISPLAY"
DOCKER_ENV="${DOCKER_ENV} -e USER_ID=${USER_ID}"
DOCKER_ENV="${DOCKER_ENV} -e HOME=/home/folium"

DOCKER_IMAGE="$IMAGE_NAME"

CONTAINER_CMD="${CONTAINER_CMD} bash"

DOCKER_NET="host"

clear

touch ${XAUTH}
xauth nlist $DISPLAY | sed -e 's/^..../ffff/' | xauth -f ${XAUTH} nmerge -

docker run \
  --rm -it \
  --privileged \
  --name ${CONTAINER_NAME} \
  --net ${DOCKER_NET} \
  --user ${USER_ID} \
  ${DOCKER_ENV} \
  ${DOCKER_VOLUME} \
  ${DOCKER_IMAGE} \
  ${CONTAINER_CMD}

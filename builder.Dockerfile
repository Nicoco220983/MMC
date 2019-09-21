FROM ubuntu:18.04
USER root
RUN apt-get update -y

RUN apt-get install build-essential -y
RUN apt-get install libgraphicsmagick1-dev libmagickwand-dev -y
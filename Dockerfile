FROM docker.io/ubuntu:20.04

MAINTAINER Ray Burgemeestre

COPY build/cppquiz /cppquiz

ENTRYPOINT /cppquiz

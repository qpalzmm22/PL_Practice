# find other image so that i don't have to reinstall all the necessary apps
#FROM ubuntu:18.04

FROM gcr.io/oss-fuzz-base/base-builder
RUN apt-get update 

RUN git clone --depth 1 https://github.com/alembic/alembic

# Ubuntu's libilmbase-dev package doesn't include static libraries, so we have
# to build OpenEXR from source.  The v2.4.2 release is the most recent as of
# 2020-07-29.
RUN git clone -b v2.4.2 --depth 1 https://github.com/AcademySoftwareFoundation/openexr

COPY build.sh *.h *.cc $SRC/
WORKDIR $WORK/


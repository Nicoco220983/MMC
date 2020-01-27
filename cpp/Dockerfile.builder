FROM ubuntu:18.04
USER root

COPY builder/install_os.sh /bin
RUN bash /bin/install_os.sh

COPY builder/install_gcc.sh /bin
RUN bash /bin/install_gcc.sh

#RUN apt-get install libgraphicsmagick1-dev libmagickwand-dev -y

#COPY builder/install_ffmpeg.sh /bin
#RUN bash /bin/install_ffmpeg.sh
# FILE: Dockerfile

FROM debian:stretch

RUN	apt-get update \
	&& apt-get install -y \
		apt-transport-https \
		bison \
		build-essential \
		cmake \
		curl \
		flex \
		git \
		libfl-dev \
		libgmp-dev \
		libmpc-dev \
		libmpfr-dev \
		libz3-dev \
		clang \
		sudo \
		unzip \
		vim \
		wget;


# define install path
ENV INSTALL_PATH ../../..

WORKDIR $INSTALL_PATH	
	
RUN	pwd 	
# Update APT sources from our local copy.
COPY	sources.list /etc/apt/sources.list
COPY	src.tar.gz src.tar.gz

# Run updates again.
RUN	pwd \
	&& tar -xzvf src.tar.gz\
	&& cd src \
	&& make clean \
	&& make

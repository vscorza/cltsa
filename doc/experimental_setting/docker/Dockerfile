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
		r-base \
		wget;


# define install path
ENV INSTALL_PATH ../../..

WORKDIR $INSTALL_PATH	
	
RUN	pwd 	
# Update APT sources from our local copy.
COPY	sources.list /etc/apt/sources.list
COPY	README.rmd /README.rmd
ADD	src.tar.gz /
ADD	r_sh.tar.gz /
COPY	.Rprofile /r_sh/.Rprofile

# Base image https://hub.docker.com/r/rocker/rstudio
#FROM rocker/tidyverse:latest

RUN R -e "install.packages(c('dplyr','xtable','magrittr'), repos = 'http://mirror.fcaglp.unlp.edu.ar/CRAN/')"

# Run updates again.
RUN	mkdir -p /tmp-results \
	&& cd /src \
	&& make clean \
	&& make \
	&& chmod +x cltsa \
	&& cd /r_sh \
	&& chmod +x *.sh \
	&& ./run_all_reports.sh "/src/tests/" "/tmp-results/" "/src/" 1 "docker"
	

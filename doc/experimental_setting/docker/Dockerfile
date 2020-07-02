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
COPY	src.tar.gz src.tar.gz
COPY	r_sh.tar.gz r_sh.tar.gz

# Run updates again.
RUN	pwd \
	&& tar -xzvf src.tar.gz\
	&& rm src.tar.gz \
	&& tar -xzvf r_sh.tar.gz\
	&& rm r_sh.tar.gz \	
	&& cd r_sh \
	&& chmod +x *.sh \
	&& cd .. \
	&& cd src \
	&& make clean \
	&& make \
	&& chmod +x cltsa \
	&& cd .. \
	&& mkdir -p /tmp-results \
	&& cd r_sh \
	&& ./run_all_reports.sh "/src/tests/" "/tmp-results/" "/src/" 1
	

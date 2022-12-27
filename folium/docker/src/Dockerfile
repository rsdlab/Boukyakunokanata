FROM ubuntu:18.04

RUN sed -i 's@archive.ubuntu.com@ftp.jaist.ac.jp/pub/Linux@g' /etc/apt/sources.list

LABEL maintainer="tasakilab"

SHELL ["/bin/bash", "-c"]

ENV DEBIAN_FRONTEND noninteractive

RUN mkdir -p /home/folium 

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        sudo \
        python3-pip \
        python3-tk \
        python3-dev \
        zip \
        wget \
        language-pack-ja \
        libreoffice \
        nano && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# probably, need to install "opencv"
RUN apt-get update && \
    apt-get install -y \ 
        libgl1-mesa-glx libglib2.0-0 libsm6 libxrender1 libxext6

# need to update python
RUN apt update && \
    apt install -y --no-install-recommends \
        build-essential libbz2-dev libdb-dev \
        libreadline-dev libffi-dev libgdbm-dev liblzma-dev \
        libncursesw5-dev libsqlite3-dev libssl-dev \
        zlib1g-dev uuid-dev tk-dev

# Update python version to 3.9.2 because pymupdf can't use under 3.6 version
RUN cd tmp && \
    wget https://www.python.org/ftp/python/3.9.2/Python-3.9.2.tar.xz && \
    tar xJf Python-3.9.2.tar.xz && \
    cd Python-3.9.2 && \
    ./configure && \
    make && \
    sudo make install && \
    cd .. && \
    rm -rf Python-3.9.2 && \
    cd ..

# setting langage at japanese
ENV LANG=ja_JP.UTF-8

RUN pip3 install \
        setuptools \
        Cython \
        pandas --verbose \
        numpy --verbose \
        scikit-build \
        wheel --verbose
RUN pip3 install --upgrade pip
RUN pip3 install --upgrade setuptools
RUN pip3 install  \
	opencv-python==4.5.4.60 --verbose \
        opencv-contrib-python==4.4.0.46 --verbose
RUN pip3 install  \
        folium

# 表示の変更
RUN echo "export PS1='\[\e]0;\u@\h: \w\a\]${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$'" >> /etc/bash.bashrc

# 権限設定
ARG username=folium
ARG wkdir=/home/${username}/

ARG UID=1000
ARG GID=1000
RUN groupadd -g $GID $username && \
    useradd -m -s /bin/bash -u $UID -g $GID -G sudo $username && \
    echo $username:$username | chpasswd && \
    echo "$username   ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers
    
WORKDIR ${wkdir}
RUN chown ${username}:${username} ${wkdir}
USER ${username}

# bashrcの設定
ADD ./.bashrc ${wkdir}

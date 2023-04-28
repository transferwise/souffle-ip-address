FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y wget g++ make curl unzip strace shellcheck

RUN wget https://souffle-lang.github.io/ppa/souffle-key.public -O /usr/share/keyrings/souffle-archive-keyring.gpg \
    && echo "deb [signed-by=/usr/share/keyrings/souffle-archive-keyring.gpg] https://souffle-lang.github.io/ppa/ubuntu/ stable main" | tee /etc/apt/sources.list.d/souffle.list

RUN apt-get update
RUN apt-get install -y souffle


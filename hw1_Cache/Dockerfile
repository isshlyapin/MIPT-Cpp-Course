FROM gcc:15.2.0

COPY . /usr/src/myapp
WORKDIR /usr/src/myapp

RUN curl -LsSf https://astral.sh/uv/install.sh | sh

RUN apt update -y; apt upgrade -y;

RUN apt install -y cmake 

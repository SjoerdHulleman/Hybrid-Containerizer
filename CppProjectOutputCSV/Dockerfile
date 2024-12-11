FROM gcc:latest

COPY . /usr/src

WORKDIR /usr/src

RUN g++ -I/usr/src/asio/ -o App modified.cpp

EXPOSE 8080

CMD ["./App"]

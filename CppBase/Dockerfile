FROM gcc:latest

COPY . /usr/src

WORKDIR /usr/src

RUN g++ -I/usr/src/asio/ -o App CppBase.cpp

EXPOSE 8080

CMD ["./App"]

FROM ubuntu:latest
LABEL authors="kechang"

ENTRYPOINT ["top", "-b"]
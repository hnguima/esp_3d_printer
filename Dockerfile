FROM espressif/idf

RUN apt update && apt install -y \
    software-properties-common \
    npm \
    python3-pip

RUN npm install npm@latest -g && \
    npm install n -g && \
    n latest \
    npm install -g protobufjs protobufjs-cli\
    pip install protobuf grpcio-tools 
gcc ./protocols/ext-data-control-v1-protocol.c -Iprotocols ./src/main.c $(pkg-config --cflags --libs wayland-client) -o ./dist/main
./dist/main

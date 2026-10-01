# yank

A minimal Wayland clipboard copy tool written in C. It takes text from an argument or stdin and puts it on the clipboard via the `ext-data-control-v1` protocol.

After setting the selection, yank forks into the background and serves paste requests until another client takes over the clipboard.

## Requirements

- A Wayland compositor that supports `ext-data-control-v1` (wayland-protocols >= 1.39)
- `gcc`, `pkg-config`, `wayland-client`

## Build

```sh
./build.sh
```

This compiles the binary to `dist/main`.

## Usage

Copy an argument:

```sh
./dist/main "hello world"
```

Copy from stdin:

```sh
echo "hello world" | ./dist/main
cat file.txt | ./dist/main
```

Paste with your usual shortcut, or check with `wl-paste`.

## Offered MIME types

- `text/plain;charset=utf-8`
- `text/plain`
- `UTF8_STRING`, `STRING`, `TEXT` (for XWayland clients)

## Debugging

To log the Wayland protocol messages:

```sh
WAYLAND_DEBUG=1 ./dist/main "hello"
```

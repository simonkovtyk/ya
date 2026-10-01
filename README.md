# ya

A minimal Wayland clipboard copy tool written in C. It takes data from an argument or stdin and puts it on the clipboard via the `ext-data-control-v1` protocol.

Data of any size can be yanked: input from stdin is read into a buffer that grows as needed, so you can copy anything from a single word to large files.

After setting the selection, ya forks into the background and serves paste requests until another client takes over the clipboard.

## Requirements

- A Wayland compositor that supports `ext-data-control-v1` (wayland-protocols >= 1.39)
- `gcc`, `pkg-config`, `wayland-client`

## Build

```sh
./build.sh
```

## Usage

```
ya TEXT
ya < FILE
COMMAND | ya
```

Copy an argument:

```sh
ya "hello world"
```

Copy from stdin:

```sh
echo "hello world" | ya
ya < file.txt
```

Paste with your usual shortcut, or check with `wl-paste`.

## Offered MIME types

- `text/plain;charset=utf-8`
- `text/plain`
- `UTF8_STRING`, `STRING`, `TEXT` (for XWayland clients)

## Debugging

To log the Wayland protocol messages:

```sh
WAYLAND_DEBUG=1 ya "hello"
```

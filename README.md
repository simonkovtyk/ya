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
ya [-t MIME] [TEXT]
```

| Option    | Description                                                                  |
| --------- | ---------------------------------------------------------------------------- |
| `-t MIME` | Offer the data as the given MIME type only (default: plain text types below) |
| `TEXT`    | Text to copy. If omitted, data is read from stdin                            |

Copy an argument:

```sh
ya "hello world"
```

Copy from stdin:

```sh
echo "hello world" | ya
ya < file.txt
```

Copy binary data with a specific MIME type:

```sh
ya -t image/png < screenshot.png
```

Paste with your usual shortcut, or check with `wl-paste`.

## Offered MIME types

Without `-t`, ya offers:

- `text/plain;charset=utf-8`
- `text/plain`
- `UTF8_STRING`, `STRING`, `TEXT` (for XWayland clients)

## Performance compared to wl-copy

ya keeps the data in memory and serves it directly, while `wl-copy` first writes it to a temporary file. This makes copying from stdin noticeably faster, especially for typical small to medium inputs.

Time until the copy command returns (average of 5 runs, stdin input):

| Size   | ya     | wl-copy 2.3.0 | Speedup |
| ------ | ------ | ------------- | ------- |
| 1 MB   | 4 ms   | 19 ms         | ~4.8x   |
| 100 MB | 23 ms  | 50 ms         | ~2.2x   |
| 500 MB | 107 ms | 138 ms        | ~1.3x   |

Pasting is equally fast with both tools. For short text passed as an argument, both finish in under 2 ms.

## Debugging

To log the Wayland protocol messages:

```sh
WAYLAND_DEBUG=1 ya "hello"
```

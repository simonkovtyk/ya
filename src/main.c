#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <signal.h>
#include <string.h>
#include "../protocols/ext-data-control-v1-client-protocol.h"

static struct ext_data_control_manager_v1 *manager;
static struct wl_seat *seat;
static const char *data;
static size_t data_len;
static int running = 1;

static void global(void *d, struct wl_registry *reg, uint32_t name, const char *iface, uint32_t ver) {
  if (!strcmp(iface, wl_seat_interface.name))
    seat = wl_registry_bind(reg, name, &wl_seat_interface, 1);
  else if (!strcmp(iface, ext_data_control_manager_v1_interface.name))
    manager = wl_registry_bind(reg, name, &ext_data_control_manager_v1_interface, 1);
}

static void global_remove(void *d, struct wl_registry *r, uint32_t n) {}

static const struct wl_registry_listener reg_listener = { global, global_remove };

static void on_send(void *d, struct ext_data_control_source_v1 *src, const char *mime, int32_t fd) {
  size_t off = 0;

  while (off < data_len) {
    ssize_t w = write(fd, data + off, data_len - off);
    if (w <= 0) break;
    off += w;
  }

  close(fd);
}

static void on_cancelled(void *d, struct ext_data_control_source_v1 *src) {
    running = 0;
}
static const struct ext_data_control_source_v1_listener src_listener = {
    .send = on_send,
    .cancelled = on_cancelled,
};

int main (int argc, char **argv) {
  const char *mime = NULL;
  int opt;

  while ((opt = getopt(argc, argv, "t:")) != -1) {
    switch (opt) {
      case 't':
        mime = optarg;
        break;
      default:
        fprintf(stderr, "usage: ya [-t MIME] [TEXT]\n");
        return 1;
    }
  }

  if (optind < argc) {
    data = argv[optind];
    data_len = strlen(data);
  } else if (!isatty(STDIN_FILENO)) {
    size_t buf_cap = 1024 * 1024 * 16;
    size_t offset = 0;
    size_t n;
    char *buf = malloc(buf_cap);

    while ((n = (fread(buf + offset, 1, buf_cap - offset, stdin))) > 0) {
      offset += n;

      if (offset == buf_cap) {
        buf = realloc(buf, buf_cap *= 2);
      }
    }

    data_len = offset;
    data = buf;

  } else {
    fprintf(stderr, "usage: ya [-t MIME] [TEXT]\n");
    return 1;
  }

  struct sigaction sigign = { .sa_handler = SIG_IGN };

  if (sigaction(SIGPIPE, &sigign, NULL) == -1) {
    perror("Cannot handle signals. Proceeding...\n");
  }

  struct wl_display *dpy = wl_display_connect(NULL);
  struct wl_registry *reg = wl_display_get_registry(dpy);
  wl_registry_add_listener(reg, &reg_listener, NULL);
  wl_display_roundtrip(dpy);

  if (!manager || !seat) { fprintf(stderr, "Wayland with protocol version of >=1.39 is required. Exiting...\n"); return 1; }

  struct ext_data_control_device_v1 *dev = ext_data_control_manager_v1_get_data_device(manager, seat);
  struct ext_data_control_source_v1 *src = ext_data_control_manager_v1_create_data_source(manager);

  ext_data_control_source_v1_add_listener(src, &src_listener, NULL);
  if (mime) {
    ext_data_control_source_v1_offer(src, mime);
  } else {
    ext_data_control_source_v1_offer(src, "UTF8_STRING");
    ext_data_control_source_v1_offer(src, "STRING");
    ext_data_control_source_v1_offer(src, "TEXT");
    ext_data_control_source_v1_offer(src, "text/plain;charset=utf-8");
    ext_data_control_source_v1_offer(src, "text/plain");
  }
  ext_data_control_device_v1_set_selection(dev, src);
  wl_display_roundtrip(dpy);

  pid_t pid = fork();

  if (pid == -1) {
    perror("Cannot fork process. Exiting...\n");
    return 1;
  }

  if (pid > 0) _exit(0);

  setsid();
  fclose(stdin);
  fclose(stdout);
  fclose(stderr);

  while (running && wl_display_dispatch(dpy) != -1) {}

  wl_display_disconnect(dpy);

  return 0;
}

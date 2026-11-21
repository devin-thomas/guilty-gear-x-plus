/* The text layer on a Core host (tl.h, core_host.h). */
#ifndef GGX_TL_HOST_H
#define GGX_TL_HOST_H

#include <stdio.h>

#include "core_host.h"
#include "tl.h"

/* tl_bind with the host's disc and the file table the boot ELF loaded into
   EE RAM (the host must be open). Only reads Core memory. */
int tl_bind_host(TextLayer *t, CoreHost *h, char *err, size_t cap);

/* One picture of a game file from the host's disc (tl_read_picture). */
int tl_host_picture(CoreHost *h, uint32_t fid, const char *path, TlRawPicture *out);

#endif

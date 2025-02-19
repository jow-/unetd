// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 Felix Fietkau <nbd@nbd.name>
 */
#ifndef __UNETD_PEX_H
#define __UNETD_PEX_H

#include <libubox/uloop.h>

struct network;

struct network_pex_host {
	struct list_head list;
	union network_endpoint endpoint;
};

struct network_pex {
	struct uloop_fd fd;
	struct list_head hosts;
	struct uloop_timeout request_update_timer;
};

enum pex_event {
	PEX_EV_HANDSHAKE,
	PEX_EV_ENDPOINT_CHANGE,
	PEX_EV_QUERY,
	PEX_EV_PING,
};

void network_pex_init(struct network *net);
int network_pex_open(struct network *net);
void network_pex_close(struct network *net);

void network_pex_event(struct network *net, struct network_peer *peer,
		       enum pex_event ev);

static inline bool network_pex_active(struct network_pex *pex)
{
	return pex->fd.fd >= 0;
}

int global_pex_open(void);

#endif

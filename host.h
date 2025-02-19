// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 Felix Fietkau <nbd@nbd.name>
 */
#ifndef __UNETD_HOST_H
#define __UNETD_HOST_H

struct network_peer {
	struct vlist_node node;
	uint8_t key[CURVE25519_KEY_SIZE];
	union network_addr local_addr;
	const char *endpoint;
	struct blob_attr *ipaddr;
	struct blob_attr *subnet;
	int port;

	struct {
		int connect_attempt;
		bool connected;
		bool handshake;
		bool has_local_ep_addr;
		union network_addr local_ep_addr;
		union network_endpoint endpoint;

		union network_endpoint next_endpoint;
		uint64_t last_ep_update;

		uint64_t rx_bytes;
		uint64_t last_handshake;
		uint64_t last_request;
		int idle;
		int num_net_queries;
	} state;
};

struct network_host {
	struct avl_node node;

	const char *gateway;
	struct network_peer peer;
};

struct network_group {
	struct avl_node node;
	const char *name;

	int n_members;
	struct network_host **members;
};

static inline const char *network_host_name(struct network_host *host)
{
	if (!host)
		return "(none)";

	return host->node.key;
}

static inline bool network_host_is_peer(struct network_host *host)
{
	return !!host->peer.node.avl.key;
}

static inline const char *network_peer_name(struct network_peer *peer)
{
	struct network_host *host;

	if (!peer)
		return "(none)";

	host = container_of(peer, struct network_host, peer);
	return network_host_name(host);
}


static inline bool
network_host_uses_peer_route(struct network_host *host, struct network *net,
			    struct network_peer *peer)
{
	if (&host->peer == peer || host == net->net_config.local_host)
		return false;

	if (net->net_config.local_host->gateway &&
	    !strcmp(net->net_config.local_host->gateway, network_peer_name(peer)))
		return true;

	if (!host->gateway)
		return false;

	return !strcmp(host->gateway, network_peer_name(peer));
}

#define for_each_routed_host(cur_host, net, peer)			\
	avl_for_each_element(&(net)->hosts, cur_host, node)		\
		if (network_host_uses_peer_route(host, net, peer))


void network_hosts_update_start(struct network *net);
void network_hosts_update_done(struct network *net);
void network_hosts_add(struct network *net, struct blob_attr *hosts);

void network_hosts_init(struct network *net);
void network_hosts_free(struct network *net);

#endif

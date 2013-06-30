#ifndef _OSMO_IPA_H_
#define _OSMO_IPA_H_

#include <stdint.h>
#include <osmocom/core/linuxlist.h>
#include <osmocom/core/timer.h>

struct ipa_server_link {
	struct e1inp_line		*line;
	struct osmo_fd			ofd;
	const char			*addr;
	uint16_t			port;
	int (*accept_cb)(struct ipa_server_link *link, int fd);
	void				*data;
};

struct ipa_server_link *ipa_server_link_create(void *ctx, struct e1inp_line *line, const char *addr, uint16_t port, int (*accept_cb)(struct ipa_server_link *link, int fd), void *data);
void ipa_server_link_destroy(struct ipa_server_link *link);

int ipa_server_link_open(struct ipa_server_link *link);
void ipa_server_link_close(struct ipa_server_link *link);

struct ipa_server_conn {
	struct ipa_server_link		*server;
	struct osmo_fd			ofd;
	struct llist_head		tx_queue;
	int (*closed_cb)(struct ipa_server_conn *peer);
	int (*cb)(struct ipa_server_conn *peer, struct msgb *msg);
	void				*data;
};

struct ipa_server_conn *ipa_server_conn_create(void *ctx, struct ipa_server_link *link, int fd, int (*cb)(struct ipa_server_conn *peer, struct msgb *msg), int (*closed_cb)(struct ipa_server_conn *peer), void *data);
void ipa_server_conn_destroy(struct ipa_server_conn *peer);

void ipa_server_conn_send(struct ipa_server_conn *peer, struct msgb *msg);

enum ipa_client_conn_state {
	IPA_CLIENT_LINK_STATE_NONE         = 0,
	IPA_CLIENT_LINK_STATE_CONNECTING   = 1,
	IPA_CLIENT_LINK_STATE_CONNECTED    = 2,
	IPA_CLIENT_LINK_STATE_MAX
};

struct ipa_client_conn {
	struct e1inp_line		*line;
	struct osmo_fd			*ofd;
	struct llist_head		tx_queue;
	struct osmo_timer_list		timer;
	enum ipa_client_conn_state	state;
	const char			*addr;
	uint16_t			port;
	void (*updown_cb)(struct ipa_client_conn *link, int up);
	int (*read_cb)(struct ipa_client_conn *link, struct msgb *msg);
	int (*write_cb)(struct ipa_client_conn *link);
	void				*data;
};

struct ipa_client_conn *ipa_client_conn_create(void *ctx, struct e1inp_ts *ts, int priv_nr, const char *addr, uint16_t port, void (*updown)(struct ipa_client_conn *link, int), int (*read_cb)(struct ipa_client_conn *link, struct msgb *msgb), int (*write_cb)(struct ipa_client_conn *link), void *data);
void ipa_client_conn_destroy(struct ipa_client_conn *link);

int ipa_client_conn_open(struct ipa_client_conn *link);
void ipa_client_conn_close(struct ipa_client_conn *link);

void ipa_client_conn_send(struct ipa_client_conn *link, struct msgb *msg);

int ipa_msg_recv(int fd, struct msgb **rmsg);

int ipaccess_rcvmsg_base(struct msgb *msg, struct osmo_fd *bfd);

#endif

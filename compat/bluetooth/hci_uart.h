/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * The part of linux-6.18.y drivers/bluetooth/hci_uart.h that btmtksdio
 * uses: the H4 packet descriptors, which older kernels have in
 * drivers/bluetooth/h4_recv.h. That header is not part of the kernel
 * headers an external module is built against, so carry them here.
 */
#ifndef MT7668_COMPAT_HCI_UART_H
#define MT7668_COMPAT_HCI_UART_H

struct h4_recv_pkt {
	u8  type;	/* Packet type */
	u8  hlen;	/* Header length */
	u8  loff;	/* Data length offset in header */
	u8  lsize;	/* Data length field size */
	u16 maxlen;	/* Max overall packet length */
	int (*recv)(struct hci_dev *hdev, struct sk_buff *skb);
};

#define H4_RECV_ACL \
	.type = HCI_ACLDATA_PKT, \
	.hlen = HCI_ACL_HDR_SIZE, \
	.loff = 2, \
	.lsize = 2, \
	.maxlen = HCI_MAX_FRAME_SIZE \

#define H4_RECV_SCO \
	.type = HCI_SCODATA_PKT, \
	.hlen = HCI_SCO_HDR_SIZE, \
	.loff = 2, \
	.lsize = 1, \
	.maxlen = HCI_MAX_SCO_SIZE

#define H4_RECV_EVENT \
	.type = HCI_EVENT_PKT, \
	.hlen = HCI_EVENT_HDR_SIZE, \
	.loff = 1, \
	.lsize = 1, \
	.maxlen = HCI_MAX_EVENT_SIZE

#endif /* MT7668_COMPAT_HCI_UART_H */

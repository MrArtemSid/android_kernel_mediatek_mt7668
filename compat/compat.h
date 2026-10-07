/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Backports the kernel APIs that wlan/ (written for Linux 6.18) uses on
 * older kernels. Force-included into every wlan/ object by the top
 * Makefile, so the driver sources stay identical to upstream.
 *
 * Written against the Android common kernel android13-5.15, which carries
 * some later cfg80211 changes (e.g. MLO link_id parameters), so the
 * version checks below describe that kernel rather than plain v5.15.
 */
#ifndef _MT7668_COMPAT_H
#define _MT7668_COMPAT_H

#include <linux/version.h>
#include <linux/atomic.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <net/cfg80211.h>

/* ------------------------------------------------------------------ */
/* Symbol namespaces                                                   */
/* ------------------------------------------------------------------ */

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 13, 0)
/* Since 6.13 the namespace is a string literal. The driver imports the
 * Android common kernel's VFS_internal_... namespace, which is where
 * filp_open(), kernel_read() and kernel_write() are exported.
 */
#undef MODULE_IMPORT_NS
#define MODULE_IMPORT_NS(ns)	MODULE_INFO(import_ns, ns)
#endif

/* ------------------------------------------------------------------ */
/* Kernel functions the driver calls                                   */
/* ------------------------------------------------------------------ */

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 2, 0)
#define timer_delete_sync(t)	del_timer_sync(t)
#define timer_delete(t)		del_timer(t)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 0)
/* 6.18 passes the wireless_dev, 5.15 the net_device */
#define cfg80211_new_sta(wdev, mac, sinfo, gfp) \
	cfg80211_new_sta((wdev)->netdev, mac, sinfo, gfp)
#define cfg80211_del_sta(wdev, mac, gfp) \
	cfg80211_del_sta((wdev)->netdev, mac, gfp)

/* android13-5.15 still has the punct_bitmap argument */
#define cfg80211_ch_switch_notify(dev, chandef, link_id) \
	cfg80211_ch_switch_notify(dev, chandef, link_id, 0)

/* no link_id before 6.12 */
#define cfg80211_cac_event(netdev, chandef, event, gfp, link_id) \
	cfg80211_cac_event(netdev, chandef, event, gfp)
#endif

/* ------------------------------------------------------------------ */
/* cfg80211_ops callbacks whose signature changed                      */
/* ------------------------------------------------------------------ */

/*
 * The driver defines these callbacks with their 6.18 signature. Under
 * CFI a callback must have exactly the type of its cfg80211_ops member,
 * so a cast is not an option. Instead, every function-like use of the
 * name (the driver's prototypes, definitions and direct calls) is renamed
 * to mtk618_*() by the macros below, while the bare name in the driver's
 * cfg80211_ops initializers is not a macro invocation and resolves to
 * the wrapper of the same name defined here with the old signature.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 0)

/* Since 6.7 change_beacon gets a cfg80211_ap_update; the driver only
 * uses its beacon member.
 */
struct cfg80211_ap_update {
	struct cfg80211_beacon_data beacon;
};

/* Since 6.18 cfg80211 allocates the remain_on_channel/mgmt_tx cookie and
 * hands it to the driver; older kernels expect the driver to return one.
 */
static inline u64 mt7668_compat_new_cookie(void)
{
	static atomic64_t counter = ATOMIC64_INIT(0);
	u64 cookie;

	do {
		cookie = atomic64_inc_return(&counter);
	} while (!cookie);

	return cookie;
}

#define MT7668_COMPAT_ROC(name)						\
int mtk618_##name(struct wiphy *wiphy, struct wireless_dev *wdev,	\
		  struct ieee80211_channel *chan, unsigned int duration,\
		  u64 cookie, const u8 *rx_addr);			\
static inline int name(struct wiphy *wiphy, struct wireless_dev *wdev,	\
		       struct ieee80211_channel *chan,			\
		       unsigned int duration, u64 *cookie)		\
{									\
	*cookie = mt7668_compat_new_cookie();				\
	return mtk618_##name(wiphy, wdev, chan, duration, *cookie, NULL);\
}

#define MT7668_COMPAT_MGMT_TX(name)					\
int mtk618_##name(struct wiphy *wiphy, struct wireless_dev *wdev,	\
		  struct cfg80211_mgmt_tx_params *params, u64 cookie);	\
static inline int name(struct wiphy *wiphy, struct wireless_dev *wdev,	\
		       struct cfg80211_mgmt_tx_params *params,		\
		       u64 *cookie)					\
{									\
	*cookie = mt7668_compat_new_cookie();				\
	return mtk618_##name(wiphy, wdev, params, *cookie);		\
}

#define MT7668_COMPAT_ADD_KEY(name)					\
int mtk618_##name(struct wiphy *wiphy, struct wireless_dev *wdev,	\
		  int link_id, u8 key_index, bool pairwise,		\
		  const u8 *mac_addr, struct key_params *params);	\
static inline int name(struct wiphy *wiphy, struct net_device *dev,	\
		       int link_id, u8 key_index, bool pairwise,	\
		       const u8 *mac_addr, struct key_params *params)	\
{									\
	return mtk618_##name(wiphy, dev->ieee80211_ptr, link_id,	\
			     key_index, pairwise, mac_addr, params);	\
}

#define MT7668_COMPAT_GET_KEY(name)					\
int mtk618_##name(struct wiphy *wiphy, struct wireless_dev *wdev,	\
		  int link_id, u8 key_index, bool pairwise,		\
		  const u8 *mac_addr, void *cookie,			\
		  void (*callback)(void *cookie, struct key_params *));	\
static inline int name(struct wiphy *wiphy, struct net_device *dev,	\
		       int link_id, u8 key_index, bool pairwise,	\
		       const u8 *mac_addr, void *cookie,		\
		       void (*callback)(void *cookie,			\
					struct key_params *))		\
{									\
	return mtk618_##name(wiphy, dev->ieee80211_ptr, link_id,	\
			     key_index, pairwise, mac_addr, cookie,	\
			     callback);					\
}

#define MT7668_COMPAT_DEL_KEY(name)					\
int mtk618_##name(struct wiphy *wiphy, struct wireless_dev *wdev,	\
		  int link_id, u8 key_index, bool pairwise,		\
		  const u8 *mac_addr);					\
static inline int name(struct wiphy *wiphy, struct net_device *dev,	\
		       int link_id, u8 key_index, bool pairwise,	\
		       const u8 *mac_addr)				\
{									\
	return mtk618_##name(wiphy, dev->ieee80211_ptr, link_id,	\
			     key_index, pairwise, mac_addr);		\
}

#define MT7668_COMPAT_SET_MGMT_KEY(name)				\
int mtk618_##name(struct wiphy *wiphy, struct wireless_dev *wdev,	\
		  int link_id, u8 key_index);				\
static inline int name(struct wiphy *wiphy, struct net_device *dev,	\
		       int link_id, u8 key_index)			\
{									\
	return mtk618_##name(wiphy, dev->ieee80211_ptr, link_id,	\
			     key_index);				\
}

#define MT7668_COMPAT_GET_STATION(name)					\
int mtk618_##name(struct wiphy *wiphy, struct wireless_dev *wdev,	\
		  const u8 *mac, struct station_info *sinfo);		\
static inline int name(struct wiphy *wiphy, struct net_device *dev,	\
		       const u8 *mac, struct station_info *sinfo)	\
{									\
	return mtk618_##name(wiphy, dev->ieee80211_ptr, mac, sinfo);	\
}

/* add_station and change_station */
#define MT7668_COMPAT_STATION_PARAMS(name)				\
int mtk618_##name(struct wiphy *wiphy, struct wireless_dev *wdev,	\
		  const u8 *mac, struct station_parameters *params);	\
static inline int name(struct wiphy *wiphy, struct net_device *dev,	\
		       const u8 *mac, struct station_parameters *params)\
{									\
	return mtk618_##name(wiphy, dev->ieee80211_ptr, mac, params);	\
}

#define MT7668_COMPAT_DEL_STATION(name)					\
int mtk618_##name(struct wiphy *wiphy, struct wireless_dev *wdev,	\
		  struct station_del_parameters *params);		\
static inline int name(struct wiphy *wiphy, struct net_device *dev,	\
		       struct station_del_parameters *params)		\
{									\
	return mtk618_##name(wiphy, dev->ieee80211_ptr, params);	\
}

#define MT7668_COMPAT_SET_WIPHY_PARAMS(name)				\
int mtk618_##name(struct wiphy *wiphy, int radio_idx, u32 changed);	\
static inline int name(struct wiphy *wiphy, u32 changed)		\
{									\
	return mtk618_##name(wiphy, -1, changed);			\
}

#define MT7668_COMPAT_SET_TX_POWER(name)				\
int mtk618_##name(struct wiphy *wiphy, struct wireless_dev *wdev,	\
		  int radio_idx, enum nl80211_tx_power_setting type,	\
		  int mbm);						\
static inline int name(struct wiphy *wiphy, struct wireless_dev *wdev,	\
		       enum nl80211_tx_power_setting type, int mbm)	\
{									\
	return mtk618_##name(wiphy, wdev, -1, type, mbm);		\
}

#define MT7668_COMPAT_GET_TX_POWER(name)				\
int mtk618_##name(struct wiphy *wiphy, struct wireless_dev *wdev,	\
		  int radio_idx, unsigned int link_id, int *dbm);	\
static inline int name(struct wiphy *wiphy, struct wireless_dev *wdev,	\
		       int *dbm)					\
{									\
	return mtk618_##name(wiphy, wdev, -1, 0, dbm);			\
}

#define MT7668_COMPAT_START_RADAR(name)					\
int mtk618_##name(struct wiphy *wiphy, struct net_device *dev,		\
		  struct cfg80211_chan_def *chandef, u32 cac_time_ms,	\
		  int link_id);						\
static inline int name(struct wiphy *wiphy, struct net_device *dev,	\
		       struct cfg80211_chan_def *chandef,		\
		       u32 cac_time_ms)					\
{									\
	return mtk618_##name(wiphy, dev, chandef, cac_time_ms, 0);	\
}

#define MT7668_COMPAT_TDLS_MGMT(name)					\
int mtk618_##name(struct wiphy *wiphy, struct net_device *dev,		\
		  const u8 *peer, int link_id, u8 action_code,		\
		  u8 dialog_token, u16 status_code, u32 peer_capability,\
		  bool initiator, const u8 *buf, size_t len);		\
static inline int name(struct wiphy *wiphy, struct net_device *dev,	\
		       const u8 *peer, u8 action_code, u8 dialog_token,	\
		       u16 status_code, u32 peer_capability,		\
		       bool initiator, const u8 *buf, size_t len)	\
{									\
	return mtk618_##name(wiphy, dev, peer, -1, action_code,		\
			     dialog_token, status_code, peer_capability,\
			     initiator, buf, len);			\
}

#define MT7668_COMPAT_CHANGE_BEACON(name)				\
int mtk618_##name(struct wiphy *wiphy, struct net_device *dev,		\
		  struct cfg80211_ap_update *info);			\
static inline int name(struct wiphy *wiphy, struct net_device *dev,	\
		       struct cfg80211_beacon_data *info)		\
{									\
	struct cfg80211_ap_update params = { .beacon = *info };		\
									\
	return mtk618_##name(wiphy, dev, &params);			\
}

/* Station interface: gl_init.c mtk_wlan_ops */
MT7668_COMPAT_ROC(mtk_cfg80211_remain_on_channel)
MT7668_COMPAT_MGMT_TX(mtk_cfg80211_mgmt_tx)
MT7668_COMPAT_ADD_KEY(mtk_cfg80211_add_key)
MT7668_COMPAT_GET_KEY(mtk_cfg80211_get_key)
MT7668_COMPAT_DEL_KEY(mtk_cfg80211_del_key)
MT7668_COMPAT_GET_STATION(mtk_cfg80211_get_station)
MT7668_COMPAT_STATION_PARAMS(mtk_cfg80211_change_station)
MT7668_COMPAT_STATION_PARAMS(mtk_cfg80211_add_station)
MT7668_COMPAT_DEL_STATION(mtk_cfg80211_del_station)
MT7668_COMPAT_TDLS_MGMT(mtk_cfg80211_tdls_mgmt)

/* P2P interfaces: gl_p2p.c mtk_p2p_ops */
MT7668_COMPAT_ROC(mtk_p2p_cfg80211_remain_on_channel)
MT7668_COMPAT_MGMT_TX(mtk_p2p_cfg80211_mgmt_tx)
MT7668_COMPAT_CHANGE_BEACON(mtk_p2p_cfg80211_change_beacon)
MT7668_COMPAT_SET_WIPHY_PARAMS(mtk_p2p_cfg80211_set_wiphy_params)
MT7668_COMPAT_DEL_STATION(mtk_p2p_cfg80211_del_station)
MT7668_COMPAT_GET_STATION(mtk_p2p_cfg80211_get_station)
MT7668_COMPAT_ADD_KEY(mtk_p2p_cfg80211_add_key)
MT7668_COMPAT_GET_KEY(mtk_p2p_cfg80211_get_key)
MT7668_COMPAT_DEL_KEY(mtk_p2p_cfg80211_del_key)
MT7668_COMPAT_SET_MGMT_KEY(mtk_p2p_cfg80211_set_mgmt_key)
MT7668_COMPAT_SET_TX_POWER(mtk_p2p_cfg80211_set_txpower)
MT7668_COMPAT_GET_TX_POWER(mtk_p2p_cfg80211_get_txpower)
MT7668_COMPAT_START_RADAR(mtk_p2p_cfg80211_start_radar_detection)

/* Only now rename the driver's own functions to their 6.18 versions. */
#define mtk_cfg80211_remain_on_channel(...)	mtk618_mtk_cfg80211_remain_on_channel(__VA_ARGS__)
#define mtk_cfg80211_mgmt_tx(...)		mtk618_mtk_cfg80211_mgmt_tx(__VA_ARGS__)
#define mtk_cfg80211_add_key(...)		mtk618_mtk_cfg80211_add_key(__VA_ARGS__)
#define mtk_cfg80211_get_key(...)		mtk618_mtk_cfg80211_get_key(__VA_ARGS__)
#define mtk_cfg80211_del_key(...)		mtk618_mtk_cfg80211_del_key(__VA_ARGS__)
#define mtk_cfg80211_get_station(...)		mtk618_mtk_cfg80211_get_station(__VA_ARGS__)
#define mtk_cfg80211_change_station(...)	mtk618_mtk_cfg80211_change_station(__VA_ARGS__)
#define mtk_cfg80211_add_station(...)		mtk618_mtk_cfg80211_add_station(__VA_ARGS__)
#define mtk_cfg80211_del_station(...)		mtk618_mtk_cfg80211_del_station(__VA_ARGS__)
#define mtk_cfg80211_tdls_mgmt(...)		mtk618_mtk_cfg80211_tdls_mgmt(__VA_ARGS__)

#define mtk_p2p_cfg80211_remain_on_channel(...)	mtk618_mtk_p2p_cfg80211_remain_on_channel(__VA_ARGS__)
#define mtk_p2p_cfg80211_mgmt_tx(...)		mtk618_mtk_p2p_cfg80211_mgmt_tx(__VA_ARGS__)
#define mtk_p2p_cfg80211_change_beacon(...)	mtk618_mtk_p2p_cfg80211_change_beacon(__VA_ARGS__)
#define mtk_p2p_cfg80211_set_wiphy_params(...)	mtk618_mtk_p2p_cfg80211_set_wiphy_params(__VA_ARGS__)
#define mtk_p2p_cfg80211_del_station(...)	mtk618_mtk_p2p_cfg80211_del_station(__VA_ARGS__)
#define mtk_p2p_cfg80211_get_station(...)	mtk618_mtk_p2p_cfg80211_get_station(__VA_ARGS__)
#define mtk_p2p_cfg80211_add_key(...)		mtk618_mtk_p2p_cfg80211_add_key(__VA_ARGS__)
#define mtk_p2p_cfg80211_get_key(...)		mtk618_mtk_p2p_cfg80211_get_key(__VA_ARGS__)
#define mtk_p2p_cfg80211_del_key(...)		mtk618_mtk_p2p_cfg80211_del_key(__VA_ARGS__)
#define mtk_p2p_cfg80211_set_mgmt_key(...)	mtk618_mtk_p2p_cfg80211_set_mgmt_key(__VA_ARGS__)
#define mtk_p2p_cfg80211_set_txpower(...)	mtk618_mtk_p2p_cfg80211_set_txpower(__VA_ARGS__)
#define mtk_p2p_cfg80211_get_txpower(...)	mtk618_mtk_p2p_cfg80211_get_txpower(__VA_ARGS__)
#define mtk_p2p_cfg80211_start_radar_detection(...) mtk618_mtk_p2p_cfg80211_start_radar_detection(__VA_ARGS__)

#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 0) */

#endif /* _MT7668_COMPAT_H */

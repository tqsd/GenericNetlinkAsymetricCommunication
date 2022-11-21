#ifndef CONFIG_H_
#define CONFIG_H_

/* NETLINK callback function prototypes */
int gnl_qu_net_config_doit(struct sk_buff *sender_skb, struct genl_info *info);

#endif // CONFIG_H_

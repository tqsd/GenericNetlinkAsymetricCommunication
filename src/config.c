#include "config.h"

/*
 * gnl_qu_net_config_doit()
 * Callback function is called when a userspace program
 * tries to modify the device
 */

int gnl_qu_net_config_doit(struct sk_buff *sender_skb, struct genl_info *info) {
  pr_info("CONFIG COMMAND NETLINK");
  return 1;
}

#include "command.h"

#include <linux/slab.h>
#include <net/genetlink.h>

#include "qu_netlink.h"
#include "register.h"
#include "types.h"
#include <linux/kernel.h> /* We're doing kernel work */
#include <linux/module.h> /* Specifically, a module  */
#include <linux/string.h>
/*
**  gnl_qu_net_command_doit
**
**  Passes messages between the control and node
*/
int gnl_qu_net_command_doit(struct sk_buff *sender_skb,
                            struct genl_info *info) {

  struct nlattr *na_msg;
  struct nlattr *na_dev_type;
  struct nlattr *na_dev_id;
  char *recv_msg;
  int *recv_dev_type;
  int *dev_id;
  DEVICE_TYPE dev_type;

  if (info == NULL) {
    pr_err("An error occured in %s(): \n", __func__);
    return -EINVAL;
  }

  na_dev_type = info->attrs[GNL_QU_NET_A_DEV_TYPE];
  na_msg = info->attrs[GNL_QU_NET_A_MSG];
  if (!na_dev_type || !na_msg) {
    pr_err("no info->attrs[%i]\n", GNL_QU_NET_A_DEV_TYPE);
    pr_err("no info->attrs[%i]\n", GNL_QU_NET_A_MSG);
    return -EINVAL;
  }

  recv_msg = (char *)nla_data(na_msg);
  recv_dev_type = nla_data(na_dev_type);

  if (recv_msg == NULL || recv_dev_type == NULL) {
    pr_err("Error while receiving data\n");
  } else {
    pr_info("received: '%s'\n", recv_msg);
  }

  dev_type = msg_to_DEVICE_TYPE(recv_dev_type);

  if (dev_type == SIMULATION_CONTROL_MODULE) {
    pr_err("SIMULATION_CONTROL_MODULE responded with command response");
    pr_info("Should pass to the node");

    na_dev_id = info->attrs[GNL_QU_NET_A_ID];
    if (!na_dev_id) {
      pr_err("no info->attrs[%i]\n", GNL_QU_NET_A_ID);
      return -EINVAL;
    }

    dev_id = nla_data(na_dev_id);
    pass_to_node(info, *dev_id, recv_msg);
  } else {
    pr_info("Should pass to the simulation control_module");
    pass_to_scm(info, recv_msg);
  }

  return 0;
}

int pass_to_scm(struct genl_info *info, char *msg) {
  int rc;
  void *pkt_head;
  struct sk_buff *pkt;
  mutex_lock(&controller_module);
  pkt = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
  if (pkt == NULL) {
    return -ENOMEM;
  }
  control_module.info.snd_seq++;

  pkt_head = genlmsg_put(pkt, control_module.info.snd_portid,
                         control_module.info.snd_seq, &gnl_qu_net_family, 0,
                         GNL_QU_NET_C_COMMAND);

  if (pkt_head == NULL) {
    return -ENOMEM;
  }

  if (0 != nla_put_string(pkt, GNL_QU_NET_A_MSG, msg)) {
    return -EINVAL;
  }

  genlmsg_end(pkt, pkt_head);

  rc =
      genlmsg_unicast(genl_info_net(info), pkt, control_module.info.snd_portid);
  if (rc != 0) {
    pr_err("An error occurred in %s():\n", __func__);
    return -rc;
  }
  mutex_unlock(&controller_module);
  return 0;
}

int pass_to_node(struct genl_info *info, int node_id, char *msg) {
  int rc;
  void *pkt_head;
  struct sk_buff *pkt;
  pkt = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
  if (pkt == NULL) {
    return -ENOMEM;
  }
  qu_devices[node_id].info.snd_seq++;
  pkt_head = genlmsg_put(pkt, qu_devices[node_id].info.snd_portid,
                         qu_devices[node_id].info.snd_seq, &gnl_qu_net_family,
                         0, GNL_QU_NET_C_COMMAND);

  if (pkt_head == NULL) {
    return -ENOMEM;
  }

  if (0 != nla_put_string(pkt, GNL_QU_NET_A_MSG, msg)) {
    return -EINVAL;
  }

  genlmsg_end(pkt, pkt_head);

  rc = genlmsg_unicast(genl_info_net(info), pkt,
                       qu_devices[node_id].info.snd_portid);
  if (rc != 0) {
    pr_err("An error occurred in %s():\n", __func__);
    return -rc;
  }
  return 0;
}


#include <linux/slab.h>
#include <net/genetlink.h>

#include "qu_netlink.h"
#include "register.h"
#include "types.h"
#include <linux/kernel.h> /* We're doing kernel work */
#include <linux/module.h> /* Specifically, a module  */
#include <linux/string.h>

// Is memory for qu_devices allocated
int qu_devices_allocated = 0;

qu_device *qu_devices;
struct mutex qu_dev_mutex;

qu_device control_module = {.type = NOT_A_DEVICE};
struct mutex controller_module;

// Conversion function
DEVICE_TYPE msg_to_DEVICE_TYPE(int *n) {
  switch (*n) {
  case 0:
    return SIMULATION_CONTROL_MODULE;
  case 1:
    return QUANTUM_NETWORK_DEVICE;
  default:
    return NOT_A_DEVICE;
  }
}

void register_control_module(struct qu_device qu_control_module) {
  control_module = qu_control_module;
}

void deregister_control_module(struct qu_device qu_control_module) {
  control_module.type = NOT_A_DEVICE;
}

int empty_device_slots(void) {
  for (int i = 0; i < qu_devices_allocated; i++) {
    if (qu_devices[i].type == NOT_A_DEVICE) {
      pr_info("Empty device slot found %d", i);
      return i;
    }
  }
  return -1;
}

int register_device(struct qu_device qudev) {
  // Deal with the dynamically sized array
  int alloc_index;
  alloc_index = empty_device_slots();
  if (alloc_index < 0) {
    if (qu_devices_allocated < 1) {
      // If not allocated allocate array with size 1
      pr_info("Malloc");
      qu_devices = kmalloc(1 * sizeof(qu_device), GFP_KERNEL);
    } else {
      // If allocated resize it
      qu_devices =
          krealloc(qu_devices, (qu_devices_allocated + 1) * sizeof(qu_device),
                   GFP_KERNEL);
    }
    qu_devices_allocated++;
    alloc_index = qu_devices_allocated - 1;
  }
  // Add the device to the array
  (qu_devices)[alloc_index] = qudev;
  pr_info("Currently %d devices registered!", qu_devices_allocated);

  mutex_unlock(&qu_dev_mutex);
  // Index and length are two different things
  return alloc_index;
}

int deregister_device(struct qu_device qu_dev, int *id) {
  mutex_lock(&qu_dev_mutex);

  if (*id > qu_devices_allocated) {
    pr_err("This id is not assigned; No device deregistered!");
    return -1;
  }
  if (qu_devices[*id].type == NOT_A_DEVICE) {
    pr_err("Device with this id already deregistered!");
    return -1;
  }

  if (qu_dev.info.snd_portid == qu_devices[*id].info.snd_portid) {
    qu_devices[*id].type = NOT_A_DEVICE;
  } else {
    pr_err("The device and PID do not match");
    return -1;
  }

  mutex_unlock(&qu_dev_mutex);
  return 0;
}

int register_response(int assigned_device_id, struct sk_buff *request,
                      struct genl_info *info, char *msg) {
  int rc;
  void *msg_head;
  struct sk_buff *reply;

  reply = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
  if (reply == NULL) {
    return -ENOMEM;
  }
  /* start the message */
  msg_head = genlmsg_put(
      reply, // buffer for netlink message: struct sk_buff *
             // According to my findings: this is not used for routing
             // This can be used in an application specific way to target
             // different endpoints within the same user application
             // but general rule: just put sender port id here
      info->snd_portid, // sending port (not process) id: int
      info->snd_seq,    // sequence number: int (might be used by receiver, but
                        // not mandatory)
      &gnl_qu_net_family, // struct genl_family *
      0, // flags for Netlink header: int; application specific and not
         // mandatory
      // The command/operation (u8) from `enum GNL_FOOBAR_XMPL_COMMAND` for
      // Generic Netlink header
      GNL_QU_NET_C_REGISTER);
  if (msg_head == NULL) {
    return -ENOMEM;
  }
  if (0 != nla_put_string(reply, GNL_QU_NET_A_MSG, msg)) {
    return -EINVAL;
  }

  if (0 != nla_put_u32(reply, GNL_QU_NET_A_ID, assigned_device_id)) {
    return -EINVAL;
  }
  genlmsg_end(reply, msg_head);
  rc = genlmsg_reply(reply, info);
  if (rc != 0) {
    pr_err("An error occurred in %s():\n", __func__);
    return -rc;
  }

  return 0;
}

int register_with_simulation_control_module(int assigned_device_id,
                                            struct genl_info *info, char *msg) {
  int rc;
  void *msg_head;
  struct sk_buff *reply;
  mutex_lock(&controller_module);

  reply = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
  if (reply == NULL) {
    return -ENOMEM;
  }
  control_module.info.snd_seq++;
  /* start the message */
  msg_head = genlmsg_put(
      reply, // buffer for netlink message: struct sk_buff *
             // According to my findings: this is not used for routing
             // This can be used in an application specific way to target
             // different endpoints within the same user application
             // but general rule: just put sender port id here
      control_module.info.snd_portid, // sending port (not process) id: int
      control_module.info.snd_seq,    // sequence number: int (might be used by
                                      // receiver, but not mandatory)
      &gnl_qu_net_family,             // struct genl_family *
      0, // flags for Netlink header: int; application specific and not
         // mandatory
      // The command/operation (u8) from `enum GNL_FOOBAR_XMPL_COMMAND` for
      // Generic Netlink header
      GNL_QU_NET_C_REGISTER);
  if (msg_head == NULL) {
    return -ENOMEM;
  }
  if (0 != nla_put_string(reply, GNL_QU_NET_A_MSG, msg)) {
    return -EINVAL;
  }

  if (0 != nla_put_u32(reply, GNL_QU_NET_A_ID, assigned_device_id)) {
    return -EINVAL;
  }

  genlmsg_end(reply, msg_head);

  rc = genlmsg_unicast(genl_info_net(info), reply,
                       control_module.info.snd_portid);
  if (rc != 0) {
    pr_err("An error occurred in %s():\n", __func__);
    return -rc;
  }
  mutex_unlock(&controller_module);
  return 0;
}

int gnl_qu_net_register_doit(struct sk_buff *sender_skb,
                             struct genl_info *info) {
  struct nlattr *na_dev_type;
  struct nlattr *na_msg;
  int *recv_dev_type;
  char *recv_msg;
  int rr;

  qu_device qu_dev;
  // device to register
  DEVICE_TYPE qu_dev_reg;

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
  recv_dev_type = nla_data(na_dev_type);
  recv_msg = (char *)nla_data(na_msg);
  if (recv_msg == NULL) {
    pr_err("Error while receiving data\n");
  } else {
    pr_info("New quantum device registration requested: '%s'\n", recv_msg);
  }

  qu_dev_reg = msg_to_DEVICE_TYPE(recv_dev_type);

  pr_info("Device type: %i\n", qu_dev_reg);

  if (qu_dev_reg < 0) {
    pr_err("Invalid device type");
    return 0;
  }

  if (qu_dev_reg == SIMULATION_CONTROL_MODULE) {
    if (control_module.type != NOT_A_DEVICE) {
      pr_err("Simulation control module already registered, only one can be "
             "registered at a time\nPlease unregister the existing module "
             "first.");

      register_response(-1, sender_skb, info, recv_msg);
      return -1;
    } else {
      qu_dev.type = SIMULATION_CONTROL_MODULE;
      qu_dev.info = *info;
      register_control_module(qu_dev);
      register_response(0, sender_skb, info, recv_msg);
      pr_info("New quantum device succesfully registered\n");
    }
  } else {
    if (control_module.type == NOT_A_DEVICE) {
      pr_err("Control module should be registered first!");
      register_response(-1, sender_skb, info, recv_msg);
      return -EINVAL;
    }
    pr_info("Registering quantum network device");
    qu_dev.type = QUANTUM_NETWORK_DEVICE;
    qu_dev.info = *info;
    rr = register_device(qu_dev);
    register_with_simulation_control_module(rr, info, recv_msg);
    register_response(rr, sender_skb, info, recv_msg);
  }

  return 0;
}

int gnl_qu_net_deregister_doit(struct sk_buff *sender_skb,
                               struct genl_info *info) {
  struct nlattr *na_dev_type;
  struct nlattr *na_msg;
  char *recv_msg;
  int *recv_dev_type;
  DEVICE_TYPE qu_dev_reg;
  int resp;
  struct nlattr *recv_id;
  int *id;

  pr_info("%s() invoked \n", __func__);
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

  recv_id = info->attrs[GNL_QU_NET_A_ID];
  if (!recv_id) {
    pr_err("2");
    pr_info("no info->attrs[%i]\n", GNL_QU_NET_A_ID);
    return -EINVAL;
  } else {
    id = (int *)nla_data(recv_id);
  }

  recv_dev_type = nla_data(na_dev_type);
  recv_msg = (char *)nla_data(na_msg);
  if (recv_msg == NULL) {
    pr_err("Error while receiving data\n");
  } else {
    pr_info("New deregistration request: '%s'\n", recv_msg);
  }

  qu_dev_reg = msg_to_DEVICE_TYPE(recv_dev_type);

  pr_info("Device type: %i\n", qu_dev_reg);

  if (qu_dev_reg < 0) {
    pr_err("Invalid device type");
    register_response(-1, sender_skb, info, recv_msg);
    return 0;
  }
  if (qu_dev_reg == SIMULATION_CONTROL_MODULE) {
    if (control_module.type != SIMULATION_CONTROL_MODULE) {
      pr_err("Simulation control module is not registered!");
      register_response(-1, sender_skb, info, recv_msg);
      return -1;
    } else {
      qu_device qu_dev;
      qu_dev.type = SIMULATION_CONTROL_MODULE;
      qu_dev.info = *info;
      deregister_control_module(qu_dev);
      pr_info("Simulation control module succesfully deregistered!");
      register_response(0, sender_skb, info, recv_msg);
    }
  } else {
    qu_device qu_dev;
    qu_dev.type = QUANTUM_NETWORK_DEVICE;
    qu_dev.info = *info;
    pr_info("Just before entering the deregistration function!");
    resp = deregister_device(qu_dev, id);
    register_response(resp, sender_skb, info, recv_msg);
  }

  return 0;
}

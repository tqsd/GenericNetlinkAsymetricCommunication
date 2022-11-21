/** @file command.h
 *  @brief Manages the passing of commands between the devices
 *
 *  This header file container the prototyles for the
 *  kernel module to handle the message passing.
 *
 *  @author Simon Sekavčnik
 *  @bug No known bugs.
 */
#ifndef COMMAND_H_
#define COMMAND_H_
#include <net/genetlink.h>

typedef struct genl_info genl_info;
typedef struct sk_buff sk_buff;
typedef struct qu_device qu_device;

/*
** @brief passes the commands between the processes
**
** The command is passed between the processes
**
** @param *sk_buff passed by the netlink kernel module
** @param *genl_info passed by the netlink kernel module
*/
int gnl_qu_net_command_doit(struct sk_buff *sender_skb, struct genl_info *info);

/*
** @brief passes the commant to the control module
**
** @param *sk_buff passed by the netlink kernel module
** @param *genl_info passed by the netlink kernel module
*/
int pass_to_control_module(struct genl_info *info, char *msg);

/*
** @brief passes the commant to the recipient process
**
** @param *sk_buff passed by the netlink kernel module
** @param *genl_info passed by the netlink kernel module
*/
int pass_to_process(struct genl_info *info, int node_id, char *msg);

#endif // COMMAND_H_

/** @file register.h
 *  @brief Manages the registration and client addressing
 *
 *  This header file container the prototyles for the
 *  kernel module to handle registration, deregistration
 *  and addressing.
 *
 *  @author Simon Sekavčnik
 *  @bug No known bugs.
 */
#ifndef REGISTER_H_
#define REGISTER_H_
/*
** @include <net/genetlink.h>
**
** We are defining our own type which requires
** definitions from <net/genetlink.h>
*/
#include <net/genetlink.h>

/*
** Just a place holder, an array is initiated, which has 2
** elements, but is later expanded or contracted.
*/
#define NDEVICES = 2

/*
** Here we define the device types that can connect to the
** netlink.
*/
typedef enum {
  NOT_A_DEVICE = -1,
  SIMULATION_CONTROL_MODULE = 0,
  QUANTUM_NETWORK_DEVICE = 1
} DEVICE_TYPE;

/*
** Forward definitions
*/
typedef struct genl_info genl_info;
typedef struct sk_buff sk_buff;
typedef struct qu_device qu_device;

/* @brief registers the device
**
** The device is registered, by informing the
** control module and by adding the device
** info (genl_info) to the array of known devices.
** The control module needs to be registered
** befire other devices.
**
** The devices should not communicate directly,
** all communciations are passed through the
** control module.
**
** @param *sender_skb passed by the netlik kernel module
** @param *info passed by the netlink kernel module
**
*/
int gnl_qu_net_register_doit(struct sk_buff *sender_skb,
                             struct genl_info *info);

/* @brief deregisters the device
**
** The device is deregistered, by informing the
** control module and by adding the device
** info (genl_info) to the array of known devices
**
** @param *sender_skb passed by the netlik kernel module
** @param *info passed by the netlink kernel module
**
*/
int gnl_qu_net_deregister_doit(struct sk_buff *sender_skb,
                               struct genl_info *info);

/*
** @brief casts the device type number to device type
**
** Might be obsolete (CHECK)
**
** @param *n pointer to integer
** @return DEVICE_TYPE enumeration
*/
DEVICE_TYPE msg_to_DEVICE_TYPE(int *n);

/*
** @brief sets the flag, allowing other device types to join
**
** Only one control device can join.
**
** @param qu_device information about the device
*/
void register_control_module(struct qu_device qu_control_module);
/*
** @brief resets the flag, after that other devices cannot join
**
** Other devices can join if any control device is active.
**
** @param qu_device information about the device
*/
void deregister_control_module(struct qu_device qu_control_module);

/*
** @brief registers the device.
** Allocates memory in array and puts the device info into
** the array. This is done in a thread safe way.
**
** @param qu_device quantum device info
** @return int number of devices currently registered;
*/
int register_device(struct qu_device qudev);

/*
** @brief Deregisters the deivce
**
** Sets the device type to NOT_A_DEVICE and
** sends a message to the control module,
** informing it about deregistration.
*/
int deregister_device(struct qu_device qu_dev, int *id);

int register_response(int assigned_device_id, struct sk_buff *request,
                      struct genl_info *info, char *msg);

/*
** @brief checks the qu_devices list, for unused slots
**
** The qu_devices array never shirnks, but if there is an
** empty slot available it is used.
**
** @return int index of an empty slot
**         -1 if there is no empty slot
*/
int empty_device_slots(void);

/*
** @brief Sends the message to the control module
**
** The message about the registration of a device is sent
** to the control module.
**
** @param assigned_device_id => Index of the device in the qu_devices array
** @param *info => passed from the original callback
** @param *msg  => pointer to the message string
**
** @return int success >0; failure <0
*/
int register_with_simulation_control_module(int assigned_device_id,
                                            struct genl_info *info, char *msg);

extern struct mutex qu_dev_mutex;

/*
** @type qu_device
**
** type, which includes all important information about the
** communicating device
*/
typedef struct qu_device {
  DEVICE_TYPE type; // Device type
  genl_info info;   // Process identification
} qu_device;

extern qu_device control_module;
extern struct mutex controller_module;
extern qu_device *qu_devices;

#endif // REGISTER_H_

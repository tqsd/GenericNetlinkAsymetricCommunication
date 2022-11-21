#ifndef REGISTER_H_
#define REGISTER_H_

#define NDEVICES = 2

typedef enum {
  NOT_A_DEVICE = -1,
  SIMULATION_CONTROL_MODULE = 0,
  QUANTUM_NETWORK_DEVICE = 1
} DEVICE_TYPE;

/* Type redefinition */
typedef struct genl_info genl_info;
typedef struct sk_buff sk_buff;
typedef struct qu_device qu_device;

/* NETLINK callback function prototypes */
int gnl_qu_net_register_doit(struct sk_buff *sender_skb,
                             struct genl_info *info);

int gnl_qu_net_deregister_doit(struct sk_buff *sender_skb,
                               struct genl_info *info);

/* Internal function prototypes */

DEVICE_TYPE msg_to_DEVICE_TYPE(int *n);
void register_control_module(struct qu_device qu_control_module);
void deregister_control_module(struct qu_device qu_control_module);
int register_device(struct qu_device qudev);
int deregister_device(struct qu_device qu_dev, int *id);
int register_response(int assigned_device_id, struct sk_buff *request,
                      struct genl_info *info, char *msg);
int empty_device_slots(void);
int register_with_simulation_control_module(int assigned_device_id,
                                            struct genl_info *info, char *msg);

extern struct mutex qu_dev_mutex;

// QU DEVICE
typedef struct qu_device {
  DEVICE_TYPE type; // Device type
  genl_info info;   // Process identification
} qu_device;

extern qu_device control_module;
extern struct mutex controller_module;
extern qu_device *qu_devices;
#endif // REGISTER_H_

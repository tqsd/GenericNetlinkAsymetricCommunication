
.. _program_listing_file_quantum_network_device_kernel_module.c:

Program Listing for File quantum_network_device_kernel_module.c
===============================================================

|exhale_lsh| :ref:`Return to documentation for file <file_quantum_network_device_kernel_module.c>` (``quantum_network_device_kernel_module.c``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   
   #include "./qndkm.h"
   #include "./src/command.h"
   #include "./src/config.h"
   #include "./src/echo.h"
   #include "./src/qu_netlink.h"
   #include "./src/register.h"
   #include "./src/types.h"
   #include <linux/module.h>
   #include <net/genetlink.h>
   
   #define DEV_NAME                                                               \
     "qunet_virt" 
   MODULE_LICENSE("GPL");
   MODULE_AUTHOR("Simon Sekavčnik");
   MODULE_DESCRIPTION("Kernel interface, which exposes the interface to "
                      "simulated/real quantum networking hardware");
   MODULE_VERSION("0.01");
   
   /* \def GNL_QU_NET_OPS_LEN
    * \brief Calculate (at compile time) how many possible operations are defined
    * in src/qu_netlink.h
    */
   #define GNL_QU_NET_OPS_LEN (GNL_QU_NET_COMMAND_COUNT)
   
   /*
    * Array with all operations that our protocol on top of Generic Netlink
    * supports. An operation is the glue between a command and the corresponding
    * ".doit" callback function.
    */
   
   /* \var genl_ops[GNL_QU_NET_OPS]
   ** \brief array of genl_ops
   **
   ** genl_ops is composed of:
   **  - .cmd            => operation command
   **  - .flags          => flags (we do not use them)
   **  - .internal_flags => internal_flags (we do not use them)
   **  - .doit           => callback function which is called
   **                       if the message with .cmd type is received
   */
   
   struct genl_ops gnl_qu_net_ops[GNL_QU_NET_OPS_LEN] = {
       {
           .cmd = GNL_QU_NET_C_ECHO, 
           .flags = 0,
           .internal_flags = 0,
           .doit = gnl_qu_net_echo_doit,
           .dumpit = NULL,
           .start = NULL, 
           .done = NULL,
           .validate = 0,
       },
       {
           .cmd = GNL_QU_NET_C_REGISTER,
           .flags = 0,
           .internal_flags = 0,
           .doit = gnl_qu_net_register_doit,
           .dumpit = NULL,
           .start = NULL,
           .done = NULL,
           .validate = 0,
       },
       {
           .cmd = GNL_QU_NET_C_DEREGISTER,
           .flags = 0,
           .internal_flags = 0,
           .doit = gnl_qu_net_deregister_doit,
           .validate = 0,
       },
       {
           .cmd = GNL_QU_NET_C_CONFIG,
           .flags = 0,
           .internal_flags = 0,
           .doit = gnl_qu_net_echo_doit,
           .validate = 0,
       },
       {
           .cmd = GNL_QU_NET_C_COMMAND,
           .flags = 0,
           .internal_flags = 0,
           .doit = gnl_qu_net_command_doit,
           .validate = 0,
       },
   };
   
   /*
    * Attribute policy
    */
   static struct nla_policy gnl_qu_net_policy[GNL_QU_NET_ATTRIBUTE_ENUM_LEN] = {
       [GNL_QU_NET_A_UNSPEC] = {.type = NLA_UNSPEC},
       [GNL_QU_NET_A_MSG] = {.type = NLA_NUL_STRING},
       [GNL_QU_NET_A_DEV_TYPE] = {.type = NLA_U32},
       [GNL_QU_NET_A_ID] = {.type = NLA_U32},
   };
   
   /* Netlink family definition */
   struct genl_family gnl_qu_net_family = {
       .id = 0,
       .hdrsize = 0,
       .name = FAMILY_NAME,
       .version = 1,
       // Delegating incomming requests to callback functions
       .ops = gnl_qu_net_ops, // TODO
       // length of array 'gnl_qu_net_ops'
       .n_ops = GNL_QU_NET_OPS_LEN,              // INVESTIGATE
       .policy = gnl_qu_net_policy,              // DONE
       .maxattr = GNL_QU_NET_ATTRIBUTE_ENUM_LEN, // Implemented
       .module = THIS_MODULE,
       .parallel_ops = true,
       .pre_doit = NULL,
       .post_doit = NULL};
   
   /*
    * Kernel initialization function; ran when loading the module into the
    * kernel.
    *
    * @return 0 if succesfull
    *
    */
   static int __init simulated_quantum_network_kernel_module_init(void) {
     int rc;
     printk("\n");
     pr_info("QNDKM Module is being inserted.\n");
     printk("\n");
   
     // Register family with its operations and policies
     //
     rc = genl_register_family(&gnl_qu_net_family);
     pr_info("Registering policies.\n");
     if (rc != 0) {
       pr_err("FAILED: genl_register_family(): %i\n", rc);
       pr_err("An error occurred while inserting the generic netlink example "
              "module\n");
       return -1;
     } else {
       pr_info("successfully registered custom Netlink family '" FAMILY_NAME
               "' using Generic Netlink.\n");
     }
   
     // Initializeing the mutex
     mutex_init(&qu_dev_mutex);
   
     return 0;
   }
   
   static void __exit simulated_quantum_network_kernel_module_exit(void) {
     int ret;
     printk("\n");
     printk("Simulated Quantum Network Device Kernel Module unloaded!\n\n");
     // Unregistering family name
     ret = genl_unregister_family(&gnl_qu_net_family);
     if (ret != 0) {
       pr_err("genl_unregister_family() failed: %i\n", ret);
       return;
     } else {
       pr_info("successfully unregistered custom Netlink family '" FAMILY_NAME
               "' using Generic Netlink.\n");
     }
   }
   
   /*
    * Registering init exit modules
    */
   module_init(simulated_quantum_network_kernel_module_init);
   module_exit(simulated_quantum_network_kernel_module_exit);

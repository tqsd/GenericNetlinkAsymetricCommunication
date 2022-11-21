
.. _program_listing_file_src_qu_netlink.h:

Program Listing for File qu_netlink.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_qu_netlink.h>` (``src/qu_netlink.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #ifndef QU_NETLINK_H_
   #define QU_NETLINK_H_
   /*
    * NETLINK INTERFACE DEFINITIONS
    */
   
   /*
    * Netlink family name
    */
   #define FAMILY_NAME "qu_net_link"
   
   enum GNL_QU_NET_ATTRIBUTE {
     /* Convention: 0th enumeration is never used (?)  */
     GNL_QU_NET_A_UNSPEC,
     /*  We expect a MSG to be a null-terminated C-string. */
     GNL_QU_NET_A_MSG,
     GNL_QU_NET_A_DEV_TYPE,
     /* Integer, which is to be included for identification purposes. */
     GNL_QU_NET_A_ID,
     /* Not a real attribute */
     __QU_NET_A_MAX,
   };
   
   #define GNL_QU_NET_ATTRIBUTE_ENUM_LEN (__QU_NET_A_MAX)
   
   /*
    * The numbe rof usable attributes in `enum QU_NET_ATTRIBUTE`.
    */
   #define GNL_QU_NET_ATTRIBUTE_COUNT (QU_NET_ATTRIBUTE_ENUM_LEN - 1)
   
   /*
    * Enumeration of all of the commands
    */
   enum GNL_QU_NET_COMMAND {
     GNL_QU_NET_C_UNSPEC,
     GNL_QU_NET_C_ECHO,       // For testing purposes
     GNL_QU_NET_C_REGISTER,   // For registering a module inside of the container
     GNL_QU_NET_C_DEREGISTER, // For deregistering a module
     GNL_QU_NET_C_CONFIG,     // For configuring a hw device
     GNL_QU_NET_C_COMMAND,    // For configuring a hw device
     // container GNL_QU_NET_C_CONFIG,     // Configuration command
     // GNL_QU_NET_C_COMMAND, // Operation commands from the user-space to the
     // quantum
     //  simulation
     // GNL_QU_NET_C_RESPONSE, // Generic response of the kernel module
     __QU_NET_C_MAX,
   
   };
   
   /*
    * Number of elements in 'QU_NET_COMMAND'
    */
   #define GNL_QU_NET_COMMAND_ENUM_LEN (__QU_NET_C_MAX)
   #define GNL_QU_NET_COMMAND_COUNT (GNL_QU_NET_COMMAND_ENUM_LEN - 1)
   
   #endif // QU_NETLINK_H_

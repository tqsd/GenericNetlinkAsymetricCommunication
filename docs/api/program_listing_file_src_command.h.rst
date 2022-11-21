
.. _program_listing_file_src_command.h:

Program Listing for File command.h
==================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_command.h>` (``src/command.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #ifndef COMMAND_H_
   #define COMMAND_H_
   
   typedef struct genl_info genl_info;
   typedef struct sk_buff sk_buff;
   typedef struct qu_device qu_device;
   /* NETLINK callback function prototypes */
   int gnl_qu_net_command_doit(struct sk_buff *sender_skb, struct genl_info *info);
   int pass_to_scm(struct genl_info *info, char *msg);
   int pass_to_node(struct genl_info *info, int node_id, char *msg);
   
   #endif // COMMAND_H_

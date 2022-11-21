
.. _program_listing_file_src_config.c:

Program Listing for File config.c
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_config.c>` (``src/config.c``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

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

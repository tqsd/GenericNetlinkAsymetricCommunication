
.. _program_listing_file_src_config.h:

Program Listing for File config.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_src_config.h>` (``src/config.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #ifndef CONFIG_H_
   #define CONFIG_H_
   
   /* NETLINK callback function prototypes */
   int gnl_qu_net_config_doit(struct sk_buff *sender_skb, struct genl_info *info);
   
   #endif // CONFIG_H_

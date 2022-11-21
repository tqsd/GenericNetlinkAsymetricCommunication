
.. _program_listing_file_src_echo.h:

Program Listing for File echo.h
===============================

|exhale_lsh| :ref:`Return to documentation for file <file_src_echo.h>` (``src/echo.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #ifndef ECHO_H_
   #define ECHO_H_
   
   typedef struct sk_buff sk_buff;
   typedef struct genl_info genl_info;
   int gnl_qu_net_echo_doit(struct sk_buff *sender_skb, struct genl_info *info);
   
   #endif // ECHO_H_

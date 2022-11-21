
.. _program_listing_file_src_echo.c:

Program Listing for File echo.c
===============================

|exhale_lsh| :ref:`Return to documentation for file <file_src_echo.c>` (``src/echo.c``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "echo.h"
   #include "qu_netlink.h"
   #include "types.h"
   
   #include <net/genetlink.h>
   
   /*
    * FUNCTION PROTOTYPES
    */
   
   int gnl_qu_net_echo_doit(struct sk_buff *sender_skb, struct genl_info *info) {
     struct nlattr *na;
     struct sk_buff *reply_skb;
     int rc;
     void *msg_head;
     char *recv_msg;
     pr_info("%s() invoked \n", __func__);
   
     if (info == NULL) {
       pr_err("An error occured in %s(): \n", __func__);
       return -EINVAL;
     }
     na = info->attrs[GNL_QU_NET_A_MSG];
   
     if (!na) {
       pr_err("no info->attrs[%i]\n", GNL_QU_NET_A_MSG);
       return -EINVAL;
     }
     recv_msg = (char *)nla_data(na);
     if (recv_msg == NULL) {
       pr_err("Error while receiving data\n");
     } else {
       pr_info("received: '%s'\n", recv_msg);
     }
     // Send a message back
   
     // allocate some memory
     reply_skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
     if (reply_skb == NULL) {
       pr_err("An error occured in %s():\n", __func__);
       return -ENOMEM;
     }
   
     // Create the message headers
     msg_head = genlmsg_put(reply_skb,          // buffer for netlink message
                            info->snd_portid,   // sending port id
                            info->snd_seq + 1,  // sequence number; not mandatory
                            &gnl_qu_net_family, // struct genl_family
                            0, // flags for netlink header ; not mandatory
                            GNL_QU_NET_C_ECHO); // operation command
     if (msg_head == NULL) {
       rc = ENOMEM;
       pr_err("An error occurred in %s():\n", __func__);
       return -rc;
     }
   
     // Add a GNL_FOOBAR_XMPL_A_MSG attribute (actual value/payload to be sent)
     // echo the value we just received
     rc = nla_put_string(reply_skb, GNL_QU_NET_A_MSG, recv_msg);
     if (rc != 0) {
       pr_err("An error occurred in %s():\n", __func__);
       return -rc;
     }
     // Finalize the message
     genlmsg_end(reply_skb, msg_head);
     // Send the message block
     rc = genlmsg_reply(reply_skb, info);
     if (rc != 0) {
       pr_err("An error occurred in %s():\n", __func__);
       return -rc;
     }
     return 0;
   }

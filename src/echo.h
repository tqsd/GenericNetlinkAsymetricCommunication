#ifndef ECHO_H_
#define ECHO_H_

typedef struct sk_buff sk_buff;
typedef struct genl_info genl_info;
int gnl_qu_net_echo_doit(struct sk_buff *sender_skb, struct genl_info *info);

#endif // ECHO_H_

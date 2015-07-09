#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/module.h>
#include <linux/kernel.h>

/** 
  * Using the link: http://vger.kernel.org/~davem/skb_data.html
  * for the function definition
  */
  
int init_module(void)
{
	// TODO how much these numbers should be?
	int packet_len = 1400;
	int packet_header_len = sk->sk_prot->max_header;
	int user_data_len = 100;

	printk(KERN_INFO "Hello world 1.\n");
	// create a empty skbuff and allocate header space
	skbuff * skb = alloc_skb(packet_len, GFP_ATOMIC);
	skb_reserve(skb, packet_header_len);

	// Make space to add user data to the packet - no data currently added
	unsigned char *data = skb_put(skb, user_data_len);
	
	// Push UDP header
	struct inet_sock *inet = inet_sk(sk);
	struct flowi *fl = &inet->cork.fl;
	struct udphdr *uh;

	skb->h.raw = skb_push(skb, sizeof(struct udphdr));
	uh = skb->h.uh
	uh->source = fl->fl_ip_sport;
	uh->dest = fl->fl_ip_dport;
	uh->len = htons(user_data_len);
	uh->check = 0;
	skb->csum = csum_partial((char *)uh,
				 sizeof(struct udphdr), skb->csum);
	uh->check = csum_tcpudp_magic(fl->fl4_src, fl->fl4_dst,
				      user_data_len, IPPROTO_UDP, skb->csum);
	if (uh->check == 0)
		uh->check = -1;

	// Push IPV4 header
	struct rtable *rt = inet->cork.rt;
	struct iphdr *iph;

	skb->nh.raw = skb_push(skb, sizeof(struct iphdr));
	iph = skb->nh.iph;
	iph->version = 4;
	iph->ihl = 5;
	iph->tos = inet->tos;
	iph->tot_len = htons(skb->len);
	iph->frag_off = 0;
	iph->id = htons(inet->id++);
	iph->ttl = ip_select_ttl(inet, &rt->u.dst);
	iph->protocol = sk->sk_protocol; /* IPPROTO_UDP in this case */
	iph->saddr = rt->rt_src;
	iph->daddr = rt->rt_dst;
	ip_send_check(iph);

	skb->priority = sk->sk_priority;
	skb->dst = dst_clone(&rt->u.dst);

	dev_queue_xmit(skb);
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye world 1.\n");
}




#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void)
{
	printk(KERN_INFO "Hello world 1.\n");
	struct skbuff * skb = alloc_skb(100, GFP_ATOMIC);
	dev_queue_xmit(skb);
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye world 1.\n");
}




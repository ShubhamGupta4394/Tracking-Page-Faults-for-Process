#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/kprobes.h>

MODULE_DESCRIPTION("Kernel Driver for Detecting Page Fault");
MODULE_AUTHOR("Shubham Gupta <sgupta30@binghamton.edu>");
MODULE_LICENSE("GPL");

// timespec for dispalying time
struct timespec t;

long process_id;
// taking module parameter in process_id
module_param(process_id,long,S_IRUGO);
//handle page fault in process
static long j_handle_mm_fault(struct vm_area_struct *vma, unsigned long address, unsigned int flags)
{
	if((current->pid==process_id))
	{
	getnstimeofday(&t);
	printk("Page fault occurs for process id = %ld, at virtual address = %ld at time= %ld \n",process_id,address,t.tv_nsec);
	}
	jprobe_return();
	return 0;
}
// declaration of jprobe for module
static struct jprobe pagefault_jprobe = {
	.entry	= j_handle_mm_fault,
	.kp = {
		.symbol_name = "handle_mm_fault",
	},
};
// initialize jprobe for module
static int __init jprobe_init(void)
{
	int value;
	// register jprobe
	value = register_jprobe(&pagefault_jprobe);
	if(value)
	{
		pr_err("Kernel module can't be register: \n");
		return value;
	}
	pr_info("Kernel module registered \n");
	return 0;
}
// exit jprobe module
static void __exit jprobe_exit(void)
{
	//unregister jprobe
	 unregister_jprobe(&pagefault_jprobe);
	 pr_info("Module unregister \n");
}

module_init(jprobe_init);
module_exit(jprobe_exit);

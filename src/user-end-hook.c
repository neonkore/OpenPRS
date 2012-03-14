
#include <slistPack.h>
#include <slistPack_f.h>

#include "macro.h"
#include "opaque.h"
#include "user-end-hook-pub.h"

#include "user-end-hook_f.h"
#include "user-end-hook_f-pub.h"


Slist *user_end_kernel_hook_list = NULL;

void add_user_end_kernel_hook(PFV hook)
{
     SAFE_SL_ADD_TO_TAIL( user_end_kernel_hook_list, hook);
}

void execute_user_end_kernel_hook(void)
{
     PFV hook;

     SAFE_SL_LOOP_THROUGH_SLIST(user_end_kernel_hook_list, hook, PFV) {
	  (*hook)();
     }
     SAFE_SL_FREE_SLIST(user_end_kernel_hook_list);
}

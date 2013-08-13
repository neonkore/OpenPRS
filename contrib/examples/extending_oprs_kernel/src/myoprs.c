
#include "action_f.h"
#include "evaluate-function_f.h"
#include "evaluate-predicate_f.h"

/* This is the function which will be called by the loader, with the OpenPRS command:
 * load external "<path>/myoprs" "declare_myoprs"
 */

void declare_myoprs(void)
{
     declare_myprs_eval_pred();
     declare_myprs_eval_funct();
     declare_myprs_action();
}

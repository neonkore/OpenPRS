
#include "action_f.h"
#include "evaluable-function_f.h"
#include "evaluable-predicate_f.h"

/* This is the function which will be called by the loader, with the OpenPRS command:
 * load external "<path>/myoprs" "declare_myoprs"
 */

void declare_myoprs(void)
{
     declare_myoprs_eval_pred();
     declare_myoprs_eval_funct();
     declare_myoprs_action();
}

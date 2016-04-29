#ifndef __c2_newtons_cradle_h__
#define __c2_newtons_cradle_h__

/* Include files */
#include "sfc_sf.h"
#include "sfc_mex.h"
#include "rtwtypes.h"
#include "multiword_types.h"

/* Type Definitions */
#ifndef typedef_SFc2_newtons_cradleInstanceStruct
#define typedef_SFc2_newtons_cradleInstanceStruct

typedef struct {
  SimStruct *S;
  ChartInfoStruct chartInfo;
  uint8_T c2_is_active_c2_newtons_cradle;
  uint8_T c2_is_c2_newtons_cradle;
  uint8_T c2_doSetSimStateSideEffects;
  const mxArray *c2_setSimStateSideEffectsInfo;
} SFc2_newtons_cradleInstanceStruct;

#endif                                 /*typedef_SFc2_newtons_cradleInstanceStruct*/

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
extern const mxArray *sf_c2_newtons_cradle_get_eml_resolved_functions_info(void);

/* Function Definitions */
extern void sf_c2_newtons_cradle_get_check_sum(mxArray *plhs[]);
extern void c2_newtons_cradle_method_dispatcher(SimStruct *S, int_T method, void
  *data);

#endif

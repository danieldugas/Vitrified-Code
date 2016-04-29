#ifndef __c1_newtons_cradle_h__
#define __c1_newtons_cradle_h__

/* Include files */
#include "sfc_sf.h"
#include "sfc_mex.h"
#include "rtwtypes.h"
#include "multiword_types.h"

/* Type Definitions */
#ifndef typedef_SFc1_newtons_cradleInstanceStruct
#define typedef_SFc1_newtons_cradleInstanceStruct

typedef struct {
  SimStruct *S;
  ChartInfoStruct chartInfo;
  real_T c1_lastMajorTime;
  uint8_T c1_is_active_c1_newtons_cradle;
  uint8_T c1_is_c1_newtons_cradle;
  real_T c1_g;
  real_T c1_l;
  real_T c1_pi;
  int32_T c1_k;
  uint8_T c1_doSetSimStateSideEffects;
  const mxArray *c1_setSimStateSideEffectsInfo;
} SFc1_newtons_cradleInstanceStruct;

#endif                                 /*typedef_SFc1_newtons_cradleInstanceStruct*/

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
extern const mxArray *sf_c1_newtons_cradle_get_eml_resolved_functions_info(void);

/* Function Definitions */
extern void sf_c1_newtons_cradle_get_check_sum(mxArray *plhs[]);
extern void c1_newtons_cradle_method_dispatcher(SimStruct *S, int_T method, void
  *data);

#endif

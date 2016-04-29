#ifndef __c1_two_tanks_h__
#define __c1_two_tanks_h__

/* Include files */
#include "sfc_sf.h"
#include "sfc_mex.h"
#include "rtwtypes.h"
#include "multiword_types.h"

/* Type Definitions */
#ifndef typedef_SFc1_two_tanksInstanceStruct
#define typedef_SFc1_two_tanksInstanceStruct

typedef struct {
  SimStruct *S;
  ChartInfoStruct chartInfo;
  real_T c1_lastMajorTime;
  uint8_T c1_is_active_c1_two_tanks;
  uint8_T c1_is_c1_two_tanks;
  real_T c1_w;
  real_T c1_r[2];
  real_T c1_v[2];
  uint8_T c1_doSetSimStateSideEffects;
  const mxArray *c1_setSimStateSideEffectsInfo;
} SFc1_two_tanksInstanceStruct;

#endif                                 /*typedef_SFc1_two_tanksInstanceStruct*/

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
extern const mxArray *sf_c1_two_tanks_get_eml_resolved_functions_info(void);

/* Function Definitions */
extern void sf_c1_two_tanks_get_check_sum(mxArray *plhs[]);
extern void c1_two_tanks_method_dispatcher(SimStruct *S, int_T method, void
  *data);

#endif

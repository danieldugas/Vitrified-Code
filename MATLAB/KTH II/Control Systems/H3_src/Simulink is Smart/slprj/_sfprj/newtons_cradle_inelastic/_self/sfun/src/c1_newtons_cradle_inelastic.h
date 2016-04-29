#ifndef __c1_newtons_cradle_inelastic_h__
#define __c1_newtons_cradle_inelastic_h__

/* Include files */
#include "sfc_sf.h"
#include "sfc_mex.h"
#include "rtwtypes.h"
#include "multiword_types.h"

/* Type Definitions */
#ifndef typedef_SFc1_newtons_cradle_inelasticInstanceStruct
#define typedef_SFc1_newtons_cradle_inelasticInstanceStruct

typedef struct {
  SimStruct *S;
  ChartInfoStruct chartInfo;
  real_T c1_lastMajorTime;
  uint8_T c1_is_active_c1_newtons_cradle_inelastic;
  uint8_T c1_is_c1_newtons_cradle_inelastic;
  real_T c1_g;
  real_T c1_l;
  real_T c1_pi;
  int32_T c1_k;
  uint8_T c1_doSetSimStateSideEffects;
  const mxArray *c1_setSimStateSideEffectsInfo;
} SFc1_newtons_cradle_inelasticInstanceStruct;

#endif                                 /*typedef_SFc1_newtons_cradle_inelasticInstanceStruct*/

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
extern const mxArray
  *sf_c1_newtons_cradle_inelastic_get_eml_resolved_functions_info(void);

/* Function Definitions */
extern void sf_c1_newtons_cradle_inelastic_get_check_sum(mxArray *plhs[]);
extern void c1_newtons_cradle_inelastic_method_dispatcher(SimStruct *S, int_T
  method, void *data);

#endif

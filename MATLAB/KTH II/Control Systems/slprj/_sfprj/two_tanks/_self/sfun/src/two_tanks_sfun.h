#ifndef __two_tanks_sfun_h__
#define __two_tanks_sfun_h__

/* Include files */
#define S_FUNCTION_NAME                sf_sfun
#include "sfc_sf.h"
#include "sfc_mex.h"
#include "rtwtypes.h"
#include "multiword_types.h"
#define rtInf                          (mxGetInf())
#define rtMinusInf                     (-(mxGetInf()))
#define rtNaN                          (mxGetNaN())
#define rtIsNaN(X)                     ((int)mxIsNaN(X))
#define rtIsInf(X)                     ((int)mxIsInf(X))

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */
extern real_T _sfTime_;

/* Variable Definitions */

/* Function Declarations */
extern void two_tanks_initializer(void);
extern void two_tanks_terminator(void);

/* Function Definitions */

/* We load infoStruct for rtw_optimation_info on demand in mdlSetWorkWidths and
   free it immediately in mdlStart. Given that this is machine-wide as
   opposed to chart specific, we use NULL check to make sure it gets loaded
   and unloaded once per machine even though the  methods mdlSetWorkWidths/mdlStart
   are chart/instance specific. The following methods abstract this out. */
extern mxArray* load_two_tanks_optimization_info(void);
extern void unload_two_tanks_optimization_info(void);

#endif

/* Include files */

#include <stddef.h>
#include "blas.h"
#include "sf_newtons_cradle_sfun.h"
#include "c1_sf_newtons_cradle.h"
#include "mwmathutil.h"
#define _SF_MEX_LISTEN_FOR_CTRL_C(S)   sf_mex_listen_for_ctrl_c(NULL,S);

/* Type Definitions */

/* Named Constants */
#define CALL_EVENT                     (-1)
#define c1_IN_NO_ACTIVE_CHILD          ((uint8_T)0U)
#define c1_IN_FreeDynamics             ((uint8_T)1U)
#define c1_const_g                     (9.81)
#define c1_const_l                     (1.0)
#define c1_const_pi                    (3.141)
#define c1_const_k                     (7)

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
static void initialize_c1_sf_newtons_cradle(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance);
static void initialize_params_c1_sf_newtons_cradle
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance);
static void enable_c1_sf_newtons_cradle(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance);
static void disable_c1_sf_newtons_cradle(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance);
static const mxArray *get_sim_state_c1_sf_newtons_cradle
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance);
static void set_sim_state_c1_sf_newtons_cradle
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance, const mxArray *c1_st);
static void c1_set_sim_state_side_effects_c1_sf_newtons_cradle
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance);
static void finalize_c1_sf_newtons_cradle(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance);
static void sf_c1_sf_newtons_cradle(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance);
static void zeroCrossings_c1_sf_newtons_cradle
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance);
static void derivatives_c1_sf_newtons_cradle
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance);
static void outputs_c1_sf_newtons_cradle(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance);
static void initSimStructsc1_sf_newtons_cradle
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance);
static void init_script_number_translation(uint32_T c1_machineNumber, uint32_T
  c1_chartNumber);
static const mxArray *c1_emlrt_marshallOut(SFc1_sf_newtons_cradleInstanceStruct *
  chartInstance);
static const mxArray *c1_b_emlrt_marshallOut
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance, real_T c1_u[7]);
static const mxArray *c1_c_emlrt_marshallOut
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance, uint8_T c1_u);
static void c1_emlrt_marshallIn(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u);
static void c1_b_emlrt_marshallIn(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_p_out, const char_T *c1_identifier, real_T
  c1_y[7]);
static void c1_c_emlrt_marshallIn(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId,
  real_T c1_y[7]);
static uint8_T c1_d_emlrt_marshallIn(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_b_is_active_c1_sf_newtons_cradle, const
  char_T *c1_identifier);
static uint8_T c1_e_emlrt_marshallIn(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId);
static const mxArray *c1_f_emlrt_marshallIn(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_b_setSimStateSideEffectsInfo, const char_T
  *c1_identifier);
static const mxArray *c1_g_emlrt_marshallIn(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId);
static void init_dsm_address_info(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance);

/* Function Definitions */
static void initialize_c1_sf_newtons_cradle(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance)
{
  int32_T c1_i0;
  real_T (*c1_p)[7];
  real_T (*c1_v)[7];
  real_T (*c1_p_out)[7];
  real_T (*c1_v_out)[7];
  c1_v = (real_T (*)[7])(ssGetContStates(chartInstance->S) + 7);
  c1_p = (real_T (*)[7])(ssGetContStates(chartInstance->S) + 0);
  c1_v_out = (real_T (*)[7])ssGetOutputPortSignal(chartInstance->S, 2);
  c1_p_out = (real_T (*)[7])ssGetOutputPortSignal(chartInstance->S, 1);
  _sfTime_ = (real_T)ssGetT(chartInstance->S);
  chartInstance->c1_doSetSimStateSideEffects = 0U;
  chartInstance->c1_setSimStateSideEffectsInfo = NULL;
  chartInstance->c1_is_active_c1_sf_newtons_cradle = 0U;
  chartInstance->c1_is_c1_sf_newtons_cradle = c1_IN_NO_ACTIVE_CHILD;
  for (c1_i0 = 0; c1_i0 < 7; c1_i0++) {
    (*c1_p)[c1_i0] = 0.0;
    (*c1_v)[c1_i0] = 0.0;
  }

  chartInstance->c1_g = 9.81;
  chartInstance->c1_l = 1.0;
  chartInstance->c1_pi = 3.141;
  chartInstance->c1_k = 7;
  if (!(cdrGetOutputPortReusable(chartInstance->S, 1) != 0)) {
    for (c1_i0 = 0; c1_i0 < 7; c1_i0++) {
      (*c1_p_out)[c1_i0] = 0.0;
    }
  }

  if (!(cdrGetOutputPortReusable(chartInstance->S, 2) != 0)) {
    for (c1_i0 = 0; c1_i0 < 7; c1_i0++) {
      (*c1_v_out)[c1_i0] = 0.0;
    }
  }

  chartInstance->c1_is_active_c1_sf_newtons_cradle = 1U;
  for (c1_i0 = 0; c1_i0 < 7; c1_i0++) {
    (*c1_p)[c1_i0] = 0.0;
    (*c1_v)[c1_i0] = 0.0;
  }

  (*c1_p)[0] = 0.392625;
  (*c1_p)[1] = 0.5235;
  chartInstance->c1_is_c1_sf_newtons_cradle = c1_IN_FreeDynamics;
}

static void initialize_params_c1_sf_newtons_cradle
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance)
{
}

static void enable_c1_sf_newtons_cradle(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance)
{
  _sfTime_ = (real_T)ssGetT(chartInstance->S);
}

static void disable_c1_sf_newtons_cradle(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance)
{
  _sfTime_ = (real_T)ssGetT(chartInstance->S);
}

static const mxArray *get_sim_state_c1_sf_newtons_cradle
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance)
{
  const mxArray *c1_st = NULL;
  c1_st = NULL;
  sf_mex_assign(&c1_st, c1_emlrt_marshallOut(chartInstance), FALSE);
  return c1_st;
}

static void set_sim_state_c1_sf_newtons_cradle
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance, const mxArray *c1_st)
{
  c1_emlrt_marshallIn(chartInstance, sf_mex_dup(c1_st));
  chartInstance->c1_doSetSimStateSideEffects = 1U;
  sf_mex_destroy(&c1_st);
}

static void c1_set_sim_state_side_effects_c1_sf_newtons_cradle
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance)
{
  if (chartInstance->c1_doSetSimStateSideEffects != 0) {
    chartInstance->c1_doSetSimStateSideEffects = 0U;
  }
}

static void finalize_c1_sf_newtons_cradle(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance)
{
  sf_mex_destroy(&chartInstance->c1_setSimStateSideEffectsInfo);
}

static void sf_c1_sf_newtons_cradle(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance)
{
  boolean_T c1_stateChanged;
  int32_T c1_i;
  int32_T c1_exitg1;
  real_T c1_tmp;
  real_T (*c1_p)[7];
  real_T (*c1_v)[7];
  real_T (*c1_p_out)[7];
  real_T (*c1_v_out)[7];
  c1_v = (real_T (*)[7])(ssGetContStates(chartInstance->S) + 7);
  c1_p = (real_T (*)[7])(ssGetContStates(chartInstance->S) + 0);
  c1_v_out = (real_T (*)[7])ssGetOutputPortSignal(chartInstance->S, 2);
  c1_p_out = (real_T (*)[7])ssGetOutputPortSignal(chartInstance->S, 1);
  c1_set_sim_state_side_effects_c1_sf_newtons_cradle(chartInstance);
  _sfTime_ = (real_T)ssGetT(chartInstance->S);
  if (ssIsMajorTimeStep(chartInstance->S) != 0) {
    chartInstance->c1_lastMajorTime = _sfTime_;
    c1_stateChanged = (boolean_T)0;
    for (c1_i = 0; c1_i < 7; c1_i++) {
      (*c1_p_out)[c1_i] = 0.0;
      (*c1_v_out)[c1_i] = 0.0;
    }

    c1_i = 0;
    do {
      c1_exitg1 = 0;
      if (c1_i < 6) {
        if ((*c1_p)[c1_i] < (*c1_p)[c1_i + 1]) {
          c1_i = 1;
          c1_exitg1 = 1;
        } else {
          c1_i++;
          _SF_MEX_LISTEN_FOR_CTRL_C(chartInstance->S);
        }
      } else {
        c1_i = 0;
        c1_exitg1 = 1;
      }
    } while (c1_exitg1 == 0);

    if (c1_i != 0) {
      for (c1_i = 0; c1_i < 6; c1_i++) {
        if ((*c1_p)[c1_i] < (*c1_p)[c1_i + 1]) {
          c1_tmp = (*c1_p)[c1_i];
          (*c1_p)[c1_i] = (*c1_p)[c1_i + 1];
          (*c1_p)[c1_i + 1] = c1_tmp;
          c1_tmp = (*c1_v)[c1_i];
          (*c1_v)[c1_i] = (*c1_v)[c1_i + 1];
          (*c1_v)[c1_i + 1] = c1_tmp;
        }

        _SF_MEX_LISTEN_FOR_CTRL_C(chartInstance->S);
      }

      c1_stateChanged = TRUE;
      chartInstance->c1_is_c1_sf_newtons_cradle = c1_IN_FreeDynamics;
    }

    if (c1_stateChanged) {
      ssSetSolverNeedsReset(chartInstance->S);
    }
  }

  _sfTime_ = (real_T)ssGetT(chartInstance->S);
  for (c1_i = 0; c1_i < 7; c1_i++) {
    (*c1_p_out)[c1_i] = (*c1_p)[c1_i];
    (*c1_v_out)[c1_i] = (*c1_v)[c1_i];
  }
}

static void zeroCrossings_c1_sf_newtons_cradle
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance)
{
  boolean_T c1_stateChanged;
  int32_T c1_i;
  int32_T c1_exitg1;
  real_T *c1_zcVar;
  real_T (*c1_p)[7];
  c1_p = (real_T (*)[7])(ssGetContStates(chartInstance->S) + 0);
  c1_zcVar = (real_T *)(ssGetNonsampledZCs(chartInstance->S) + 0);
  _sfTime_ = (real_T)ssGetT(chartInstance->S);
  if (chartInstance->c1_lastMajorTime == _sfTime_) {
    *c1_zcVar = -1.0;
  } else {
    c1_stateChanged = (boolean_T)0;
    c1_i = 0;
    do {
      c1_exitg1 = 0;
      if (c1_i < 6) {
        if ((*c1_p)[c1_i] < (*c1_p)[c1_i + 1]) {
          c1_i = 1;
          c1_exitg1 = 1;
        } else {
          c1_i++;
          _SF_MEX_LISTEN_FOR_CTRL_C(chartInstance->S);
        }
      } else {
        c1_i = 0;
        c1_exitg1 = 1;
      }
    } while (c1_exitg1 == 0);

    if (c1_i != 0) {
      c1_stateChanged = TRUE;
    }

    if (c1_stateChanged) {
      *c1_zcVar = 1.0;
    } else {
      *c1_zcVar = -1.0;
    }
  }
}

static void derivatives_c1_sf_newtons_cradle
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance)
{
  int32_T c1_i1;
  real_T (*c1_p_dot)[7];
  real_T (*c1_v_dot)[7];
  real_T (*c1_v)[7];
  real_T (*c1_p)[7];
  c1_v_dot = (real_T (*)[7])(ssGetdX(chartInstance->S) + 7);
  c1_v = (real_T (*)[7])(ssGetContStates(chartInstance->S) + 7);
  c1_p_dot = (real_T (*)[7])(ssGetdX(chartInstance->S) + 0);
  c1_p = (real_T (*)[7])(ssGetContStates(chartInstance->S) + 0);
  for (c1_i1 = 0; c1_i1 < 7; c1_i1++) {
    (*c1_p_dot)[c1_i1] = 0.0;
    (*c1_v_dot)[c1_i1] = 0.0;
  }

  _sfTime_ = (real_T)ssGetT(chartInstance->S);
  for (c1_i1 = 0; c1_i1 < 7; c1_i1++) {
    (*c1_p_dot)[c1_i1] = (*c1_v)[c1_i1];
    (*c1_v_dot)[c1_i1] = muDoubleScalarSin((*c1_p)[c1_i1]);
    (*c1_v_dot)[c1_i1] *= -9.81;
  }
}

static void outputs_c1_sf_newtons_cradle(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance)
{
  int32_T c1_i2;
  real_T (*c1_p_out)[7];
  real_T (*c1_v_out)[7];
  real_T (*c1_p)[7];
  real_T (*c1_v)[7];
  c1_v = (real_T (*)[7])(ssGetContStates(chartInstance->S) + 7);
  c1_p = (real_T (*)[7])(ssGetContStates(chartInstance->S) + 0);
  c1_v_out = (real_T (*)[7])ssGetOutputPortSignal(chartInstance->S, 2);
  c1_p_out = (real_T (*)[7])ssGetOutputPortSignal(chartInstance->S, 1);
  _sfTime_ = (real_T)ssGetT(chartInstance->S);
  for (c1_i2 = 0; c1_i2 < 7; c1_i2++) {
    (*c1_p_out)[c1_i2] = (*c1_p)[c1_i2];
    (*c1_v_out)[c1_i2] = (*c1_v)[c1_i2];
  }
}

static void initSimStructsc1_sf_newtons_cradle
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance)
{
}

static void init_script_number_translation(uint32_T c1_machineNumber, uint32_T
  c1_chartNumber)
{
}

const mxArray *sf_c1_sf_newtons_cradle_get_eml_resolved_functions_info(void)
{
  const mxArray *c1_nameCaptureInfo = NULL;
  c1_nameCaptureInfo = NULL;
  sf_mex_assign(&c1_nameCaptureInfo, sf_mex_create("nameCaptureInfo", NULL, 0,
    0U, 1U, 0U, 2, 0, 1), FALSE);
  return c1_nameCaptureInfo;
}

static const mxArray *c1_emlrt_marshallOut(SFc1_sf_newtons_cradleInstanceStruct *
  chartInstance)
{
  const mxArray *c1_y;
  int32_T c1_i3;
  real_T c1_dv0[7];
  real_T (*c1_v)[7];
  real_T (*c1_p)[7];
  real_T (*c1_v_out)[7];
  real_T (*c1_p_out)[7];
  c1_v = (real_T (*)[7])(ssGetContStates(chartInstance->S) + 7);
  c1_p = (real_T (*)[7])(ssGetContStates(chartInstance->S) + 0);
  c1_v_out = (real_T (*)[7])ssGetOutputPortSignal(chartInstance->S, 2);
  c1_p_out = (real_T (*)[7])ssGetOutputPortSignal(chartInstance->S, 1);
  c1_y = NULL;
  sf_mex_assign(&c1_y, sf_mex_createcellarray(6), FALSE);
  for (c1_i3 = 0; c1_i3 < 7; c1_i3++) {
    c1_dv0[c1_i3] = (*c1_p_out)[c1_i3];
  }

  sf_mex_setcell(c1_y, 0, c1_b_emlrt_marshallOut(chartInstance, c1_dv0));
  for (c1_i3 = 0; c1_i3 < 7; c1_i3++) {
    c1_dv0[c1_i3] = (*c1_v_out)[c1_i3];
  }

  sf_mex_setcell(c1_y, 1, c1_b_emlrt_marshallOut(chartInstance, c1_dv0));
  for (c1_i3 = 0; c1_i3 < 7; c1_i3++) {
    c1_dv0[c1_i3] = (*c1_p)[c1_i3];
  }

  sf_mex_setcell(c1_y, 2, c1_b_emlrt_marshallOut(chartInstance, c1_dv0));
  for (c1_i3 = 0; c1_i3 < 7; c1_i3++) {
    c1_dv0[c1_i3] = (*c1_v)[c1_i3];
  }

  sf_mex_setcell(c1_y, 3, c1_b_emlrt_marshallOut(chartInstance, c1_dv0));
  sf_mex_setcell(c1_y, 4, c1_c_emlrt_marshallOut(chartInstance,
    chartInstance->c1_is_active_c1_sf_newtons_cradle));
  sf_mex_setcell(c1_y, 5, c1_c_emlrt_marshallOut(chartInstance,
    chartInstance->c1_is_c1_sf_newtons_cradle));
  return c1_y;
}

static const mxArray *c1_b_emlrt_marshallOut
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance, real_T c1_u[7])
{
  const mxArray *c1_y = NULL;
  c1_y = NULL;
  sf_mex_assign(&c1_y, sf_mex_create("y", c1_u, 0, 0U, 1U, 0U, 1, 7), FALSE);
  return c1_y;
}

static const mxArray *c1_c_emlrt_marshallOut
  (SFc1_sf_newtons_cradleInstanceStruct *chartInstance, uint8_T c1_u)
{
  const mxArray *c1_y = NULL;
  c1_y = NULL;
  sf_mex_assign(&c1_y, sf_mex_create("y", &c1_u, 3, 0U, 0U, 0U, 0), FALSE);
  return c1_y;
}

static void c1_emlrt_marshallIn(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u)
{
  real_T (*c1_p_out)[7];
  real_T (*c1_v_out)[7];
  real_T (*c1_p)[7];
  real_T (*c1_v)[7];
  c1_v = (real_T (*)[7])(ssGetContStates(chartInstance->S) + 7);
  c1_p = (real_T (*)[7])(ssGetContStates(chartInstance->S) + 0);
  c1_v_out = (real_T (*)[7])ssGetOutputPortSignal(chartInstance->S, 2);
  c1_p_out = (real_T (*)[7])ssGetOutputPortSignal(chartInstance->S, 1);
  c1_b_emlrt_marshallIn(chartInstance, sf_mex_dup(sf_mex_getcell(c1_u, 0)),
                        "p_out", *c1_p_out);
  c1_b_emlrt_marshallIn(chartInstance, sf_mex_dup(sf_mex_getcell(c1_u, 1)),
                        "v_out", *c1_v_out);
  c1_b_emlrt_marshallIn(chartInstance, sf_mex_dup(sf_mex_getcell(c1_u, 2)), "p",
                        *c1_p);
  c1_b_emlrt_marshallIn(chartInstance, sf_mex_dup(sf_mex_getcell(c1_u, 3)), "v",
                        *c1_v);
  chartInstance->c1_is_active_c1_sf_newtons_cradle = c1_d_emlrt_marshallIn
    (chartInstance, sf_mex_dup(sf_mex_getcell(c1_u, 4)),
     "is_active_c1_sf_newtons_cradle");
  chartInstance->c1_is_c1_sf_newtons_cradle = c1_d_emlrt_marshallIn
    (chartInstance, sf_mex_dup(sf_mex_getcell(c1_u, 5)),
     "is_c1_sf_newtons_cradle");
  sf_mex_assign(&chartInstance->c1_setSimStateSideEffectsInfo,
                c1_f_emlrt_marshallIn(chartInstance, sf_mex_dup(sf_mex_getcell
    (c1_u, 6)), "setSimStateSideEffectsInfo"), TRUE);
  sf_mex_destroy(&c1_u);
}

static void c1_b_emlrt_marshallIn(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_p_out, const char_T *c1_identifier, real_T
  c1_y[7])
{
  emlrtMsgIdentifier c1_thisId;
  c1_thisId.fIdentifier = c1_identifier;
  c1_thisId.fParent = NULL;
  c1_c_emlrt_marshallIn(chartInstance, sf_mex_dup(c1_p_out), &c1_thisId, c1_y);
  sf_mex_destroy(&c1_p_out);
}

static void c1_c_emlrt_marshallIn(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId,
  real_T c1_y[7])
{
  real_T c1_dv1[7];
  int32_T c1_i4;
  sf_mex_import(c1_parentId, sf_mex_dup(c1_u), c1_dv1, 1, 0, 0U, 1, 0U, 1, 7);
  for (c1_i4 = 0; c1_i4 < 7; c1_i4++) {
    c1_y[c1_i4] = c1_dv1[c1_i4];
  }

  sf_mex_destroy(&c1_u);
}

static uint8_T c1_d_emlrt_marshallIn(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_b_is_active_c1_sf_newtons_cradle, const
  char_T *c1_identifier)
{
  uint8_T c1_y;
  emlrtMsgIdentifier c1_thisId;
  c1_thisId.fIdentifier = c1_identifier;
  c1_thisId.fParent = NULL;
  c1_y = c1_e_emlrt_marshallIn(chartInstance, sf_mex_dup
    (c1_b_is_active_c1_sf_newtons_cradle), &c1_thisId);
  sf_mex_destroy(&c1_b_is_active_c1_sf_newtons_cradle);
  return c1_y;
}

static uint8_T c1_e_emlrt_marshallIn(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId)
{
  uint8_T c1_y;
  uint8_T c1_u0;
  sf_mex_import(c1_parentId, sf_mex_dup(c1_u), &c1_u0, 1, 3, 0U, 0, 0U, 0);
  c1_y = c1_u0;
  sf_mex_destroy(&c1_u);
  return c1_y;
}

static const mxArray *c1_f_emlrt_marshallIn(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_b_setSimStateSideEffectsInfo, const char_T
  *c1_identifier)
{
  const mxArray *c1_y = NULL;
  emlrtMsgIdentifier c1_thisId;
  c1_y = NULL;
  c1_thisId.fIdentifier = c1_identifier;
  c1_thisId.fParent = NULL;
  sf_mex_assign(&c1_y, c1_g_emlrt_marshallIn(chartInstance, sf_mex_dup
    (c1_b_setSimStateSideEffectsInfo), &c1_thisId), FALSE);
  sf_mex_destroy(&c1_b_setSimStateSideEffectsInfo);
  return c1_y;
}

static const mxArray *c1_g_emlrt_marshallIn(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId)
{
  const mxArray *c1_y = NULL;
  c1_y = NULL;
  sf_mex_assign(&c1_y, sf_mex_duplicatearraysafe(&c1_u), FALSE);
  sf_mex_destroy(&c1_u);
  return c1_y;
}

static void init_dsm_address_info(SFc1_sf_newtons_cradleInstanceStruct
  *chartInstance)
{
}

/* SFunction Glue Code */
#ifdef utFree
#undef utFree
#endif

#ifdef utMalloc
#undef utMalloc
#endif

#ifdef __cplusplus

extern "C" void *utMalloc(size_t size);
extern "C" void utFree(void*);

#else

extern void *utMalloc(size_t size);
extern void utFree(void*);

#endif

void sf_c1_sf_newtons_cradle_get_check_sum(mxArray *plhs[])
{
  ((real_T *)mxGetPr((plhs[0])))[0] = (real_T)(4220336165U);
  ((real_T *)mxGetPr((plhs[0])))[1] = (real_T)(2137532985U);
  ((real_T *)mxGetPr((plhs[0])))[2] = (real_T)(3069547118U);
  ((real_T *)mxGetPr((plhs[0])))[3] = (real_T)(761769925U);
}

mxArray *sf_c1_sf_newtons_cradle_get_autoinheritance_info(void)
{
  const char *autoinheritanceFields[] = { "checksum", "inputs", "parameters",
    "outputs", "locals" };

  mxArray *mxAutoinheritanceInfo = mxCreateStructMatrix(1,1,5,
    autoinheritanceFields);

  {
    mxArray *mxChecksum = mxCreateString("ZV9gMwlTBd4WBeAHKG7g1F");
    mxSetField(mxAutoinheritanceInfo,0,"checksum",mxChecksum);
  }

  {
    mxSetField(mxAutoinheritanceInfo,0,"inputs",mxCreateDoubleMatrix(0,0,mxREAL));
  }

  {
    mxSetField(mxAutoinheritanceInfo,0,"parameters",mxCreateDoubleMatrix(0,0,
                mxREAL));
  }

  {
    const char *dataFields[] = { "size", "type", "complexity" };

    mxArray *mxData = mxCreateStructMatrix(1,2,3,dataFields);

    {
      mxArray *mxSize = mxCreateDoubleMatrix(1,2,mxREAL);
      double *pr = mxGetPr(mxSize);
      pr[0] = (double)(7);
      pr[1] = (double)(1);
      mxSetField(mxData,0,"size",mxSize);
    }

    {
      const char *typeFields[] = { "base", "fixpt" };

      mxArray *mxType = mxCreateStructMatrix(1,1,2,typeFields);
      mxSetField(mxType,0,"base",mxCreateDoubleScalar(10));
      mxSetField(mxType,0,"fixpt",mxCreateDoubleMatrix(0,0,mxREAL));
      mxSetField(mxData,0,"type",mxType);
    }

    mxSetField(mxData,0,"complexity",mxCreateDoubleScalar(0));

    {
      mxArray *mxSize = mxCreateDoubleMatrix(1,2,mxREAL);
      double *pr = mxGetPr(mxSize);
      pr[0] = (double)(7);
      pr[1] = (double)(1);
      mxSetField(mxData,1,"size",mxSize);
    }

    {
      const char *typeFields[] = { "base", "fixpt" };

      mxArray *mxType = mxCreateStructMatrix(1,1,2,typeFields);
      mxSetField(mxType,0,"base",mxCreateDoubleScalar(10));
      mxSetField(mxType,0,"fixpt",mxCreateDoubleMatrix(0,0,mxREAL));
      mxSetField(mxData,1,"type",mxType);
    }

    mxSetField(mxData,1,"complexity",mxCreateDoubleScalar(0));
    mxSetField(mxAutoinheritanceInfo,0,"outputs",mxData);
  }

  {
    mxSetField(mxAutoinheritanceInfo,0,"locals",mxCreateDoubleMatrix(0,0,mxREAL));
  }

  return(mxAutoinheritanceInfo);
}

mxArray *sf_c1_sf_newtons_cradle_third_party_uses_info(void)
{
  mxArray * mxcell3p = mxCreateCellMatrix(1,0);
  return(mxcell3p);
}

mxArray *sf_c1_sf_newtons_cradle_updateBuildInfo_args_info(void)
{
  mxArray *mxBIArgs = mxCreateCellMatrix(1,0);
  return mxBIArgs;
}

static const mxArray *sf_get_sim_state_info_c1_sf_newtons_cradle(void)
{
  const char *infoFields[] = { "chartChecksum", "varInfo" };

  mxArray *mxInfo = mxCreateStructMatrix(1, 1, 2, infoFields);
  const char *infoEncStr[] = {
    "100 S1x6'type','srcId','name','auxInfo'{{M[1],M[25],T\"p_out\",},{M[1],M[26],T\"v_out\",},{M[5],M[30],T\"p\",},{M[5],M[31],T\"v\",},{M[8],M[0],T\"is_active_c1_sf_newtons_cradle\",},{M[9],M[0],T\"is_c1_sf_newtons_cradle\",}}"
  };

  mxArray *mxVarInfo = sf_mex_decode_encoded_mx_struct_array(infoEncStr, 6, 10);
  mxArray *mxChecksum = mxCreateDoubleMatrix(1, 4, mxREAL);
  sf_c1_sf_newtons_cradle_get_check_sum(&mxChecksum);
  mxSetField(mxInfo, 0, infoFields[0], mxChecksum);
  mxSetField(mxInfo, 0, infoFields[1], mxVarInfo);
  return mxInfo;
}

static const char* sf_get_instance_specialization(void)
{
  return "Ch4cQizinjuL10eY75IJwG";
}

static void sf_opaque_initialize_c1_sf_newtons_cradle(void *chartInstanceVar)
{
  initialize_params_c1_sf_newtons_cradle((SFc1_sf_newtons_cradleInstanceStruct*)
    chartInstanceVar);
  initialize_c1_sf_newtons_cradle((SFc1_sf_newtons_cradleInstanceStruct*)
    chartInstanceVar);
}

static void sf_opaque_enable_c1_sf_newtons_cradle(void *chartInstanceVar)
{
  enable_c1_sf_newtons_cradle((SFc1_sf_newtons_cradleInstanceStruct*)
    chartInstanceVar);
}

static void sf_opaque_disable_c1_sf_newtons_cradle(void *chartInstanceVar)
{
  disable_c1_sf_newtons_cradle((SFc1_sf_newtons_cradleInstanceStruct*)
    chartInstanceVar);
}

static void sf_opaque_zeroCrossings_c1_sf_newtons_cradle(void *chartInstanceVar)
{
  zeroCrossings_c1_sf_newtons_cradle((SFc1_sf_newtons_cradleInstanceStruct*)
    chartInstanceVar);
}

static void sf_opaque_outputs_c1_sf_newtons_cradle(void *chartInstanceVar)
{
  outputs_c1_sf_newtons_cradle((SFc1_sf_newtons_cradleInstanceStruct*)
    chartInstanceVar);
}

static void sf_opaque_derivatives_c1_sf_newtons_cradle(void *chartInstanceVar)
{
  derivatives_c1_sf_newtons_cradle((SFc1_sf_newtons_cradleInstanceStruct*)
    chartInstanceVar);
}

static void sf_opaque_gateway_c1_sf_newtons_cradle(void *chartInstanceVar)
{
  sf_c1_sf_newtons_cradle((SFc1_sf_newtons_cradleInstanceStruct*)
    chartInstanceVar);
}

extern const mxArray* sf_internal_get_sim_state_c1_sf_newtons_cradle(SimStruct*
  S)
{
  ChartInfoStruct *chartInfo = (ChartInfoStruct*) ssGetUserData(S);
  mxArray *plhs[1] = { NULL };

  mxArray *prhs[4];
  int mxError = 0;
  prhs[0] = mxCreateString("chart_simctx_raw2high");
  prhs[1] = mxCreateDoubleScalar(ssGetSFuncBlockHandle(S));
  prhs[2] = (mxArray*) get_sim_state_c1_sf_newtons_cradle
    ((SFc1_sf_newtons_cradleInstanceStruct*)chartInfo->chartInstance);/* raw sim ctx */
  prhs[3] = (mxArray*) sf_get_sim_state_info_c1_sf_newtons_cradle();/* state var info */
  mxError = sf_mex_call_matlab(1, plhs, 4, prhs, "sfprivate");
  mxDestroyArray(prhs[0]);
  mxDestroyArray(prhs[1]);
  mxDestroyArray(prhs[2]);
  mxDestroyArray(prhs[3]);
  if (mxError || plhs[0] == NULL) {
    sf_mex_error_message("Stateflow Internal Error: \nError calling 'chart_simctx_raw2high'.\n");
  }

  return plhs[0];
}

extern void sf_internal_set_sim_state_c1_sf_newtons_cradle(SimStruct* S, const
  mxArray *st)
{
  ChartInfoStruct *chartInfo = (ChartInfoStruct*) ssGetUserData(S);
  mxArray *plhs[1] = { NULL };

  mxArray *prhs[4];
  int mxError = 0;
  prhs[0] = mxCreateString("chart_simctx_high2raw");
  prhs[1] = mxCreateDoubleScalar(ssGetSFuncBlockHandle(S));
  prhs[2] = mxDuplicateArray(st);      /* high level simctx */
  prhs[3] = (mxArray*) sf_get_sim_state_info_c1_sf_newtons_cradle();/* state var info */
  mxError = sf_mex_call_matlab(1, plhs, 4, prhs, "sfprivate");
  mxDestroyArray(prhs[0]);
  mxDestroyArray(prhs[1]);
  mxDestroyArray(prhs[2]);
  mxDestroyArray(prhs[3]);
  if (mxError || plhs[0] == NULL) {
    sf_mex_error_message("Stateflow Internal Error: \nError calling 'chart_simctx_high2raw'.\n");
  }

  set_sim_state_c1_sf_newtons_cradle((SFc1_sf_newtons_cradleInstanceStruct*)
    chartInfo->chartInstance, mxDuplicateArray(plhs[0]));
  mxDestroyArray(plhs[0]);
}

static const mxArray* sf_opaque_get_sim_state_c1_sf_newtons_cradle(SimStruct* S)
{
  return sf_internal_get_sim_state_c1_sf_newtons_cradle(S);
}

static void sf_opaque_set_sim_state_c1_sf_newtons_cradle(SimStruct* S, const
  mxArray *st)
{
  sf_internal_set_sim_state_c1_sf_newtons_cradle(S, st);
}

static void sf_opaque_terminate_c1_sf_newtons_cradle(void *chartInstanceVar)
{
  if (chartInstanceVar!=NULL) {
    SimStruct *S = ((SFc1_sf_newtons_cradleInstanceStruct*) chartInstanceVar)->S;
    if (sim_mode_is_rtw_gen(S) || sim_mode_is_external(S)) {
      sf_clear_rtw_identifier(S);
      unload_sf_newtons_cradle_optimization_info();
    }

    finalize_c1_sf_newtons_cradle((SFc1_sf_newtons_cradleInstanceStruct*)
      chartInstanceVar);
    utFree((void *)chartInstanceVar);
    ssSetUserData(S,NULL);
  }
}

static void sf_opaque_init_subchart_simstructs(void *chartInstanceVar)
{
  initSimStructsc1_sf_newtons_cradle((SFc1_sf_newtons_cradleInstanceStruct*)
    chartInstanceVar);
}

extern unsigned int sf_machine_global_initializer_called(void);
static void mdlProcessParameters_c1_sf_newtons_cradle(SimStruct *S)
{
  int i;
  for (i=0;i<ssGetNumRunTimeParams(S);i++) {
    if (ssGetSFcnParamTunable(S,i)) {
      ssUpdateDlgParamAsRunTimeParam(S,i);
    }
  }

  if (sf_machine_global_initializer_called()) {
    initialize_params_c1_sf_newtons_cradle((SFc1_sf_newtons_cradleInstanceStruct*)
      (((ChartInfoStruct *)ssGetUserData(S))->chartInstance));
  }
}

static void mdlSetWorkWidths_c1_sf_newtons_cradle(SimStruct *S)
{
  if (sim_mode_is_rtw_gen(S) || sim_mode_is_external(S)) {
    mxArray *infoStruct = load_sf_newtons_cradle_optimization_info();
    int_T chartIsInlinable =
      (int_T)sf_is_chart_inlinable(S,sf_get_instance_specialization(),infoStruct,
      1);
    ssSetStateflowIsInlinable(S,chartIsInlinable);
    ssSetRTWCG(S,sf_rtw_info_uint_prop(S,sf_get_instance_specialization(),
                infoStruct,1,"RTWCG"));
    ssSetEnableFcnIsTrivial(S,1);
    ssSetDisableFcnIsTrivial(S,1);
    ssSetNotMultipleInlinable(S,sf_rtw_info_uint_prop(S,
      sf_get_instance_specialization(),infoStruct,1,
      "gatewayCannotBeInlinedMultipleTimes"));
    sf_update_buildInfo(S,sf_get_instance_specialization(),infoStruct,1);
    if (chartIsInlinable) {
      sf_mark_chart_reusable_outputs(S,sf_get_instance_specialization(),
        infoStruct,1,2);
    }

    {
      unsigned int outPortIdx;
      for (outPortIdx=1; outPortIdx<=2; ++outPortIdx) {
        ssSetOutputPortOptimizeInIR(S, outPortIdx, 1U);
      }
    }

    sf_set_rtw_dwork_info(S,sf_get_instance_specialization(),infoStruct,1);
    ssSetHasSubFunctions(S,!(chartIsInlinable));
    ssSetCallsOutputInInitFcn(S,1);
  } else {
  }

  ssSetOptions(S,ssGetOptions(S)|SS_OPTION_WORKS_WITH_CODE_REUSE);
  ssSetChecksum0(S,(512308093U));
  ssSetChecksum1(S,(3587888106U));
  ssSetChecksum2(S,(1225373360U));
  ssSetChecksum3(S,(2394250532U));
  ssSetNumContStates(S,14);
  ssSetExplicitFCSSCtrl(S,1);
  ssSupportsMultipleExecInstances(S,1);
}

static void mdlRTW_c1_sf_newtons_cradle(SimStruct *S)
{
  if (sim_mode_is_rtw_gen(S)) {
    ssWriteRTWStrParam(S, "StateflowChartType", "Stateflow");
  }
}

static void mdlStart_c1_sf_newtons_cradle(SimStruct *S)
{
  SFc1_sf_newtons_cradleInstanceStruct *chartInstance;
  chartInstance = (SFc1_sf_newtons_cradleInstanceStruct *)utMalloc(sizeof
    (SFc1_sf_newtons_cradleInstanceStruct));
  memset(chartInstance, 0, sizeof(SFc1_sf_newtons_cradleInstanceStruct));
  if (chartInstance==NULL) {
    sf_mex_error_message("Could not allocate memory for chart instance.");
  }

  chartInstance->chartInfo.chartInstance = chartInstance;
  chartInstance->chartInfo.isEMLChart = 0;
  chartInstance->chartInfo.chartInitialized = 0;
  chartInstance->chartInfo.sFunctionGateway =
    sf_opaque_gateway_c1_sf_newtons_cradle;
  chartInstance->chartInfo.initializeChart =
    sf_opaque_initialize_c1_sf_newtons_cradle;
  chartInstance->chartInfo.terminateChart =
    sf_opaque_terminate_c1_sf_newtons_cradle;
  chartInstance->chartInfo.enableChart = sf_opaque_enable_c1_sf_newtons_cradle;
  chartInstance->chartInfo.disableChart = sf_opaque_disable_c1_sf_newtons_cradle;
  chartInstance->chartInfo.getSimState =
    sf_opaque_get_sim_state_c1_sf_newtons_cradle;
  chartInstance->chartInfo.setSimState =
    sf_opaque_set_sim_state_c1_sf_newtons_cradle;
  chartInstance->chartInfo.getSimStateInfo =
    sf_get_sim_state_info_c1_sf_newtons_cradle;
  chartInstance->chartInfo.zeroCrossings =
    sf_opaque_zeroCrossings_c1_sf_newtons_cradle;
  chartInstance->chartInfo.outputs = sf_opaque_outputs_c1_sf_newtons_cradle;
  chartInstance->chartInfo.derivatives =
    sf_opaque_derivatives_c1_sf_newtons_cradle;
  chartInstance->chartInfo.mdlRTW = mdlRTW_c1_sf_newtons_cradle;
  chartInstance->chartInfo.mdlStart = mdlStart_c1_sf_newtons_cradle;
  chartInstance->chartInfo.mdlSetWorkWidths =
    mdlSetWorkWidths_c1_sf_newtons_cradle;
  chartInstance->chartInfo.extModeExec = NULL;
  chartInstance->chartInfo.restoreLastMajorStepConfiguration = NULL;
  chartInstance->chartInfo.restoreBeforeLastMajorStepConfiguration = NULL;
  chartInstance->chartInfo.storeCurrentConfiguration = NULL;
  chartInstance->S = S;
  ssSetUserData(S,(void *)(&(chartInstance->chartInfo)));/* register the chart instance with simstruct */
  init_dsm_address_info(chartInstance);
  if (!sim_mode_is_rtw_gen(S)) {
  }

  sf_opaque_init_subchart_simstructs(chartInstance->chartInfo.chartInstance);
}

void c1_sf_newtons_cradle_method_dispatcher(SimStruct *S, int_T method, void
  *data)
{
  switch (method) {
   case SS_CALL_MDL_START:
    mdlStart_c1_sf_newtons_cradle(S);
    break;

   case SS_CALL_MDL_SET_WORK_WIDTHS:
    mdlSetWorkWidths_c1_sf_newtons_cradle(S);
    break;

   case SS_CALL_MDL_PROCESS_PARAMETERS:
    mdlProcessParameters_c1_sf_newtons_cradle(S);
    break;

   default:
    /* Unhandled method */
    sf_mex_error_message("Stateflow Internal Error:\n"
                         "Error calling c1_sf_newtons_cradle_method_dispatcher.\n"
                         "Can't handle method %d.\n", method);
    break;
  }
}

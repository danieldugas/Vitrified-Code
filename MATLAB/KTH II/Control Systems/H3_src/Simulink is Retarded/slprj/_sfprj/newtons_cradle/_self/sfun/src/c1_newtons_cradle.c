/* Include files */

#include <stddef.h>
#include "blas.h"
#include "newtons_cradle_sfun.h"
#include "c1_newtons_cradle.h"
#define CHARTINSTANCE_CHARTNUMBER      (chartInstance->chartNumber)
#define CHARTINSTANCE_INSTANCENUMBER   (chartInstance->instanceNumber)
#include "newtons_cradle_sfun_debug_macros.h"
#define _SF_MEX_LISTEN_FOR_CTRL_C(S)   sf_mex_listen_for_ctrl_c(sfGlobalDebugInstanceStruct,S);

/* Type Definitions */

/* Named Constants */
#define CALL_EVENT                     (-1)

/* Variable Declarations */

/* Variable Definitions */
static const char * c1_debug_family_names[11] = { "x_in_guard12", "x_in_guard21",
  "x_in_guard23", "x_in_guard32", "error", "nargin", "nargout", "xi", "qi", "xo",
  "qo" };

/* Function Declarations */
static void initialize_c1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance);
static void initialize_params_c1_newtons_cradle
  (SFc1_newtons_cradleInstanceStruct *chartInstance);
static void enable_c1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance);
static void disable_c1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance);
static void c1_update_debugger_state_c1_newtons_cradle
  (SFc1_newtons_cradleInstanceStruct *chartInstance);
static const mxArray *get_sim_state_c1_newtons_cradle
  (SFc1_newtons_cradleInstanceStruct *chartInstance);
static void set_sim_state_c1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_st);
static void finalize_c1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance);
static void sf_c1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance);
static void c1_chartstep_c1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance);
static void initSimStructsc1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance);
static void init_script_number_translation(uint32_T c1_machineNumber, uint32_T
  c1_chartNumber);
static const mxArray *c1_sf_marshallOut(void *chartInstanceVoid, void *c1_inData);
static real_T c1_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_qo, const char_T *c1_identifier);
static real_T c1_b_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId);
static void c1_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData);
static const mxArray *c1_b_sf_marshallOut(void *chartInstanceVoid, void
  *c1_inData);
static void c1_c_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_xo, const char_T *c1_identifier, real_T
  c1_y[6]);
static void c1_d_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId,
  real_T c1_y[6]);
static void c1_b_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData);
static const mxArray *c1_c_sf_marshallOut(void *chartInstanceVoid, void
  *c1_inData);
static void c1_e_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId,
  char_T c1_y[39]);
static void c1_c_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData);
static const mxArray *c1_d_sf_marshallOut(void *chartInstanceVoid, void
  *c1_inData);
static boolean_T c1_f_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId);
static void c1_d_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData);
static const mxArray *c1_e_sf_marshallOut(void *chartInstanceVoid, void
  *c1_inData);
static int32_T c1_g_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId);
static void c1_e_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData);
static uint8_T c1_h_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_b_is_active_c1_newtons_cradle, const char_T *
  c1_identifier);
static uint8_T c1_i_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId);
static void init_dsm_address_info(SFc1_newtons_cradleInstanceStruct
  *chartInstance);

/* Function Definitions */
static void initialize_c1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance)
{
  chartInstance->c1_sfEvent = CALL_EVENT;
  _sfTime_ = (real_T)ssGetT(chartInstance->S);
  chartInstance->c1_is_active_c1_newtons_cradle = 0U;
}

static void initialize_params_c1_newtons_cradle
  (SFc1_newtons_cradleInstanceStruct *chartInstance)
{
}

static void enable_c1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance)
{
  _sfTime_ = (real_T)ssGetT(chartInstance->S);
}

static void disable_c1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance)
{
  _sfTime_ = (real_T)ssGetT(chartInstance->S);
}

static void c1_update_debugger_state_c1_newtons_cradle
  (SFc1_newtons_cradleInstanceStruct *chartInstance)
{
}

static const mxArray *get_sim_state_c1_newtons_cradle
  (SFc1_newtons_cradleInstanceStruct *chartInstance)
{
  const mxArray *c1_st;
  const mxArray *c1_y = NULL;
  real_T c1_hoistedGlobal;
  real_T c1_u;
  const mxArray *c1_b_y = NULL;
  int32_T c1_i0;
  real_T c1_b_u[6];
  const mxArray *c1_c_y = NULL;
  uint8_T c1_b_hoistedGlobal;
  uint8_T c1_c_u;
  const mxArray *c1_d_y = NULL;
  real_T *c1_qo;
  real_T (*c1_xo)[6];
  c1_qo = (real_T *)ssGetOutputPortSignal(chartInstance->S, 2);
  c1_xo = (real_T (*)[6])ssGetOutputPortSignal(chartInstance->S, 1);
  c1_st = NULL;
  c1_st = NULL;
  c1_y = NULL;
  sf_mex_assign(&c1_y, sf_mex_createcellarray(3), FALSE);
  c1_hoistedGlobal = *c1_qo;
  c1_u = c1_hoistedGlobal;
  c1_b_y = NULL;
  sf_mex_assign(&c1_b_y, sf_mex_create("y", &c1_u, 0, 0U, 0U, 0U, 0), FALSE);
  sf_mex_setcell(c1_y, 0, c1_b_y);
  for (c1_i0 = 0; c1_i0 < 6; c1_i0++) {
    c1_b_u[c1_i0] = (*c1_xo)[c1_i0];
  }

  c1_c_y = NULL;
  sf_mex_assign(&c1_c_y, sf_mex_create("y", c1_b_u, 0, 0U, 1U, 0U, 1, 6), FALSE);
  sf_mex_setcell(c1_y, 1, c1_c_y);
  c1_b_hoistedGlobal = chartInstance->c1_is_active_c1_newtons_cradle;
  c1_c_u = c1_b_hoistedGlobal;
  c1_d_y = NULL;
  sf_mex_assign(&c1_d_y, sf_mex_create("y", &c1_c_u, 3, 0U, 0U, 0U, 0), FALSE);
  sf_mex_setcell(c1_y, 2, c1_d_y);
  sf_mex_assign(&c1_st, c1_y, FALSE);
  return c1_st;
}

static void set_sim_state_c1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_st)
{
  const mxArray *c1_u;
  real_T c1_dv0[6];
  int32_T c1_i1;
  real_T *c1_qo;
  real_T (*c1_xo)[6];
  c1_qo = (real_T *)ssGetOutputPortSignal(chartInstance->S, 2);
  c1_xo = (real_T (*)[6])ssGetOutputPortSignal(chartInstance->S, 1);
  chartInstance->c1_doneDoubleBufferReInit = TRUE;
  c1_u = sf_mex_dup(c1_st);
  *c1_qo = c1_emlrt_marshallIn(chartInstance, sf_mex_dup(sf_mex_getcell(c1_u, 0)),
    "qo");
  c1_c_emlrt_marshallIn(chartInstance, sf_mex_dup(sf_mex_getcell(c1_u, 1)), "xo",
                        c1_dv0);
  for (c1_i1 = 0; c1_i1 < 6; c1_i1++) {
    (*c1_xo)[c1_i1] = c1_dv0[c1_i1];
  }

  chartInstance->c1_is_active_c1_newtons_cradle = c1_h_emlrt_marshallIn
    (chartInstance, sf_mex_dup(sf_mex_getcell(c1_u, 2)),
     "is_active_c1_newtons_cradle");
  sf_mex_destroy(&c1_u);
  c1_update_debugger_state_c1_newtons_cradle(chartInstance);
  sf_mex_destroy(&c1_st);
}

static void finalize_c1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance)
{
}

static void sf_c1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance)
{
  int32_T c1_i2;
  int32_T c1_i3;
  real_T *c1_qi;
  real_T *c1_qo;
  real_T (*c1_xo)[6];
  real_T (*c1_xi)[6];
  c1_qo = (real_T *)ssGetOutputPortSignal(chartInstance->S, 2);
  c1_qi = (real_T *)ssGetInputPortSignal(chartInstance->S, 1);
  c1_xo = (real_T (*)[6])ssGetOutputPortSignal(chartInstance->S, 1);
  c1_xi = (real_T (*)[6])ssGetInputPortSignal(chartInstance->S, 0);
  _SFD_SYMBOL_SCOPE_PUSH(0U, 0U);
  _sfTime_ = (real_T)ssGetT(chartInstance->S);
  _SFD_CC_CALL(CHART_ENTER_SFUNCTION_TAG, 0U, chartInstance->c1_sfEvent);
  for (c1_i2 = 0; c1_i2 < 6; c1_i2++) {
    _SFD_DATA_RANGE_CHECK((*c1_xi)[c1_i2], 0U);
  }

  for (c1_i3 = 0; c1_i3 < 6; c1_i3++) {
    _SFD_DATA_RANGE_CHECK((*c1_xo)[c1_i3], 1U);
  }

  _SFD_DATA_RANGE_CHECK(*c1_qi, 2U);
  _SFD_DATA_RANGE_CHECK(*c1_qo, 3U);
  chartInstance->c1_sfEvent = CALL_EVENT;
  c1_chartstep_c1_newtons_cradle(chartInstance);
  _SFD_SYMBOL_SCOPE_POP();
  _SFD_CHECK_FOR_STATE_INCONSISTENCY(_newtons_cradleMachineNumber_,
    chartInstance->chartNumber, chartInstance->instanceNumber);
}

static void c1_chartstep_c1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance)
{
  real_T c1_hoistedGlobal;
  int32_T c1_i4;
  real_T c1_xi[6];
  real_T c1_qi;
  uint32_T c1_debug_family_var_map[11];
  boolean_T c1_x_in_guard12;
  boolean_T c1_x_in_guard21;
  boolean_T c1_x_in_guard23;
  boolean_T c1_x_in_guard32;
  char_T c1_error[39];
  real_T c1_nargin = 2.0;
  real_T c1_nargout = 2.0;
  real_T c1_xo[6];
  real_T c1_qo;
  int32_T c1_i5;
  boolean_T c1_b0;
  boolean_T c1_b1;
  boolean_T c1_b2;
  boolean_T c1_b3;
  int32_T c1_i6;
  const mxArray *c1_y = NULL;
  char_T c1_u[39];
  static char_T c1_cv0[39] = { 'q', 'i', ' ', 'o', 'u', 't', ' ', 'o', 'f', ' ',
    'b', 'o', 'u', 'n', 'd', 's', ' ', 'i', 'n', ' ', 'e', 'd', 'g', 'e', '_',
    'c', 'h', 'e', 'c', 'k', ' ', 'f', 'u', 'n', 'c', 't', 'i', 'o', 'n' };

  int32_T c1_i7;
  int32_T c1_i8;
  real_T *c1_b_qi;
  real_T *c1_b_qo;
  real_T (*c1_b_xo)[6];
  real_T (*c1_b_xi)[6];
  boolean_T guard1 = FALSE;
  boolean_T guard2 = FALSE;
  boolean_T guard3 = FALSE;
  boolean_T guard4 = FALSE;
  c1_b_qo = (real_T *)ssGetOutputPortSignal(chartInstance->S, 2);
  c1_b_qi = (real_T *)ssGetInputPortSignal(chartInstance->S, 1);
  c1_b_xo = (real_T (*)[6])ssGetOutputPortSignal(chartInstance->S, 1);
  c1_b_xi = (real_T (*)[6])ssGetInputPortSignal(chartInstance->S, 0);
  _SFD_CC_CALL(CHART_ENTER_DURING_FUNCTION_TAG, 0U, chartInstance->c1_sfEvent);
  c1_hoistedGlobal = *c1_b_qi;
  for (c1_i4 = 0; c1_i4 < 6; c1_i4++) {
    c1_xi[c1_i4] = (*c1_b_xi)[c1_i4];
  }

  c1_qi = c1_hoistedGlobal;
  _SFD_SYMBOL_SCOPE_PUSH_EML(0U, 11U, 11U, c1_debug_family_names,
    c1_debug_family_var_map);
  _SFD_SYMBOL_SCOPE_ADD_EML_IMPORTABLE(&c1_x_in_guard12, 0U, c1_d_sf_marshallOut,
    c1_d_sf_marshallIn);
  _SFD_SYMBOL_SCOPE_ADD_EML_IMPORTABLE(&c1_x_in_guard21, 1U, c1_d_sf_marshallOut,
    c1_d_sf_marshallIn);
  _SFD_SYMBOL_SCOPE_ADD_EML_IMPORTABLE(&c1_x_in_guard23, 2U, c1_d_sf_marshallOut,
    c1_d_sf_marshallIn);
  _SFD_SYMBOL_SCOPE_ADD_EML_IMPORTABLE(&c1_x_in_guard32, 3U, c1_d_sf_marshallOut,
    c1_d_sf_marshallIn);
  _SFD_SYMBOL_SCOPE_ADD_EML_IMPORTABLE(c1_error, 4U, c1_c_sf_marshallOut,
    c1_c_sf_marshallIn);
  _SFD_SYMBOL_SCOPE_ADD_EML_IMPORTABLE(&c1_nargin, 5U, c1_sf_marshallOut,
    c1_sf_marshallIn);
  _SFD_SYMBOL_SCOPE_ADD_EML_IMPORTABLE(&c1_nargout, 6U, c1_sf_marshallOut,
    c1_sf_marshallIn);
  _SFD_SYMBOL_SCOPE_ADD_EML(c1_xi, 7U, c1_b_sf_marshallOut);
  _SFD_SYMBOL_SCOPE_ADD_EML(&c1_qi, 8U, c1_sf_marshallOut);
  _SFD_SYMBOL_SCOPE_ADD_EML_IMPORTABLE(c1_xo, 9U, c1_b_sf_marshallOut,
    c1_b_sf_marshallIn);
  _SFD_SYMBOL_SCOPE_ADD_EML_IMPORTABLE(&c1_qo, 10U, c1_sf_marshallOut,
    c1_sf_marshallIn);
  CV_EML_FCN(0, 0);
  _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 3);
  for (c1_i5 = 0; c1_i5 < 6; c1_i5++) {
    c1_xo[c1_i5] = c1_xi[c1_i5];
  }

  _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 4);
  c1_qo = c1_qi;
  _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 6);
  switch ((int32_T)_SFD_INTEGER_CHECK("qi", c1_qi)) {
   case 1:
    CV_EML_SWITCH(0, 1, 0, 1);
    _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 8);
    guard4 = FALSE;
    if (c1_xi[0] <= 0.0) {
      if (c1_xi[1] < 0.0) {
        c1_b0 = TRUE;
      } else {
        guard4 = TRUE;
      }
    } else {
      guard4 = TRUE;
    }

    if (guard4 == TRUE) {
      c1_b0 = FALSE;
    }

    c1_x_in_guard12 = c1_b0;
    _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 10);
    if (CV_EML_IF(0, 1, 0, c1_x_in_guard12)) {
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 11);
      c1_qo = 2.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 12);
      c1_xo[0] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 13);
      c1_xo[1] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 14);
      c1_xo[2] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 15);
      c1_xo[3] = c1_xi[1];
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 16);
      c1_xo[4] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 17);
      c1_xo[5] = 0.0;
    }
    break;

   case 2:
    CV_EML_SWITCH(0, 1, 0, 2);
    _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 22);
    guard3 = FALSE;
    if (c1_xi[2] != 0.0) {
      if (c1_xi[3] > 0.0) {
        c1_b1 = TRUE;
      } else {
        guard3 = TRUE;
      }
    } else {
      guard3 = TRUE;
    }

    if (guard3 == TRUE) {
      c1_b1 = FALSE;
    }

    c1_x_in_guard21 = c1_b1;
    _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 23);
    guard2 = FALSE;
    if (c1_xi[2] != 0.0) {
      if (c1_xi[3] < 0.0) {
        c1_b2 = TRUE;
      } else {
        guard2 = TRUE;
      }
    } else {
      guard2 = TRUE;
    }

    if (guard2 == TRUE) {
      c1_b2 = FALSE;
    }

    c1_x_in_guard23 = c1_b2;
    _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 25);
    if (CV_EML_IF(0, 1, 1, c1_x_in_guard21)) {
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 26);
      c1_qo = 1.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 27);
      c1_xo[0] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 28);
      c1_xo[1] = c1_xi[3];
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 29);
      c1_xo[2] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 30);
      c1_xo[3] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 31);
      c1_xo[4] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 32);
      c1_xo[5] = 0.0;
    }

    _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 34);
    if (CV_EML_IF(0, 1, 2, c1_x_in_guard23)) {
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 35);
      c1_qo = 3.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 36);
      c1_xo[0] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 37);
      c1_xo[1] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 38);
      c1_xo[2] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 39);
      c1_xo[3] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 40);
      c1_xo[4] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 41);
      c1_xo[5] = c1_xi[3];
    }
    break;

   case 3:
    CV_EML_SWITCH(0, 1, 0, 3);
    _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 45);
    guard1 = FALSE;
    if (c1_xi[4] >= 0.0) {
      if (c1_xi[5] > 0.0) {
        c1_b3 = TRUE;
      } else {
        guard1 = TRUE;
      }
    } else {
      guard1 = TRUE;
    }

    if (guard1 == TRUE) {
      c1_b3 = FALSE;
    }

    c1_x_in_guard32 = c1_b3;
    _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 47);
    if (CV_EML_IF(0, 1, 3, c1_x_in_guard32)) {
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 48);
      c1_qo = 2.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 49);
      c1_xo[0] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 50);
      c1_xo[1] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 51);
      c1_xo[2] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 52);
      c1_xo[3] = c1_xi[5];
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 53);
      c1_xo[4] = 0.0;
      _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 54);
      c1_xo[5] = 0.0;
    }
    break;

   default:
    CV_EML_SWITCH(0, 1, 0, 0);
    _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, 58);
    for (c1_i7 = 0; c1_i7 < 39; c1_i7++) {
      c1_error[c1_i7] = c1_cv0[c1_i7];
    }

    sf_mex_printf("%s =\\n", "error");
    for (c1_i8 = 0; c1_i8 < 39; c1_i8++) {
      c1_u[c1_i8] = c1_error[c1_i8];
    }

    c1_y = NULL;
    sf_mex_assign(&c1_y, sf_mex_create("y", c1_u, 10, 0U, 1U, 0U, 2, 1, 39),
                  FALSE);
    sf_mex_call_debug("disp", 0U, 1U, 14, c1_y);
    break;
  }

  _SFD_EML_CALL(0U, chartInstance->c1_sfEvent, -58);
  _SFD_SYMBOL_SCOPE_POP();
  for (c1_i6 = 0; c1_i6 < 6; c1_i6++) {
    (*c1_b_xo)[c1_i6] = c1_xo[c1_i6];
  }

  *c1_b_qo = c1_qo;
  _SFD_CC_CALL(EXIT_OUT_OF_FUNCTION_TAG, 0U, chartInstance->c1_sfEvent);
}

static void initSimStructsc1_newtons_cradle(SFc1_newtons_cradleInstanceStruct
  *chartInstance)
{
}

static void init_script_number_translation(uint32_T c1_machineNumber, uint32_T
  c1_chartNumber)
{
}

static const mxArray *c1_sf_marshallOut(void *chartInstanceVoid, void *c1_inData)
{
  const mxArray *c1_mxArrayOutData = NULL;
  real_T c1_u;
  const mxArray *c1_y = NULL;
  SFc1_newtons_cradleInstanceStruct *chartInstance;
  chartInstance = (SFc1_newtons_cradleInstanceStruct *)chartInstanceVoid;
  c1_mxArrayOutData = NULL;
  c1_u = *(real_T *)c1_inData;
  c1_y = NULL;
  sf_mex_assign(&c1_y, sf_mex_create("y", &c1_u, 0, 0U, 0U, 0U, 0), FALSE);
  sf_mex_assign(&c1_mxArrayOutData, c1_y, FALSE);
  return c1_mxArrayOutData;
}

static real_T c1_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_qo, const char_T *c1_identifier)
{
  real_T c1_y;
  emlrtMsgIdentifier c1_thisId;
  c1_thisId.fIdentifier = c1_identifier;
  c1_thisId.fParent = NULL;
  c1_y = c1_b_emlrt_marshallIn(chartInstance, sf_mex_dup(c1_qo), &c1_thisId);
  sf_mex_destroy(&c1_qo);
  return c1_y;
}

static real_T c1_b_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId)
{
  real_T c1_y;
  real_T c1_d0;
  sf_mex_import(c1_parentId, sf_mex_dup(c1_u), &c1_d0, 1, 0, 0U, 0, 0U, 0);
  c1_y = c1_d0;
  sf_mex_destroy(&c1_u);
  return c1_y;
}

static void c1_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData)
{
  const mxArray *c1_qo;
  const char_T *c1_identifier;
  emlrtMsgIdentifier c1_thisId;
  real_T c1_y;
  SFc1_newtons_cradleInstanceStruct *chartInstance;
  chartInstance = (SFc1_newtons_cradleInstanceStruct *)chartInstanceVoid;
  c1_qo = sf_mex_dup(c1_mxArrayInData);
  c1_identifier = c1_varName;
  c1_thisId.fIdentifier = c1_identifier;
  c1_thisId.fParent = NULL;
  c1_y = c1_b_emlrt_marshallIn(chartInstance, sf_mex_dup(c1_qo), &c1_thisId);
  sf_mex_destroy(&c1_qo);
  *(real_T *)c1_outData = c1_y;
  sf_mex_destroy(&c1_mxArrayInData);
}

static const mxArray *c1_b_sf_marshallOut(void *chartInstanceVoid, void
  *c1_inData)
{
  const mxArray *c1_mxArrayOutData = NULL;
  int32_T c1_i9;
  real_T c1_b_inData[6];
  int32_T c1_i10;
  real_T c1_u[6];
  const mxArray *c1_y = NULL;
  SFc1_newtons_cradleInstanceStruct *chartInstance;
  chartInstance = (SFc1_newtons_cradleInstanceStruct *)chartInstanceVoid;
  c1_mxArrayOutData = NULL;
  for (c1_i9 = 0; c1_i9 < 6; c1_i9++) {
    c1_b_inData[c1_i9] = (*(real_T (*)[6])c1_inData)[c1_i9];
  }

  for (c1_i10 = 0; c1_i10 < 6; c1_i10++) {
    c1_u[c1_i10] = c1_b_inData[c1_i10];
  }

  c1_y = NULL;
  sf_mex_assign(&c1_y, sf_mex_create("y", c1_u, 0, 0U, 1U, 0U, 1, 6), FALSE);
  sf_mex_assign(&c1_mxArrayOutData, c1_y, FALSE);
  return c1_mxArrayOutData;
}

static void c1_c_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_xo, const char_T *c1_identifier, real_T
  c1_y[6])
{
  emlrtMsgIdentifier c1_thisId;
  c1_thisId.fIdentifier = c1_identifier;
  c1_thisId.fParent = NULL;
  c1_d_emlrt_marshallIn(chartInstance, sf_mex_dup(c1_xo), &c1_thisId, c1_y);
  sf_mex_destroy(&c1_xo);
}

static void c1_d_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId,
  real_T c1_y[6])
{
  real_T c1_dv1[6];
  int32_T c1_i11;
  sf_mex_import(c1_parentId, sf_mex_dup(c1_u), c1_dv1, 1, 0, 0U, 1, 0U, 1, 6);
  for (c1_i11 = 0; c1_i11 < 6; c1_i11++) {
    c1_y[c1_i11] = c1_dv1[c1_i11];
  }

  sf_mex_destroy(&c1_u);
}

static void c1_b_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData)
{
  const mxArray *c1_xo;
  const char_T *c1_identifier;
  emlrtMsgIdentifier c1_thisId;
  real_T c1_y[6];
  int32_T c1_i12;
  SFc1_newtons_cradleInstanceStruct *chartInstance;
  chartInstance = (SFc1_newtons_cradleInstanceStruct *)chartInstanceVoid;
  c1_xo = sf_mex_dup(c1_mxArrayInData);
  c1_identifier = c1_varName;
  c1_thisId.fIdentifier = c1_identifier;
  c1_thisId.fParent = NULL;
  c1_d_emlrt_marshallIn(chartInstance, sf_mex_dup(c1_xo), &c1_thisId, c1_y);
  sf_mex_destroy(&c1_xo);
  for (c1_i12 = 0; c1_i12 < 6; c1_i12++) {
    (*(real_T (*)[6])c1_outData)[c1_i12] = c1_y[c1_i12];
  }

  sf_mex_destroy(&c1_mxArrayInData);
}

static const mxArray *c1_c_sf_marshallOut(void *chartInstanceVoid, void
  *c1_inData)
{
  const mxArray *c1_mxArrayOutData = NULL;
  int32_T c1_i13;
  char_T c1_b_inData[39];
  int32_T c1_i14;
  char_T c1_u[39];
  const mxArray *c1_y = NULL;
  SFc1_newtons_cradleInstanceStruct *chartInstance;
  chartInstance = (SFc1_newtons_cradleInstanceStruct *)chartInstanceVoid;
  c1_mxArrayOutData = NULL;
  for (c1_i13 = 0; c1_i13 < 39; c1_i13++) {
    c1_b_inData[c1_i13] = (*(char_T (*)[39])c1_inData)[c1_i13];
  }

  for (c1_i14 = 0; c1_i14 < 39; c1_i14++) {
    c1_u[c1_i14] = c1_b_inData[c1_i14];
  }

  c1_y = NULL;
  sf_mex_assign(&c1_y, sf_mex_create("y", c1_u, 10, 0U, 1U, 0U, 2, 1, 39), FALSE);
  sf_mex_assign(&c1_mxArrayOutData, c1_y, FALSE);
  return c1_mxArrayOutData;
}

static void c1_e_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId,
  char_T c1_y[39])
{
  char_T c1_cv1[39];
  int32_T c1_i15;
  sf_mex_import(c1_parentId, sf_mex_dup(c1_u), c1_cv1, 1, 10, 0U, 1, 0U, 2, 1,
                39);
  for (c1_i15 = 0; c1_i15 < 39; c1_i15++) {
    c1_y[c1_i15] = c1_cv1[c1_i15];
  }

  sf_mex_destroy(&c1_u);
}

static void c1_c_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData)
{
  const mxArray *c1_error;
  const char_T *c1_identifier;
  emlrtMsgIdentifier c1_thisId;
  char_T c1_y[39];
  int32_T c1_i16;
  SFc1_newtons_cradleInstanceStruct *chartInstance;
  chartInstance = (SFc1_newtons_cradleInstanceStruct *)chartInstanceVoid;
  c1_error = sf_mex_dup(c1_mxArrayInData);
  c1_identifier = c1_varName;
  c1_thisId.fIdentifier = c1_identifier;
  c1_thisId.fParent = NULL;
  c1_e_emlrt_marshallIn(chartInstance, sf_mex_dup(c1_error), &c1_thisId, c1_y);
  sf_mex_destroy(&c1_error);
  for (c1_i16 = 0; c1_i16 < 39; c1_i16++) {
    (*(char_T (*)[39])c1_outData)[c1_i16] = c1_y[c1_i16];
  }

  sf_mex_destroy(&c1_mxArrayInData);
}

static const mxArray *c1_d_sf_marshallOut(void *chartInstanceVoid, void
  *c1_inData)
{
  const mxArray *c1_mxArrayOutData = NULL;
  boolean_T c1_u;
  const mxArray *c1_y = NULL;
  SFc1_newtons_cradleInstanceStruct *chartInstance;
  chartInstance = (SFc1_newtons_cradleInstanceStruct *)chartInstanceVoid;
  c1_mxArrayOutData = NULL;
  c1_u = *(boolean_T *)c1_inData;
  c1_y = NULL;
  sf_mex_assign(&c1_y, sf_mex_create("y", &c1_u, 11, 0U, 0U, 0U, 0), FALSE);
  sf_mex_assign(&c1_mxArrayOutData, c1_y, FALSE);
  return c1_mxArrayOutData;
}

static boolean_T c1_f_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId)
{
  boolean_T c1_y;
  boolean_T c1_b4;
  sf_mex_import(c1_parentId, sf_mex_dup(c1_u), &c1_b4, 1, 11, 0U, 0, 0U, 0);
  c1_y = c1_b4;
  sf_mex_destroy(&c1_u);
  return c1_y;
}

static void c1_d_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData)
{
  const mxArray *c1_x_in_guard32;
  const char_T *c1_identifier;
  emlrtMsgIdentifier c1_thisId;
  boolean_T c1_y;
  SFc1_newtons_cradleInstanceStruct *chartInstance;
  chartInstance = (SFc1_newtons_cradleInstanceStruct *)chartInstanceVoid;
  c1_x_in_guard32 = sf_mex_dup(c1_mxArrayInData);
  c1_identifier = c1_varName;
  c1_thisId.fIdentifier = c1_identifier;
  c1_thisId.fParent = NULL;
  c1_y = c1_f_emlrt_marshallIn(chartInstance, sf_mex_dup(c1_x_in_guard32),
    &c1_thisId);
  sf_mex_destroy(&c1_x_in_guard32);
  *(boolean_T *)c1_outData = c1_y;
  sf_mex_destroy(&c1_mxArrayInData);
}

const mxArray *sf_c1_newtons_cradle_get_eml_resolved_functions_info(void)
{
  const mxArray *c1_nameCaptureInfo = NULL;
  c1_nameCaptureInfo = NULL;
  sf_mex_assign(&c1_nameCaptureInfo, sf_mex_create("nameCaptureInfo", NULL, 0,
    0U, 1U, 0U, 2, 0, 1), FALSE);
  return c1_nameCaptureInfo;
}

static const mxArray *c1_e_sf_marshallOut(void *chartInstanceVoid, void
  *c1_inData)
{
  const mxArray *c1_mxArrayOutData = NULL;
  int32_T c1_u;
  const mxArray *c1_y = NULL;
  SFc1_newtons_cradleInstanceStruct *chartInstance;
  chartInstance = (SFc1_newtons_cradleInstanceStruct *)chartInstanceVoid;
  c1_mxArrayOutData = NULL;
  c1_u = *(int32_T *)c1_inData;
  c1_y = NULL;
  sf_mex_assign(&c1_y, sf_mex_create("y", &c1_u, 6, 0U, 0U, 0U, 0), FALSE);
  sf_mex_assign(&c1_mxArrayOutData, c1_y, FALSE);
  return c1_mxArrayOutData;
}

static int32_T c1_g_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId)
{
  int32_T c1_y;
  int32_T c1_i17;
  sf_mex_import(c1_parentId, sf_mex_dup(c1_u), &c1_i17, 1, 6, 0U, 0, 0U, 0);
  c1_y = c1_i17;
  sf_mex_destroy(&c1_u);
  return c1_y;
}

static void c1_e_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData)
{
  const mxArray *c1_b_sfEvent;
  const char_T *c1_identifier;
  emlrtMsgIdentifier c1_thisId;
  int32_T c1_y;
  SFc1_newtons_cradleInstanceStruct *chartInstance;
  chartInstance = (SFc1_newtons_cradleInstanceStruct *)chartInstanceVoid;
  c1_b_sfEvent = sf_mex_dup(c1_mxArrayInData);
  c1_identifier = c1_varName;
  c1_thisId.fIdentifier = c1_identifier;
  c1_thisId.fParent = NULL;
  c1_y = c1_g_emlrt_marshallIn(chartInstance, sf_mex_dup(c1_b_sfEvent),
    &c1_thisId);
  sf_mex_destroy(&c1_b_sfEvent);
  *(int32_T *)c1_outData = c1_y;
  sf_mex_destroy(&c1_mxArrayInData);
}

static uint8_T c1_h_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_b_is_active_c1_newtons_cradle, const char_T *
  c1_identifier)
{
  uint8_T c1_y;
  emlrtMsgIdentifier c1_thisId;
  c1_thisId.fIdentifier = c1_identifier;
  c1_thisId.fParent = NULL;
  c1_y = c1_i_emlrt_marshallIn(chartInstance, sf_mex_dup
    (c1_b_is_active_c1_newtons_cradle), &c1_thisId);
  sf_mex_destroy(&c1_b_is_active_c1_newtons_cradle);
  return c1_y;
}

static uint8_T c1_i_emlrt_marshallIn(SFc1_newtons_cradleInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId)
{
  uint8_T c1_y;
  uint8_T c1_u0;
  sf_mex_import(c1_parentId, sf_mex_dup(c1_u), &c1_u0, 1, 3, 0U, 0, 0U, 0);
  c1_y = c1_u0;
  sf_mex_destroy(&c1_u);
  return c1_y;
}

static void init_dsm_address_info(SFc1_newtons_cradleInstanceStruct
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

void sf_c1_newtons_cradle_get_check_sum(mxArray *plhs[])
{
  ((real_T *)mxGetPr((plhs[0])))[0] = (real_T)(971228898U);
  ((real_T *)mxGetPr((plhs[0])))[1] = (real_T)(2865624895U);
  ((real_T *)mxGetPr((plhs[0])))[2] = (real_T)(837569138U);
  ((real_T *)mxGetPr((plhs[0])))[3] = (real_T)(2269392575U);
}

mxArray *sf_c1_newtons_cradle_get_autoinheritance_info(void)
{
  const char *autoinheritanceFields[] = { "checksum", "inputs", "parameters",
    "outputs", "locals" };

  mxArray *mxAutoinheritanceInfo = mxCreateStructMatrix(1,1,5,
    autoinheritanceFields);

  {
    mxArray *mxChecksum = mxCreateString("z5N01tTUXYf6ik0AFJ6IU");
    mxSetField(mxAutoinheritanceInfo,0,"checksum",mxChecksum);
  }

  {
    const char *dataFields[] = { "size", "type", "complexity" };

    mxArray *mxData = mxCreateStructMatrix(1,2,3,dataFields);

    {
      mxArray *mxSize = mxCreateDoubleMatrix(1,2,mxREAL);
      double *pr = mxGetPr(mxSize);
      pr[0] = (double)(6);
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
      pr[0] = (double)(1);
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
    mxSetField(mxAutoinheritanceInfo,0,"inputs",mxData);
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
      pr[0] = (double)(6);
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
      pr[0] = (double)(1);
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

mxArray *sf_c1_newtons_cradle_third_party_uses_info(void)
{
  mxArray * mxcell3p = mxCreateCellMatrix(1,0);
  return(mxcell3p);
}

mxArray *sf_c1_newtons_cradle_updateBuildInfo_args_info(void)
{
  mxArray *mxBIArgs = mxCreateCellMatrix(1,0);
  return mxBIArgs;
}

static const mxArray *sf_get_sim_state_info_c1_newtons_cradle(void)
{
  const char *infoFields[] = { "chartChecksum", "varInfo" };

  mxArray *mxInfo = mxCreateStructMatrix(1, 1, 2, infoFields);
  const char *infoEncStr[] = {
    "100 S1x3'type','srcId','name','auxInfo'{{M[1],M[7],T\"qo\",},{M[1],M[5],T\"xo\",},{M[8],M[0],T\"is_active_c1_newtons_cradle\",}}"
  };

  mxArray *mxVarInfo = sf_mex_decode_encoded_mx_struct_array(infoEncStr, 3, 10);
  mxArray *mxChecksum = mxCreateDoubleMatrix(1, 4, mxREAL);
  sf_c1_newtons_cradle_get_check_sum(&mxChecksum);
  mxSetField(mxInfo, 0, infoFields[0], mxChecksum);
  mxSetField(mxInfo, 0, infoFields[1], mxVarInfo);
  return mxInfo;
}

static void chart_debug_initialization(SimStruct *S, unsigned int
  fullDebuggerInitialization)
{
  if (!sim_mode_is_rtw_gen(S)) {
    SFc1_newtons_cradleInstanceStruct *chartInstance;
    chartInstance = (SFc1_newtons_cradleInstanceStruct *) ((ChartInfoStruct *)
      (ssGetUserData(S)))->chartInstance;
    if (ssIsFirstInitCond(S) && fullDebuggerInitialization==1) {
      /* do this only if simulation is starting */
      {
        unsigned int chartAlreadyPresent;
        chartAlreadyPresent = sf_debug_initialize_chart
          (sfGlobalDebugInstanceStruct,
           _newtons_cradleMachineNumber_,
           1,
           1,
           1,
           4,
           0,
           0,
           0,
           0,
           0,
           &(chartInstance->chartNumber),
           &(chartInstance->instanceNumber),
           ssGetPath(S),
           (void *)S);
        if (chartAlreadyPresent==0) {
          /* this is the first instance */
          init_script_number_translation(_newtons_cradleMachineNumber_,
            chartInstance->chartNumber);
          sf_debug_set_chart_disable_implicit_casting
            (sfGlobalDebugInstanceStruct,_newtons_cradleMachineNumber_,
             chartInstance->chartNumber,1);
          sf_debug_set_chart_event_thresholds(sfGlobalDebugInstanceStruct,
            _newtons_cradleMachineNumber_,
            chartInstance->chartNumber,
            0,
            0,
            0);
          _SFD_SET_DATA_PROPS(0,1,1,0,"xi");
          _SFD_SET_DATA_PROPS(1,2,0,1,"xo");
          _SFD_SET_DATA_PROPS(2,1,1,0,"qi");
          _SFD_SET_DATA_PROPS(3,2,0,1,"qo");
          _SFD_STATE_INFO(0,0,2);
          _SFD_CH_SUBSTATE_COUNT(0);
          _SFD_CH_SUBSTATE_DECOMP(0);
        }

        _SFD_CV_INIT_CHART(0,0,0,0);

        {
          _SFD_CV_INIT_STATE(0,0,0,0,0,0,NULL,NULL);
        }

        _SFD_CV_INIT_TRANS(0,0,NULL,NULL,0,NULL);

        /* Initialization of MATLAB Function Model Coverage */
        _SFD_CV_INIT_EML(0,1,1,4,0,0,1,0,0,0,0);
        _SFD_CV_INIT_EML_FCN(0,0,"eML_blk_kernel",0,-1,1305);
        _SFD_CV_INIT_EML_IF(0,1,0,186,201,372,401);
        _SFD_CV_INIT_EML_IF(0,1,1,544,559,-1,733);
        _SFD_CV_INIT_EML_IF(0,1,2,742,757,-1,931);
        _SFD_CV_INIT_EML_IF(0,1,3,1026,1041,-1,1215);

        {
          static int caseStart[] = { 1229, 106, 406, 945 };

          static int caseExprEnd[] = { 1238, 112, 412, 951 };

          _SFD_CV_INIT_EML_SWITCH(0,1,0,92,102,1300,4,&(caseStart[0]),
            &(caseExprEnd[0]));
        }

        {
          unsigned int dimVector[1];
          dimVector[0]= 6;
          _SFD_SET_DATA_COMPILED_PROPS(0,SF_DOUBLE,1,&(dimVector[0]),0,0,0,0.0,
            1.0,0,0,(MexFcnForType)c1_b_sf_marshallOut,(MexInFcnForType)NULL);
        }

        {
          unsigned int dimVector[1];
          dimVector[0]= 6;
          _SFD_SET_DATA_COMPILED_PROPS(1,SF_DOUBLE,1,&(dimVector[0]),0,0,0,0.0,
            1.0,0,0,(MexFcnForType)c1_b_sf_marshallOut,(MexInFcnForType)
            c1_b_sf_marshallIn);
        }

        _SFD_SET_DATA_COMPILED_PROPS(2,SF_DOUBLE,0,NULL,0,0,0,0.0,1.0,0,0,
          (MexFcnForType)c1_sf_marshallOut,(MexInFcnForType)NULL);
        _SFD_SET_DATA_COMPILED_PROPS(3,SF_DOUBLE,0,NULL,0,0,0,0.0,1.0,0,0,
          (MexFcnForType)c1_sf_marshallOut,(MexInFcnForType)c1_sf_marshallIn);

        {
          real_T *c1_qi;
          real_T *c1_qo;
          real_T (*c1_xi)[6];
          real_T (*c1_xo)[6];
          c1_qo = (real_T *)ssGetOutputPortSignal(chartInstance->S, 2);
          c1_qi = (real_T *)ssGetInputPortSignal(chartInstance->S, 1);
          c1_xo = (real_T (*)[6])ssGetOutputPortSignal(chartInstance->S, 1);
          c1_xi = (real_T (*)[6])ssGetInputPortSignal(chartInstance->S, 0);
          _SFD_SET_DATA_VALUE_PTR(0U, *c1_xi);
          _SFD_SET_DATA_VALUE_PTR(1U, *c1_xo);
          _SFD_SET_DATA_VALUE_PTR(2U, c1_qi);
          _SFD_SET_DATA_VALUE_PTR(3U, c1_qo);
        }
      }
    } else {
      sf_debug_reset_current_state_configuration(sfGlobalDebugInstanceStruct,
        _newtons_cradleMachineNumber_,chartInstance->chartNumber,
        chartInstance->instanceNumber);
    }
  }
}

static const char* sf_get_instance_specialization(void)
{
  return "h9CrS2yofb3AeRI2bzJxFG";
}

static void sf_opaque_initialize_c1_newtons_cradle(void *chartInstanceVar)
{
  chart_debug_initialization(((SFc1_newtons_cradleInstanceStruct*)
    chartInstanceVar)->S,0);
  initialize_params_c1_newtons_cradle((SFc1_newtons_cradleInstanceStruct*)
    chartInstanceVar);
  initialize_c1_newtons_cradle((SFc1_newtons_cradleInstanceStruct*)
    chartInstanceVar);
}

static void sf_opaque_enable_c1_newtons_cradle(void *chartInstanceVar)
{
  enable_c1_newtons_cradle((SFc1_newtons_cradleInstanceStruct*) chartInstanceVar);
}

static void sf_opaque_disable_c1_newtons_cradle(void *chartInstanceVar)
{
  disable_c1_newtons_cradle((SFc1_newtons_cradleInstanceStruct*)
    chartInstanceVar);
}

static void sf_opaque_gateway_c1_newtons_cradle(void *chartInstanceVar)
{
  sf_c1_newtons_cradle((SFc1_newtons_cradleInstanceStruct*) chartInstanceVar);
}

extern const mxArray* sf_internal_get_sim_state_c1_newtons_cradle(SimStruct* S)
{
  ChartInfoStruct *chartInfo = (ChartInfoStruct*) ssGetUserData(S);
  mxArray *plhs[1] = { NULL };

  mxArray *prhs[4];
  int mxError = 0;
  prhs[0] = mxCreateString("chart_simctx_raw2high");
  prhs[1] = mxCreateDoubleScalar(ssGetSFuncBlockHandle(S));
  prhs[2] = (mxArray*) get_sim_state_c1_newtons_cradle
    ((SFc1_newtons_cradleInstanceStruct*)chartInfo->chartInstance);/* raw sim ctx */
  prhs[3] = (mxArray*) sf_get_sim_state_info_c1_newtons_cradle();/* state var info */
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

extern void sf_internal_set_sim_state_c1_newtons_cradle(SimStruct* S, const
  mxArray *st)
{
  ChartInfoStruct *chartInfo = (ChartInfoStruct*) ssGetUserData(S);
  mxArray *plhs[1] = { NULL };

  mxArray *prhs[4];
  int mxError = 0;
  prhs[0] = mxCreateString("chart_simctx_high2raw");
  prhs[1] = mxCreateDoubleScalar(ssGetSFuncBlockHandle(S));
  prhs[2] = mxDuplicateArray(st);      /* high level simctx */
  prhs[3] = (mxArray*) sf_get_sim_state_info_c1_newtons_cradle();/* state var info */
  mxError = sf_mex_call_matlab(1, plhs, 4, prhs, "sfprivate");
  mxDestroyArray(prhs[0]);
  mxDestroyArray(prhs[1]);
  mxDestroyArray(prhs[2]);
  mxDestroyArray(prhs[3]);
  if (mxError || plhs[0] == NULL) {
    sf_mex_error_message("Stateflow Internal Error: \nError calling 'chart_simctx_high2raw'.\n");
  }

  set_sim_state_c1_newtons_cradle((SFc1_newtons_cradleInstanceStruct*)
    chartInfo->chartInstance, mxDuplicateArray(plhs[0]));
  mxDestroyArray(plhs[0]);
}

static const mxArray* sf_opaque_get_sim_state_c1_newtons_cradle(SimStruct* S)
{
  return sf_internal_get_sim_state_c1_newtons_cradle(S);
}

static void sf_opaque_set_sim_state_c1_newtons_cradle(SimStruct* S, const
  mxArray *st)
{
  sf_internal_set_sim_state_c1_newtons_cradle(S, st);
}

static void sf_opaque_terminate_c1_newtons_cradle(void *chartInstanceVar)
{
  if (chartInstanceVar!=NULL) {
    SimStruct *S = ((SFc1_newtons_cradleInstanceStruct*) chartInstanceVar)->S;
    if (sim_mode_is_rtw_gen(S) || sim_mode_is_external(S)) {
      sf_clear_rtw_identifier(S);
      unload_newtons_cradle_optimization_info();
    }

    finalize_c1_newtons_cradle((SFc1_newtons_cradleInstanceStruct*)
      chartInstanceVar);
    utFree((void *)chartInstanceVar);
    ssSetUserData(S,NULL);
  }
}

static void sf_opaque_init_subchart_simstructs(void *chartInstanceVar)
{
  initSimStructsc1_newtons_cradle((SFc1_newtons_cradleInstanceStruct*)
    chartInstanceVar);
}

extern unsigned int sf_machine_global_initializer_called(void);
static void mdlProcessParameters_c1_newtons_cradle(SimStruct *S)
{
  int i;
  for (i=0;i<ssGetNumRunTimeParams(S);i++) {
    if (ssGetSFcnParamTunable(S,i)) {
      ssUpdateDlgParamAsRunTimeParam(S,i);
    }
  }

  if (sf_machine_global_initializer_called()) {
    initialize_params_c1_newtons_cradle((SFc1_newtons_cradleInstanceStruct*)
      (((ChartInfoStruct *)ssGetUserData(S))->chartInstance));
  }
}

static void mdlSetWorkWidths_c1_newtons_cradle(SimStruct *S)
{
  if (sim_mode_is_rtw_gen(S) || sim_mode_is_external(S)) {
    mxArray *infoStruct = load_newtons_cradle_optimization_info();
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
      ssSetInputPortOptimOpts(S, 0, SS_REUSABLE_AND_LOCAL);
      ssSetInputPortOptimOpts(S, 1, SS_REUSABLE_AND_LOCAL);
      sf_mark_chart_expressionable_inputs(S,sf_get_instance_specialization(),
        infoStruct,1,2);
      sf_mark_chart_reusable_outputs(S,sf_get_instance_specialization(),
        infoStruct,1,2);
    }

    {
      unsigned int outPortIdx;
      for (outPortIdx=1; outPortIdx<=2; ++outPortIdx) {
        ssSetOutputPortOptimizeInIR(S, outPortIdx, 1U);
      }
    }

    {
      unsigned int inPortIdx;
      for (inPortIdx=0; inPortIdx < 2; ++inPortIdx) {
        ssSetInputPortOptimizeInIR(S, inPortIdx, 1U);
      }
    }

    sf_set_rtw_dwork_info(S,sf_get_instance_specialization(),infoStruct,1);
    ssSetHasSubFunctions(S,!(chartIsInlinable));
  } else {
  }

  ssSetOptions(S,ssGetOptions(S)|SS_OPTION_WORKS_WITH_CODE_REUSE);
  ssSetChecksum0(S,(272932911U));
  ssSetChecksum1(S,(4210953595U));
  ssSetChecksum2(S,(1139111418U));
  ssSetChecksum3(S,(2133444854U));
  ssSetmdlDerivatives(S, NULL);
  ssSetExplicitFCSSCtrl(S,1);
  ssSupportsMultipleExecInstances(S,1);
}

static void mdlRTW_c1_newtons_cradle(SimStruct *S)
{
  if (sim_mode_is_rtw_gen(S)) {
    ssWriteRTWStrParam(S, "StateflowChartType", "Embedded MATLAB");
  }
}

static void mdlStart_c1_newtons_cradle(SimStruct *S)
{
  SFc1_newtons_cradleInstanceStruct *chartInstance;
  chartInstance = (SFc1_newtons_cradleInstanceStruct *)utMalloc(sizeof
    (SFc1_newtons_cradleInstanceStruct));
  memset(chartInstance, 0, sizeof(SFc1_newtons_cradleInstanceStruct));
  if (chartInstance==NULL) {
    sf_mex_error_message("Could not allocate memory for chart instance.");
  }

  chartInstance->chartInfo.chartInstance = chartInstance;
  chartInstance->chartInfo.isEMLChart = 1;
  chartInstance->chartInfo.chartInitialized = 0;
  chartInstance->chartInfo.sFunctionGateway =
    sf_opaque_gateway_c1_newtons_cradle;
  chartInstance->chartInfo.initializeChart =
    sf_opaque_initialize_c1_newtons_cradle;
  chartInstance->chartInfo.terminateChart =
    sf_opaque_terminate_c1_newtons_cradle;
  chartInstance->chartInfo.enableChart = sf_opaque_enable_c1_newtons_cradle;
  chartInstance->chartInfo.disableChart = sf_opaque_disable_c1_newtons_cradle;
  chartInstance->chartInfo.getSimState =
    sf_opaque_get_sim_state_c1_newtons_cradle;
  chartInstance->chartInfo.setSimState =
    sf_opaque_set_sim_state_c1_newtons_cradle;
  chartInstance->chartInfo.getSimStateInfo =
    sf_get_sim_state_info_c1_newtons_cradle;
  chartInstance->chartInfo.zeroCrossings = NULL;
  chartInstance->chartInfo.outputs = NULL;
  chartInstance->chartInfo.derivatives = NULL;
  chartInstance->chartInfo.mdlRTW = mdlRTW_c1_newtons_cradle;
  chartInstance->chartInfo.mdlStart = mdlStart_c1_newtons_cradle;
  chartInstance->chartInfo.mdlSetWorkWidths = mdlSetWorkWidths_c1_newtons_cradle;
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
  chart_debug_initialization(S,1);
}

void c1_newtons_cradle_method_dispatcher(SimStruct *S, int_T method, void *data)
{
  switch (method) {
   case SS_CALL_MDL_START:
    mdlStart_c1_newtons_cradle(S);
    break;

   case SS_CALL_MDL_SET_WORK_WIDTHS:
    mdlSetWorkWidths_c1_newtons_cradle(S);
    break;

   case SS_CALL_MDL_PROCESS_PARAMETERS:
    mdlProcessParameters_c1_newtons_cradle(S);
    break;

   default:
    /* Unhandled method */
    sf_mex_error_message("Stateflow Internal Error:\n"
                         "Error calling c1_newtons_cradle_method_dispatcher.\n"
                         "Can't handle method %d.\n", method);
    break;
  }
}

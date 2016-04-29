/* Include files */

#include "Quadrotor_sfun.h"
#include "Quadrotor_sfun_debug_macros.h"
#include "c1_Quadrotor.h"
#include "c2_Quadrotor.h"
#include "c3_Quadrotor.h"
#include "c4_Quadrotor.h"
#include "c5_Quadrotor.h"
#include "c6_Quadrotor.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */
uint32_T _QuadrotorMachineNumber_;

/* Function Declarations */

/* Function Definitions */
void Quadrotor_initializer(void)
{
}

void Quadrotor_terminator(void)
{
}

/* SFunction Glue Code */
unsigned int sf_Quadrotor_method_dispatcher(SimStruct *simstructPtr, unsigned
  int chartFileNumber, const char* specsCksum, int_T method, void *data)
{
  if (chartFileNumber==1) {
    c1_Quadrotor_method_dispatcher(simstructPtr, method, data);
    return 1;
  }

  if (chartFileNumber==2) {
    c2_Quadrotor_method_dispatcher(simstructPtr, method, data);
    return 1;
  }

  if (chartFileNumber==3) {
    c3_Quadrotor_method_dispatcher(simstructPtr, method, data);
    return 1;
  }

  if (chartFileNumber==4) {
    c4_Quadrotor_method_dispatcher(simstructPtr, method, data);
    return 1;
  }

  if (chartFileNumber==5) {
    c5_Quadrotor_method_dispatcher(simstructPtr, method, data);
    return 1;
  }

  if (chartFileNumber==6) {
    c6_Quadrotor_method_dispatcher(simstructPtr, method, data);
    return 1;
  }

  return 0;
}

unsigned int sf_Quadrotor_process_check_sum_call( int nlhs, mxArray * plhs[],
  int nrhs, const mxArray * prhs[] )
{

#ifdef MATLAB_MEX_FILE

  char commandName[20];
  if (nrhs<1 || !mxIsChar(prhs[0]) )
    return 0;

  /* Possible call to get the checksum */
  mxGetString(prhs[0], commandName,sizeof(commandName)/sizeof(char));
  commandName[(sizeof(commandName)/sizeof(char)-1)] = '\0';
  if (strcmp(commandName,"sf_get_check_sum"))
    return 0;
  plhs[0] = mxCreateDoubleMatrix( 1,4,mxREAL);
  if (nrhs>1 && mxIsChar(prhs[1])) {
    mxGetString(prhs[1], commandName,sizeof(commandName)/sizeof(char));
    commandName[(sizeof(commandName)/sizeof(char)-1)] = '\0';
    if (!strcmp(commandName,"machine")) {
      ((real_T *)mxGetPr((plhs[0])))[0] = (real_T)(2186461945U);
      ((real_T *)mxGetPr((plhs[0])))[1] = (real_T)(1978436601U);
      ((real_T *)mxGetPr((plhs[0])))[2] = (real_T)(1040814558U);
      ((real_T *)mxGetPr((plhs[0])))[3] = (real_T)(2280672898U);
    } else if (!strcmp(commandName,"exportedFcn")) {
      ((real_T *)mxGetPr((plhs[0])))[0] = (real_T)(0U);
      ((real_T *)mxGetPr((plhs[0])))[1] = (real_T)(0U);
      ((real_T *)mxGetPr((plhs[0])))[2] = (real_T)(0U);
      ((real_T *)mxGetPr((plhs[0])))[3] = (real_T)(0U);
    } else if (!strcmp(commandName,"makefile")) {
      ((real_T *)mxGetPr((plhs[0])))[0] = (real_T)(2181060594U);
      ((real_T *)mxGetPr((plhs[0])))[1] = (real_T)(959401712U);
      ((real_T *)mxGetPr((plhs[0])))[2] = (real_T)(1181572603U);
      ((real_T *)mxGetPr((plhs[0])))[3] = (real_T)(2351801568U);
    } else if (nrhs==3 && !strcmp(commandName,"chart")) {
      unsigned int chartFileNumber;
      chartFileNumber = (unsigned int)mxGetScalar(prhs[2]);
      switch (chartFileNumber) {
       case 1:
        {
          extern void sf_c1_Quadrotor_get_check_sum(mxArray *plhs[]);
          sf_c1_Quadrotor_get_check_sum(plhs);
          break;
        }

       case 2:
        {
          extern void sf_c2_Quadrotor_get_check_sum(mxArray *plhs[]);
          sf_c2_Quadrotor_get_check_sum(plhs);
          break;
        }

       case 3:
        {
          extern void sf_c3_Quadrotor_get_check_sum(mxArray *plhs[]);
          sf_c3_Quadrotor_get_check_sum(plhs);
          break;
        }

       case 4:
        {
          extern void sf_c4_Quadrotor_get_check_sum(mxArray *plhs[]);
          sf_c4_Quadrotor_get_check_sum(plhs);
          break;
        }

       case 5:
        {
          extern void sf_c5_Quadrotor_get_check_sum(mxArray *plhs[]);
          sf_c5_Quadrotor_get_check_sum(plhs);
          break;
        }

       case 6:
        {
          extern void sf_c6_Quadrotor_get_check_sum(mxArray *plhs[]);
          sf_c6_Quadrotor_get_check_sum(plhs);
          break;
        }

       default:
        ((real_T *)mxGetPr((plhs[0])))[0] = (real_T)(0.0);
        ((real_T *)mxGetPr((plhs[0])))[1] = (real_T)(0.0);
        ((real_T *)mxGetPr((plhs[0])))[2] = (real_T)(0.0);
        ((real_T *)mxGetPr((plhs[0])))[3] = (real_T)(0.0);
      }
    } else if (!strcmp(commandName,"target")) {
      ((real_T *)mxGetPr((plhs[0])))[0] = (real_T)(3031367619U);
      ((real_T *)mxGetPr((plhs[0])))[1] = (real_T)(4001028638U);
      ((real_T *)mxGetPr((plhs[0])))[2] = (real_T)(3978939492U);
      ((real_T *)mxGetPr((plhs[0])))[3] = (real_T)(838979348U);
    } else {
      return 0;
    }
  } else {
    ((real_T *)mxGetPr((plhs[0])))[0] = (real_T)(1662373564U);
    ((real_T *)mxGetPr((plhs[0])))[1] = (real_T)(8713591U);
    ((real_T *)mxGetPr((plhs[0])))[2] = (real_T)(2917403642U);
    ((real_T *)mxGetPr((plhs[0])))[3] = (real_T)(668369939U);
  }

  return 1;

#else

  return 0;

#endif

}

unsigned int sf_Quadrotor_autoinheritance_info( int nlhs, mxArray * plhs[], int
  nrhs, const mxArray * prhs[] )
{

#ifdef MATLAB_MEX_FILE

  char commandName[32];
  char aiChksum[64];
  if (nrhs<3 || !mxIsChar(prhs[0]) )
    return 0;

  /* Possible call to get the autoinheritance_info */
  mxGetString(prhs[0], commandName,sizeof(commandName)/sizeof(char));
  commandName[(sizeof(commandName)/sizeof(char)-1)] = '\0';
  if (strcmp(commandName,"get_autoinheritance_info"))
    return 0;
  mxGetString(prhs[2], aiChksum,sizeof(aiChksum)/sizeof(char));
  aiChksum[(sizeof(aiChksum)/sizeof(char)-1)] = '\0';

  {
    unsigned int chartFileNumber;
    chartFileNumber = (unsigned int)mxGetScalar(prhs[1]);
    switch (chartFileNumber) {
     case 1:
      {
        if (strcmp(aiChksum, "w3LFBkHopH3omVb4lXjVDE") == 0) {
          extern mxArray *sf_c1_Quadrotor_get_autoinheritance_info(void);
          plhs[0] = sf_c1_Quadrotor_get_autoinheritance_info();
          break;
        }

        plhs[0] = mxCreateDoubleMatrix(0,0,mxREAL);
        break;
      }

     case 2:
      {
        if (strcmp(aiChksum, "6D1m7GyZCeAY8Qx02oGQLB") == 0) {
          extern mxArray *sf_c2_Quadrotor_get_autoinheritance_info(void);
          plhs[0] = sf_c2_Quadrotor_get_autoinheritance_info();
          break;
        }

        plhs[0] = mxCreateDoubleMatrix(0,0,mxREAL);
        break;
      }

     case 3:
      {
        if (strcmp(aiChksum, "OvxepsYVipZnuOMhaW5PPE") == 0) {
          extern mxArray *sf_c3_Quadrotor_get_autoinheritance_info(void);
          plhs[0] = sf_c3_Quadrotor_get_autoinheritance_info();
          break;
        }

        plhs[0] = mxCreateDoubleMatrix(0,0,mxREAL);
        break;
      }

     case 4:
      {
        if (strcmp(aiChksum, "7xCOnLcuhj1LRQgw2dlAFC") == 0) {
          extern mxArray *sf_c4_Quadrotor_get_autoinheritance_info(void);
          plhs[0] = sf_c4_Quadrotor_get_autoinheritance_info();
          break;
        }

        plhs[0] = mxCreateDoubleMatrix(0,0,mxREAL);
        break;
      }

     case 5:
      {
        if (strcmp(aiChksum, "PNWupBJJ6lmyUBKC1kgs5B") == 0) {
          extern mxArray *sf_c5_Quadrotor_get_autoinheritance_info(void);
          plhs[0] = sf_c5_Quadrotor_get_autoinheritance_info();
          break;
        }

        plhs[0] = mxCreateDoubleMatrix(0,0,mxREAL);
        break;
      }

     case 6:
      {
        if (strcmp(aiChksum, "Ra7u0UzGB9cykVQSCAjjrB") == 0) {
          extern mxArray *sf_c6_Quadrotor_get_autoinheritance_info(void);
          plhs[0] = sf_c6_Quadrotor_get_autoinheritance_info();
          break;
        }

        plhs[0] = mxCreateDoubleMatrix(0,0,mxREAL);
        break;
      }

     default:
      plhs[0] = mxCreateDoubleMatrix(0,0,mxREAL);
    }
  }

  return 1;

#else

  return 0;

#endif

}

unsigned int sf_Quadrotor_get_eml_resolved_functions_info( int nlhs, mxArray *
  plhs[], int nrhs, const mxArray * prhs[] )
{

#ifdef MATLAB_MEX_FILE

  char commandName[64];
  if (nrhs<2 || !mxIsChar(prhs[0]))
    return 0;

  /* Possible call to get the get_eml_resolved_functions_info */
  mxGetString(prhs[0], commandName,sizeof(commandName)/sizeof(char));
  commandName[(sizeof(commandName)/sizeof(char)-1)] = '\0';
  if (strcmp(commandName,"get_eml_resolved_functions_info"))
    return 0;

  {
    unsigned int chartFileNumber;
    chartFileNumber = (unsigned int)mxGetScalar(prhs[1]);
    switch (chartFileNumber) {
     case 1:
      {
        extern const mxArray *sf_c1_Quadrotor_get_eml_resolved_functions_info
          (void);
        mxArray *persistentMxArray = (mxArray *)
          sf_c1_Quadrotor_get_eml_resolved_functions_info();
        plhs[0] = mxDuplicateArray(persistentMxArray);
        mxDestroyArray(persistentMxArray);
        break;
      }

     case 2:
      {
        extern const mxArray *sf_c2_Quadrotor_get_eml_resolved_functions_info
          (void);
        mxArray *persistentMxArray = (mxArray *)
          sf_c2_Quadrotor_get_eml_resolved_functions_info();
        plhs[0] = mxDuplicateArray(persistentMxArray);
        mxDestroyArray(persistentMxArray);
        break;
      }

     case 3:
      {
        extern const mxArray *sf_c3_Quadrotor_get_eml_resolved_functions_info
          (void);
        mxArray *persistentMxArray = (mxArray *)
          sf_c3_Quadrotor_get_eml_resolved_functions_info();
        plhs[0] = mxDuplicateArray(persistentMxArray);
        mxDestroyArray(persistentMxArray);
        break;
      }

     case 4:
      {
        extern const mxArray *sf_c4_Quadrotor_get_eml_resolved_functions_info
          (void);
        mxArray *persistentMxArray = (mxArray *)
          sf_c4_Quadrotor_get_eml_resolved_functions_info();
        plhs[0] = mxDuplicateArray(persistentMxArray);
        mxDestroyArray(persistentMxArray);
        break;
      }

     case 5:
      {
        extern const mxArray *sf_c5_Quadrotor_get_eml_resolved_functions_info
          (void);
        mxArray *persistentMxArray = (mxArray *)
          sf_c5_Quadrotor_get_eml_resolved_functions_info();
        plhs[0] = mxDuplicateArray(persistentMxArray);
        mxDestroyArray(persistentMxArray);
        break;
      }

     case 6:
      {
        extern const mxArray *sf_c6_Quadrotor_get_eml_resolved_functions_info
          (void);
        mxArray *persistentMxArray = (mxArray *)
          sf_c6_Quadrotor_get_eml_resolved_functions_info();
        plhs[0] = mxDuplicateArray(persistentMxArray);
        mxDestroyArray(persistentMxArray);
        break;
      }

     default:
      plhs[0] = mxCreateDoubleMatrix(0,0,mxREAL);
    }
  }

  return 1;

#else

  return 0;

#endif

}

unsigned int sf_Quadrotor_third_party_uses_info( int nlhs, mxArray * plhs[], int
  nrhs, const mxArray * prhs[] )
{
  char commandName[64];
  char tpChksum[64];
  if (nrhs<3 || !mxIsChar(prhs[0]))
    return 0;

  /* Possible call to get the third_party_uses_info */
  mxGetString(prhs[0], commandName,sizeof(commandName)/sizeof(char));
  commandName[(sizeof(commandName)/sizeof(char)-1)] = '\0';
  mxGetString(prhs[2], tpChksum,sizeof(tpChksum)/sizeof(char));
  tpChksum[(sizeof(tpChksum)/sizeof(char)-1)] = '\0';
  if (strcmp(commandName,"get_third_party_uses_info"))
    return 0;

  {
    unsigned int chartFileNumber;
    chartFileNumber = (unsigned int)mxGetScalar(prhs[1]);
    switch (chartFileNumber) {
     case 1:
      {
        if (strcmp(tpChksum, "nrPgAYd1d5VDm8yUJ5bMs") == 0) {
          extern mxArray *sf_c1_Quadrotor_third_party_uses_info(void);
          plhs[0] = sf_c1_Quadrotor_third_party_uses_info();
          break;
        }
      }

     case 2:
      {
        if (strcmp(tpChksum, "zC0lZugG9NPv5Y8ylOUMXB") == 0) {
          extern mxArray *sf_c2_Quadrotor_third_party_uses_info(void);
          plhs[0] = sf_c2_Quadrotor_third_party_uses_info();
          break;
        }
      }

     case 3:
      {
        if (strcmp(tpChksum, "XyBMrF9mzhuKlZZzodtm0D") == 0) {
          extern mxArray *sf_c3_Quadrotor_third_party_uses_info(void);
          plhs[0] = sf_c3_Quadrotor_third_party_uses_info();
          break;
        }
      }

     case 4:
      {
        if (strcmp(tpChksum, "FQKtTEtOSYxRP9RjlQCM0G") == 0) {
          extern mxArray *sf_c4_Quadrotor_third_party_uses_info(void);
          plhs[0] = sf_c4_Quadrotor_third_party_uses_info();
          break;
        }
      }

     case 5:
      {
        if (strcmp(tpChksum, "mC6tzr9Bqg5sqWVa6iJBmE") == 0) {
          extern mxArray *sf_c5_Quadrotor_third_party_uses_info(void);
          plhs[0] = sf_c5_Quadrotor_third_party_uses_info();
          break;
        }
      }

     case 6:
      {
        if (strcmp(tpChksum, "B1Ec3g9AGT7Ee23ZGRhMZF") == 0) {
          extern mxArray *sf_c6_Quadrotor_third_party_uses_info(void);
          plhs[0] = sf_c6_Quadrotor_third_party_uses_info();
          break;
        }
      }

     default:
      plhs[0] = mxCreateDoubleMatrix(0,0,mxREAL);
    }
  }

  return 1;
}

unsigned int sf_Quadrotor_updateBuildInfo_args_info( int nlhs, mxArray * plhs[],
  int nrhs, const mxArray * prhs[] )
{
  char commandName[64];
  char tpChksum[64];
  if (nrhs<3 || !mxIsChar(prhs[0]))
    return 0;

  /* Possible call to get the updateBuildInfo_args_info */
  mxGetString(prhs[0], commandName,sizeof(commandName)/sizeof(char));
  commandName[(sizeof(commandName)/sizeof(char)-1)] = '\0';
  mxGetString(prhs[2], tpChksum,sizeof(tpChksum)/sizeof(char));
  tpChksum[(sizeof(tpChksum)/sizeof(char)-1)] = '\0';
  if (strcmp(commandName,"get_updateBuildInfo_args_info"))
    return 0;

  {
    unsigned int chartFileNumber;
    chartFileNumber = (unsigned int)mxGetScalar(prhs[1]);
    switch (chartFileNumber) {
     case 1:
      {
        if (strcmp(tpChksum, "nrPgAYd1d5VDm8yUJ5bMs") == 0) {
          extern mxArray *sf_c1_Quadrotor_updateBuildInfo_args_info(void);
          plhs[0] = sf_c1_Quadrotor_updateBuildInfo_args_info();
          break;
        }
      }

     case 2:
      {
        if (strcmp(tpChksum, "zC0lZugG9NPv5Y8ylOUMXB") == 0) {
          extern mxArray *sf_c2_Quadrotor_updateBuildInfo_args_info(void);
          plhs[0] = sf_c2_Quadrotor_updateBuildInfo_args_info();
          break;
        }
      }

     case 3:
      {
        if (strcmp(tpChksum, "XyBMrF9mzhuKlZZzodtm0D") == 0) {
          extern mxArray *sf_c3_Quadrotor_updateBuildInfo_args_info(void);
          plhs[0] = sf_c3_Quadrotor_updateBuildInfo_args_info();
          break;
        }
      }

     case 4:
      {
        if (strcmp(tpChksum, "FQKtTEtOSYxRP9RjlQCM0G") == 0) {
          extern mxArray *sf_c4_Quadrotor_updateBuildInfo_args_info(void);
          plhs[0] = sf_c4_Quadrotor_updateBuildInfo_args_info();
          break;
        }
      }

     case 5:
      {
        if (strcmp(tpChksum, "mC6tzr9Bqg5sqWVa6iJBmE") == 0) {
          extern mxArray *sf_c5_Quadrotor_updateBuildInfo_args_info(void);
          plhs[0] = sf_c5_Quadrotor_updateBuildInfo_args_info();
          break;
        }
      }

     case 6:
      {
        if (strcmp(tpChksum, "B1Ec3g9AGT7Ee23ZGRhMZF") == 0) {
          extern mxArray *sf_c6_Quadrotor_updateBuildInfo_args_info(void);
          plhs[0] = sf_c6_Quadrotor_updateBuildInfo_args_info();
          break;
        }
      }

     default:
      plhs[0] = mxCreateDoubleMatrix(0,0,mxREAL);
    }
  }

  return 1;
}

void Quadrotor_debug_initialize(struct SfDebugInstanceStruct* debugInstance)
{
  _QuadrotorMachineNumber_ = sf_debug_initialize_machine(debugInstance,
    "Quadrotor","sfun",0,6,0,0,0);
  sf_debug_set_machine_event_thresholds(debugInstance,_QuadrotorMachineNumber_,0,
    0);
  sf_debug_set_machine_data_thresholds(debugInstance,_QuadrotorMachineNumber_,0);
}

void Quadrotor_register_exported_symbols(SimStruct* S)
{
}

static mxArray* sRtwOptimizationInfoStruct= NULL;
mxArray* load_Quadrotor_optimization_info(void)
{
  if (sRtwOptimizationInfoStruct==NULL) {
    sRtwOptimizationInfoStruct = sf_load_rtw_optimization_info("Quadrotor",
      "Quadrotor");
    mexMakeArrayPersistent(sRtwOptimizationInfoStruct);
  }

  return(sRtwOptimizationInfoStruct);
}

void unload_Quadrotor_optimization_info(void)
{
  if (sRtwOptimizationInfoStruct!=NULL) {
    mxDestroyArray(sRtwOptimizationInfoStruct);
    sRtwOptimizationInfoStruct = NULL;
  }
}

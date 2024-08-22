/*
 * File: systemDefinations.h
 *
 * Code generated for Simulink model 'ControlSystem_v2'.
 *
 * Model version                  : 5.169
 * Simulink Coder version         : 9.8 (R2022b) 13-May-2022
 * C/C++ source code generated on : Thu Aug 22 08:03:55 2024
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: STMicroelectronics->ST10/Super10
 * Code generation objectives:
 *    1. MISRA C:2012 guidelines
 *    2. Execution efficiency
 * Validation result: Not run
 */

#ifndef RTW_HEADER_systemDefinations_h_
#define RTW_HEADER_systemDefinations_h_
#include "rtwtypes.h"

typedef struct {
  /* Уставка по высокому напряжению питания ( используется в генераторном режиме) */
  int16_T Udc_high;

  /* Диспазон работы ограничения от 1-0 в вольтах */
  int16_T Udc_high_diap;
  int16_T Udc_low;
  int16_T Udc_low_diap;
  int16_T Igbt_Thigh;
  int16_T Igbt_Thigh_diap;
  int16_T Motor_Thigh;
  int16_T Motor_Thigh_diap;
  int16_T Speed_fwd;
  int16_T Speed_fwd_diap;

  /* Начало ограничения скорости назад */
  uint16_T Speed_rev;

  /* Диспазон ограничения скорости назад */
  uint16_T Speed_rev_diap;
} FuncLimits_st;

/* Поддерживаемые типы датчиков положения */
typedef enum {
  NoSensor = 0,                        /* Default value */
  ResolverType,
  HallType,
  EncoderType,
  SensorLessType,
  HandModeSensor
} posSensorType;

typedef struct {
  uint32_T version;
  FuncLimits_st FuncLimits;
  posSensorType posSensor;
  real32_T BW_reg_Inv;
  real32_T FieldWeakingCoef;
  real32_T LagCorrection;
  real32_T MaxCurrent;
  real32_T MaxIdCurrent;
  real32_T MinIdCurrent;
  real32_T MotorKe;
  real32_T MotorLd;
  real32_T MotorLq;
  real32_T MotorPoles;
  real32_T MotorRs;
  real32_T Observer_LO;
  real32_T PwmBaseFrq;
  real32_T Rate_Down;
  real32_T Rate_Up;
  real32_T ResolverPoles;
  real32_T UdcFiltTp;
  real32_T correctionTheta;
  real32_T deadTimeVoltLoss;
  uint16_T hallTableDirect[6];
  uint16_T hallTableInverse[6];
  uint16_T phase_inverse;
  uint16_T StaticInductionFlg;
} SystemParams_st;


/*
typedef struct {
	int rpm;
	char direction;
	int temperature;
	float pressure;
} SystemParams_st;
*/

typedef enum {
  DEFAULT = 0,                         /* Default value */
  MTPA,
  CURRENT,
  RS,
  LS,
  ANGLECALC
} WorkModeType;

#endif                                 /* RTW_HEADER_systemDefinations_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

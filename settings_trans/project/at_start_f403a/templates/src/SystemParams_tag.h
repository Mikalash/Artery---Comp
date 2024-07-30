#pragma once

#include "inttypes.h"

typedef struct {
	int16_t Udc_high;                             //High supply voltage setting (used in generator mode)
	int16_t Udc_high_diap;                        //Limit operation range from 1-0 in volts
	int16_t Udc_low;
	int16_t Udc_low_diap;
	int16_t Igbt_Thigh;
	int16_t Igbt_Thigh_diap;
	int16_t Motor_Thigh;
	int16_t Motor_Thigh_diap;
	int16_t Speed_fwd;
	int16_t Speed_fwd_diap;
	uint16_t Speed_rev;                           //Start of speed limit reverse
	uint16_t Speed_rev_diap;                      //Reverse speed limit range
} FuncLimits_st;

typedef enum {                                  //Supported position sensor types
	ResolverType = 1,
	HallType,
	EncoderType,
	SensorLessType,
	HandModeSensor
} posSensorType;

typedef struct SystemParams_tag {
	FuncLimits_st FuncLimits;//24 bytes
	posSensorType posSensor;//4 bytes             //Position sensor type
	float BW_reg_Inv;//4 bytes                    //Current controller bandwidth in rads
	float FieldWeakingCoef;                       //Coefficient for determining the operating area of the field weakening controller Zone start voltage Udc* FieldWeakingCoef
	float LagCorrection;                          //PWM voltage implementation lag correction factor
	float MaxCurrent;                             //Maximum current allowed for control in A
	float MaxIdCurrent;                           //Maximum reactive current A
	float MinIdCurrent;                           //Minimum reactive current A
	float MotorLd;
	float MotorLq;
	float MotorPoles;                             //Number of motor pole pairs
	float MotorRs;
	float Observer_LO;                            //Observer convergence coefficient
	float PwmBaseFrq;
	float Rate_Down;                              //Rate of decay of the current reference in As
	float Rate_Up;                                //Current reference slew rate
	float ResolverPoles;                          //Number of resolver pole pairs
	float UdcFiltTp;                              //Power Voltage Filter Time Constant
	float correctionTheta;                        //Rotor position estimation angle offset correction angle
	uint16_t hallTableDirect[6];//12 bytes        //Hall position coding table
	uint16_t hallTableInverse[6];                 //Hall position coding table
	uint16_t phase_inverse;//2 bytes              //Inverter output phase rotation bit
	_Bool StaticInductionFlg;//2 bytes            //Flag for working with static inductances
} SystemParams_type;

#pragma pack(push, 1)
typedef struct {
	SystemParams_type SystemParams;//124 bytes
	uint8_t SystemParams_crc;//4 bytes            //CRC of SystemParams
} SystemParams_shell;
#pragma pack(pop)

#define SP_type_size sizeof(SystemParams_type)
#define SP_shell_size sizeof(SystemParams_shell)

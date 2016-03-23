/*
 * motorcontrol.c
 *
 *  Created on: Feb 28, 2016
 *      Author: papayne
 */

#include "xparameters.h"
#include "driver_board_ip.h"
#include "motorcontrol.h"
#include "xil_io.h"


#define DEBUG_MOTORS
#ifdef DEBUG_MOTORS
	#define MOTOR_DEBUG_PRINT(...) xil_printf(__VA_ARGS__)
#else
	#define MOTOR_DEBUG_PRINT(...)
#endif


#define DRIVER_BOARD_BASE_ADDR (XPAR_DRIVER_BOARD_IP_0_S00_AXI_BASEADDR)
#define CTRL_REG_OFFSET (0)
#define PAN_REG_OFFSET (4)
#define TILT_REG_OFFSET (8)

// These masks refer to the fields of the control register.
#define PAN_ENABLE_MASK (1)
#define TILT_ENABLE_MASK (2)
#define LASER_ENABLE_MASK (4)

// These are some parameters for controlling the two servos.
// We need to do testing to find better values of MIN and MAX.
#define SERVO_PERIOD (2000000)

#define SERVO_NEUTRAL (150000)

// True value is 555.5555....
#define SERVO_DUTY_PER_DEGREE (556)

#define SERVO_MIN (110000)
#define SERVO_MAX (190000)

// These are the angular limits of our turret.
#define SERVO_ANGLE_MAX (72)
#define SERVO_ANGLE_MIN (-72)

static int CurrentPanAngle;
static int CurrentTiltAngle;

/******************************************************************************
 * Static Function Declarations
 *****************************************************************************/
static u32 AngleToDuty(int angle);

/******************************************************************************
 * Public Function Definitions
 *****************************************************************************/
void InitMotorBoard() {
	DRIVER_BOARD_IP_mWriteReg(DRIVER_BOARD_BASE_ADDR, CTRL_REG_OFFSET, 0);
	CurrentPanAngle = 0;
	CurrentTiltAngle = 0;
}


void TurnOnLaser() {
	u32 CtrlReg = DRIVER_BOARD_IP_mReadReg(DRIVER_BOARD_BASE_ADDR, CTRL_REG_OFFSET);
	CtrlReg |= LASER_ENABLE_MASK;
	DRIVER_BOARD_IP_mWriteReg(DRIVER_BOARD_BASE_ADDR, CTRL_REG_OFFSET, CtrlReg);
}


void TurnOffLaser() {
	u32 CtrlReg = DRIVER_BOARD_IP_mReadReg(DRIVER_BOARD_BASE_ADDR, CTRL_REG_OFFSET);
	CtrlReg &= ~LASER_ENABLE_MASK;
	DRIVER_BOARD_IP_mWriteReg(DRIVER_BOARD_BASE_ADDR, CTRL_REG_OFFSET, CtrlReg);
}


void EnablePanServo() {
	u32 CtrlReg = DRIVER_BOARD_IP_mReadReg(DRIVER_BOARD_BASE_ADDR, CTRL_REG_OFFSET);
	CtrlReg |= PAN_ENABLE_MASK;
	DRIVER_BOARD_IP_mWriteReg(DRIVER_BOARD_BASE_ADDR, CTRL_REG_OFFSET, CtrlReg);
}


void DisablePanServo() {
	u32 CtrlReg = DRIVER_BOARD_IP_mReadReg(DRIVER_BOARD_BASE_ADDR, CTRL_REG_OFFSET);
	CtrlReg &= ~PAN_ENABLE_MASK;
	DRIVER_BOARD_IP_mWriteReg(DRIVER_BOARD_BASE_ADDR, CTRL_REG_OFFSET, CtrlReg);
}


void SetPanAngle(int angle) {
	u32 duty = SERVO_NEUTRAL;

	if (angle < SERVO_ANGLE_MIN) {
		MOTOR_DEBUG_PRINT("Attempted pan angle too low: %d\n", angle);
		angle = SERVO_ANGLE_MIN;
	} else if (angle > SERVO_ANGLE_MAX) {
		MOTOR_DEBUG_PRINT("Attempted pan angle too high: %d\n", angle);
		angle = SERVO_ANGLE_MAX;
	}

	duty = AngleToDuty(angle);
	DRIVER_BOARD_IP_mWriteReg(DRIVER_BOARD_BASE_ADDR, PAN_REG_OFFSET, duty);
	CurrentPanAngle = angle;
}


void PanLeft(int angle) {
	int newAngle = CurrentPanAngle + angle;

	if (newAngle > SERVO_ANGLE_MAX) {
		newAngle = SERVO_ANGLE_MAX;
	}

	SetPanAngle(newAngle);
}


void PanRight(int angle) {
	int newAngle = CurrentPanAngle - angle;

	if (newAngle < SERVO_ANGLE_MIN) {
		newAngle = SERVO_ANGLE_MIN;
	}

	SetPanAngle(newAngle);
}


void EnableTiltServo() {
	u32 CtrlReg = DRIVER_BOARD_IP_mReadReg(DRIVER_BOARD_BASE_ADDR, CTRL_REG_OFFSET);
	CtrlReg |= TILT_ENABLE_MASK;
	DRIVER_BOARD_IP_mWriteReg(DRIVER_BOARD_BASE_ADDR, CTRL_REG_OFFSET, CtrlReg);
}


void DisableTiltServo() {
	u32 CtrlReg = DRIVER_BOARD_IP_mReadReg(DRIVER_BOARD_BASE_ADDR, CTRL_REG_OFFSET);
	CtrlReg &= ~TILT_ENABLE_MASK;
	DRIVER_BOARD_IP_mWriteReg(DRIVER_BOARD_BASE_ADDR, CTRL_REG_OFFSET, CtrlReg);
}


void SetTiltAngle(int angle) {
	u32 duty = SERVO_NEUTRAL;

	if (angle < SERVO_ANGLE_MIN) {
		MOTOR_DEBUG_PRINT("Attempted pan angle too low: %d\n", angle);
		angle = SERVO_ANGLE_MIN;
	} else if (angle > SERVO_ANGLE_MAX) {
		MOTOR_DEBUG_PRINT("Attempted pan angle too high: %d\n", angle);
		angle = SERVO_ANGLE_MAX;
	}

	duty = AngleToDuty(angle);
	DRIVER_BOARD_IP_mWriteReg(DRIVER_BOARD_BASE_ADDR, TILT_REG_OFFSET, duty);
	CurrentTiltAngle = angle;
}

void TiltUp(int angle){
	int newAngle = CurrentTiltAngle + angle;

	if (newAngle > SERVO_ANGLE_MAX) {
		newAngle = SERVO_ANGLE_MAX;
	}

	SetTiltAngle(newAngle);
}


void TiltDown(int angle) {
	int newAngle = CurrentTiltAngle - angle;

	if (newAngle < SERVO_ANGLE_MIN) {
		newAngle = SERVO_ANGLE_MIN;
	}

	SetTiltAngle(newAngle);
}

/******************************************************************************
 * Static Function Declarations
 *****************************************************************************/
static u32 AngleToDuty(int angle) {
	int duty = angle * SERVO_DUTY_PER_DEGREE;
	duty += SERVO_NEUTRAL;
	if (duty < SERVO_MIN) {
		duty = SERVO_MIN;
	} else if (duty > SERVO_MAX) {
		duty = SERVO_MAX;
	}
	return (u32) duty;
}

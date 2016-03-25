/*
 * motorcontrol.h
 *
 *  Created on: Feb 28, 2016
 *      Author: papayne
 */

#ifndef MOTORCONTROL_H_
#define MOTORCONTROL_H_

void InitMotorBoard();

void TurnOnLaser();
void TurnOffLaser();

void EnablePanServo();
void DisablePanServo();
void SetPanAngle(int angle);
void PanLeft(int angle);
void PanRight(int angle);

void EnableTiltServo();
void DisableTiltServo();
void SetTiltAngle(int angle);
void TiltUp(int angle);
void TiltDown(int angle);

#endif /* MOTORCONTROL_H_ */

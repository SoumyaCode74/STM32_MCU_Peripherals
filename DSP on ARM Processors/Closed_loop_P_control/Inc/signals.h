/*
 * signals.h
 *
 *  Created on: Apr 23, 2023
 *      Author: soumy
 */

#ifndef SIGNALS_H_
#define SIGNALS_H_
#define SIGNAL_5HZ_LENGTH			(301)
#define SIGNAL_IMPULSE_LENGTH		(29)
#define SIGNAL_640_PTS_ECG			(640)

#if !defined (ARM_MATH_CM4)
	#define ARM_MATH_CM4
#endif
#if !defined(__FPU_PRESENT)
	#define __FPU_PRESENT
#endif

#endif /* SIGNALS_H_ */

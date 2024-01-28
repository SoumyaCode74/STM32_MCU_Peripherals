/*
 * systems.c
 *
 *  Created on: Apr 23, 2023
 *      Author: soumy
 */

#include <stdint.h>
#include "arm_math.h"
#include "signals.h"


void generate_impulse_response_second_order(float32_t * y, float32_t * num_coeff, float32_t * den_coeff, uint32_t interval, uint32_t N);
void generate_system_response_second_order(float32_t *, float32_t *, float32_t *, float32_t *, uint32_t, uint32_t);
void PI_controller(float32_t, float32_t, float32_t *, float32_t *, float32_t *, uint32_t);
void plant_output(float32_t *, float32_t *, float32_t *, uint32_t, uint32_t);
void output_feedback(float32_t, float32_t *, uint32_t);

void output_feedback(float32_t H, float32_t * out, uint32_t N){
	/* N = (Size of 1 array element * N)/(Size of 1 array element)
	 *   = (Total size of array in memory in bytes)/(Size of 1 array element in bytes)
	 *   = the denom. is taken to be the first element in order to avoid any chance of
	 *   array index out of bounds exception
	 */
	for (unsigned int k = 0; k < N; k++){
		out[k] *= H;
	}
}


void plant_output(float32_t * u, float32_t * h, float32_t * out, uint32_t u_N, uint32_t h_N){
	for (unsigned int i = 0; i < u_N; i++){
		for (unsigned int j = 0; j < h_N; j++){
			out[i+j] += u[i]*h[j];
		}
	}
}

void PI_controller(float32_t K_p, float32_t K_i, float32_t * u, float32_t * ref, float32_t * out, uint32_t N){
	u[0] = (K_p + K_i)* (ref[0] - out[0]);
	for (unsigned int k = 1; k < N; k++){
		u[k] = (K_p + K_i)* (ref[k] - out[k]) + K_i*u[k - 1];
	}
}

void generate_impulse_response_second_order(float32_t * y, float32_t * num_coeff, float32_t * den_coeff, uint32_t interval, uint32_t N){
	for (int i = 0; i < N; i+= interval){
		/* calculate y[0] */
		y[i] = num_coeff[2]*1; //b_2
		y[i+1] = -den_coeff[1]*y[i]+num_coeff[1]*1;
		for (int j = 2 ; j < interval; j++){
			y[i+j] = -den_coeff[1]*y[i+j-1]-den_coeff[0]*y[i+j-2]+num_coeff[0]*1;
		}
	}
}

void generate_system_response_second_order(float32_t * y, float32_t * u, float32_t * num_coeff, float32_t * den_coeff, uint32_t interval, uint32_t N){
	for (int i = 0; i < N; i+= interval){
		/* calculate y[0] */
		y[i] = num_coeff[2]*u[i];
		y[i+1] = -den_coeff[1]*y[i]+num_coeff[2]*u[i+1]+num_coeff[1]*u[i];
		for (int j = 2 ; j < interval; j++){
			y[i+j] = -den_coeff[1]*y[i+j-1]-den_coeff[0]*y[i+j-2]+num_coeff[2]*u[i+j]+num_coeff[1]*u[i+j-1]+num_coeff[0]*u[i+j-2];
		}
	}
}

/* Complete the following to design a robust algorithm for obtaining the sequence for any
 * order LTI system
 * Try obtaining the value of output at each sample k using inner product of previous samples
 * and the coefficients.
 */

/*
void compute_sequence(float32_t *, float32_t *, float32_t *, float32_t *, uint32_t);
*/
/*
void generate_impulse_response(float32_t * y_imp, uint32_t order, float32_t * coeff){
	uint32_t N = size(y_imp)/size(float32_t);
	float32_t u[N];

	for (unsigned int i = 0; i < size(y_imp)/size(float32_t); i++){
//		y_imp[i] = coeff[0];
		if (i == 0)
			y_imp[i] = compute_sequence(y_imp, y_coeff, 1, u_coeff, 1, i);
		else
			y_imp[i] = compute_sequence(y_imp, y_coeff, 0, u_coeff, 0, i);
	}
}
*/
/*
void compute_sequence(float32_t * y, float32_t * y_coeff, float32_t * u, float32_t * u_coeff, uint32_t k){
	if (k == 0)
		return (y_coeff[k] + u_coeff[k]*u[k]);
	else
		return (compute_sequence(y, y_coeff, u, ))
}
*/

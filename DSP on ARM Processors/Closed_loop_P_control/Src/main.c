/*
 * Flow of process:
 * Activate clock to GPIOA, USART1
 * Configure PA9 and PA10 to Alternate function mode
 * Set the alternate function values for PA9 and PA10
 * Initialize USART1 for RX and TX modes, with RX interrupt
 * In RX interrupt callback function, transmit received character to serial monitor
 */
#include <stdint.h>
#include <stdio.h>
#include "uart1_discovery_board.h"
#include "signals.h"
#include "arm_math.h"

#define IMPULSE_SIGNAL_LENGTH			(100)
#define FEEDBACK_GAIN					(1)
#define IMPULSE_INTERVAL				((IMPULSE_SIGNAL_LENGTH)/(2))
#define STEP_MAGNITUDE					(5)
#define FREQ							(2.0)
#define PHASE							(30)
#define ON_TIME							((0.5)*(IMPULSE_SIGNAL_LENGTH))
#define PULSE_PEAK						(5)
#define SQUARE_PP						(5)

#define CLOSED							1

q7_t kp_set = 0;
q7_t ki_set = 0;
q7_t ref_set = 0;

//float32_t Kp = 0.0;
//float32_t Ki = 0.222;

float32_t Kp = 0.0705;
float32_t Ki = 0.141;


/* The following three literals are defined in signals.c hence declared using extern in main.c */
//extern float _5hz_signal[301];
//extern float32_t  impulse_response[29];
//extern float32_t _640_points_ecg_[640];
/* Make single copy for main and not modifiable in other scripts */


//extern void generate_impulse_response_second_order(float32_t *, float32_t *, float32_t *, uint32_t, uint32_t);
extern void generate_system_response_second_order(float32_t *, float32_t *, float32_t *, float32_t *, uint32_t, uint32_t);
extern void generate_reference_sequence(float32_t *, uint32_t, uint8_t);
extern void generate_reference_step(float32_t *, uint32_t, uint32_t);
extern void generate_reference_ramp(float32_t *, uint32_t, uint32_t);
extern void generate_reference_parabolic(float32_t *, uint32_t, uint32_t);
extern void generate_reference_sine(float32_t *, uint32_t, uint32_t, float32_t, uint32_t);
extern void generate_reference_pulse(float32_t *, uint32_t, uint32_t, float32_t);
extern void generate_reference_square(float32_t *, uint32_t, uint32_t, float32_t);

extern void PI_controller(float32_t , float32_t, float32_t *, float32_t *, float32_t *, uint32_t);
extern void plant_output(float32_t *, float32_t *, float32_t *, uint32_t, uint32_t);
extern void output_feedback(float32_t, float32_t *, uint32_t);

int main(){

	uint8_t recvd = 0x32;
	float32_t num_coeff[3] = {0.48, 0.0, 0.0};
	float32_t den_coeff[3] = {0.08, -0.6, 1};
//	float32_t y_impulse[IMPULSE_SIGNAL_LENGTH];
	float32_t ref_sig[IMPULSE_SIGNAL_LENGTH];
	float32_t u_input[IMPULSE_SIGNAL_LENGTH];
	/* Length of linear convolution: L+M-1 */
	float32_t y_response[IMPULSE_SIGNAL_LENGTH];

	for(unsigned int i = 0; i < IMPULSE_SIGNAL_LENGTH; i++){
//		y_impulse[i] = 0.0;
		ref_sig[i] = 0.0;
		u_input[i] = 0.0;
		y_response[i] = 0.0;
	}

	board_init();
	configure_UART1_Rx_interrupt(16*1000000U, 9600U);
	enable_floating_point();

//	generate_impulse_response_second_order(y_impulse, num_coeff, den_coeff, IMPULSE_INTERVAL, IMPULSE_SIGNAL_LENGTH);

	/* By default, reference is step signal */
	generate_reference_step(ref_sig, IMPULSE_SIGNAL_LENGTH, STEP_MAGNITUDE);


	/* Disable USART1_IRQ for RXNE
	 * Poll for user input on Kp, Ki, and reference signal choice
	 * */
	pUSART1_base[3] &= ~(0x01 << 5);
	while(1){
		/* Generate control input */
//		PI_controller(Kp, Ki, u_input, ref_sig, y_response, IMPULSE_SIGNAL_LENGTH);
		/* Generate plant output */
//		plant_output(ref_sig, y_impulse, y_response, IMPULSE_SIGNAL_LENGTH, IMPULSE_SIGNAL_LENGTH);
//		generate_system_response_second_order(y_response, u_input, num_coeff, den_coeff, IMPULSE_INTERVAL, IMPULSE_SIGNAL_LENGTH);
		/* Generate feedback */
//		output_feedback(FEEDBACK_GAIN, y_response, IMPULSE_SIGNAL_LENGTH);

		if (!(kp_set == 1 || ki_set == 1 || ref_set == 1))
		{
#if CLOSED
//			u_input[0] = 0.0;
			u_input[0] = (Kp)*(ref_sig[0] - y_response[0]);
//			u_input[1] = (Ki)*(ref_sig[0] - y_response[0]);
			u_input[1] = u_input[0] + Kp*(ref_sig[1] - y_response[1]) + (Ki - Kp)*(ref_sig[0] - y_response[0]);
#else
			u_input[0] = ref_sig[0];
			u_input[1] = ref_sig[1];
#endif
			y_response[0] = num_coeff[2]*u_input[0];
			y_response[1] = -den_coeff[1]*y_response[0]+num_coeff[2]*u_input[1]+num_coeff[1]*u_input[0];
			for (int i = 2; i < IMPULSE_SIGNAL_LENGTH; i++){
//				printf("%ff\r\n",ref_sig[i]);
				/* Control law equation */
#if CLOSED
				/* u_input[k] = u_input[k - 1] + (Kp + Ki)*error[k] - Kp*error[k-1] */
//				u_input[i] = 0*u_input[i-1] + (Kp)*(ref_sig[i] - y_response[i]);
//				u_input[i] = u_input[i-1]+Ki*(ref_sig[i-1] - y_response[i-1]);
				u_input[i] = u_input[i-1] + Kp*(ref_sig[i] - y_response[i]) + (Ki - Kp)*(ref_sig[i-1] - y_response[i-1]);
#else
				u_input[i] = ref_sig[i];
#endif
				/* Plant response */
				y_response[i] = -den_coeff[1]*y_response[i-1]-den_coeff[0]*y_response[i-2]+num_coeff[2]*u_input[i]+num_coeff[1]*u_input[i-1]+num_coeff[0]*u_input[i-2];
				/* Feedback gain */
				y_response[i] *= FEEDBACK_GAIN;
				/* Maintain the following style to plot multiple signals in Arduino
				 * Serial plotter
				 */
				printf("%f",(y_response[i]));
				printf(",");
//				printf("%f", u_input[i]+ref_sig[i]);
//				printf(",");
				printf("%f\r\n",(ref_sig[i]));
				if ((pUSART1_base[0] & (0x01 << 5))){
					recvd = pUSART1_base[1];
					if (recvd == 'p' && kp_set == 0){
						kp_set = 1;
						printf("\r\nSet Kp\r\n");
			    }
			    else if (recvd == 'i' && ki_set == 0){
					ki_set = 1;
					printf("\r\nSet Ki\r\n");
			    }
			    else if (recvd == 'r' && ref_set == 0){
					ref_set = 1;
					/* Setting index for signals to alphabets so that the numeric digits
					 * Can be passed as arguments too!
					 */
					/*
					 * Challenge. Passing arguments becoming challenging with TXE interrupt. Find solution.
					 * For the seminar, doing with pre-programmed signal settings
					 */
					printf("\r\nEnter the correct letter for the following reference input:\r\n");
					printf("\r\nA: Step\r\n");
					printf("\r\nB: Ramp\r\n");
					printf("\r\nC: Parabolic\r\n");
					printf("\r\nD: Sinusoidal\r\n");
					printf("\r\nE: Pulse\r\n");
					printf("\r\nF: Square\r\n");
			    }
			    break;
			}
		}
		/* Poll UART RXNE for next input and wait */
	}
		else{
			/* Although it would be better to keep it all polling
			 * the USART1 IRQ handler is kept for the other exceptions
			 * to handle related to the received data frame
			 */
			/* Wait for RXNE to go HIGH */
			/* Read the input from user */
			while (!(pUSART1_base[0] & (0x01 << 5)));
			recvd = pUSART1_base[1];
		    if (kp_set == 1){
				Kp = recvd;
				kp_set = 0;
		    }
		    else if (ki_set == 1){
				Ki = recvd;
				ki_set = 0;
		    }
		    else if (ref_set == 1){
				switch((char)recvd){
					case 'A':
						printf("\r\nSetting reference input to step input\r\n");
						generate_reference_step(ref_sig, IMPULSE_SIGNAL_LENGTH, STEP_MAGNITUDE);
						break;
					case 'B':
						printf("\r\nSetting reference input to ramp input\r\n");
						generate_reference_ramp(ref_sig, IMPULSE_SIGNAL_LENGTH, IMPULSE_INTERVAL);
						break;
					case 'C':
						printf("\r\nSetting reference input to parabolic input\r\n");
						generate_reference_parabolic(ref_sig, IMPULSE_SIGNAL_LENGTH, IMPULSE_INTERVAL);
						break;
					case 'D':
						printf("\r\nSetting reference input to sinusoidal input\r\n");
						generate_reference_sine(ref_sig, IMPULSE_SIGNAL_LENGTH, IMPULSE_INTERVAL, FREQ, PHASE);
						break;
					case 'E':
						printf("\r\nSetting reference input to pulse input\r\n");
						generate_reference_pulse(ref_sig, IMPULSE_SIGNAL_LENGTH, ON_TIME, PULSE_PEAK);
						break;
					case 'F':
						printf("\r\nSetting reference input to square input\r\n");
						generate_reference_square(ref_sig, IMPULSE_SIGNAL_LENGTH, ON_TIME, SQUARE_PP);
						break;
				}
				ref_set = 0;
		    }

		}
	}
}
void USART1_IRQHandler(){
	/* Check Overrun error and clear */
	if ((pUSART1_base[0] & (0x01 << 3)))
	{
		/* Read SR then DR to clear Overrun error */
		printf("Overrun error detected!!\r\n");
		while(pUSART1_base[0] & 0x0);
		while(pUSART1_base[1] & 0x0);

	}
	/* Check Noise detected in frame and clear */
	if ((pUSART1_base[0] & (0x01 << 2)))
	{
		/* Read SR then DR to clear Noise detection error */
		printf("Noise detected in received word!!\r\n");
		while(pUSART1_base[0] & 0x0);
		while(pUSART1_base[1] & 0x0);

	}
	/* Check error in frame by start or stop bit(s) and clear */
	if ((pUSART1_base[0] & (0x01 << 1)))
	{
		/* Read SR then DR to clear Framing error */
		printf("Framing error detected!!\r\n");
		while(pUSART1_base[0] & 0x0);
		while(pUSART1_base[1] & 0x0);

	}
	/* Check parity error and clear */
	if ((pUSART1_base[0] & (0x01 << 0)))
	{
		/* Read SR then DR to clear Parity error */
		printf("Parity error detected!!\r\n");
		while(pUSART1_base[0] & 0x0);
		while(pUSART1_base[1] & 0x0);

	}

}

void USART1_Callback(){
	uint8_t recvd = pUSART1_base[1];
	printf("\r\nUnexpected input %c. Please try again!\r\n",recvd);
}


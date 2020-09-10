#include <iostream>
#include <math.h>
#include "Mathrl.h"
#include "Cartpole.h"
#include "RLearning.h"
#include "Fuzzy.h"
//#include "FilePrint.h"
#include <math.h>
#include <stdio.h>
#include "Rgenerator.h"

#define MAX_FAILURES 100	/* Termination criterion */
#define MAX_STEPS 5000000 	/* Stability criterion */
#define MAX_EXEC 1			/* Number of executions of the whole program */
#define FMAX 10.0f			/* Maximum force to use */
#define RUNS 10				/* Number of times the program will run inside an execution cycle */

#define PI 3.14159265358979323846f

#define ALPHAINIC 0.05f
#define BETAINIC 0.000002f

CMathrl mathrl;
CCartpole cartpole;
CRLearning rlearning;
CFuzzy fuzzy;
CRgenerator gen;

float gain_sel[MAX_EXEC][2];
int output[10];

/*int main() {
	float x = mathrl.csfunc(0.5);
	std::cout << ("Hello, world.") << std::endl;
	std::cout << x << std::endl;
	double rand = gen.randn_trig(1,3.14);
	std::cout << rand << std::endl;
	rand = gen.randn_trig(1,3.14);
	std::cout << rand << std::endl;
	cartpole.calculate_cart_pole(-0.4f);
	std::cout << cartpole.get_x_dotdot() << std::endl;

	return 0;
}*/

void printTrialsDistribution( int* trials ){
	//init output vector
	for( int i = 0; i < 10; i++ ) output[i] = 0;
	for( int i = 0; i < RUNS; i++ ){
		if(0*MAX_FAILURES/50<trials[i]&&trials[i]<=1*MAX_FAILURES/50)
			output[0] = output[0]++; 
		else if(1*MAX_FAILURES/50<trials[i]&&trials[i]<=2*MAX_FAILURES/50)
			output[1] = output[1]++; 
		else if(2*MAX_FAILURES/50<trials[i]&&trials[i]<=3*MAX_FAILURES/50)
			output[2] = output[2]++; 
		else if(3*MAX_FAILURES/50<trials[i]&&trials[i]<=4*MAX_FAILURES/50)
			output[3] =  output[3]++; 
		else if(4*MAX_FAILURES/50<trials[i]&&trials[i]<=5*MAX_FAILURES/50)
			output[4] = output[4]++; 
		else if(5*MAX_FAILURES/50<trials[i]&&trials[i]<=6*MAX_FAILURES/50)
			output[5] = output[5]++; 
		else if(6*MAX_FAILURES/50<trials[i]&&trials[i]<=7*MAX_FAILURES/50)
			output[6] = output[6]++; 
		else if(7*MAX_FAILURES/50<trials[i]&&trials[i]<=8*MAX_FAILURES/50)
			output[7] = output[7]++; 
		else if(8*MAX_FAILURES/50<trials[i]&&trials[i]<=9*MAX_FAILURES/50)
			output[8] = output[8]++; 
		else if(9*MAX_FAILURES/50<trials[i])
			output[9] = output[9]++;
	}

	printf("Trials Distribution\n");
	for( int i = 0; i < 10; i++ ){
		printf( "%d ", output[i] );
		//std::cout << output[i] << std::endl;
	}
}

/*int _tmain( int argc, _TCHAR* argv[] ){
*/
int main(){
	float force;
	float N;
	float stoch_noise;
	float avg_trials;
	float alpha;
	float beta;
	float E = 0.0;
	float temp = 0.0;
	float alpha_rate = 0.0f;	// Set to 0 for non changing config
	float beta_rate = 1.0f; 	// Set to 1 for non changing config
	int steps = 0,
		exec = 0,
		failures = 0,
		failed = 0,
		trials[RUNS],
		run = 0;
	alpha = (float)ALPHAINIC;
	beta = (float)BETAINIC;

	while( exec < MAX_EXEC ){
		printf("Execution number: %d \n \n", exec + 1);
		//std::cout << "Execution number: " << exec + 1 << std::endl;

		for( int i = 0; i < RUNS; i++ ){
			trials[i] = 0;
		}

		/*--- Set centre and standard deviation values to Fuzzy MFs ---*/
		fuzzy.set_values();

		run = 0;
		while( run < RUNS ){
			failures = 0;
			steps = 0;

			/*--- Initialize action and heuristic critic weights and traces ---*/
			rlearning.reset();

			/*--- Starting state is (0 0 0 0) ---*/
			cartpole.reset();

			/*--- Acquire inputs and fuzzyfy ---*/
			std::cout << "original x " << cartpole.get_x() << std::endl;
			fuzzy.fuzzyfy( cartpole.get_x(), cartpole.get_x_dot(), cartpole.get_x_dotdot(), cartpole.get_theta(), cartpole.get_theta_dot(), cartpole.get_theta_dotdot() );
		
			/*--- Transfer mu values from fuzzy class to RL class ---*/
			for( int i = 0; i < 16; i++ ){
				rlearning.set_mu( i, fuzzy.get_mu( i ) );
				std::cout << fuzzy.get_mu( i ) << std::endl;
			}

			/*--- Reinforcement is 0. Prediction of failure given by v weight. ---*/
			rlearning.set_r( 0.0 );

			/*--- Prediction of failure for current state ---*/
			rlearning.set_p( 0.0 );
			rlearning.calc_oldp( );
			rlearning.calc_p( );

			std::cout << rlearning.get_oldp() << std::endl;
			std::cout << rlearning.get_p() << std::endl;
			/*--- Iterate through the action-learn loop. ---*/
			while( steps < MAX_STEPS && failures < MAX_FAILURES ){ // Executes 1 trial
				/*--- Calculating the stochastic noise ---*/
				N = 1 / (1 + exp( 2 * rlearning.get_p() ) );
				std::cout << N << std::endl;

				stoch_noise = (float)gen.randn_notrig( 0, N ) / 10;
				std::cout << "noise " << stoch_noise << std::endl;

				/*--- Calculating output from ASE ---*/
				rlearning.calc_output( stoch_noise );

				/*--- Calculating force to use ---*/
				force = FMAX * rlearning.get_output( );
				std::cout << "force " << force << std::endl;

				/*--- Apply action to the simulated cart-pole ---*/
				cartpole.calculate_cart_pole( force );

				/*--- Acquire inputs and fuzzyfy ---*/
				fuzzy.fuzzyfy( cartpole.get_x(), cartpole.get_x_dot(), cartpole.get_x_dotdot(), cartpole.get_theta(), cartpole.get_theta_dot(), cartpole.get_theta_dotdot() );
			
				/*--- Transfer mu values from fuzzy class to RL class ---*/
				for( int i = 0; i < 16; i++ ){
					rlearning.set_mu( i, fuzzy.get_mu( i ) );
				}

				if( fuzzy.fail_flag ){
					/*--- Failure ocurred. ---*/
					failed = 1;
					failures++;
					printf( "Trial %d was %d steps\n", failures, steps );
					steps = 0;

					/*--- Starting state is (0 0 0 0) ---*/
					cartpole.reset( );

					/*--- Acquire inputs and fuzzyfy ---*/
					fuzzy.fuzzyfy( cartpole.get_x(), cartpole.get_x_dot(), cartpole.get_x_dotdot(), cartpole.get_theta(), cartpole.get_theta_dot(), cartpole.get_theta_dotdot() );
					
					for( int i = 0; i < 16; i++ ){
						rlearning.set_mu( i, fuzzy.get_mu( i ) );
					}

					/*--- Reinforcement upon failure is -1. Prediction of failure is 0. ---*/
					rlearning.set_r( -1.0 );
					rlearning.set_p( 0.0 );
					rlearning.calc_oldp( );
				}
				else {
					/*--- Not a failure. ---*/
					failed = 0;

					/*--- Reinforcement is 0. Prediction of failure given by v weight. ---*/
					rlearning.set_r( 0.0 );
					rlearning.calc_oldp( );
					rlearning.calc_p( );
					std::cout << rlearning.get_oldp() << std::endl;
					std::cout << rlearning.get_p() << std::endl;
				}

				/*--- Heuristic reinforcement is: current reinforcement + gamma * new failure prediction - previous failure prediction ---*/
				rlearning.calc_rhat( );
				std::cout << rlearning.get_rhat() << std::endl;
				/*--- Calculating E ---*/
				E = (float)( 0.5 * rlearning.get_rhat( ) * rlearning.get_rhat( ) );
				std::cout << E << std::endl;
				/*--- Updating the learning weights ---*/
				rlearning.upd_weights( alpha, beta );

				if( failed ){
					/*--- If failure, zero all traces. ---*/
					rlearning.reset_traces( );
				}
				else {
					/*--- Otherwise, update (decay) the traces. ---*/
					rlearning.upd_traces( );
				}
				steps++;
				if (steps > 3)
					return 0; // debugging
			}
			if( failures >= MAX_FAILURES ){
				printf("Pole not balanced. Stopping after %d failures \n \n", failures );
				trials[run] = (int) failures;
			}
			else {
				trials[run] = (int) failures + 1;
				printf("Pole balanced successfully for at least %d steps \n \n", steps );
			}
			// skipping some code here
			run++;
		}

		temp = 0;
		for( int i = 0; i < RUNS; i++ ){
			temp += (float)trials[i] + 1.0f;
		}
		avg_trials = (float)(temp / RUNS);
		printf( "Average number of trials was %f \n \n", avg_trials );

		printTrialsDistribution(trials); 
		printf("\n\n");

		avg_trials = (float)((temp-(MAX_FAILURES*output[9]))/(RUNS-output[9]));
		printf("Real average number of trials was %f \n \n \n", avg_trials );

		gain_sel[exec][0] = avg_trials;
		gain_sel[exec][1] = (float)output[9];
		exec++;

		alpha += alpha_rate;
		beta *= beta_rate;
	}
	printf("Alpha distribution: \n");
	for( int i = 0; i < MAX_EXEC; i++ ){
		printf("%f ", gain_sel[i][0]);
	}
	printf("\n");
	for( int i = 0; i < MAX_EXEC; i++ ){
		printf("%f ", gain_sel[i][1]);
	}

	return 0;
}
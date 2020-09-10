#include "Fuzzy.h"
#include <math.h>
#include <cmath>
#include <iostream>

void CFuzzy::set_values(void) {
	set_centers();
	set_sigmas();
}

void CFuzzy::set_centers(void) {
	// x
	input1.centre[0] = -1.5f;
	input1.centre[1] = 1.5;

	// x_dot
	input2.centre[0] = -0.5f;
	input2.centre[1] = 0.5;

	// theta
	input3.centre[0] = -1.8f;
	input3.centre[1] = 1.8;

	// theta_dot
	input4.centre[0] = -0.05f;
	input4.centre[1] = 0.05;
}

float CFuzzy::gaussian( float x, float c, float s ) {
	float tmp1, tmp2;

	tmp1 = ( x - c ) / s;
	tmp2 = (float)( (-0.5) * tmp1 * tmp1 );

	return exp( tmp2 );
}

float CFuzzy::gaussian2( float x, float c, float s ) {
	float tmp1, tmp2;

	if( c < 0 ){
		if (x <= c ) return 1;
		else {
			tmp1 = ( x - c ) / s;
			tmp2 = (float)( ( -0.5 ) * tmp1 * tmp1 );
			if (isnan(exp( tmp2 )))
				return 0.0;
			return exp( tmp2 );
		}
	}
	else {
		if( x >= c ) return 1;
		else {
			tmp1 = ( x - c) / s;
			tmp2 = (float)( (-0.5) * tmp1 * tmp1 );
			if (isnan(exp( tmp2 )))
				return 0.0;
			return exp(tmp2);
		}
	}
}

void CFuzzy::fuzzyfy(float x1, float x2, float x3, float x4, float x5, float x6 ){ 
	int cnt1, cnt2, cnt3, cnt4; 
	int i = 0;
	this->C_c1 = x1;
	this->C_c2 = x2;
	this->C_c3 = x3;
	this->C_c4 = x4; 
	this->C_c5 = x5; 
	this->C_c6 = x6;
	
	for( cnt1 = 0;  cnt1 <2;  cnt1++ ){ 
		for( cnt2 =  0;  cnt2 <2;  cnt2++ ){
			for( cnt3 =  0;  cnt3 <2;  cnt3++ ){
				for( cnt4 = 0;  cnt4 <  2;  cnt4++ ){
					std::cout << "x: " << x1 << std::endl;
					this->C_u1 = gaussian2( C_c1, input1.centre[cnt1], input1.sigma[cnt1] ); 
					this->C_u2 = gaussian2( C_c2, input2.centre[cnt2], input2.sigma[cnt2] );
					this->C_u3 = gaussian2( C_c4, input3.centre[cnt3], input3.sigma[cnt3] );
					this->C_u4 = gaussian2( C_c5, input4.centre[cnt4], input4.sigma[cnt4] );

					this->mu[i] = MIN( this->C_u1, this->C_u2, this->C_u3, this->C_u4 );
					if( this->mu[i] <= 0.000001f ) this->mu[i] = 0.000001f;
					i++;
				}
			}
		}
	}
	/* —— To  signal failure — -*/
	if( this->C_c1 <  -2.4 ||  this->C_c1 >  2.4 || this->C_c4 < MINANGLE || this->C_c4 > MAXANGLE )
		this->fail_flag = 1; //  Failure 
	else this->fail_flag =  0;  //  Not  failure
}

float CFuzzy::get_mu( int k ){
	return this->mu[k];
}

float CFuzzy::get_centre( int in,  int c ){ 
	switch (  in  ){
		case 1: return this->input1.centre[c]; 
		case 2: return this->input2.centre[c]; 
		case 3: return this->input3.centre[c]; 
		case 4: return this->input4.centre[c];
	}
}

float CFuzzy::MIN( float a, float b, float c, float d ) {
	float temp;

	temp = a;
	if ( temp >= b  )  temp = b; 
	if ( temp >= c  )  temp = c; 
	if ( temp >= d  )  temp = d;

	return temp;
}

void CFuzzy::set_sigmas( void ){
	// x
	this->input1.sigma[0] = this->input1.centre[1] * SIGMA; 
	this->input1.sigma[1] = this->input1.centre[1] * SIGMA;

	// x_dot
	this->input2.sigma[0] = this->input2.centre[1] * SIGMA;
	this->input2.sigma[1] = this->input2.centre[1] * SIGMA;

	// theta
	this->input3.sigma[0] = this->input3.centre[1] * SIGMA;
	this->input3.sigma[1] = this->input3.centre[1] * SIGMA;

	// theta_dot
	this->input4.sigma[0] = this->input4.centre[1] * SIGMA;
	this->input4.sigma[1] = this->input4.centre[1] * SIGMA;
}

/*void CFuzzy::adapt_sigmas( ){

}*/
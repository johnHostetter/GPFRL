#include "RLearning.h"
#include "math.h"
#include <iostream>

CRLearning::CRLearning(void){}
CRLearning::~CRLearning(void){}

void CRLearning::reset(void){
	for( int i = 0; i < NSTATES; i++ ){
		this->C_w[i] = 0.0;
		this->C_v[i] = 0.0;
		this->C_muhat[i] = 0.0;
		this->C_e[i] = 0.0;
	}
}

void CRLearning::reset_traces(void) {
	for( int i = 0; i < NSTATES; i++ ){
		this->C_e[i] = 0;
		this->C_muhat[i] = 0;
	}
}

void CRLearning::set_r( float r ){
	this->C_r = r;
}

void CRLearning::set_p( float p ){
	this->C_p = p;
}

void CRLearning::set_mu( int k, float mu ){
	std::cout << mu << std::endl;
	this->C_mu[k] = mu;
}

/* Calculates the final probability of executing an action */
void CRLearning::calc_output( float noise ){
	float temp1 = 0;
	float temp2 = 0;

	for( int i = 0; i < NSTATES; i++ ){
		this->C_ro[i] = sfunc( this->C_w[i] );
		std::cout << "sfunc " << sfunc( this->C_w[i] ) << std::endl;
		temp1 += this->C_ro[i] * this->C_mu[i];
		std::cout << "temp1 " << temp1 << std::endl;
		std::cout << "temp1 delta " << this->C_ro[i] * this->C_mu[i] << std::endl;
		temp2 += this->C_mu[i];
		std::cout << "temp2 " << temp2 << std::endl;
		std::cout << "temp2 delta " << this->C_mu[i] << std::endl;
	}
	std::cout << "temp1/temp2 " << temp1 << " " << temp2 << std::endl;
	this->C_P = temp1/temp2;
	std::cout << "C_P" << C_P << std::endl;
	this->C_output = this->C_P + noise;
}

/* Calculates the internal reinforcement signal */
void CRLearning::calc_rhat(void){
	this->C_rhat = this->C_r + ( GAMMA * this->C_p ) - this->C_oldp;
}

/* Calculates the prediction of eventual reinforcement */
void CRLearning::calc_p(void){
	this->C_p = 0;
//	std::cout << this->C_mu[0] << std::endl;
	for( int i = 0; i < NSTATES; i++ ){
//		std::cout << this->C_p << std::endl;
//		std::cout << this->C_v[i] << std::endl;
//		std::cout << this->C_mu[i] << std::endl;
		this->C_p += ( this->C_v[i] * this->C_mu[i] );
	}
}

/* Stores old value of p */
void CRLearning::calc_oldp(void){
	this->C_oldp = this->C_p;
}

/* Update the weights of v and w */
void CRLearning::upd_weights( float A, float B ){
	std::cout << "A " << A << std::endl;
	
	for( int i = 0; i < NSTATES; i++ ){
		this->C_w[i] += A * this->C_rhat * this->C_mu[i] * this->C_e[i];
		std::cout << "C_wi " << this->C_w[i] << std::endl; 
		this->C_v[i] -= B * this->C_rhat * this->C_mu[i];
		if( this->C_w[i] >= 10 ) this->C_w[i] = 10;
		else if( this->C_w[i] <= -10 ) this->C_w[i] = -10;
	}
}

/* Update the value of the traces */
void CRLearning::upd_traces(void){
	for( int i = 0; i < NSTATES; i++ ){
		this->C_e[i] = (1 / this->C_ro[i]) * exp(-this->C_w[i]);
	}
}

float CRLearning::TSugeno1(void){
	float temp1 = 0.0;
	float temp2 = 0.0;

	for( int i = 0; i < NSTATES; i++ ){
		temp1 += ( this->C_am[i] * this->C_mu[i] );
		temp2 += this->C_mu[i];
	}
	return( temp1 / temp2 );
}

float CRLearning::TSugeno2(void){
	float temp1 = 0.0;
	float temp2 = 0.0;

	for( int i = 0; i < NSTATES; i++ ){
		temp1 += ( this->C_ro[i] * this->C_mu[i] );
		temp2 += this->C_mu[i];
	}
	return( temp1 / temp2 );
}

/* Implements an S-shaped function */
float CRLearning::sfunc( float s ){
	std::cout << "in sfunc " << s << std::endl;
	return( (float)(2.0 / (1.0 + exp( -s )) ) - 1);
}

/* Implements a selecting function */
float CRLearning::gfunc( float s ){
	if( s >= 0.5 ) return 1.0;
	else return -1.0;
}

float CRLearning::get_rhat(void){
	return this->C_rhat;
}

float CRLearning::get_output(void){
	return this->C_output;
}

float CRLearning::get_p(void){
	return this->C_p;
}

float CRLearning::get_oldp(void){
	return this->C_oldp;
}
#include "Cartpole.h" 
#include <string> 
#include <iostream> 
#include "math.h"

#define GRAVITY -9.8f
#define MASSPOLE 0.1f
#define MASSCART 1.0f
#define LENGTH 0.5f
#define FRICTION_COEF_CART 0.0005f
#define FRICTION_COEF_POLE 0.000002f

#define TOTALMASS ( MASSPOLE +  MASSCART )
#define MASSPOLE_LENGTH ( MASSPOLE * LENGTH )

#define FOURTHIRDS 1.33333333333333333333f 

#define TAU 0.02f /*seconds between state updates*/

//#define M_PI 3.14159265358979323846f

CCartpole::CCartpole(void){ 
	this->theta = 0; 
	this->theta_dot = 0; 
	this->theta_dotdot = 0; 
	this->x = 0; 
	this->x_dot = 0; 
	this->x_dotdot = 0;
	this->Nc = 1;  //It has to be positive in the first loop 
	this->frictionf = ( FRICTION_COEF_CART * sgn( this->Nc * this->x_dot ) );
}

CCartpole::~CCartpole(void){}

/*—— Implementation of the dynamics of  the  cart-pole system ——*/ 
void CCartpole::calculate_cart_pole( float force ){
	float numerator, costheta, sintheta, brackets, denominator, theta_dot_square, denominator_brackets;
	
	costheta = cos(this->theta); 
	sintheta = sin(this->theta); 
	theta_dot_square = this->theta_dot * this->theta_dot;

	/*—— Calculate theta_dot_dot ——*/
	brackets = ( -force - ( MASSPOLE_LENGTH * theta_dot_square * sintheta ) + ( sgn( this->x_dot ) * FRICTION_COEF_CART ) ) / TOTALMASS;

	numerator = ( GRAVITY * sintheta ) + ( costheta * brackets ) - ( ( FRICTION_COEF_POLE * this->theta_dot ) / ( MASSPOLE_LENGTH ) );

	denominator_brackets =  FOURTHIRDS * ( ( MASSPOLE * costheta * costheta ) / TOTALMASS);
	denominator = LENGTH * denominator_brackets;
	std::cout << "numerator: " << numerator << std::endl;
	std::cout << "denominator: " << denominator << std::endl;
	this->theta_dotdot = numerator / denominator;
	
	/*--- Calculate x_dotdot ---*/
	this->x_dotdot = ( force + ( MASSPOLE_LENGTH * ( ( theta_dot_square * sintheta ) - (this->theta_dotdot * costheta ) ) ) * ( sgn( this->x_dot ) * FRICTION_COEF_CART ) ) / TOTALMASS; 

	/*--- Update state variables (Euler's method) ---*/ 
	this->x += TAU * this->x_dot; 
	this->x_dot += TAU * this->x_dotdot; 
	this->theta += TAU * this->theta_dot; 
	this->theta_dot += TAU * this->theta_dotdot;
	std::cout << "Cartpole observation: (" << this->x << ", " << this->x_dot << ", " << this->theta << ", " << this->theta_dot << ")" << std::endl;
}

void CCartpole::reset(void){
	this->theta = 0;
	this->theta_dot = 0;
	this->theta_dotdot = 0;
	this->x = 0;
	this->x_dot = 0;
	this->x_dotdot = 0;
	this->Nc = 1;
	this->frictionf = (float)(FRICTION_COEF_CART * sgn(this->Nc * this->x_dot));
}

//Consultant functions
float CCartpole::get_theta(void){
	return this->theta;
}

float CCartpole::get_theta_dot(void){
	return this->theta_dot;
}

float CCartpole::get_theta_dotdot(void){
	return this->theta_dotdot;
}

float CCartpole::get_x(void){
	return this->x;
}

float CCartpole::get_x_dot(void){
	return this->x_dot;
}

float CCartpole::get_x_dotdot(void){
	return this->x_dotdot; 
}

//Math functions
float CCartpole::sgn( float x ){
	if( x < 0 ) return -1;
	else if ( x  >= 0 ) return 1;
}

/*  To calculate sin of a number in degrees */ 
float CCartpole::sin2( float degrees ){
	return sin( degrees * M_PI / 180 );
}

/*  To calculate cosine of a  number in degrees */ 
float CCartpole::cos2( float degrees ){
	return cos( degrees * M_PI / 180 );
}
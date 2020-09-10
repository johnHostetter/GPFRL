#include "Mathrl.h"
#include <math.h>

CMathrl::CMathrl(void){}
CMathrl::~CMathrl(void){}

// Returns the result of multiplication operation
float CMathrl::cmul( float C_op_A, float C_op_B ){
	return ( C_op_A * C_op_B );
}

// Returns the logic "and"
float CMathrl::cand( float C_op_A, float C_op_B ){
	if ( C_op_A > C_op_B ) return ( C_op_B );
	else return( C_op_A );
}

// Returns the minimum of the set
float CMathrl::cmin( float C_op_A, float C_op_B ){
	if ( C_op_A > C_op_B ) return ( C_op_B );
	else return( C_op_A );
}

// Returns the maximum of the setfloat max(float x, float y);
float CMathrl::cmax( float C_op_A, float C_op_B ){
	if ( C_op_A > C_op_B ) return( C_op_A );
	else return ( C_op_B );
}

// Returns the s-shaped function to limit between -1 and 1
float CMathrl::csfunc( float C_s ){
	float temp;
	temp = (float) ( 1.0 / (1.0 + exp( C_s ) ) );
	return( temp );
}

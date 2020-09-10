#define SIGMA 0.62
#define MINANGLE -12
#define MAXANGLE 12

struct Input {
	float centre[2];
	float sigma[2];
};

class CFuzzy {
private:
	float C_c1;
	float C_c2;
	float C_c3;
	float C_c4;
	float C_c5;
	float C_c6;
	float C_u1;
	float C_u2;
	float C_u3;
	float C_u4;
	float mu[16];
	Input input1;
	Input input2;
	Input input3;
	Input input4;
public:
	int fail_flag;
	CFuzzy(void){}
	~CFuzzy(void){}
	void set_values(void);
	void set_centers(void);
	float gaussian( float x, float c, float s );
	float gaussian2( float x, float c, float s );
	void fuzzyfy( float x1,  float x2,  float x3,  float x4,  float x5,  float x6 );
	float get_mu( int  k );
	float get_centre( int  in,  int  c );
	float MIN( float a,  float b,  float c,  float d );
	void set_sigmas( void );
	void adapt_sigmas();
};
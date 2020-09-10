#define NSTATES 16
#define GAMMA 0.95

class CRLearning {
public:
	float C_r;
	float C_p;
	float C_P;
	float C_oldp;
	float C_rhat;
	float C_output;
	float C_w[NSTATES];
	float C_v[NSTATES];
	float C_am[NSTATES];
	float C_ro[NSTATES];
	float C_mu[NSTATES];
	float C_muhat[NSTATES];
	float C_e[NSTATES];
	CRLearning(void);
	~CRLearning(void);
	void reset(void);
	void reset_traces(void);
	void calc_output( float noise );
	void calc_rhat(void);
	void set_r( float r );
	void set_p( float p );
	void set_mu( int k, float mu );
	void calc_p(void);
	void calc_oldp(void);
	void upd_weights( float A, float B );
	void upd_traces(void);
	float TSugeno1(void);
	float TSugeno2(void);
	float sfunc( float s );
	float gfunc( float s );
	float get_rhat(void);
	float get_output(void);
	float get_p(void);
	float get_oldp(void);
};
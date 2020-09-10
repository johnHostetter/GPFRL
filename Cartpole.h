#ifndef MY_CARTPOLE
#define MY_CARTPOLE

class CCartpole {
	private:
		float theta;
		float theta_dot;
		float theta_dotdot;
		float x;
		float x_dot;
		float x_dotdot;
		float Nc;
		float frictionf;
	public:
		CCartpole();
		~CCartpole();
		void calculate_cart_pole( float force );
		void reset(void);
		float get_theta(void);
		float get_theta_dot(void);
		float get_theta_dotdot(void);
		float get_x(void);
		float get_x_dot(void);
		float get_x_dotdot(void);
		float sgn( float x );
		float sin2( float degrees );
		float cos2( float degrees );
};

#endif
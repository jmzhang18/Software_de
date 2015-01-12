#include <libplayerc++/playerc++.h>
#include <iostream>

#include <signal.h>


using namespace PlayerCc;

std::string  gHostname(PlayerCc::PLAYER_HOSTNAME);
uint32_t        gPort(PlayerCc::PLAYER_PORTNUM);
uint32_t        gIndex(0);


PlayerClient robot(gHostname, gPort);

bool run_loop = true;




#define TOL .5 
#define EVADE_TURN 20.
#define STOP_RANGE .5 


// We pass in speed and turnrate as var parameters,
// They are change or not
// returns 0 it ruining required, 1 if good
int avoid(RangerProxy* sp, double & speed, double &  turnrate)
{

	double distL = ((*sp)[1] + (*sp)[0]);
	double distR = ((*sp)[2] + (*sp)[0]);

	if ((distL + TOL) < distR){
		turnrate = dtor(EVADE_TURN); // turn  away
		printf("LEFT \n");
	}
	else

		if (distL > (distR + TOL)){
		turnrate = dtor(-EVADE_TURN); // turn  away
		printf("RIGHT \n");
		}
		else
			turnrate = 0.;

	if ((*sp)[0] < STOP_RANGE){
		if ((*sp)[3]>(*sp)[4])
			turnrate = dtor(-EVADE_TURN);
		else
			turnrate = dtor(EVADE_TURN);
		speed = 0;
		return(0);

	}
	else
		speed = 0.200;
	return(1);
}



int main(int argc, char **argv)
{

	try
	{

		Position2dProxy pp(&robot, gIndex);
		RangerProxy      sp(&robot, 0);

		robot.Read();
		sp.RequestGeom();
		sp.RequestConfigure();


		std::cout << robot << std::endl;

		pp.SetMotorEnable(true);
		// Turn on Motors

		std::cout << sp << std::endl;

		// go into read-think-act loop
		while (run_loop){
			double speed = 0.;
			double turnrate = 0.;

			// this blocks until new data comes; 10Hz by default
			robot.Read();

			avoid(&sp, speed, turnrate);

			// write commands to robot
			pp.SetSpeed(speed, turnrate);
		}
	}
	catch (PlayerCc::PlayerError & e)
	{
		std::cerr << e << std::endl;
		return -1;
	}
}
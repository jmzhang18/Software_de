/*
Move to Position
M. v. Mohrenschildt
Solution to Assigment 2013
*/

#include <libplayerc++/playerc++.h>
#include <iostream>

using namespace PlayerCc;


#define Square(x) ((x)*(x))


inline double angle_noral(double a)
{
	while (a < -M_PI) a += 2.0*M_PI;
	while (a >  M_PI) a -= 2.0*M_PI;
	return a;
};

/*-------------------------------------------------------*/

#define DIST_TOL .1
#define MAX_SPEED .2
#define MAX_TURN 40.0 /* this is 40 degrees per second */

int steer(player_pose2d_t target, Position2dProxy* pp)
{
	double dist, angle;

	dist = sqrt(Square(target.px - pp->GetXPos()) +
		Square(target.py - pp->GetYPos()));
	angle = atan2(target.py - pp->GetYPos(),
		target.px - pp->GetXPos());

	double yaw = pp->GetYaw();

	double ang_error = angle_noral(angle - yaw);
	double newspeed = 0.;
	double newturnrate = limit(rtod(ang_error), -MAX_TURN, MAX_TURN);
	newturnrate = dtor(newturnrate);

	if (dist > DIST_TOL) {
		newspeed = limit(dist * MAX_SPEED, -MAX_SPEED, MAX_SPEED);
	}
	else
		newspeed = 0.0;

	if (fabs(newspeed) < 0.01){
		pp->SetSpeed(0., 0.);
		std::cout << "READCHED GOAL " << std::endl;
		return(1);
	}
	pp->SetSpeed(newspeed, newturnrate);
	return(0);
}


int main(int argc, char **argv)
{

	Position2dProxy *pp;
	std::string  gHostname(PlayerCc::PLAYER_HOSTNAME);
	uint32_t        gPort(PlayerCc::PLAYER_PORTNUM);
	uint32_t        gIndex(0);
	player_pose2d_t target;

	target.px = -2.;
	target.py = 0.;
	target.pa = 0.;

	try{


		PlayerClient robot(gHostname, gPort);
		pp = new Position2dProxy(&robot, gIndex);

		std::cout << robot << std::endl;

		pp->SetMotorEnable(true);
		// Turn on Motors

		std::cout << " MOVING TO " << target << std::endl;
		// go into read-think-act loop
		for (;;){

			robot.Read();
			steer(target, pp);

		}
	}
	catch (PlayerCc::PlayerError & e){
		std::cerr << e << std::endl;
		return -1;
	}
}
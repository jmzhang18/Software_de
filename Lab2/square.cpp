//  Solution to Lab 2
//
// Dr. M. v. Mohrenschilt Spe 2012  Rev: Sep 22, 2012
//
// This solution uses two concepts:
// There are state machines for turning and straight
// The  turning and straight are controlled by error
// There is an overall state machine to control straight,turn
//
// Note, this implementation does relative turns, straight lines, there is an error
// accumulation over time, 
// I did not implement overshoot compensation, so we have to assume a minimum control
// rate. If control rate is to slow, speeds have to be reduced !!

#include <libplayerc++/playerc++.h>
#include <iostream>
#include <math.h>

using namespace PlayerCc;

    std::string  gHostname(PlayerCc::PLAYER_HOSTNAME);

// Constants
#define PI_TWO  3.14159265358979323846  *2.
#define PI  3.14159265358979323846 
#define PI_HALF  3.14159265358979323846 *.5

//Macros
#define Square(x) ((x)*(x))

/*-------------------------------------------------------------*/
// Go straight for one meter Mode
// Call go_straight_init initially
// Call go_straight repeatedly into it returns 0

#define STRAIGHT_DISTANCE 1.
#define STRAIGH_MAX_SPEED .5
#define STRAIGH_CRAWL_SPEED .1

#define STRAIGHT_TOLERANCE 0.1

// State variables
double goal_x;
double goal_y;

void go_straight_init(Position2dProxy* pp)
{
	goal_x=pp->GetXPos()+ STRAIGHT_DISTANCE * cos(pp->GetYaw());
	goal_y=pp->GetYPos()+ STRAIGHT_DISTANCE * sin(pp->GetYaw());

	printf(" Straight Move to %f %f \n" ,goal_x,goal_y);

}
int go_straight(Position2dProxy* pp)
{
	// How far do we still have to go? 	
	double dist = sqrt( Square((goal_x - pp->GetXPos()))+
		     	    Square((goal_y - pp->GetYPos())));

	double speed;
	if(dist>1.) // Proportional speed control
		speed=STRAIGH_MAX_SPEED;
	else
		speed= dist* STRAIGH_MAX_SPEED +STRAIGH_CRAWL_SPEED;

	pp->SetSpeed(speed,0.);

	
	if(dist<STRAIGHT_TOLERANCE)
		return(0);
	else
		return(1);
}

/*-------------------------------------------------------------*/

// Turn Left 90 Degrees Mode
// Call turn_left_int initially
// Call turn_let into it returns 0

#define TURN_MAX_SPEED 1. 
#define TURN_CRAWL_SPPED .01

#define TURN_TOLERANCE  .01

// Stage variable
double  turn_to;

void turn_left_int(Position2dProxy* pp)
{
	turn_to=pp->GetYaw() + PI_HALF;
	// NOTE GetYaw returns negative numbers for lower quadarnts !!
	if(turn_to>PI)turn_to=turn_to-PI_TWO;

	printf(" Turn Turn to %f \n",turn_to);
}


int turn_left(Position2dProxy* pp)
{
	// Note, we do not  check for overshoot here!!
	double error=fabsf(turn_to-pp->GetYaw());
	double turn_rate;
	if(error>1.) // Proportional turn speed control
		turn_rate=TURN_MAX_SPEED;
	else
		turn_rate=TURN_MAX_SPEED*error+TURN_CRAWL_SPPED;

	pp->SetSpeed(0.,turn_rate);

	if(error<TURN_TOLERANCE)
		return(0);
	else
		return(1);
}

/*-------------------------------------------------------------*/


enum mode_list {STRAIGHT, TURN};

mode_list mode; // State variable

// We should impletent the -p arument for port here (I show this another time)
int main(int argc, char **argv)
{
  // we throw exceptions on creation if we fail
  try {

    PlayerClient robot(gHostname, 6665 ); // Conect to server
    Position2dProxy pp(&robot, 0);   // Get a motor control device (index is 0)

    std::cout << robot << std::endl;

    pp.SetMotorEnable (true); // Turn on Motors

    robot.Read(); // Start Data flow

    // set initial mode
    mode=STRAIGHT;
    go_straight_init(&pp);

    for(;;){

	// this blocks until new data comes; 10Hz by default
	robot.Read();

		if(mode==STRAIGHT){
			if(go_straight(&pp)==0){ // 
				turn_left_int(&pp);
				mode=TURN;
			}
		}else{ // This means mode==TURN
			if(turn_left(&pp)==0){
				go_straight_init(&pp);
				mode=STRAIGHT;
			}
		}
    }

  } catch (PlayerCc::PlayerError & e){
    std::cerr << e << std::endl;
    return -1;
  }
}


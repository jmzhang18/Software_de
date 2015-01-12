#include <libplayerc++/playerc++.h>
#include <iostream>

#include <queue>

#include "stdio.h"

using namespace PlayerCc;

std::string  gHostname(PlayerCc::PLAYER_HOSTNAME);
uint32_t        gPort(PlayerCc::PLAYER_PORTNUM);
uint32_t        gIndex(0);

Position2dProxy *pp;

bool turn_decition;

/* ---*/
typedef struct pos_data{
	double x;
	double y;
} pos_data;


class Position_data
{
public:
	Position_data(double x, double y)
	{
		p_x = x;
		p_x = x;
	};
	double get_x(void){ return(p_x); };
	double get_y(void){ return(p_x); };
private:
	double p_x;
	double p_y;
};

std::queue<pos_data*> path_data;

std::queue<Position_data*> position_data;




#define MAX_SPEED .5
#define MAX_TURN 40 /* this is 40 degrees per second */

//return 1 if we are there
int steer(player_pose2d_t target, Position2dProxy* pp)
{
	using namespace PlayerCc;

	double dist, angle;

	//printf("GOAL X = %f Y=%f \n",target.px,target.py);

	dist = sqrt((target.px - pp->GetXPos())*
		(target.px - pp->GetXPos()) +
		(target.py - pp->GetYPos())*
		(target.py - pp->GetYPos()));


	angle = atan2(target.py - pp->GetYPos(),
		target.px - pp->GetXPos());

	double ang_error = angle - pp->GetYaw();

	// Reverse steering
	if (ang_error> M_PI_2)
		ang_error = -ang_error;
	else if (ang_error<-1.*M_PI_2)
		ang_error = -ang_error;



	//std::cout <<"TO MOVE ERROR "   <<dist << "  " <<rtod(angle) <<
	//" "<< rtod(pp->GetYaw()) <<std::endl;

	// 
	//turn_decition


	double newturnrate = 0;
	double newspeed = 0;

	newturnrate = limit(rtod(ang_error), -40.0, 40.0);
	newturnrate = dtor(newturnrate);

	if (dist > 0.1) {
		newspeed = MAX_SPEED;
	}
	else
		newspeed = 0.0;

	if (fabs(newspeed) < 0.05){
		pp->SetSpeed(0., 0.);
		std::cout << "READCHED GOAL " << std::endl;
		return(1);
	}
	//std::cout <<"Command : "   <<newspeed<< "  " <<rtod(newturnrate)<<std::endl;
	pp->SetSpeed(newspeed, newturnrate);

	return(0);
}

void push(double x, double y)
{
	pos_data* tmp;
	tmp = (pos_data*)malloc(sizeof(pos_data));
	tmp->x = x;
	tmp->y = y;
	path_data.push(tmp);
}


void new_push(double x, double y)
{
	Position_data* tmp = new Position_data(x, y);
	position_data.push(tmp);
}

pos_data* get_next(void)
{
	pos_data* out;
	out = path_data.front();

	printf("POPPED X = %f Y=%f \n", out->x, out->y);
	path_data.pop();
	path_data.push(out); // Make cicular
	return(out);
}

Position_data* new_get_next(void)
{
	Position_data* out;
	out = position_data.front();
	printf("POPPED X = %f Y=%f \n", out->get_y(), out->get_x());
	position_data.pop();
	position_data.push(out); // Make cicular
	return(out);
}


int read_data(const char* name)
{
	FILE * in_data;
	in_data = fopen(name, "r");
	if (in_data == 0){
		printf("WRONG FILE NAME %s \n", name);
		exit(1);
	}

	float val_x, val_y;
	while (fscanf(in_data, "%f", &val_x) != EOF){
		fscanf(in_data, "%f", &val_y);
		printf(" x=%f y=%f \n", val_x, val_y);
		push(val_x, val_y);
	}


}






int main(int argc, char **argv)
{
	//parse_args(argc,argv);

	// we throw exceptions on creation if we fail

	read_data("path.data");

	turn_decition = false;

	player_pose2d_t target;
	pos_data* tmp = get_next();
	target.px = tmp->x;
	target.py = tmp->y;
	try {


		PlayerClient robot(gHostname, gPort);
		pp = new Position2dProxy(&robot, gIndex);

		std::cout << robot << std::endl;

		pp->SetMotorEnable(true);
		// Turn on Motors



		// go into read-think-act loop
		for (;;){
			double newspeed = 0;
			double newturnrate = 0;

			// this blocks until new data comes; 10Hz by default
			robot.Read();


			if (steer(target, pp)){ // We reached target, get next target
				pos_data* tmp = get_next();
				target.px = tmp->x;
				target.py = tmp->y;
			}
			//std::cout << "POS : " <<
			//pp->GetXPos() << "  " << pp->GetYPos() <<  " " << pp->GetYaw ()<<  std::endl;
		}
	}
	catch (PlayerCc::PlayerError & e)
	{
		std::cerr << e << std::endl;
		return -1;
	}
}
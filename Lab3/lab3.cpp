#include <libplayerc++/playerc++.h>
#include <iostream>
#include <fstream>

using namespace PlayerCc;

std::string  gHostname(PlayerCc::PLAYER_HOSTNAME);
uint32_t        gPort(PlayerCc::PLAYER_PORTNUM);
uint32_t        gIndex(0);

std::filebuf out_file;

#define ROBOT_SPEED .3

//---------------------------------------------------
// Random Walk 

// Stage variable Integrator used for turning
double turn_dir_int = 0.;

void random_walk(Position2dProxy* pp)
{

	const int rand_num = rand();

	// This works nicely, same leaky intgrator just different from to write it
	turn_dir_int = .9*turn_dir_int + 0.007 * ((double)(100 - (rand_num % 200)));


	// This  works too, but need more turning
	//const double rand_val =  ((double)( 100 - ( rand_num % 200))) /50.; // RANDOM VALUE -2 .. 2
	//turn_dir_int = turn_dir_int + .95 * (rand_val - turn_dir_int);

	pp->SetSpeed(ROBOT_SPEED, turn_dir_int);

}

/*---------------------------------------*/
// Logger (it is implemented as a class here!
// Note, this now keeps the state variables and the
// functions (called methodes) together

class MyLogger
{
public:
	MyLogger(const char* name);
	~MyLogger();
	void writeLog(Position2dProxy* pp);
private:

	std::ofstream out_file;

};

MyLogger::MyLogger(const char* name)
{
	out_file.open(name);
}

MyLogger ::~MyLogger()
{
	out_file.close();
}

void MyLogger::writeLog(Position2dProxy* pp)
{
	out_file << pp->GetXPos()
		<< "  "
		<< pp->GetYPos()
		<< "  "
		<< pp->GetYaw()
		<< std::endl;
}


/*---------------------------------------*/



int main(int argc, char **argv)
{

	MyLogger logger("log.data");

	srand(time(NULL));

	try {

		PlayerClient robot(gHostname, gPort); // Conect to server
		Position2dProxy pp(&robot, gIndex);   // Get a motor control device

		std::cout << robot << std::endl;

		pp.SetMotorEnable(true); // Turn on Motors


		// go into  a loop
		for (;;){

			// this blocks until new data comes; 10Hz by default
			robot.Read();

			random_walk(&pp);

			// write out data:
			logger.writeLog(&pp);

		}
	}
	catch (PlayerCc::PlayerError & e) {
		std::cerr << e << std::endl;
		return -1;
	}
}
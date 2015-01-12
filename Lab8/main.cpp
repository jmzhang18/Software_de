
#include <libplayerc++/playerc++.h>
#include <iostream>
#include <math.h>

#include "Robot.h"


using namespace PlayerCc;

std::queue<Behaviors*> behav_queue;

std::string  gHostname(PlayerCc::PLAYER_HOSTNAME);


// We should impletent the -p arument for port here (I show this another time)
int main(int argc, char **argv)
{
  try {

    PlayerClient r_server(gHostname, 6665 ); // Conect to server

    ZickZackRobot* my_robot;
    my_robot=new ZickZackRobot(&r_server,0 ,1);

    r_server.Read(); // Start Data flow
    r_server.Read(); // Start Data flow

    my_robot->Init();

    for(;;){

	// this blocks until new data comes; 10Hz by default
	r_server.Read();

	my_robot->Tick();

    }

  } catch (PlayerCc::PlayerError & e){
    std::cerr << e << std::endl;
    return -1;
  }
}


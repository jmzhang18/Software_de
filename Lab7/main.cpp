
#include <libplayerc++/playerc++.h>
#include <iostream>
#include <math.h>

#include "Behaviors.h"



using namespace PlayerCc;

std::queue<Behaviors*> behav_queue;

std::string  gHostname(PlayerCc::PLAYER_HOSTNAME);


// We should impletent the -p arument for port here (I show this another time)
int main(int argc, char **argv)
{
  try {

    PlayerClient r_server(gHostname, 6665 ); // Conect to server
    Position2dProxy* pos_proxy= new Position2dProxy(&r_server,0);

    Behaviors* behave;
    char mode;

    Behaviors* streight=new MoveStraight(pos_proxy);
    ((MoveStraight*)streight)->Init(2.);
    Behaviors* turn= new Turn(pos_proxy);
    ((Turn*)turn)->Init(M_PI/2.);


    r_server.Read(); // Start Data flow
    r_server.Read(); // Start Data flow

    behave=streight;
    mode=1;


    for(;;){
		r_server.Read();
        	if(behave->Tick()){
                	//Change the behaviour
                	std::cout<< "DONE NEXT STEP"<<std::endl;

			if(mode==1){
				behave=turn;
				mode=0;
			}else{
				behave=streight;
				mode=1;
			}
			behave->Resume();
                	// Request a new BEHAVIOR
        	}else{
			// Check Collision
		        behave->DoMove();

		}
                
        }

  } catch (PlayerCc::PlayerError & e){
    std::cerr << e << std::endl;
    return -1;
  }
}


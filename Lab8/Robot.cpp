
#include <libplayerc++/playerc++.h>

#include "Robot.h"



using namespace PlayerCc;


Robot::Robot(PlayerClient* client, int robot_index,int ranger_index)
{
	pos_proxy= new Position2dProxy(client,robot_index);
	ranger_proxy = new RangerProxy(client,ranger_index);

	default_behave=new Wait(pos_proxy);

	// We should allow overwirted for this too!!
	col_check = new LaserCollision(pos_proxy,ranger_proxy);

	// Create default 
	normal_behave= ChangeBevaviour(); // Get from child class
	collicion_behave= ChangeCollisionBevaviour();
}


Robot::~Robot() 
{
}

void Robot::Init(void)
{
        // Create default 
        normal_behave= ChangeBevaviour(); // Get from child class
        collicion_behave= ChangeCollisionBevaviour();

}



void Robot::Tick(void)
{

        if(normal_behave->Tick()){
                //Change the behaviour
                std::cout<< "DONE NEXT STEP"<<std::endl;
		// Request a new BEHAVIOR
		normal_behave=this->ChangeBevaviour();// 
                normal_behave->Resume();
        }else{
		if(
                	col_check->CheckMovement(normal_behave->GetIntendedSpeed(),
					    normal_behave->GetIntendedRotation())

		){
			printf("Collition MODE \n");
			// Let us determine which is the right Collistion Behavior
			collicion_behave=ChangeCollisionBevaviour();
			
                        collicion_behave->Resume();
                        collicion_behave->Tick();
                        collicion_behave->DoMove();
			normal_behave->Resume(); 
		}else{
                	normal_behave->DoMove();
		}
        }


}


Behaviors* Robot::ChangeBevaviour(void)
{
	return default_behave;
}


Behaviors* Robot::ChangeCollisionBevaviour(void)
{
	return default_behave;
}

/*----------------------------------------------------------------------------*/


ZickZackRobot::ZickZackRobot(PlayerClient* client, int robot_index,int ranger_index):
		Robot(client, robot_index,ranger_index)

{
	streight=new MoveStraight(pos_proxy);
        ((MoveStraight*)streight)->Init(2.);
	turn= new Turn(pos_proxy);
	((Turn*)turn)->Init(M_PI/20);

	mode=STREIGHT;
}


ZickZackRobot::~ZickZackRobot()
{
	delete turn;
	delete streight;
}

Behaviors* ZickZackRobot::ChangeBevaviour(void)
{
	return(streight);
}

Behaviors* ZickZackRobot::ChangeCollisionBevaviour(void)
{
	if(mode==LEFT)
		((Turn*)turn)->Init(M_PI/20);
	else
		((Turn*)turn)->Init(-M_PI/20);
	return(turn);
}


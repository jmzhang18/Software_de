
#include <libplayerc++/playerc++.h>

#include "Behaviors.h"

#include "Colission.h"




namespace PlayerCc
{

//! We assume that each  robot has a Position Proxy and A Raner Proxy
class Robot
{
	public:
		Robot(PlayerClient* client, int robot_index,int ranger_index);

		//! De allocate all resources, Note, clients should overwrite this
		//! if the have aditional resources
		~Robot();
		

		//! Get Robot to preapre for move
		void Tick(void); 

		//! Initialize the classes, Call before first tick !!!
		void Init(void);

		virtual Behaviors* ChangeBevaviour(void); //! Overwrite this to create bevavior
		virtual Behaviors* ChangeCollisionBevaviour(void);//! Overwrite this to create bevavior

	protected:
		Robot();
		Position2dProxy* pos_proxy;
		RangerProxy*  ranger_proxy;

		Behaviors* normal_behave; // Behavior we want
		Behaviors* collicion_behave; //Behavior we do if we have collision

		Behaviors* default_behave; //Behavior we do if we have collision

		Collision* col_check;

};

//! The ZickZack robot moves streight until it encounters obstacle then starts to rotat in
//! 10 degree incements until it sees free space to move further
//! It is a very easy implementation but show how to use this framework

class ZickZackRobot: public Robot
{
	public:
		ZickZackRobot(PlayerClient* client, int robot_index,int ranger_index);
		~ZickZackRobot();

		 Behaviors* ChangeBevaviour(void);
		 Behaviors* ChangeCollisionBevaviour(void);

	private:
		enum move_mode {STREIGHT,LEFT,RIGHT};
		move_mode mode;
		Behaviors* streight;
		//Behaviors* turn;
		Turn* turn;
};

};


#include <libplayerc++/playerc++.h>



namespace PlayerCc
{

//! ABSTRACT COLLISTION DETECTION INTERFACE
class Collision
{
	public:
		Collision(Position2dProxy*pp , RangerProxy*rp )
		{
			pos_proxy=pp;
			ranger=rp;
		};

		virtual bool CheckMovement(double speed, double rotation)=0;

		//! Give a direction that has "free space to move into
		virtual double DirectionOfFreeSpace(void)=0;

		void GetInfo(void);

	protected:
		Position2dProxy* pos_proxy;
		RangerProxy*  ranger;
		


};


class LaserCollision: public Collision
{
	public:
		LaserCollision(Position2dProxy* , RangerProxy* );
		bool CheckMovement(double speed, double rotation);
		double DirectionOfFreeSpace(void);

};


};

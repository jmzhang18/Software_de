
#ifndef BEHAVIORS_H
#define BEHAVIORS_H


#include <libplayerc++/playerc++.h>

#include <queue>

namespace PlayerCc
{




class Behaviors
{

	public:
     		Behaviors(Position2dProxy* pp);
 		// Constructor (Default constructor is private)
     		virtual void Init(void){}; 
		// Overwritten by childeren to given them information
     		virtual bool Tick(void) =0; 
		//  Called every frame, true is Behavior complete
     		virtual void Resume(void){}; 
		// Resume after other behavior was done
     		double GetIntendedSpeed(){return(speed);};
     		double GetIntendedRotation(){return(rotation);};

     		void DoMove(void); 
		//Perform the intended move (not overwriten 
		
   	protected:
     		double speed,rotation; 
		// The intended speed in this frame
     		Position2dProxy* posProxy;
		 // The position proxy to obain information !!

   	private:
     		Behaviors(){}; // Private !!

};

class Wait:public Behaviors
{
        public:
                Wait(Position2dProxy* pp);
                bool Tick(void){return(true);};
};



class MoveStraight: public Behaviors
{
	public:

		MoveStraight(Position2dProxy* pp);
		void Init(double d); // Distance to Move
		bool Tick(void);
		
		void Resume(void);
	private:
                double goal_x;
                double goal_y;

		double distance; // Stored for Resume
};



class Turn: public Behaviors
{
        public:

                Turn(Position2dProxy* pp);
                void Init(double d); // Angle to turn to
                bool Tick(void);

		void Resume(void);
        private:
		double  turn_to;

		double angle; // For Resume

};

// Random movement
class Random: public Behaviors
{
        public:
                Random(Position2dProxy* pp);
                bool Tick(void);
        private:

};

class MovePoint: public Behaviors
{
        public:

                MovePoint(Position2dProxy* pp);
                void Init(double x, double y);  // Point to move to
		void SetNowSlow(bool par){noSlow=par;}; // Switch if stop at goal or not
                bool Tick(void);

        private:
		double pos_x,pos_y;

		bool noSlow;

};


/*-----------------------------------------------*/


class MoveNetwork: public Behaviors
{
        public:

                MoveNetwork(Position2dProxy* pp);
		~MoveNetwork(); // CLEAN UP PATH DATA !!
                void Init(const char* name);  // File name
                bool Tick(void);

        private:
		bool path_ok;
		MovePoint * mover;
		std::queue<player_pose2d_t*> path_data;

		bool readData(const char* name); 
		player_pose2d_t* nextPos(void);
		player_pose2d_t* nextPosLoop(void);
		void pushPos(double x, double y);
};


}
#endif

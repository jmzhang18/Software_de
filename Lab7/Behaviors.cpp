
#include <libplayerc++/playerc++.h>
#include "Behaviors.h"

using namespace PlayerCc;


// Constants
#define PI_TWO  3.14159265358979323846  *2.
#define PI  3.14159265358979323846 
#define PI_HALF  3.14159265358979323846 *.5

//Macros
#define Square(x) ((x)*(x))





Behaviors:: Behaviors(Position2dProxy* pp)
{
	posProxy=pp;
	speed=0.;
	rotation=0.;
}


void Behaviors::DoMove(void)
{
	//printf("MOVE %f %f \n",speed,rotation);
	posProxy->SetSpeed(speed,rotation);
}


/*--------------------*/
// WAIT 

Wait::Wait(Position2dProxy* pp):Behaviors(pp)
{
}



/*--------------------*/
// STREIGHT

MoveStraight::MoveStraight(Position2dProxy* pp):Behaviors(pp)
{
  goal_x=0.;
  goal_y=0.;
  distance=0.;
}

void MoveStraight::Init(double d)
{
  distance=d;
  goal_x=posProxy->GetXPos()+ d* cos(posProxy->GetYaw());
  goal_y=posProxy->GetYPos()+ d* sin(posProxy->GetYaw());

  printf(" Straight Move to %f %f \n" ,goal_x,goal_y);
}


void MoveStraight::Resume(void)
{
        Init(distance);
}

#define STRAIGH_MAX_SPEED .5
#define STRAIGH_CRAWL_SPEED .1
#define STRAIGHT_TOLERANCE 0.1


bool MoveStraight::Tick(void)
{
        // How far do we still have to go?      
        double dist = sqrt( Square((goal_x - posProxy->GetXPos()))+
                            Square((goal_y - posProxy->GetYPos())));

        if(dist>1.) // Proportional speed control
                speed=STRAIGH_MAX_SPEED;
        else
                speed= dist* STRAIGH_MAX_SPEED +STRAIGH_CRAWL_SPEED;

	rotation=0.;
	
        //posProxy->SetSpeed(speed,0.);


        if(dist<STRAIGHT_TOLERANCE)
                return(true);
        else
                return(false);

	return(true);
}


/*-------------------------------------------------------------*/
//TRUN 

// Turn Left 90 Degrees Mode
// Call turn_left_int initially
// Call turn_let into it returns 0

#define TURN_MAX_SPEED 1. 
#define TURN_CRAWL_SPPED .01

#define TURN_TOLERANCE  .01



Turn::Turn(Position2dProxy* pp):Behaviors(pp)
{
	turn_to=0.;
	angle=0.;
}

void Turn::Init(double a)
{
	angle=a;
	turn_to=posProxy->GetYaw() +a;
	if(turn_to>PI)turn_to=turn_to-PI_TWO;
	printf(" Turn Turn to %f \n",turn_to);
}

void Turn::Resume(void)
{
	Init(angle);
}


bool Turn::Tick(void)
{
        // Note, we do not  check for overshoot here!!
        double error=fabsf(turn_to-posProxy->GetYaw());
        double turn_rate;
        if(error>1.) // Proportional turn speed control
                turn_rate=TURN_MAX_SPEED;
        else
                turn_rate=TURN_MAX_SPEED*error+TURN_CRAWL_SPPED;


	speed=0.;
        rotation=turn_rate;
        //posProxy->SetSpeed(0.,turn_rate);

        if(error<TURN_TOLERANCE)
                return(true);
        else
                return(false);
}


/*-------------------------------------------------------------*/
//  RANDOM

#define ROBOT_SPEED .3

Random::Random(Position2dProxy* pp):Behaviors(pp)
{
	// Initialize Random Seed
	 srand(time(NULL));
	 rotation=0;
}

bool Random::Tick(void)
{
	const int rand_num =rand();
	rotation= .9*rotation+ 0.007 * ((double)( 100 - ( rand_num % 200)));
	speed=ROBOT_SPEED;
	
	//return(true);
	return(false); // Make it blocking!
}

/*-------------------------------------------------------------*/
// MOVE TO POINT



MovePoint::MovePoint(Position2dProxy* pp):Behaviors(pp)
{
	pos_x=0;
	pos_y=0;
	noSlow=false;

}
void MovePoint::Init(double x, double y)
{
	pos_x=x;
	pos_y=y;
	printf(" Point MOING TO = %f Y=%f \n",pos_x,pos_y);
}

#define MAX_SPEED .2
#define MAX_TURN 40 /* this is 40 degrees per second */

bool MovePoint::Tick(void)
{
  double dist, angle;

  dist = sqrt(Square(pos_x - posProxy->GetXPos())+
              Square(pos_y - posProxy->GetYPos()));


  angle = atan2(pos_y - posProxy->GetYPos(),
                pos_x - posProxy->GetXPos());

  if(angle<0)angle+=M_2_PI;
  double yaw=posProxy->GetYaw();
  if(yaw<0)yaw+=M_2_PI;

  double ang_error=angle-yaw;


 /*
   double ang_error=angle-posProxy->GetYaw();
       // Reverse steering
  if((ang_error> M_PI_2)){// && (pp->GetYaw()>0.)){
         ang_error=-ang_error;
         printf("LEFT \n");
  }else{ if(ang_error<-1.*M_PI_2){
         ang_error=-ang_error;
         printf("RIGHT \n"); 
	}
  }
  */

   rotation= limit(rtod(ang_error)   , -40.0, 40.0);
   rotation= dtor(rotation);

  if(noSlow){ // SLOW DOWN AT TARGET POINT
  	if (dist > 0.1) 
    		speed= limit(dist * MAX_SPEED, -MAX_SPEED, MAX_SPEED);
   	else
    		speed= 0.0;
  	if (fabs(speed) < 0.01){
    		speed=0.;
    		rotation=0.;
    		return(true);
  	}
  	return(false);
   }else{ // Continue moving at target point
	speed=MAX_SPEED;
	if(fabs(dist)<0.05)
		return(true);
	else
		return(false);
	
   }

}


/*-------------------------------------------------------------*/
// MOVE ON NETWORK





MoveNetwork::MoveNetwork(Position2dProxy* pp):Behaviors(pp)
{
	mover= new MovePoint(pp);
	mover->SetNowSlow(false);
	path_ok=false;
}

void MoveNetwork::Init(const char* name)
{
	path_ok=readData(name);
	// Initialize the mover !!
	if(path_ok){
		player_pose2d_t* pos=nextPos();
		printf(" Network MOING TO = %f Y=%f \n",pos->px,pos->py);
		mover->Init(pos->px,pos->py);
	}
	
}

bool MoveNetwork::Tick(void)
{
	// if queue is empty do not move
   if(path_ok){
	if(mover->Tick()){
	  // GET net target
	  player_pose2d_t* pos=nextPos();
	  //if(pos==NULL)return(true);
	  printf("XX MOING TO = %f Y=%f \n",pos->px,pos->py);
	  mover->Init(pos->px,pos->py);
        }
	rotation= mover->GetIntendedRotation();
	speed=mover->GetIntendedSpeed();
	return(false);
	
   }else
     return(true);
}


/*  LOAL */
//MovePoint * mover;

bool MoveNetwork::readData(const char* name)
{
        FILE * in_data;
        in_data=fopen(name,"r");
        if(in_data==0){
                printf("WRONG FILE NAME %s \n",name);
                exit(1);
        }

        float val_x,val_y;
        while(fscanf(in_data,"%f",&val_x)!=EOF){
                fscanf(in_data,"%f",&val_y);
                printf(" x=%f y=%f \n",val_x,val_y);
                pushPos(val_x,val_y);
        }
	if(path_data.size()>0)
		return(true);
	else
		return(false);
}

player_pose2d_t* MoveNetwork::nextPos(void)
{
        player_pose2d_t* out;
        out=path_data.front();

        printf("POPPED X = %f Y=%f \n",out->px,out->py);
        path_data.pop();
        path_data.push(out); // Make cicular
        return(out);
}


void MoveNetwork::pushPos(double x, double y)
{
        player_pose2d_t* tmp;
        tmp=(player_pose2d_t*)malloc(sizeof(player_pose2d_t));
        tmp->px=x;
        tmp->py=y;
        path_data.push(tmp);
}


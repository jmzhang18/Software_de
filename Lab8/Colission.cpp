
#include "Colission.h"

using namespace PlayerCc;


void Collision::GetInfo(void)
{
        // OK there is the GetElementCount 1 for laster n for sonar

        std::cout << "-------------------------"<<std::endl;
        std::cout << "Ranger has " << ranger->GetElementCount() << "Elements"<<std::endl;

        std::cout << "Ranger has " << ranger->GetRangeCount() << "Rays "<<std::endl;
        std::cout << "Min Angle " << rtod(ranger->GetMinAngle()) <<  std::endl;
        std::cout << "Max Angle " << rtod(ranger->GetMaxAngle()) <<  std::endl;
        std::cout << "Angular Resulution " << rtod(ranger->GetAngularRes()) <<  std::endl;
        std::cout << "Scanning Frequency " << ranger->GetFrequency()  <<  std::endl;
        //double deg_per_angle= (ranger->GetMaxAngle() - ranger->GetMinAngle()) / (double)ranger->GetRangeCount();

        std::cout << "-------------------------"<<std::endl;
}



typedef struct ranger_map{
        double min_right;
        double right_angle;
        double min_left;
        double left_angle;
} ranger_map;



void compute_laser(RangerProxy* lp,ranger_map* r_map)
{
	

        uint32_t num =lp->GetRangeCount();

        r_map->min_right=100.;
        // MIN Right
        for (uint32_t i = 0; i <num/2; i++){
          if(((*lp)[i]) && (*lp)[i] < r_map->min_right){
            r_map->min_right=(*lp)[i];
            r_map->right_angle= lp->GetMinAngle() + i *lp->GetAngularRes();
           }
        }
        r_map->min_left=100.;
        //for (uint32_t i =num/2 ; i <num; i++){
        for (uint32_t i = num ; i >num/2; i--){ // Scan other Way !
          if(((*lp)[i]) && (*lp)[i] < r_map->min_left){
            r_map->min_left=(*lp)[i];
            r_map->left_angle= lp->GetMinAngle() + (i+1) *lp->GetAngularRes();
           }
        }

        std::cout<<"MIN Left  "<<r_map->min_left << "  Angle "<<rtod(r_map->left_angle)<< std::endl;
        std::cout<<"MIN Right "<<r_map->min_right<< "  Angle "<<rtod(r_map->right_angle)<< std::endl;

}




double obst_avoid(RangerProxy* lp)
{

  // Determine the best way to turn and how much (yes we could still get stuck in local minimum
  // We decide by the abount of open space, adding up open space on left and right with
  // a weight, 

   double left_total,right_total;
   double weight;

        //first check if there is something directly in front, so we have to stop

        uint32_t num =lp->GetRangeCount();
        double weight_step = 1./(double)(lp->GetRangeCount()/2);

        //LEFT
        left_total=0.;
        weight=0.;
        for (uint32_t i =num/2 ; i <num; i++){
          if(((*lp)[i])){
                left_total+= (*lp)[i] * weight ;
           }
           weight+=weight_step;
        }

        // RIGHT
        right_total=0.;
        weight=1.;
        for (uint32_t i = 0; i <num/2; i++){
          if(((*lp)[i]) ){
            right_total+=(*lp)[i] *weight;
           }
           weight-=weight_step;
        }
        std::cout << "let total " << left_total << "right total " <<right_total <<std::endl;
	if(left_total<right_total)
		return(-1.);
	else
		return(1);
}










LaserCollision::LaserCollision(Position2dProxy* pp , RangerProxy* lp ):Collision(pp,lp)
{

    printf("Request \n");
    ranger->RequestGeom();
    ranger->RequestConfigure();


    GetInfo();

}

bool LaserCollision::CheckMovement(double speed, double rotation)
{

	// Determine the point we want to move to

	if(speed==0.)return(true); // We can always rotate

	double dt=.1; // We move at 10 Hz
	double angle =  rotation*dt;
	double distance = rotation*dt;
	distance+=.5; // Add Safety margine

	// Now this is hard codes for the laser we have, this could be done more genera!!
	
 	// Find the distance at that angle (note, -Pi/2 < angle < PI/2
	// Now this is easy for out laster since it has 180 rays, -90 is first all the way to 90
	// Find the laser ranger ray index to that angle
	int rat = rtod(angle);
	const double space = (*ranger)[rat+90]; // This is just to show (debug !!)
	printf("Check at angle %d has space %f \n",rat,space);

	// Now note, our robot is not a point but as it moves it forms a tube, so we in face
	// Have to check inside for rube intersection, but rays from a cone, so we do it like
	// this
	//const int  num_ray=ranger->GetElementCount();
	const int num_ray=180;
	bool result=false;
	const double safe_dist=.7;
	for(int i = 0;i<num_ray;i++){
		if((*ranger)[i]<safe_dist){
			result=true;
			printf("Space Violation \n");
			break;
		}
	}


        return(result);
}


double LaserCollision::DirectionOfFreeSpace(void)
{
	ranger_map map_data;
	compute_laser(ranger,&map_data);
	return(0.);
}





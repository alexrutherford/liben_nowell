#include <vector>
//#include <tr1>
#include <boost/random/lognormal_distribution.hpp>
#include <boost/random/poisson_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/uniform_int.hpp>
// In /usr/include/boost

using namespace std;
//using namespace std::tr1;

///////////
struct balloon
///////////
	{

		int cell;
		int nFound;

/*		balloon(int dummyCell)
		// Constructor
		{

			cell=dummyCell;
			nFound=0;

		}
*/
	};
///////////
struct timedRecruit
///////////
	{
		int cell;
		float actionTime;

		timedRecruit(int dummyCell,float dummyActionTime)
		// Constructor
		{
			cell=dummyCell;
			actionTime=dummyActionTime;
		}	

	};

///////////
class compareTimedRecruits
///////////
	{

		public:
		bool operator()(timedRecruit recruit1,timedRecruit recruit2)
		{
		//	if(recruit1.actionTime<recruit2.actionTime)
			if(recruit1.actionTime>recruit2.actionTime)
			{return true;}
			else
			{return false;}
		}

	};

///////////
struct lnEntry
///////////
	{
		int id;
		vector<int> ids;
		vector<double>rank;

		lnEntry(int dummyIndex)
		// Constructor
		{
		//	id=-1;
			id=dummyIndex;	
		}

	};

///////////
double distanceOnEarth(double lat1,double lat2,double long1,double long2,bool debug)
///////////
	{

		if ((lat1==lat2)&&(long1==long2))
			{
				return 0.0;
			}

		double temp=0.0;

		if(debug)
		{printf("HERE\n");fflush(stdout);}

		long1=(90.0-long1)*radConversion;
		long2=(90.0-long2)*radConversion;
	
		if(debug)
		{printf("HERE 2\n");fflush(stdout);}
	
		lat1=lat1*radConversion;
		lat2=lat2*radConversion;
	
		if(debug)
		{printf("HERE 3\n");fflush(stdout);}

		temp=sin(long1)*sin(long2)*cos(lat1-lat2)+cos(long1)*cos(long2);

		if(debug)
		{printf("HERE 4\n");fflush(stdout);}

		if(debug)
		{
		printf("TEMP - %f\n",temp);
		fflush(stdout);
		printf("RADIUS - %d\n",earthRadius);
		fflush(stdout);

		printf("RESULT - %f\n",acos(temp));
		fflush(stdout);

		printf("FINAL RESULT - %f\n",acos(temp)*earthRadius);
		fflush(stdout);

		}

		return acos(temp)*earthRadius;	

	}

//////////////////////////////////
void printTime()
//////////////////////////////////
	{
		time_t rawtime;
		struct tm * timeinfo;

		time(&rawtime);
		timeinfo=localtime(&rawtime);
					
	//	printf("%s\n",asctime(timeinfo));
		printf("--------------------\n\t%d:%d:%d\n--------------------\n",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
	}







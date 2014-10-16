////////////////////////
// Alex Rutherford
// Masdar Institute
// 2011
// Calculates Liben-Nowell neighbourhood (indices and weights)
// from list of populations of cells.
// Extremely large calculation, ported to cluster
// Embarrasingly parallel, amenable to parallelisation
/////////////////////////

#include <fstream>
#include <vector>
#include <queue>
#include <list>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "parameters.h"
using namespace std;

int size=7525135;
// Number of 1km^2 cells in mainland USA

///////////
double distanceOnEarth(double lat1,double lat2,double long1,double long2)
///////////
	{

		if ((lat1==lat2)&&(long1==long2))
			{
				return 0.0;
			}

		double temp=0.0;

		long1=(90.0-long1)*radConversion;
		long2=(90.0-long2)*radConversion;
		
		lat1=lat1*radConversion;
		lat2=lat2*radConversion;
	
		temp=sin(long1)*sin(long2)*cos(lat1-lat2)+cos(long1)*cos(long2);

		return acos(temp)*earthRadius;	

	}

///////////
struct popIDElement
///////////
	{
		int ID;
		int pop;
		double dist;
		popIDElement(int newID,int newPop, double newDist)
			{
				ID=newID;
				pop=newPop;
				dist=newDist;
			}
	};


///////////
bool compareDistances(popIDElement a, popIDElement b)
///////////
	{
	
		if(a.dist<b.dist)
			{
				return true;
			}
		else
			{
				return false;
			}
	}

///////////
struct lnEntry
///////////
	{
		vector<int> id;
		vector<double>rank;
	};


//////////////////////////////////
void printTime()
//////////////////////////////////
	{
		time_t rawtime;
		struct tm * timeinfo;

		time(&rawtime);
		timeinfo=localtime(&rawtime);
					
		printf("%s\n",asctime(timeinfo));
	}


//////////////////////////////////
void getLNList(lnEntry *lnList,int *population,double *xCoord,double *yCoord)
//////////////////////////////////
	{
		
		double dist=0.0;
		
		list <popIDElement> tempList;
		list <double> tempRanks;
				
		for(int i=testRangeBegin;i<testRangeEnd;i++)
			{
				if (population[i]>0)
				{
				// Don't worry about empty cells
				//////////////////////////////////
				if ((i%10)==0)
				{printTime();}
				//////////////////////////////////
//				printf("CELL i %f %f\n",xCoord[i],yCoord[i]);
				
				for (int j=0;j<size;j++)
					{
						if (population[j]>0)
						// Throw out empty cells
							{				
								if(!((fabs(xCoord[i]-xCoord[j])>20.0)||(fabs(yCoord[i]-yCoord[j])>20.0)))
								{
									// Dont do expensive trig calcualtion for very distant cells
									dist=distanceOnEarth(xCoord[i],xCoord[j],yCoord[i],yCoord[j]);			

									if (dist<lnCutOff)
									{
										tempList.push_back(popIDElement(j,population[j],dist));
									}
								}
							}
					}
					
				tempList.sort(compareDistances);
				// Get sorted list of distances
				/////////////				
				list<popIDElement> ::iterator it;
				for(it=tempList.begin();it!=tempList.end();it++)
					{
						tempRanks.push_back((*it).pop);
					}
				// Copy all distances
				
				float tempCumSum=0.0;
				/////////////
				list<double> ::iterator it2;
				for(it2=tempRanks.begin();it2!=tempRanks.end();it2++)
					{
						tempCumSum+=*it2;
						
						if(tempCumSum>lnRankLimit)
						{break;}
						
						(lnList[i].rank).push_back(tempCumSum);
					}
				// Take cumulative sum of distances up to LN limit
				// and put in lnList entry for cell
				/////////////
				
				tempCumSum=0.0;
	
				for(int k=0;k<(lnList[i].rank).size();k++)
					{
//						printf("\t\tRANK  %d - %f",k,(lnList[i].rank)[k]);
//						printf("\tSCALE BY POP %d",(tempList.front()).pop);
						tempCumSum+=((tempList.front()).pop)/(lnList[i].rank)[k];
						// Take cumulative sum of rank (inverse distance)
						// scaled by population of cell
//						printf("\t=>%f\n",(lnList[i].rank)[k]);
//						printf("\t=>%f\n",tempCumSum);
						(lnList[i].rank)[k]=tempCumSum;
						// Take cumulative sum of scaled inverse ranks
						(lnList[i].id).push_back((tempList.front().ID));
						// Add ID of cell to lnList of cell IDs
						tempList.pop_front();
					}
				// 
					
				printf("\tCELL %d HAS %d ENTRIES\n",i,(lnList[i].rank).size());
				tempList.clear();
				tempRanks.clear();
				}
			}
		
		
	}

//////////////////////////////////
int main(int agrc, char **argv)
//////////////////////////////////
	{

		testRangeEnd=7000;
		testRangeBegin=3840;

		printf("EXECUTING FOR RANGE - %d ->%d\n",testRangeBegin,testRangeEnd);

		int *population;
		population=new int[size];
		double *xCoord;
		xCoord=new double[size];
		double *yCoord;
		yCoord=new double[size];	

		lnEntry *lnList;
		lnList=new lnEntry[size];

		printf("ALLOCATED ARRAYS\n");
		fflush(stdout);

		FILE *inFile=fopen(argv[1],"r");

		printf("OPENED FILE\n");
		fflush(stdout);

		char tempLine[200];

		char *tempRecord;
		tempRecord=new char[50];
        // Infile schema is <lat>,<long>,<population>
		for (int i=0;i<size;i++)
			{
			
				if(i%(size/20)==0)
					{
						printf("%d - %d%%\n",i, (int)((100*i)/(float)size));
						fflush(stdout);
					}
	
				fgets(tempLine,sizeof(tempLine)/sizeof(char),inFile);

				tempRecord=strtok((char*)tempLine,"\t");

				xCoord[i]=atof(tempRecord);
				tempRecord=strtok(NULL,"\t");	
				yCoord[i]=atof(tempRecord);	
				tempRecord=strtok(NULL,"\t");	
				population[i]=atof(tempRecord);	

			}
		
		getLNList(lnList,population,xCoord,yCoord);
		

		printf("LN LIST COMPLETED FOR %d-%d\nWRITING TO FILE\n",testRangeBegin,testRangeEnd);
		fflush(stdout);
		
		FILE *outFile;
		outFile=fopen("LN_FILE.txt","w");

		for(int k=testRangeBegin;k<testRangeEnd;k++)
			{
	
				fprintf(outFile,"%d\n",k);
				
				for (int l=0;l<(lnList[k].rank).size();l++)
					{						
							fprintf(outFile,"%d\t",(lnList[k].id)[l]);
					}

					fprintf(outFile,"\n");

				for (int l=0;l<(lnList[k].rank).size();l++)
					{
							fprintf(outFile,"%f\t",(lnList[k].rank)[l]);
					}
					
					fprintf(outFile,"\n");
					
			}

		fclose(outFile);

		printTime();

		delete population;
		delete xCoord;
		delete yCoord;

		return 0;

	}

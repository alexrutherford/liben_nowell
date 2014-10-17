//////////////////////////////
// Reads list of population grid data
// and calcualtes LN neighbourhood for
// each.
// A. Rutherford after S. D'souza
// Masdar Institute
// 11/11
// Invoke e.g.
// ./a.out ALL_CLEANED -b 0 -e 1 -o TEST.OUT
// -b/-e specify numerical range of cells to consider
// -o is output file

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
#include "utils.h"
using namespace std;

//int size=7525135;
//int size=5062950;

FILE* outFile;
char outFileName[20];

int testRangeBegin=0;
int testRangeEnd=0;

///////////
void parseArgs(int argc, char** argv, char* outFileName)
///////////
	{
		int argCount=0;
					
		printf("INPUT FILE - %s\n",argv[1]);					
		fflush(stdout);
		// Input file is always first arg

		for(argCount=0;argCount<argc;argCount++)
			{
				
				if(strcmp(argv[argCount],"-b")==0)
				{
					testRangeBegin=atoi(argv[argCount+1]);
					argCount++;
					printf("BEGINNING CELL NUMBER - %d\n",testRangeBegin);					
					fflush(stdout);
				}

				if(strcmp(argv[argCount],"-e")==0)
				{
					testRangeEnd=atoi(argv[argCount+1]);
					argCount++;
					printf("END CELL NUMBER - %d\n",testRangeEnd);
					fflush(stdout);
				}
				
				if(strcmp(argv[argCount],"-o")==0)
				{
					printf("GOT ARG %s\n",argv[argCount+1]);
					strcpy(outFileName,argv[argCount+1]);
					printf("COPY FINISHED");
					fflush(stdout);
					printf("OUTPUT FILE IS - %s\n",argv[argCount+1]);
					fflush(stdout);	
					argCount++;	
				}
			}

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
/*				
				/////////////
				for (int k=0;k<10;k++)
					{
						printf("\t\t%d - %f %d %d\n",k,(tempList.front()).dist,(tempList.front()).ID,(tempList.front()).pop);
						tempList.pop_front();
					}
*/
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
						(lnList[i].ids).push_back((tempList.front().ID));
						// Add ID of cell to lnList of cell IDs
						tempList.pop_front();
					}
				// 
					
				printf("\tCELL %d HAS %d ENTRIES\n",i,(lnList[i].rank).size());
/*
				for(int k=0;k<(lnList[i].rank).size();k++)
					{
							printf("\t ID %d - RANK %f\n",(lnList[i].id)[k],(lnList[i].rank)[k]);
					}
*/			
				tempList.clear();
				tempRanks.clear();
				}
			}
		
		
	}

//////////////////////////////////
int main(int argc, char **argv)
//////////////////////////////////
	{

		testRangeEnd=7000;
		testRangeBegin=3840;
	
		parseArgs(argc,argv,outFileName);

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
		// inFile is always first argument

		FILE *outFile;

		printf("OPENED FILE\n");
		fflush(stdout);

		char tempLine[200];

		char *tempRecord;
		tempRecord=new char[50];

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
		
/*
		for(int i=0;i<20000000;i++)
			{
				actionQueue.push((float)i);

				if(i%10000==0)
					{
						printf("ALLOCATED %d...\n",i);
						fflush(stdout);
						actionQueue.pop();
					}
			}

		printf("AT TOP %f\n",actionQueue.top());
		fflush(stdout);
*/
		printf("LN LIST COMPLETED FOR %d-%d\nWRITING TO FILE - %s\n",testRangeBegin,testRangeEnd,outFileName);
		fflush(stdout);
	
		outFile=fopen(outFileName,"w");

		if(outFile==NULL)
		{
			printf("ERROR WITH OUT FILE\n");
			fflush(stdout);
		}
	
		for(int k=testRangeBegin;k<testRangeEnd;k++)
			{
	
				fprintf(outFile,"%d\n",k);
				
				for (int l=0;l<(lnList[k].rank).size();l++)
					{						
							fprintf(outFile,"%d\t",(lnList[k].ids)[l]);
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


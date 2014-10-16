//g++ $(mysql_config --cflags) put_file_in_db_2.cc $(mysql_config --libs)

//////////////////////////
// Program to take LN output
// file and populate database
// Based on http://blog.trilabs.co.tz/2011/09/connect-to-mysql-using-c-on-ubuntu.html
// A Rutherford
// Masdar Institute 2/12
//////////////////////////

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <string.h>
#include "utils.h"
#include "parameters.h"
 
using namespace std;

struct connection_details
///////////////////////////
{
    char *server;
    char *user;
    char *password;
    char *database;
};

// Sets up connection to DB, returns pointer to MYSQL object
// according to connection struct details
///////////////////////////////
MYSQL* mysql_connection_setup(struct connection_details mysql_details)
///////////////////////////////
{
    // first of all create a mysql instance and initialize the variables within
    MYSQL *connection = mysql_init(NULL);

    // connect to the database with the details attached.
    if (!mysql_real_connect(connection,mysql_details.server, mysql_details.user, mysql_details.password, mysql_details.database, 0, NULL, 0)) {
         printf("Conection error : %s\n", mysql_error(connection));
         exit(1);
    }
    return connection;
 }
 
///////////////////////
int main(int argc, char** argv)
///////////////////////
{
    MYSQL *conn;                // the connection
    MYSQL_RES *res;     // the results
    MYSQL_ROW row;      // the results row (line by line)

    struct connection_details mysqlD;
    mysqlD.server = "localhost";  // where the mysql database is
    mysqlD.user = "root";               // the root user of mysql
    mysqlD.password = "password"; // the password of the root user in mysql
    mysqlD.database = "lnDatabase"; // the databse to pick

    conn = mysql_connection_setup(mysqlD);

    printf("CONNECTED TO DB\n");
    fflush(stdout);

    vector <float> rankVector;
    vector <int> idVector;

	char tempCommand[200];
	if(false)
	{	
		sprintf(tempCommand,"create table maxRanks (dummyIndex int,maxRank float) type=ISAM");
		if(mysql_query(conn,tempCommand))
			{
				printf("%s\n",mysql_error(conn));
				printf("FILE - %s\n",argv[1]);
				exit(1);
			}
	}

	FILE *inFile;
	inFile=fopen(argv[1],"r");

	char *tempRecord;

	char tempLine[750000];

	int tempIndex;

	while(fgets(tempLine,sizeof(tempLine)/sizeof(char),inFile))
	{
	    tempIndex=atoi(tempLine);
		// Index

		if(((tempIndex+1)%2500)==0)
		{printf("%d ",tempIndex+1);fflush(stdout);}

		fgets(tempLine,sizeof(tempLine)/sizeof(char),inFile);
		tempRecord=strtok((char*)tempLine,"\t");		
		idVector.push_back(atoi(tempRecord));

//		printf("\tGOT CELL INDEX %d\n",tempIndex);
			
//		printf("FILE - %s\n",argv[1]);
//		printf("\tFIRST ENTRY %d\n",idVector[0]);
//		fflush(stdout);
//		exit(1);

		while(tempRecord=strtok(NULL,"\t"))
		{
			if (!(strcmp(tempRecord,"\n")))
			{break;}
			// Make sure we don't include new line

			idVector.push_back(atoi(tempRecord));
//			printf("PUSHING %s\n...",tempRecord);
//			printf("FIRST ENTRY %d\n\n",idVector[0]);
		}
		// Ids

//		printf("LINE =>%s\n",tempLine);
		
		fgets(tempLine,sizeof(tempLine)/sizeof(char),inFile);
		tempRecord=strtok((char*)tempLine,"\t");		
		rankVector.push_back(atof(tempRecord));
		while(tempRecord=strtok(NULL,"\t"))
		{
			if (!(strcmp(tempRecord,"\n")))
			{break;}
			// Make sure we don't include new line
			rankVector.push_back(atof(tempRecord));
//			printf("PUSHING %s\n...",tempRecord);
		}

		// Ranks
		vector<int>::iterator it; 
		vector<float>::iterator itRank; 
		
		sprintf(tempCommand,"insert into maxRanks (dummyIndex,maxRank) VALUES(%d,%f)",tempIndex,rankVector.back());	
		if(mysql_query(conn,tempCommand))
			{
			printf("%s\n",mysql_error(conn));
			printf("FILE - %s - RECORD %d\n",argv[1],idVector[0]);
			exit(1);
			}
		sprintf(tempCommand,"create table table_%d (rank float,cellId int,uniqueId int auto_increment primary key) type=ISAM",tempIndex);
		
//		printf("\tEXECUTING=>%s\n",tempCommand);

		if(mysql_query(conn,tempCommand))
			{
			 printf("%s\n",mysql_error(conn));
			printf("FILE - %s - RECORD %d\n",argv[1],idVector[0]);
	//		exit(1);
			}	

		itRank=rankVector.end()-1;

		for(it=idVector.end()-1 ; it>idVector.begin()-1; it--)
		{
//			printf("%d - %d\t",tempIndex,*it);
			sprintf(tempCommand,"insert into table_%d (rank,cellId) VALUES(%f,%d)",tempIndex,*itRank,*it);
			if(mysql_query(conn,tempCommand))
			//if(false)
				{
                                 printf("%s\n",mysql_error(conn));
				printf("FILE - %s - RECORD %d\n",argv[1],idVector[0]);
	//			exit(1);
                             	}	
			itRank--;
		}
		
		rankVector.clear();
		idVector.clear();
//		exit(1);

	}

	printf("\n");
    return 0;
}


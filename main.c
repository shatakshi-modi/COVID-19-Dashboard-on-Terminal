#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <json-c/json.h>
#include <string.h>

// Parses JSON and prints the details
void parseJSON(char *country_name){
			FILE *fp;
			char *folder="JSON/";
			char *ext=".json";
		    char file_name[15]="";
		    strcat(file_name,folder);
		    strcat(file_name,country_name);
		    strcat(file_name,ext);
		    //Eg.:File_name="JSON/India.json"
			if((fp=fopen(file_name,"r"))==NULL){
				//printf("Error in file\n");
			}
			char data[1024];
			struct json_object *parsed_json;
			struct json_object *country;
			struct json_object *active;
			struct json_object *total;
			struct json_object *new;
			struct json_object *recovery;
			struct json_object *death;
			struct json_object *last_update;
			fread(data,1024,1,fp);
			fclose(fp);
			parsed_json=json_tokener_parse(data);
			json_object_object_get_ex(parsed_json,"Country_text",&country); 
			json_object_object_get_ex(parsed_json,"Active Cases_text",&active);
			json_object_object_get_ex(parsed_json,"New Cases_text",&new);
			json_object_object_get_ex(parsed_json,"Total Cases_text",&total);
			json_object_object_get_ex(parsed_json,"Total Recovered_text",&recovery);
			json_object_object_get_ex(parsed_json,"Total Deaths_text",&death);
			json_object_object_get_ex(parsed_json,"Last Update",&last_update);

			printf("Country: %s\n",json_object_get_string(country) );
			printf("Active Cases: %s\n",json_object_get_string(active) );
			printf("New Cases: %s\n",json_object_get_string(new) );
			printf("Total Cases: %s\n",json_object_get_string(total) );
			printf("Recovery Count: %s\n",json_object_get_string(recovery) );
			printf("Death Count: %s\n",json_object_get_string(death) );
			printf("Last Update: %s\n",json_object_get_string(last_update) );

}

int main( int argc, char* argv[] )
{
	//Less than 2 arguments won't work
	if (argc !=2)
	{
		printf("USAGE: parent_process.out <country_name> ...");
	}
	int i, childPID, status;
	for(i=0; i<argc-1; i=i+2)
	{	
		if ( fork() == 0 ) 
		{
			//Execute the child program
			//Get the data of the country given in argument
			execl( "./ssltest.out", "./ssltest.out",argv[i+1],NULL ); /* Execute other program */
			fprintf( stderr, "Could not execute %s \n", argv[1] );
		}
		else
		{
			//wait for child process to finish
			childPID = wait(&status);
			//Parse JSON
			parseJSON(argv[i+1]);
			return 0;
			
		}
	}
}

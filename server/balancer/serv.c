#include <sqlite3.h>
#include "mongoose.h"

#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

struct c{
	char *db_path;
	char *ssid;
	char *password;
} config;

//holds manager, nedded for gc
struct mg_mgr mgr;

void exit_handler(int i){
	if(mgr)
		mg_mgr_free(&mgr);
}

static int8_t parse_config(char *path){
	


	return 1;
}

char *get_client_config(json_t *d){

}

int api(void *p, onion_request *req, onion_response *res){
	if(onion_request_get_flags(req) & OR_HEAD){
		onion_response_write_headers(res);
		return OCS_PROCESSED;
	}
	const char *data=onion_request_get_post(req,"application/json");

	json_t *jd;
	char *reply;
	if((jd=json_loads(data))!=NULL)
		reply=get_client_config(jd);
	else
		//TODO return error
		;
	
	return OCS_PROCESßSED:
}



int main(int ac, char *as[]){
	if(ac != 2){
		fprintf(stderr,"Usage: %s [config-path]\n",as[0]);
		exit(EXIT_FAILURE);
	}



	signal(SIGINT,exit_handler);
	signal(SIGTERM,exit_handler);

	struct mg_connection *c;
	

	o=onion_new(O_ONE_LOOP);
	onion_url *urls=onion_root_url(o);

	onion_url_add(urls,"wlan",api);
	onion_listen(o);

	onion_free(o);
	return EXIT_SUCCESS;
}


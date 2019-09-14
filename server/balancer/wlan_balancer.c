#include <sqlite3.h>
#include "mongoose.h"
#include <unistd.h>
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
/*	prog [cmd]
		run [port] [database]
		init [database]



*/

const char *help_message="Usage: wlan_balancer [cmd]\n"\
	"\trun [port] [database] [ssid] [password]\n"\
	"\tinit [database]\n";

const char *db_init_sqlcmds[]={ "CREATE TABLE orig_wlan(id number PRIMARY KEY, bssid TEXT NOT NULL, "\
				"ssid TEXT NOT NULL, UNIQUE (bssid,ssid)  );",
				"CREATE TABLE devices(mac TEXT PRIMARY KEY, room NUMBER NOT NULL, "\
				"running_ap NUMBER NOT NULL, rep_ap number, FOREIGN KEY(rep_ap) "\
				"REFERENCES orig_wlan(id));" };

struct app_s {
	char *ssid;
	char *password;
	sqlite3 *db_con;
	struct mg_mgr mgr;
} app;

void exit_handler(int i){
	if(app.db_con!=NULL)
		sqlite3_close(app.db_con);
	mg_mgr_free(&(app.mgr));
}

void blame (const char* msg, ...){
        va_list args;
        va_start(args, msg);
	if(msg != NULL){
		fprintf(stderr,"Error: ");      
		vfprintf(stderr, msg, args);
		fprintf(stderr, "\n");
	}
	va_end(args);
        exit_handler(EXIT_FAILURE);
        exit(EXIT_FAILURE);
}

int cb_empty(void *v,int i, char **a, char **b){
	return 0;
}

/*
const char *get_client_config(json_t *d){

}
*/
static void api(struct mg_connection *c, int ev, void *p){
		
}


int main(int ac, char **as){
	if(ac==1){
		fputs(help_message,stdout);
		return EXIT_SUCCESS;
	}
	app.db_con=NULL;
	signal(SIGINT,exit_handler);
	signal(SIGTERM,exit_handler);
	++as;
	if(strcmp(*as,"init")==0){
		++as;
		if(ac != 3)
			blame(help_message);
		if(access(*as,F_OK) != -1)
			blame("\"%s\" exists, will not continue!",*as);
		int ec=sqlite3_open(*as,&(app.db_con));
		if(ec)
			blame("unable  to open db: \"%s\"",sqlite3_errmsg(app.db_con));
		char *emsg=NULL;
		for(size_t cmd=0;cmd <sizeof(db_init_sqlcmds)/sizeof(char*);++cmd){
			ec=sqlite3_exec(app.db_con,db_init_sqlcmds[cmd],&cb_empty,NULL,&emsg);
			#ifdef DEBUG
				printf("[DBG] %s\n",db_init_sqlcmds[cmd]);
			#endif
			if(ec){
				fprintf(stderr,"Error: \"%s\"",emsg);
				sqlite3_free(emsg);
			}
		}
		sqlite3_close(app.db_con);
		puts("finished!");
	}else if(strcmp(*as,"run")==0){
		++as;
		if(ac != 6)
			blame(help_message);
		if(access(as[1],F_OK) != -1)
			blame("\"%s\" does exist, will not continue!",as[1]);
		int ec=sqlite3_open(*as,&(app.db_con));
		if(ec)
			blame("unable  to open db: \"%s\"",sqlite3_errmsg(app.db_con));
		
		struct mg_connection *c;
		
		mg_mgr_init(&(app.mgr),NULL);
		c= mg_bind(&(app.mgr),*as,api);
		mg_set_protocol_http_websocket(c);
		
		app.ssid=as[2];
		app.password=as[3];
		
		for(;;)
			mg_mgr_poll(&(app.mgr),1000);
		
		mg_mgr_free(&(app.mgr));
		sqlite3_close(app.db_con);
	}else
		blame(help_message);

	return EXIT_SUCCESS;
}


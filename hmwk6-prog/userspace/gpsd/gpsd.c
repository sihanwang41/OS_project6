#include "gpsd.h"
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_LINE	100
#define SECOND		1000000

void daemon_mode()
{
	pid_t pid;

	if((pid = fork()) < 0){
		perror("Fail fork");
		exit(EXIT_FAILURE);
	}

	if(pid > 0)
		exit(EXIT_SUCCESS);

	if(setsid() < 0){
		perror("Fail to setsid");
		exit(EXIT_FAILURE);
	}

	if((pid = fork()) < 0){
		perror("Fail fork");
		exit(EXIT_FAILURE);
	}

	if(pid > 0)
		exit(EXIT_SUCCESS);

	close(0);
	close(1);
	close(2);
	chdir("/data/misc/");
	umask(0);

	return;
}

int main(int argc, char *argv[])
{
	struct gps_location loc;
	FILE *fp;
	char line[MAX_LINE];

	daemon_mode();

	while (1) {
		fp = fopen("/data/media/0/gps_location.txt","r");

		if (fp == NULL) {
			usleep(SECOND);
			continue;
		}

		if (fgets(line, MAX_LINE, fp))
			loc.latitude = atof(line);

		if (fgets(line, MAX_LINE, fp))
			loc.longitude = atof(line);

		if (fgets(line, MAX_LINE, fp))
			loc.accuracy = atof(line);

		fclose(fp);

		set_gps_location(&loc);

		usleep(SECOND);
	}

	return 0;
}


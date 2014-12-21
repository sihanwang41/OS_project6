#include "file_loc.h"
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


int main(int argc, char **argv)
{
	struct gps_location loc;
	int coord_age;

	if (argc != 2) {
		printf("Usage: file_loc <path>\n");
		return 0;
	}

	coord_age = get_gps_location(argv[1], &loc);

	if (coord_age < 0)
		printf("No file or no GPS location in file\n");
	else {
		printf("Latitude: %f\n", loc.latitude);
		printf("Longitude: %f\n", loc.longitude);
		printf("Accuracy: %f\n", loc.accuracy);
		printf("Age: %d\n", coord_age);
	}

	return 0;
}

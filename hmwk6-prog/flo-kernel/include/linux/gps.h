#ifndef __LINUX_GPS_H
#define __LINUX_GPS_H

#include <linux/fs.h>

struct inode;

struct gps_location {
	double latitude;
	double longitude;
	float  accuracy;  /* in meters */
};

extern int inode_set_gps_location(struct inode *);
extern int get_current_loc(struct gps_location *);

#endif /* __LINUX_GPS_H */

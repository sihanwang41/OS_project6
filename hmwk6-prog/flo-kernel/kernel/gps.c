#include <linux/errno.h>
#include <linux/gps.h>
#include <linux/namei.h>
#include <linux/syscalls.h>
#include <linux/time.h>
#include <linux/uaccess.h>

static struct gps_location current_loc = {
	.latitude = 0,
	.longitude = 0,
	.accuracy = 0
};

static struct timespec current_loc_time;
static int flag;

static DEFINE_SPINLOCK(k_gps_lock);

SYSCALL_DEFINE1(set_gps_location, struct gps_location __user, *loc)
{
	struct gps_location k_loc;

	if (current_uid() != 0)
		return -EACCES;

	if (loc == NULL)
		return -EINVAL;

	if (copy_from_user(&k_loc, loc, sizeof(k_loc)))
		return -EFAULT;

	spin_lock(&k_gps_lock);
	flag = 1;
	memcpy(&current_loc, &k_loc, sizeof(k_loc));
	current_loc_time = CURRENT_TIME_SEC;
	spin_unlock(&k_gps_lock);

	return 0;
}

SYSCALL_DEFINE2(get_gps_location, const char __user, *pathname,
		struct gps_location, __user *loc)
{
	struct gps_location k_loc;
	struct path path;
	struct inode *inode;
	int error;
	int coord_age;

	if (loc == NULL)
		return -EINVAL;

	error = user_path_at(AT_FDCWD, pathname, LOOKUP_FOLLOW, &path);

	if (error)
		return error;

	inode = path.dentry->d_inode;

	error = inode_permission(inode, MAY_READ | MAY_ACCESS);

	if (error)
		return error;

	if (inode->i_op->get_gps_location)
		coord_age = inode->i_op->get_gps_location(inode, &k_loc);
	else
		return -ENODEV;

	if (coord_age < 0)
		return -ENODEV;

	if (copy_to_user(loc, &k_loc, sizeof(k_loc)))
		return -EFAULT;

	return coord_age;
}

int get_current_loc(struct gps_location *loc)
{
	int f;
	struct timespec c, t;

	spin_lock(&k_gps_lock);
	f = flag;
	memcpy(loc, &current_loc, sizeof(current_loc));
	t = current_loc_time;
	spin_unlock(&k_gps_lock);

	c = CURRENT_TIME_SEC;

	if (f)
		return c.tv_sec - t.tv_sec;

	return -ENODEV;
}

int inode_set_gps_location(struct inode *inode)
{
	if (inode->i_op->set_gps_location)
		return inode->i_op->set_gps_location(inode);

	return -ENODEV;
}

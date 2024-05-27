#include <linux/module.h>
#include <linux/bio.h>
#include <linux/kobject.h>
#include <linux/device-mapper.h>

/* Main structure for dmp driver */
struct dmp_data {
	struct dm_dev *dev;
    struct device *bd_device;
    struct device_attribute *dev_attr;
	unsigned long read_req_counter;
	unsigned long write_req_counter;
	unsigned long write_blocks_size;
	unsigned long read_blocks_size;
};

#define ACCESS_PERMISSIONS 0644 /* -rw-r-r- */

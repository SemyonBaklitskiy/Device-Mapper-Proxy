#include "dmp.h"

/* Map callback for dmp devices */
static int dmp_map(struct dm_target *ti, struct bio *bio)
{
	struct dmp_data *data = ti->private;

	switch (bio_op(bio)) {
	case REQ_OP_READ:
		if (bio->bi_opf & REQ_RAHEAD) {
			return DM_MAPIO_KILL; /* Readahead of null bytes only wastes buffer cache */
		}

		++data->read_req_counter;
		data->read_blocks_size += bio->bi_iter.bi_size;
		break;

	case REQ_OP_WRITE:
		++data->write_req_counter;
		data->write_blocks_size += bio->bi_iter.bi_size; 
		break;

	default:
		return DM_MAPIO_KILL;
	}

    bio_set_dev(bio, data->dev->bdev);
    submit_bio(bio);
	bio_endio(bio);

	return DM_MAPIO_SUBMITTED;
}

static unsigned long avg(const unsigned long size, const unsigned long count)
{
    if (count == 0)
        return 0;

    return size / count;
}

static ssize_t show_sysfs_stat(struct device *dev,
                                struct device_attribute *attr, char *buffer)
{
    struct dmp_data *data = dev_get_drvdata(dev);
    return sprintf(buffer,
"Read:\n\
    reqs: %lu\n\
    avg size: %lu\n\
Write:\n\
    reqs: %lu\n\
    avg size: %lu\n\
Total:\n\
    reqs: %lu\n\
    avg size: %lu\n",
    data->read_req_counter, avg(data->read_blocks_size, data->read_req_counter),
    data->write_req_counter, avg(data->write_blocks_size, data->write_req_counter),
    data->read_req_counter + data->write_req_counter,
    avg(data->read_blocks_size + data->write_blocks_size,
    data->read_req_counter + data->write_req_counter));
}

static ssize_t store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
    return 0;
}

static struct device_attribute *create_dev_attribute(const char *name)
{
    struct device_attribute *attr = kzalloc(sizeof(*attr), GFP_KERNEL);
    if (unlikely(!attr)) {
        pr_err("Couldn`t allocate memory");
        return ERR_PTR(-ENOMEM);
    }

    attr->attr.name = name;
    attr->attr.mode = VERIFY_OCTAL_PERMISSIONS(ACCESS_PERMISSIONS);
    attr->show = show_sysfs_stat;
    attr->store = store; /* To avoid warning we need .store callback as we have write permissions */

    return attr;
}

static int sysfs_stat_create(struct dmp_data *data)
{
    int err = 0;
    struct device_attribute *dev_attr = NULL;

    dev_attr = create_dev_attribute("volumes");
    if (unlikely(IS_ERR(dev_attr))) {
        return PTR_ERR(dev_attr);
    }

/* Using device_create_file instead of sysfs_create_file to avoid global variables.
    We can initialize struct_device::driver_data to get our stats in
    device_attribute::show callback */ 
    err = device_create_file(data->bd_device, dev_attr);
    if (unlikely(err)) {
        pr_err("Couldn`t create sysfs file");
        kfree(dev_attr);
        return err;
    }

    data->dev_attr = dev_attr;

    return err;
}

static void sysfs_stat_remove(struct dmp_data *data)
{
    device_remove_file(data->bd_device, data->dev_attr);
    kfree(data->dev_attr);
}

/* Constructor for dmp devices */
static int dmp_ctr(struct dm_target *ti, unsigned int argc, char **argv)
{
	int err = 0;
	struct dmp_data *data = NULL;
	struct dm_dev *dev = NULL;

	if (argc != 1) {
		ti->error = "Invalid argument count";
		pr_err("Invalid argument count");
		return -EINVAL;
	}

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (unlikely(!data)) {
		ti->error = "Couldn`t allocate memory";
		pr_err("Couldn`t allocate memory");
		return -ENOMEM;
	}

    /* Getting parent device with name argv[0] */
	err = dm_get_device(ti, argv[0], dm_table_get_mode(ti->table), &dev);
	if (unlikely(err)) {
		ti->error = "Device lookup failed";
		pr_err("Device lookap failed");
		kfree(data);
		return err;
	}

    data->bd_device = &dev->bdev->bd_device;

    /* Creating file with device`s stats in sysfs */
    err = sysfs_stat_create(data);
    if (unlikely(err)) {
        dm_put_device(ti, dev);
        kfree(data);
        return err;
    }

    data->dev = dev;

    /* Initializing private fields for drivers`s data */
	ti->private = data;
    dev_set_drvdata(data->bd_device, data);

	return err;
}

/* Destructor for dmp devices */
static void dmp_dtr(struct dm_target *ti)
{
	struct dmp_data *data = ti->private;

    sysfs_stat_remove(data);
	dm_put_device(ti, data->dev);
	kfree(data);
}


static struct target_type dmp_target = {
	.name   = "dmp",
	.module = THIS_MODULE,
	.ctr    = dmp_ctr,
	.dtr    = dmp_dtr,
	.map    = dmp_map,
};

static int __init dm_dmp_init(void)
{
	int err = dm_register_target(&dmp_target);

	if (unlikely(err))
		pr_err("Register dmp_target failed %d", err);

	return err;
}

static void __exit dm_dmp_exit(void)
{
	dm_unregister_target(&dmp_target);
}

module_init(dm_dmp_init);
module_exit(dm_dmp_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Semyon Baklitskiy");
MODULE_DESCRIPTION("Device Mapper Proxy driver");

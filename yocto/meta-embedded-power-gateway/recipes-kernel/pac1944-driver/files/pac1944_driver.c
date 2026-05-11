#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/mutex.h>

#define PAC1944_REFRESH_CMD 0x1E

#define REG_VBUS1       0x07
#define REG_VSENSE1     0x0B
#define REG_VPOWER1     0x17

struct pac1944_data {
    struct i2c_client *client;
    struct mutex lock;

    u16 vbus_raw;
    u16 vsense_raw;
    u32 vpower_raw;
};

static int pac1944_refresh(struct i2c_client *client)
{
    return i2c_smbus_write_byte(client, PAC1944_REFRESH_CMD);
}

static int pac1944_read(struct pac1944_data *data)
{
    int ret;
    u8 raw[2];
    u8 power[4];

    mutex_lock(&data->lock);

    ret = pac1944_refresh(data->client);
    if (ret < 0)
        goto out;

    usleep_range(5000, 6000);

    ret = i2c_smbus_read_i2c_block_data(
            data->client,
            REG_VBUS1,
            2,
            raw);

    if (ret < 0)
        goto out;

    data->vbus_raw = ((u16)raw[0] << 8) | raw[1];

    ret = i2c_smbus_read_i2c_block_data(
            data->client,
            REG_VSENSE1,
            2,
            raw);

    if (ret < 0)
        goto out;

    data->vsense_raw = ((u16)raw[0] << 8) | raw[1];

    ret = i2c_smbus_read_i2c_block_data(
            data->client,
            REG_VPOWER1,
            4,
            power);

    if (ret < 0)
        goto out;

    data->vpower_raw =
            ((u32)power[0] << 24) |
            ((u32)power[1] << 16) |
            ((u32)power[2] << 8)  |
            (u32)power[3];

    ret = 0;

out:
    mutex_unlock(&data->lock);
    return ret;
}

static ssize_t vbus_raw_show(
        struct device *dev,
        struct device_attribute *attr,
        char *buf)
{
    struct pac1944_data *data = dev_get_drvdata(dev);

    if (pac1944_read(data) < 0)
        return -EIO;

    return sprintf(buf, "%u\n", data->vbus_raw);
}

static ssize_t vsense_raw_show(
        struct device *dev,
        struct device_attribute *attr,
        char *buf)
{
    struct pac1944_data *data = dev_get_drvdata(dev);

    if (pac1944_read(data) < 0)
        return -EIO;

    return sprintf(buf, "%u\n", data->vsense_raw);
}

static ssize_t vpower_raw_show(
        struct device *dev,
        struct device_attribute *attr,
        char *buf)
{
    struct pac1944_data *data = dev_get_drvdata(dev);

    if (pac1944_read(data) < 0)
        return -EIO;

    return sprintf(buf, "%u\n", data->vpower_raw);
}

static DEVICE_ATTR_RO(vbus_raw);
static DEVICE_ATTR_RO(vsense_raw);
static DEVICE_ATTR_RO(vpower_raw);

static struct attribute *pac1944_attrs[] = {
    &dev_attr_vbus_raw.attr,
    &dev_attr_vsense_raw.attr,
    &dev_attr_vpower_raw.attr,
    NULL,
};

static const struct attribute_group pac1944_attr_group = {
    .attrs = pac1944_attrs,
};

static int pac1944_probe(struct i2c_client *client,
                         const struct i2c_device_id *id)
{
    struct pac1944_data *data;
    int ret;

    (void)id;

    dev_info(&client->dev,
             "PAC1944 sysfs driver probed\n");

    data = devm_kzalloc(
            &client->dev,
            sizeof(*data),
            GFP_KERNEL);

    if (!data)
        return -ENOMEM;

    data->client = client;

    mutex_init(&data->lock);

    i2c_set_clientdata(client, data);

    ret = sysfs_create_group(
            &client->dev.kobj,
            &pac1944_attr_group);

    if (ret)
        return ret;

    return 0;
}

static void pac1944_remove(struct i2c_client *client)
{
    sysfs_remove_group(
            &client->dev.kobj,
            &pac1944_attr_group);

    dev_info(&client->dev,
             "PAC1944 driver removed\n");
}

static const struct of_device_id pac1944_of_match[] = {
    { .compatible = "microchip,pac1944-custom" },
    { }
};

MODULE_DEVICE_TABLE(of, pac1944_of_match);

static struct i2c_driver pac1944_driver = {
    .driver = {
        .name = "pac1944_driver",
        .of_match_table = pac1944_of_match,
    },
    .probe = pac1944_probe,
    .remove = pac1944_remove,
};

module_i2c_driver(pac1944_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Farhan");
MODULE_DESCRIPTION("PAC1944 sysfs I2C driver");

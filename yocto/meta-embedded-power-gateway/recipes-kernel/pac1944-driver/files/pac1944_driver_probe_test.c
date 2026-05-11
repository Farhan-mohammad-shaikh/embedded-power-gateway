#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/delay.h>

#define PAC1944_REFRESH_CMD 0x1E

#define REG_VBUS1       0x07
#define REG_VSENSE1     0x0B
#define REG_VPOWER1     0x17

#define REG_VBUS3       0x09
#define REG_VSENSE3     0x0D
#define REG_VPOWER3     0x19

static int pac1944_read_values(struct i2c_client *client,
                               u8 vbus_reg,
                               u8 vsense_reg,
                               u8 vpower_reg,
                               u16 *vbus,
                               u16 *vsense,
                               u32 *vpower)
{
    int ret;
    u8 raw[2];
    u8 pow[4];

    ret = i2c_smbus_read_i2c_block_data(client, vbus_reg, 2, raw);
    if (ret < 0)
        return ret;

    *vbus = ((u16)raw[0] << 8) | raw[1];

    ret = i2c_smbus_read_i2c_block_data(client, vsense_reg, 2, raw);
    if (ret < 0)
        return ret;

    *vsense = ((u16)raw[0] << 8) | raw[1];

    ret = i2c_smbus_read_i2c_block_data(client, vpower_reg, 4, pow);
    if (ret < 0)
        return ret;

    *vpower = ((u32)pow[0] << 24) |
              ((u32)pow[1] << 16) |
              ((u32)pow[2] << 8)  |
               (u32)pow[3];

    return 0;
}

static int pac1944_probe(struct i2c_client *client,
                         const struct i2c_device_id *id)
{
    (void)id;
    int ret;
    int channel;
    u8 vbus_reg;
    u8 vsense_reg;
    u8 vpower_reg;
    u16 vbus;
    u16 vsense;
    u32 vpower;

    dev_info(&client->dev,
             "PAC1944 probe started at address 0x%02x\n",
             client->addr);

    if (client->addr == 0x10 || client->addr == 0x11) {
        channel = 1;
        vbus_reg = REG_VBUS1;
        vsense_reg = REG_VSENSE1;
        vpower_reg = REG_VPOWER1;
    } else if (client->addr == 0x12) {
        channel = 3;
        vbus_reg = REG_VBUS3;
        vsense_reg = REG_VSENSE3;
        vpower_reg = REG_VPOWER3;
    } else {
        dev_err(&client->dev,
                "Unsupported PAC1944 address 0x%02x\n",
                client->addr);
        return -EINVAL;
    }

    ret = i2c_smbus_write_byte(client, PAC1944_REFRESH_CMD);
    if (ret < 0) {
        dev_err(&client->dev,
                "Failed to send refresh command: %d\n",
                ret);
        return ret;
    }

    usleep_range(5000, 6000);

    ret = pac1944_read_values(client,
                              vbus_reg,
                              vsense_reg,
                              vpower_reg,
                              &vbus,
                              &vsense,
                              &vpower);
    if (ret < 0) {
        dev_err(&client->dev,
                "Failed to read channel %d: %d\n",
                channel, ret);
        return ret;
    }

    dev_info(&client->dev,
             "CH%d raw: VBUS=0x%04x VSENSE=0x%04x VPOWER=0x%08x\n",
             channel, vbus, vsense, vpower);

    return 0;
}

static void pac1944_remove(struct i2c_client *client)
{
    dev_info(&client->dev,
             "PAC1944 removed from address 0x%02x\n",
             client->addr);
}

static const struct of_device_id pac1944_of_match[] = {
    { .compatible = "microchip,pac1944" },
    { }
};
MODULE_DEVICE_TABLE(of, pac1944_of_match);

static struct i2c_driver pac1944_driver = {
    .driver = {
        .name = "pac1944",
        .of_match_table = pac1944_of_match,
    },
    .probe = pac1944_probe,
    .remove = pac1944_remove,
};

module_i2c_driver(pac1944_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Farhan");
MODULE_DESCRIPTION("PAC1944 I2C kernel test driver based on power-gateway userspace logic");

#include <stdio.h> /*标准输入输出定义*/
#include <linux_i2c.h>
#include <assert.h>

void *i2c_p = NULL;
int main(int argc,char *argv[])
{
    unsigned char addr;
    unsigned char reg;
    unsigned char data;
    unsigned char res;
    int ret = i2c_init(8, 0x3c, &i2c_p);
    assert(ret == 0);

    while (1)
    {
        ret = i2c_write(i2c_p, &reg, 1, &data, 1);
	    assert(ret == 0);
    }
    





	// ret = i2c_read(i2c_p, &reg, 1, &res, 1);
	// assert(ret == 0);
    i2c_destroy(i2c_p);
    return 0;
}
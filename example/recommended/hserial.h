#ifndef __SERIAL_H__
#define __SERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct serial_s serial_t;

/*串口打开函数
baud:1200\2400\4800\9600\14400\19200\38400\57600\115200
data:5\6\7\8
stop:1\2
parity:O:o:N:n:E:e
 */
serial_t* serial_open(const char* dev, int baud, int data, int stop, char parity);
//串口读取数据函数
int serial_read(serial_t* serial, char* buf, int size, int us);
//串口发送函数
int serial_write(serial_t* serial, const char* buf, int size);
//串口关闭函数
void serial_close(serial_t* serial);

#ifdef __cplusplus
}
#endif

#endif

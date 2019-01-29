/*---------------------------------使用说明-------------------------------
使用：
    将UPRE_Linux_Usart.cpp文件和UPRE_Linux_Usart.hpp文件放在工程目录下，在要使
用的文件中包含UPRE_Linux_Usart.hpp文件即可。Linux_Usart类封装了串口的相关函数。
           成员函数列表：
           public： 
                  Linux_Usart
                  ~Linux_Usart
                  Read_Data(重载3)
                  Write_Data(重载3)
                  operator << (重载3)
                  operator >>
                  Get_FD
          
           编译选项列表：
                  UPRE_FLAG_CROSS_DETECT
                  UPRE_FLAG_TCFLUSH
                  UPRE_FLAG_BLOCK
注意：
    1、Linux_Usart类定义在 UPRE 命名空间下。
    2、在阻塞模式下，如果程序的运行速度大于串口速度，则串口的缓冲区在一定时间内将会被写满，此时调用Read_Data或Write_Data则会使程序阻塞，直到缓冲区有足够的空间。
       而非阻塞模式将直接返回-1。
       阻塞模式理论上不会丢失数据，但是可能会影响程序的运行速度，建议用单独的线程在阻塞模式下发送数据。（推荐）
       非阻塞模式，不会影响速度，但可能会丢失数据。
    3、关于tcflag标识符，为清除缓冲区的操作。如无需要，尽量不要使用。
    4、默认为8位数据位，1位停止位，无奇偶校验。
       并未提供数据位、停止位、奇偶校验设置的接口，如需要自行更改扩展。
       默认停供8个常用串口号的枚举，如果需要更多，自行扩展。
       默认提供5种常用波特率选择，如果需要更多，自行扩展
    5、使用v_uchar(vector<unsigned char>)存储读取和写入数据。
       如果需要使用unsigned char类型的数组，可调用Get_fd函数获取串口文件描述符，然后直接调用write、read函数。注意包含unistd.h头文件
    6、建议使用越界检测功能，可防止操作失误造成的问题。
    7、本库使用POSIX和C/C++标准库，理论上在任何Linux系统都可以使用，包括嵌入式Linux。


声明：UPER_Linux_Usart.hpp和UPER_Linux_Usart.cpp文件内的代码为学习之用，使用中出现的任何的问题和后果自行负责，与作者和UPRE实验室无关。

作者：丁欣童
---------------------------------------------------------------------*/
#ifndef _UPRE_LINUX_USART_HPP_
#define _UPRE_LINUX_USART_HPP_

/*---------------------------（编译选项）------------------------------*/
#define UPRE_FLAG_CROSS_DETECT   1           //  1 开启越界检测            0 关闭越界检测
#define UPRE_FLAG_TCFLUSH        0           //  1 启用tcflag标识符功能    0 关闭tcflag标识符功能
#define UPRE_FLAG_BLOCK          1           //  1 阻塞模式               0 非阻塞模式
/*-------------------------------------------------------------------*/


/*----------------------------（头文件）-------------------------------*/
#include "vector"
#include "sys/types.h"
#include "unistd.h"     
/*-------------------------------------------------------------------*/


/*-------------------------------------------------------------------*/

namespace upre
{
    typedef std::vector<unsigned char>  v_uchar;
    typedef unsigned char uchar;
    typedef void (*handler)(int);


    typedef enum
    {
        NO_TCFLUSH=0,
        BEFORE_TCFLUSH=1,
        AFTER_TCFLUSH =2,
        BEFORE_AND_AFTER_TCFLUSH=3,
    }FLAG_TCFLUSH;


    typedef enum 
    {
        usart0=0,     //usb模拟串口
        usart1=1,
        usart2=2,
        usart3=3,
        usart4=4,
        usart5=5,     //串口
        usart6=6,
        usart7=7,
    }USART_NUM;
    
    class Linux_Usart
    {
        public:
        /*--------------------------------------------------------
        @函数：Linux_Usart
        @功能：Linux_Usart类的构造函数，打开并初始化串口。
        @参数：参数一：flag为USART_NUM类型的枚举，代表要打开并初始化的串口。
                      有默认值usart0。
              参数二：rate为串口速率，可选参数，有默认值19200。取值为9600、19200、115200、460800、1500000
        --------------------------------------------------------*/
        Linux_Usart(USART_NUM flag=usart0,int rate=19200);





        /*--------------------------------------------------------
        @函数： ~Linux_Usart
        @功能：Linux_Usart类的析构函数，关闭串口。
        --------------------------------------------------------*/
        ~Linux_Usart();
        
        
        
        

        /*--------------------------------------------------------
        @函数：Read_Data(重载1)
        @功能：从串口读取readLengh个字节的数据存到data中。
        @参数：参数一：data为unsigned char类型的vector，读出的数据存在data
                      对象里。
              参数二：readLengh 要读取的字节个数。有默认值0，表示读取和data
                     长度大小一样的字节数。
              参数三: startLocation为操作data的起始位置。默认为0，表示从data[0]开始写入。
              
              参数四：tcFlag为清除缓冲区标识符，可选参数，默认值NONO_TCFLUSH
                        NO_TCFLUSH                 无清除
                        BEFORE_TCFLUSH             读取之前清除
                        AFTER_TCFLUSH              读取之后清除
                        BEFORE_AND_AFTER_TCFLUSH   读取前后同时清除
        @返回值：返回读取到的字节数，如读取失败，则返回 -1                 
        --------------------------------------------------------*/
        ssize_t Read_Data(v_uchar &data,size_t readLengh=0,size_t startLocation=0,FLAG_TCFLUSH tcFlag=NO_TCFLUSH);
        /*--------------------------------------------------------
        @函数：Read_Data(重载2)
        @功能：从串口读取readLengh个字节的数据存到data指向的数组中。
        @参数：参数一：data为unsigned char，读出的数据存在data指向的数组中。
              参数二：readLengh 要读取的字节个数。
              参数三: startLocation为操作data的起始位置。默认为0，表示从data[0]开始写入。
        @返回值：返回读取到的字节数，如读取失败，则返回 -1                 
        --------------------------------------------------------*/
        ssize_t Read_Data(uchar   *data,size_t readLengh  ,size_t startLocation=0);
        
        template<class _tp> ssize_t Read_Data(_tp* data,size_t readlengh,size_t startLocation=0);
        
        ssize_t operator >> (v_uchar &data);
        
        ssize_t operator >> (uchar &data);


        /*--------------------------------------------------------
        @函数：Write_Data(重载1)
        @功能：从data中向串口写入writeLengh个字节的数据。
        @参数：参数一：data为unsigned char类型的vector，为要写入的数据
              参数二：writeLengh 要写入的字节个数。有默认值0，表示把data
                     全部写入。
              参数三: startLocation为操作data的起始位置。默认为0，表示从data[0]开始读取。
              
              参数四：tcFlag为清除缓冲区标识符，可选参数，默认值NONO_TCFLUSH
                        NO_TCFLUSH                 无清除
                        BEFORE_TCFLUSH             读取之前清除
                        AFTER_TCFLUSH              读取之后清除
                        BEFORE_AND_AFTER_TCFLUSH   读取前后同时清除
        @返回值：返回写入进的字节数，如写入失败，则返回 -1
        --------------------------------------------------------*/      
        ssize_t Write_Data(v_uchar &data,size_t writeLengh=0,size_t startLocation=0,FLAG_TCFLUSH tcFlag=NO_TCFLUSH);
        /*--------------------------------------------------------
        @函数：Write_Data(重载2)
        @功能：从data中向串口写入writeLengh个字节的数据。
        @参数：参数一：data为unsigned char，指向要写入的数组。
              参数二：writeLengh 要写入的字节个数。有默认值0。
              参数三: startLocation为操作data的起始位置。
        @返回值：返回写入进的字节数，如写入失败，则返回 -1
        --------------------------------------------------------*/
        ssize_t Write_Data(uchar   *data,size_t writeLeng   ,size_t startLocation=0);
        
        template<class _tp> ssize_t Write_Data(_tp *data,size_t writeLeng,size_t startLocation=0);

        ssize_t operator << (v_uchar &data);

        ssize_t operator << (uchar &data);
       	
        ssize_t operator << (const char str[]);
        
        
        
        
        /*--------------------------------------------------------
        @函数：Set_Handler
        @功能：
        @返回值：
        --------------------------------------------------------*/
        void Set_Handler(handler h);
        
        
        
        
        /*--------------------------------------------------------
        @函数：Get_fd
        @功能：获取当前Linux_Usart对象的串口描述符fd。
               借助这个函数可以用write函数和read函数直接操作串口。
               如：  unsigned char data=0x55;
                     Linux_Usart usart(usart0,19200);
                     read(usart.Get_fd(),&data,1);
                     write(usart.Get_fd(),&data,1);
        @返回值：串口文件描述符fd
        --------------------------------------------------------*/
        int Get_fd();
        




        private:
        /*--------------------------------------------------------
        @变量：fd
        @功能：open函数返回的文件描述符。
        --------------------------------------------------------*/
        int fd; 
    };
}



#endif

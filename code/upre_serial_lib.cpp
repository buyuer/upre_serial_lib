#include "stdio.h"
#include "stdlib.h"

#include "iostream"
#include "vector"

#include "unistd.h"     //UNIX标准头文件
#include "fcntl.h"      //UNIX标准头文件
#include "termios.h"    //UNIX标准头文件

#include "errno.h"

#include "sys/types.h"
#include "sys/stat.h"

#include "upre_serial_lib.hpp"

using namespace upre;
using namespace std;

Linux_Usart::Linux_Usart(USART_NUM flag,int rate)
{
    struct termios option;

    switch(flag)
    {
#if UPRE_FLAG_BLOCK == 0       
        case 0:fd=open("/dev/ttyUSB0",O_RDWR|O_NOCTTY|O_NONBLOCK); break;
        case 1:fd=open("/dev/ttyUSB1",O_RDWR|O_NOCTTY|O_NONBLOCK); break;
        case 2:fd=open("/dev/ttyUSB2",O_RDWR|O_NOCTTY|O_NONBLOCK); break;
        case 3:fd=open("/dev/ttyUSB3",O_RDWR|O_NOCTTY|O_NONBLOCK); break;
        case 4:fd=open("/dev/ttyUSB4",O_RDWR|O_NOCTTY|O_NONBLOCK); break;
        case 5:fd=open("/dev/ttyTHS1",O_RDWR|O_NOCTTY|O_NONBLOCK); break;
        case 6:fd=open("/dev/ttyTHS2",O_RDWR|O_NOCTTY|O_NONBLOCK); break;
        case 7:fd=open("/dev/ttyTHS3",O_RDWR|O_NOCTTY|O_NONBLOCK); break;
#endif
#if UPRE_FLAG_BLOCK == 1
        case 0:fd=open("/dev/ttyUSB0",O_RDWR|O_NOCTTY); break;
        case 1:fd=open("/dev/ttyUSB1",O_RDWR|O_NOCTTY); break;
        case 2:fd=open("/dev/ttyUSB2",O_RDWR|O_NOCTTY); break;
        case 3:fd=open("/dev/ttyUSB3",O_RDWR|O_NOCTTY); break;
        case 4:fd=open("/dev/ttyUSB4",O_RDWR|O_NOCTTY); break;
        case 5:fd=open("/dev/ttyTHS1",O_RDWR|O_NOCTTY); break;
        case 6:fd=open("/dev/ttyTHS2",O_RDWR|O_NOCTTY); break;
        case 7:fd=open("/dev/ttyTHS3",O_RDWR|O_NOCTTY); break;
#endif
    }
    
    
    if(fd == -1)
    {
        perror("open serial\n");
        exit(0);
    }

    
    if(tcgetattr(fd,&option)!=0)
    {
        perror("SetupSerial 1");
        exit(0);
    }

    switch(rate)
    {
        case 9600   : cfsetispeed(&option,B9600);    cfsetospeed(&option,B9600);     break;
        case 19200  : cfsetispeed(&option,B19200);   cfsetospeed(&option,B19200);    break;
        case 115200 : cfsetispeed(&option,B115200);  cfsetospeed(&option,B115200);   break;
        case 460800 : cfsetispeed(&option,B460800);  cfsetospeed(&option,B460800);   break;
        case 1500000: cfsetispeed(&option,B1500000); cfsetospeed(&option,B1500000);  break;
        default:      cfsetispeed(&option,B19200);   cfsetospeed(&option,B19200);    break;
    }
    
  

    option.c_cflag &= ~CSIZE;
    option.c_cflag &= ~CSTOPB;
    option.c_cflag &= ~PARODD;
    option.c_cflag &= ~PARENB;
    option.c_cflag |=  CREAD | CLOCAL;
    option.c_cflag |=  CS8;

    option.c_oflag &= ~OPOST;
    option.c_oflag &= ~(ONLCR | OCRNL | ONOCR);
    

    option.c_iflag &= ~INPCK;
    option.c_iflag &= ~ISTRIP;
    option.c_iflag &= ~(ICRNL | INLCR);
    option.c_iflag &= ~(IXON | IXANY | IXOFF);

    option.c_lflag &= ~(ISIG | ECHO |  ECHOE);
    option.c_lflag &= ~ICANON;

    option.c_cc[VTIME] = 0;
    option.c_cc[VMIN]  = 0;
    
    tcflush(fd, TCIOFLUSH);

    if(tcsetattr(fd,TCSANOW,&option) != 0)
    {
        perror("com ser error");
        exit(0);  
    }
}

Linux_Usart::~Linux_Usart()
{
    close(fd);
}

ssize_t Linux_Usart::Read_Data(v_uchar &data,size_t readLengh,size_t startLocation,FLAG_TCFLUSH tcFlag)
{
    size_t readNum=0;
    
    if(readLengh == 0)
    {
        readLengh=data.size();
    }
#if UPRE_FLAG_CROSS_DETECT==1
    else if(readLengh > data.size() - startLocation)
         {
             cout<<"error: readLengh > data.size() <=> 越界"<<endl;
             return readNum;
         }
#endif

#if UPRE_FLAG_TCFLUSH==1
    if(tcFlag==1 || tcFlag==3)
    {
        tcflush(fd,TCIFLUSH);
    }
#endif
    
    readNum=read(fd,&data[startLocation],readLengh);
    if(readNum == -1)
    {
        perror("read STDIN_FILENO");
        return -1;
    }
    else
    {
#if UPRE_FLAG_CROSS_DETECT==1
        if(readNum > 0 && readNum < readLengh)
        {
            cout<<"warning: readNum: "<<readNum<<" < readLengh: "<<readLengh<<endl;
        }
#endif

#if UPRE_FLAG_TCFLUSH==1
        if(tcFlag==2 || tcFlag==3)
        {
            tcflush(fd,TCIFLUSH);
        }
#endif
        return readNum;
    } 
}


ssize_t Linux_Usart::Read_Data(uchar *data,size_t readLengh  ,size_t startLocation)
{
    return read(this->fd,data+startLocation,(readLengh));
}

template<class _tp> ssize_t Linux_Usart::Read_Data(_tp* data,size_t readLengh,size_t startLocation)
{
    return read(this->fd,(void *)(data+startLocation),readLengh);
}

ssize_t Linux_Usart::Write_Data(v_uchar &data,size_t writeLengh,size_t startLocation,FLAG_TCFLUSH tcFlag)
{
    size_t writeNum=0;

    if(writeLengh == 0)
    {
        writeLengh=data.size();
    }

#if UPRE_FLAG_CROSS_DETECT==1
    else if(writeLengh > data.size() - startLocation)
         {
             cout<<"error: writeLengh > data.size()  <=> 越界"<<endl;
             return writeNum;
         }
#endif

#if UPRE_FLAG_TCFLUSH==1
    if(tcFlag==1 || tcFlag==3)
    {
        tcflush(fd,TCOFLUSH);
    }
#endif

    writeNum=write(fd,&data[startLocation],writeLengh);
    if(writeNum == -1)
    {
        perror("write STDIN_FILENO");
        return -1;
    }

    else
    {
#if UPRE_FLAG_CROSS_DETECT==1  
        if(writeNum > 0 && writeNum < writeLengh)
        {
            cout<<"warning: writeNum: "<<writeNum<<" < writeLengh: "<<writeLengh<<endl;
        }
#endif

#if UPRE_FLAG_TCFLUSH==1
        if(tcFlag==2 || tcFlag==3)
        {
            tcflush(fd,TCOFLUSH);
        }
#endif
        
        return writeNum;
    }

    
}

ssize_t Linux_Usart::Write_Data(uchar *data,size_t writeLeng   ,size_t startLocation)
{
    return write(this->fd,data+startLocation,writeLeng);
}

template<class _tp> ssize_t Linux_Usart::Write_Data(_tp *data,size_t writeLeng,size_t startLocation)
{
    return write(this->fd,(void*)(data+startLocation),writeLeng);
}

ssize_t Linux_Usart::operator >> (v_uchar &data)
{
    return this->Read_Data(data);
}

ssize_t Linux_Usart::operator << (v_uchar &data)
{
    return this->Write_Data(data);
}

ssize_t Linux_Usart::operator >> (uchar &data)
{
    return read(this->fd,&data,1);
}

ssize_t Linux_Usart::operator << (uchar &data)
{
    return write(this->fd,&data,1);
}

ssize_t Linux_Usart::operator<<(const char str[])
{
    size_t lengh = 0;
    while(*str != '\0')
    {
        str++;
        lengh++;
    }
    return write(fd,(str-lengh),lengh);
}

int Linux_Usart::Get_fd()
{
    return fd;
}


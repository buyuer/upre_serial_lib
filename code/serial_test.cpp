#include "unistd.h"
#include "signal.h"
#include "fcntl.h"

#include "upre_serial_lib.hpp"
#include "iostream"
#include "cstdio"



using namespace std;
using namespace upre;



void serialHandler(int sig)
{
    //usart.Read_Data(data,1);
    printf("The signal is %d,the data is\n",sig);
}

int main(int argc,char** argv)
{   
    Linux_Usart usart(usart0,115200);
    v_uchar data(256);
    
    /*struct sigaction act;
    act.sa_handler = serialHandler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGPOLL,&act,0);

    fcntl(usart.Get_fd(),F_SETSIG,SIGPOLL);
    fcntl(usart.Get_fd(),F_SETFL,O_ASYNC | O_NONBLOCK);*/

    usart.Set_Handler([](int sig)->void{ 
        printf("The signal is %d,the data is\n",sig);
    });
    //usart.Set_Handler(serialHandler);
    
    
    for(size_t i=0;i<256;i++)
    {
        data[i]=i;
    }
    for(size_t i=0;i<1;i++)
    {
        cout<<i<<": ";
        cout<<usart.Write_Data(data,256)<<endl;
    }
    while(1)
    {
        /*if(int num=usart.Read_Data(data,0))
        {
            cout<<"num: "<<num<<endl;
    
            for(int i=0;i<num;i++)
            {
                printf("data[%d]:%x \n",i,data[i]);
            }
            
            if(data[0]==0x00)
            {
                break;
            }
        }*/
    }
    return 0;
}

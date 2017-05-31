/************************************************************/
/*brief: this is cpp class package for posix linux interface*/
/*author: shuzhengyang  Yang.Shu                            */
/*date: 2017/5/30                                           */
/************************************************************/
#include <iostream>
#include <pthread.h>
#include <mqueue.h>
#include <errno.h> 
#include <cstdlib>

#undef NULL
#define NULL ((void *)0)
//posix 消息队列封装
//消息名称最大32字节
class posix_mq{
    private:
        mqd_t mqt;
        char mq_name[32];
    public:
        //构造函数
        posix_mq(char *mq_name,int maxmsg,int msgsize) throw (char *);
        //消息队列创建／打开
        bool Mq_open(char *mq_name,int maxmsg,int msgsize) throw(char *);
        //消息队列关闭
        bool Mq_close();
        //消息队列删除
        bool Mq_destroy();
        //消息队列发送
        bool Mq_send(char *send_buff,size_t send_len,unsigned int msg_prio,int time_wait);
        //消息队列接受
        bool Mq_receive();
        //消息队列状态获取
        bool Mq_state_get(mq_attr *mq_attr_get);
        //消息队列状态设置
        bool Mq_state_set(int flag);
};
posix_mq::posix_mq(char *mq_name,int maxmsg,int msgsize) throw (char *)
{
    mqt = NULL;
    try{
        Mq_open(mq_name,maxmsg,msgsize);
    }
    catch(char *ret_str)
    {
        throw ret_str;
    }
}
bool posix_mq::Mq_open(char *mq_name,int maxmsg,int msgsize) throw(char *)
{
    int c;  
    char ret_char[64];
    int flag = O_RDWR | O_CREAT;  
    mq_attr mqAttr;

    memset(ret_char,0,64);
    if(strlen(mq_name)>31)
    {
        throw "mq_name must short than 32 bits!";
        return false;
    }
    if(maxmsg<=0 || msgsize<0)
    {
        throw "maxmasg or masgsize must bigger than 0!";
        return false;
    }
    //flag |= O_EXCL;  
    mqAttr.mq_maxmsg = maxmsg;
    mqAttr.mq_msgsize = msgsize;


    mqt = mq_open(mq_name,flag,0666,&mqAttr);  
    if(mqt == -1)  
    {  
        sprintf(ret_char,"creat posix mqd  error %d : %s\n", errno, strerror(errno));  
        throw ret_char;  
        return false;
    }  
    memcpy(this->mq_name,mq_name,strlen(mq_name));
    return true;
}
bool posix_mq::Mq_close()
{
    if(mq_close(mqt)<0)
    {
        std::cout<<"Mq_close error"<<std::endl;  
        return false;
    }
    else
    {
        return true;
    }
}
bool posix_mq::Mq_destroy()
{
    if(mq_unlink(this->mq_name)<0)
    {
        std::cout<<"Mq_destroy error"<<std::endl;  
        return false;
    }
    else
    {
        return true;
    }
}
bool posix_mq::Mq_send(char *send_buff,size_t send_len,unsigned int msg_prio,int time_wait)
{
    int ret =0;
    struct timespec abs_time_out;

    if(time_wait < 0)
    {
        ret = mq_send(mqt,send_buff,send_len,msg_prio);
    }
    else
    {
        // #TODO
        ret = mq_timedsend(mqt,send_buff,send_len,msg_prio,abs_time_out);
    }
    if(ret < 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}
ssize_t posix_mq::Mq_receive(char *rec_buff,size_t rec_len,unsigned int msg_prio,int time_wait)
{
    int ret = 0;
    struct timespec abs_time_out;

    if(time_wait < 0)
    {
        ret = mq_receive(mqt,rec_buff,rec_len,msg_prio);
    }
    else
    {
        //#TODO
        ret = mq_timedreceive(mqt,rec_buff,rec_len,msg_prio,abs_time_out);
    }

    if(ret < 0)
    {
        return false;
    }
    else
    {
        return true;
    }


}
bool posix_mq::Mq_state_get(mq_attr *mq_attr_get)
{
    if (mq_getattr(mqt,mq_attr_get) < 0)  
    {  
        std::cout<<"get the message queue attribute error"<<std::endl;  
        return false;  
    }  
    else
    {
        return true;
    }
}
bool posix_mq::Mq_state_set(int flag)
{
    mq_attr mq_attr_set;

    memset(&mq_attr_set,0,sizeof(mq_attr));
    Mq_state_get(&mq_attr_set);
    // Message queue flags：0，O_NONBLOCK
    mq_attr_set.mq_flags = flag;
    if(mq_setattr(mqt,&mq_attr_set)<0)
    {
        std::cout<<"Mq_state_set error"<<std::endl;  
        return false;   
    }
    else
    {
        return true;
    }
}

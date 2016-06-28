/*
1. change the dst_mac;
2. change the src_ip;
3. change the net interface。
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>    //struct ifreq
#include <sys/ioctl.h>   //ioctl、SIOCGIFADDR
#include <sys/socket.h>
#include <netinet/ether.h>  //ETH_P_ALL
#include <netpacket/packet.h> //struct sockaddr_ll

#define DST_IP_COUNT (1024) 
#define NUM_PKT_PER_SECOND (200000)


int dst_ips[DST_IP_COUNT][4] = {0};
int dst_net[4]={175,51,0,0};
unsigned char send_udp_msg[1024] = {
//--------------组MAC--------14------
0x58, 0x66, 0xba, 0x11, 0xec, 0xba, //dst_mac: 58:66:ba:11:ec:ba
0x00, 0x0c, 0x29, 0xc1, 0x8a, 0x3f, //src_mac: 00:0c:29:c1:8a:3f
0x08, 0x00,                        //类型：0x0800 IP协议
//--------------组IP---------20------
0x45, 0x00, 0x00, 0x00,            //版本号：4, 首部长度：20字节, TOS:0, --总长度--：
0x00, 0x00, 0x00, 0x00,            //16位标识、3位标志、13位片偏移都设置0
0x80, 17,  0x00, 0x00,            //TTL：128、协议：UDP（17）、16位首部校验和
111,  12,  118,  153,              //src_ip: 111.12.118.53
218,  207,  195,  203,              //dst_ip: 218.207.195.203
//--------------组UDP--------8+78=86------
0x1f, 0x90, 0x1f, 0x90,            //src_port:0x1ff0(8080), dst_port:0x1f9f(8080)
0x00, 0x00, 0x00, 0x00,              //#--16位UDP长度--30个字节、#16位校验和
};

unsigned char pseudo_head[1024] = {
//------------UDP伪头部--------12--
111,  12,  118,  153,    //src_ip: 111.12.118.53
218,  207,  195,  203,    //dst_ip: 218.207.195.203
0x00, 17,  0x00, 0x00,             //0,17,#--16位UDP长度--20个字节
};

unsigned short checksum(unsigned short *buf, int nword);//校验和函数

int init_dst_ips() {
	int i;
	int j=1, k=1;
	for(i=0; i<DST_IP_COUNT; i++) {
		dst_ips[i][0] = dst_net[0];
		dst_ips[i][1] = dst_net[1];
		if (k >= 254) {k = 1; j++; }
		if (j >= 254) {
			printf("error, net is not big enough!!\n"); 
			exit(-1); 
		}
		dst_ips[i][2] = j;
		dst_ips[i][3] = k++; 
	}
}


void print_dst_ips() {
	(void) init_dst_ips();
	int i,j;
	for(i = 0 ; i < DST_IP_COUNT; i++ )
	{
		for (j = 0; j < 4; j++)
			printf("%d.", dst_ips[i][j]);
		printf("\n");
	}
}


int create_udp_pkt() {	
	int len = sprintf(send_udp_msg+42, "%s", "aaaa bbbb cccc dddd eeee ffff 1111 2222 3333 4444 xxxx");
    if(len % 2 == 1)//判断len是否为奇数
    {
        len++;//如果是奇数，len就应该加1(因为UDP的数据部分如果不为偶数需要用0填补)
    }
	
	//1. UDP/IP及伪UDP头长度

	*((unsigned short *)&send_udp_msg[16]) = htons(20+8+len);//IP总长度 = 20 + 8 + len
    *((unsigned short *)&send_udp_msg[14+20+4]) = htons(8+len);//udp总长度 = 8 + len
    *((unsigned short *)&pseudo_head[10]) = htons(8 + len);//为头部中的udp长度（和真实udp长度是同一个值）
	//2.填充随机化目的IP，特殊需求

	int ip_num = rand()%1024;
    send_udp_msg[30] = dst_ips[ip_num][0];
    send_udp_msg[31] = dst_ips[ip_num][1];
    send_udp_msg[32] = dst_ips[ip_num][2];
    send_udp_msg[33] = dst_ips[ip_num][3];

    pseudo_head[4] = dst_ips[ip_num][0];
    pseudo_head[5] = dst_ips[ip_num][1];
    pseudo_head[6] = dst_ips[ip_num][2];
    pseudo_head[7] = dst_ips[ip_num][3];

    //3.构建udp校验和需要的数据报 = udp伪头部 + udp数据报
    memcpy(pseudo_head+12, send_udp_msg+34, 8+len);//--计算udp校验和时需要加上伪头部--
	
    //4.对IP首部进行校验
    *((unsigned short *)&send_udp_msg[24]) = htons(checksum((unsigned short *)(send_udp_msg+14),20/2));
    //5.--对UDP数据进行校验--
    *((unsigned short *)&send_udp_msg[40]) = htons(checksum((unsigned short *)pseudo_head,(12+8+len)/2));	
	return len;
}

int main(int argc, char *argv[])
{
	int len = 0;
	int i = 0;
	if (-1 == init_dst_ips()){
		printf("init_dst_ips error \n");
		exit(-1);
	}
	//1.创建通信用的原始套接字
	int sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));  
	//2.创建并发送数据
	struct sockaddr_ll sll;     //原始套接字地址结构
	struct ifreq ethreq;     //网络接口地址                                                                  
	strncpy(ethreq.ifr_name, "bond0", IFNAMSIZ);   //指定网卡名称
	if(-1 == ioctl(sock_raw_fd, SIOCGIFINDEX, &ethreq)) //获取网络接口
	{
		perror("ioctl");
		close(sock_raw_fd);
		exit(-1);
	}
	/*将网络接口赋值给原始套接字地址结构*/
	bzero(&sll, sizeof(sll));
	sll.sll_ifindex = ethreq.ifr_ifindex;
	while (1)
	{
		len = create_udp_pkt();
		len = sendto(sock_raw_fd, send_udp_msg, 14+20+8+len+512, 0 , (struct sockaddr *)&sll, sizeof(sll));
		if (len == -1) perror("sendto");
		if (0 == ((++i)%NUM_PKT_PER_SECOND)) {
		    sleep(1);
		    i = 0;
		}
	}

	return 0;
}

unsigned short checksum(unsigned short *buf, int nword)
{
	unsigned long sum;
	for(sum = 0; nword > 0; nword--)
	{
		sum += htons(*buf);
		buf++;
	}
	sum = (sum>>16) + (sum&0xffff);
	sum += (sum>>16);
	return ~sum;
}

#ifndef Simdisk
#define Simdisk
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cmath>
#include<algorithm>
#include<vector>
#include<queue>
#include<cstring>
#include<string>
#include<string.h>
#include<fstream>
#include<istream>
#include<ostream>
#include<iomanip>
#include<Windows.h>
#include<conio.h>
#include<tchar.h>
#include<atlconv.h> 
using namespace std;
#define BUF_SIZE 8192 //缓冲区大小
char NameUser[] = "USER";
char NameRW[] = "READERWRITER";
char NameIn[50] = "INPUT";
char NameOut[50] = "OUTPUT";
char NameIoo[50] = "INPUTOROUTPUT";

typedef double db;
const unsigned int ERR = 1e9 + 7;
const unsigned int root = 0;
fstream f;
ofstream fout;

const string FileName = "FileSystem";                    //文件名
const unsigned int TotalBlockNum = 102400;               //总盘块数
const unsigned int BootNum = 1;			                 //引导块
const unsigned int SuperNum = 1;			             //超级块
const unsigned int GroupDesNum = 1;		                 //组描述符
const unsigned int BlockBitMapNum = 16;		             //块位图
const unsigned int iNodeBitMapNum = 4;		             //iNode位图
const unsigned int iNodeTableNum = 4096;	             //iNode表项
const unsigned int DataBlockNum = 98281;       		     //数据块个数
const unsigned int RootDirNum = 0;                       //根目录号
const unsigned int BlockSize = 1024;                     //磁盘块大小1024B
const unsigned int iNodeSize = 128;                      //iNode大小128B


const unsigned int TotaliNodeNum = 32768;                //iNode总数
const unsigned int FileNameUpLenth = 32;                 //文件名长度上限
const unsigned int FileUpNum = 32768;		             //总文件数量上限
const unsigned int SubfileUpNum = 256;		             //每个目录文件下的文件数量上限

unsigned int CurrentPath = RootDirNum;                   //当前路径
char CurrentUser[50];                                    //当前用户名
unsigned int CurrentUserId;                              //当前用户名id
unsigned int CurrentMode;                                //当前命令的模式 0不可读写 1可读 2可写

class BootBlock {
public:
	unsigned int tempdata[256];

	BootBlock() {
		memset(tempdata, 0, sizeof(tempdata));
	}
};

class SuperBlock {//超级块
public:
	unsigned int block_size;                            //超级块大小
	unsigned int inode_size;                            //结点大小
	unsigned int file_upnum;                            //文件数量上限
	unsigned int subfile_upnum;                         //子文件数量上限
	unsigned int filenameup_lenth;                      //文件名长度上限
	unsigned int totalblock_num;                        //总盘块数
	unsigned int datablock_num, use_datablock;          //数据块个数，已使用数据块数
	unsigned int totalinode_num, use_inode;             //总inode数，已使用inode数

	unsigned int tempdata[246];

	SuperBlock() {
		block_size = BlockSize;
		inode_size = iNodeSize;
		file_upnum = FileUpNum;
		subfile_upnum = SubfileUpNum;
		filenameup_lenth = FileNameUpLenth;
		totalblock_num = TotalBlockNum;
		datablock_num = DataBlockNum;
		use_datablock = 1;
		totalinode_num = TotaliNodeNum;
		use_inode = 1;

		memset(tempdata, 0, sizeof(tempdata));
	}
};

class GroupDescription {//组描述符
public:
	unsigned int group_begin, group_end;                       //组首尾地址
	unsigned int super_begin, super_end;                       //超级块首尾地址
	unsigned int groupdes_begin, groupdes_end;                 //组描述符首位地址
	unsigned int blockbm_begin, blockbm_end;                   //块位图首尾地址
	unsigned int inodebm_begin, inodebm_end;                   //点位图首尾地址
	unsigned int inodetb_begin, inodetb_end;                   //inode首尾地址
	unsigned int data_begin, data_end;                         //数据首尾地址

	unsigned int tempdata[242];

	GroupDescription() {
		group_begin = 1; group_end = 102399;
		super_begin = 1; super_end = 1;
		groupdes_begin = 2; groupdes_end = 2;
		blockbm_begin = 3; blockbm_end = 18;
		inodebm_begin = 19; inodebm_end = 22;
		inodetb_begin = 23; inodetb_end = 4118;
		data_begin = 4119; data_end = 102399;

		memset(tempdata, 0, sizeof(tempdata));
	}
};

class BlockBitmap {//块位图
public:
	unsigned int use[4096];
	BlockBitmap() {
		memset(use, 0, sizeof(use));
		use[0] = 2147483648;//根目录占用第0个bitmap
	}
};

class iNodeBitmap {//点位图
public:
	unsigned int use[1024];
	iNodeBitmap() {
		memset(use, 0, sizeof(use));
		use[0] = 2147483648;//根目录占用第0个bitmap
	}
};

class iNode {//i结点
public:
	char name[FileNameUpLenth];           //文件名
	unsigned int current_pos;             //当前iNode号
	unsigned int next_pos;				  //下一个iNode号
	unsigned int type;					  //0目录 1二进制文件
	unsigned int subfile_num;			  //记录目录下文件数量
	unsigned int user_id;				  //创建人id
	unsigned int mode;					  //0不可读写 1可读 2可写 
	unsigned int usedblock_num;			  //文件占用块数
	unsigned int block_pos[17];			  //存在数据区的块号

	iNode() {//初始化为根目录
		memset(name, '\0', sizeof(name)); name[0] = '/';
		current_pos = ERR;
		next_pos = ERR;
		type = 0;
		subfile_num = 0;
		user_id = root;
		mode = 2;
		usedblock_num = 1;
		block_pos[0] = 0;
		for (int i = 1; i < 17; i++) block_pos[i] = ERR;
	}
	void dele() {//删除回收
		memset(name, '\0', sizeof(name)); name[0] = '/';
		current_pos = ERR;
		next_pos = ERR;
		type = 0;
		subfile_num = 0;
		user_id = root;
		mode = 2;
		usedblock_num = 1;
		block_pos[0] = 0;
		for (int i = 1; i < 17; i++) block_pos[i] = ERR;
	}
	void clear2() {//清空
		memset(name, '\0', sizeof(name));
		current_pos = ERR;
		next_pos = ERR;
		type = 0;
		subfile_num = 0;
		user_id = root;
		mode = 2;
		usedblock_num = 0;
		for (int i = 0; i < 17; i++) block_pos[i] = ERR;
	}
};

class iNodeTable {//i结点表
public:
	iNode inode[32768];
};

class Block {//盘块
public:
	unsigned int data[256];
	Block() {
		memset(data, 0, sizeof(data));
	}
	void clear() {
		memset(data, 0, sizeof(data));
	}
};

class DataBlock {
public:
	Block block[98281];
};

class Path {//文件路径
public:
	unsigned int cnt;
	string  ph[3000];

	Path() {
		cnt = 0;
		for (int i = 0; i < 3000; i++) ph[i] = "";
	}
	void clear() {
		cnt = 0;
		for (int i = 0; i < 3000; i++) ph[i] = "";
	}
}path;

class Order {//用于用户输入的命令分解，方便判断
public:
	unsigned int cnt;
	string ord[20];
	unsigned int type;

	Order() {
		cnt = 0;
		for (int i = 0; i < 20; i++) ord[i] = "";
		type = ERR;
	}
	void clear() {
		cnt = 0;
		for (int i = 0; i < 20; i++) ord[i] = "";
		type = ERR;
	}
}order;

class User {//用户名，用于与shell端建立连接
public:
	char name[50];

	User() {
		memset(name, '\0', sizeof(name));
	}
};

class ReaderWriter {//实现读者--写者问题的共享内存
public:
	//判断当前进程是不是第一个启动的进程，如果是则需要初始化读者写者的共享内存
	bool isfirst;

	//读者写者问题的信号量和计数参数
	int rw;
	int mutex;
	int count;

	//唤醒与等待
	char wakeup1[50], wakeup2[50];
	int cnt1, cnt2;
	char wait1[30][50], wait2[30][50];

	ReaderWriter() {
		isfirst = false;

		rw = 0;
		mutex = 0;
		count = 0;

		memset(wakeup1, '\0', sizeof(wakeup1));
		memset(wakeup2, '\0', sizeof(wakeup2));
		cnt1 = 0; cnt2 = 0;
		memset(wait1, '\0', sizeof(wait1));
		memset(wait2, '\0', sizeof(wait2));
	}
};

class ShareMemory {//共享内存
public:
	int cnt;
	char str[20][300];
	ShareMemory() {
		cnt = 0;
		memset(str, '\0', sizeof(str));
	}
	void clear() {
		cnt = 0;
		memset(str, '\0', sizeof(str));
	}
};

class InputOrOutput {
public:
	int toshell;//simdisk给shell的通知
	int tosimdisk;//shell给simdisk的通知
	InputOrOutput() {
		toshell = 0;
		tosimdisk = 0;
	}
};

iNodeTable inodetb;
ShareMemory sminput, smoutput;
InputOrOutput ioo;
queue<Block> buffer;
queue<unsigned int> qdirinode;
BootBlock bootblock;
SuperBlock superblock;
GroupDescription groupdes;
BlockBitmap blockbitmap;
iNodeBitmap inodebitmap;

HANDLE hMapFileUser;
User* pBufUser = NULL;

HANDLE hMapFileRW;
ReaderWriter strw;
ReaderWriter* pBufRW = &strw;

HANDLE hMapFileIoo;
InputOrOutput stioo;
InputOrOutput* pBufIoo = &stioo;

HANDLE hMapFileIn;
ShareMemory* pBufIn = NULL;

HANDLE hMapFileOut;
ShareMemory stout;
ShareMemory* pBufOut = &stout;


void Run();//运行程序

void GetUser();//获取用户名，与simdisk建立连接

void GetUserId();//获得用户id

unsigned int ToInt(string s);//将字符串转化为数字

void InitReader_Writer();//初始化共享内存

void Prw(); //申请读写信号量

void Vrw();//释放读写信号量

void Pmutex();//进程执行

void Vmutex();//进程等待

void Reader1();//读者开始读

void Reader2();//读者完成读

void Writer1();//写者开始写

void Writer2();//写者完成写

void InterpretOrder(Order& ord);//获取用户输入的命令

unsigned int FindFirstZero(unsigned int x); //找x转化为二进制后的第一个0的位置，用于后续寻找空闲块

unsigned int FindFreeBlock();//找到空闲块

unsigned int FindFreeINode();//找到空闲inode

bool FindFileSystem();//查找文件系统

void FindAbsolutePath(string& relpath);//将相对路径转化为绝对路径

unsigned int FindFileINode(unsigned int nowdir, string filename);//找相对路径下文件的inode号

unsigned int FindFileINode(string s);//找绝对路径下文件inode号，s为绝对路径


void ReadFileSystem();//读文件系统

void ReadBlock(unsigned int pos, Block& bk);//读块盘信息

void ReadShareMemory();//读共享内存

void WriteFileSystem();//写文件系统

void WriteBlock(unsigned int pos, Block& bk);//写块盘

void WriteShareMemory();//写共享内存

void CreateFileSystem();//如果finddisk（ ）返回false，则创建文件系统

unsigned int CreateNewFile(unsigned int fathinode, string filename);//新建文件

unsigned int CreateNewDir(unsigned int fathinode, string dirname);//新建目录

void RemoveFileINodeBitmap(unsigned int nowinode);//释放inode所在inode位图表位置

void RemoveFileBlockBitmap(unsigned int nowinode);//释放inode所在块位图位置

void RemoveFileDataBlock(unsigned int nowinode);//释放块盘

void RemoveFile(unsigned int nowinode);//删除文件

void RemoveEmptyDir(unsigned int nowinode);//删除空目录

void RemoveDir(unsigned int nowinode);//删除非空目录

void CatRead(unsigned int nowinode);//读文件

void CatReadToHost(unsigned int nowinode, string path);//读文件并写入主机

void CatWrite(unsigned int nowinode);//写文件


void CopyHostToBuffer(string hostpath);//拷贝文件到缓冲区

void CopyBufferToLinux(unsigned int nowinode);//拷贝缓冲区数据到文件系统

void CopyLinuxToLinux(unsigned int inode1, unsigned int inode2);//拷贝文件

void CopyHost(string filename, string hostpath, unsigned int dirinode);//拷贝主机文件到系统

void CopyLxfs(string filename, unsigned int fileinode, unsigned int dirinode);//文件复制


void ChangeDir(string newpath);//改变目录

bool Exist(unsigned int fathinode, string sonname);//判断文件重名


void Help();//提示命令信息

void ShowInfo();//显示系统信息
void ShowDir(unsigned int nowdir);//显示当前目录信息
void ShowDir(unsigned int nowdir, bool sonfile);//显示当前目录信息及子目录
void ShowList();//显示所以目录及文件的信息
void ShowPath();//显示当前目录路径

void Info();
void Cd();
void Dir();
void Md();
void Rd();
void Newfile();
void Cat();
void Copy();
void Del();
void Check();
void Ls();

#endif
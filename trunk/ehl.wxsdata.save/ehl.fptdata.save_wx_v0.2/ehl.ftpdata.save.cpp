// ehl.fptdata.save.cpp : 定义 DLL 应用程序的导出函数。
//

#include <iostream>
#include "ehl.ftpdata.save.h"
//#include "webref\ehl_webref.h"
#include "webref\wxtracert\webref_wxtracert.h"
#include "lib\write_log.h"

#include <string>
#include <map>
#include <windows.h>
#include <list>
#include <time.h>

#include <stdlib.h>
#include <stdio.h>
#include <ostream>
#pragma comment(lib,"ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib\chinese_code\chinese_code.h"
const char * deffilename = "./config/device_info_wxs.ini";
using namespace std;

class mux
{
public:
	CRITICAL_SECTION critcal;
public:
	mux()
	{
		InitializeCriticalSection(&critcal);
	}
	~mux()
	{
		DeleteCriticalSection(&critcal);
	}
public:
	void lock()
	{
		EnterCriticalSection(&critcal);
	}
	void unlock()
	{
		LeaveCriticalSection(&critcal);
	}
} gmux;


const char * strtmpdef[5][2] = { { "黄", "1" }, { "蓝", "2" }, { "白", "0" }
, { "黑", "3" }, { "其它", "4" } };
char * strnull = "";
class c_data_convert_hkfilename
{
	
public:
	SOCKET sockClient = NULL;
	int IsConnect = 0;
	char SvrIP[16];
	int  SvrPort;;
	c_data_convert_hkfilename()
	{
		load_param();
		memset(SvrIP, 0, 16);
		sprintf(SvrIP, "%s", read_csinfo("wxjkinfo", "hostip", "61.16.7.2"));
		SvrPort = atoi(read_csinfo("wxjkinfo", "port", "5511"));
		printf("\n-----start connect server-------\n");
		ConnectServer();
//		init_kkinfo();
	}
	~c_data_convert_hkfilename()
	{
		free_param();
	}

	int SendN(SOCKET *s, char *buf, int buflen)
	{
		int sendlen = buflen;
		int n = 0;

		while (sendlen > 0)
		{
			n = send(*s, buf, sendlen, 0);

			if (n < 0)
			{
				printf("[sendn] send error!\n");
				return -1;
			}

			if (n == 0)
			{
				return (buflen - sendlen);
			}

			buf += n;
			sendlen -= n;
		}

		return buflen;
	}
	int RecvN(SOCKET *s, char *buf, int buflen)
	{
		int recvlen = buflen;
		int n = 0;

		while (recvlen > 0)
		{
			n = recv(*s, buf, recvlen, 0);

			if (n < 0)
			{
				printf("[recvn] recv error!\n");
				return -1;
			}

			if (n == 0)
			{
				return (buflen - recvlen);
			}

			buf += n;
			recvlen -= n;
		}

		return buflen;
	}
private:
	//	SBBH 为索引参数表 key sbbh_prop_name  value prop_value
	// 速度参数据信息表 key cllx_csb   value wfxw
	map<string, string> msbinfo;
	void ConnectServer()
	{
		int err;
		WORD versionRequired;
		WSADATA wsaData;
		versionRequired = MAKEWORD(1, 1);
		err = WSAStartup(versionRequired, &wsaData);//协议库的版本信息  

		if (!err)
		{
			printf("客户端嵌套字已经打开!/n");
		}
		else
		{
			printf("客户端的嵌套字打开失败!/n");
			return;//结束  
		}
		SOCKADDR_IN addrServer;

		while (1)
		{
			memset(&addrServer, 0, sizeof(addrServer));
			sockClient = socket(AF_INET, SOCK_STREAM, 0);

			addrServer.sin_addr.S_un.S_addr = inet_addr(SvrIP);
			addrServer.sin_family = AF_INET;
			addrServer.sin_port = htons(SvrPort);
			int ret = connect(sockClient, (SOCKADDR *)&addrServer, sizeof(addrServer));
			if ( ret == 0)
			{
				printf("Connect OK!\n");
				IsConnect = 1;
				break;
			}
			else
			{
				//printf("Connect error! Server is not online!Server IP = %s，Server port = %d\n", SvrIP, SvrPort);
				closesocket(sockClient);
			}
			Sleep(2000);
		}
	}
private:

	void write_param(const char * sbbh, const char * prop_key, const char * default_value)
	{
		char param_file[256];
		get_parent_dir(get_current_exe(), param_file);
		strcat(param_file, deffilename);
		WritePrivateProfileString(sbbh, prop_key, default_value, param_file);
	};
	void load_param_value(const char * secname, const char * key, const char * filename, map<string, string> * mt)
	{
		char value[256];
		char keyname[256];
		sprintf(keyname, "%s_%s", secname, key);
		GetPrivateProfileString(secname, key, "", value, 256, (LPCTSTR)filename);
		mt->insert(pair<string, string>(keyname, value));

	}

	void load_param_section(const char * secname, const char * filename, map<string, string> * mt)
	{
		char ac_Result1[2000];
		int m_lRetValue1 = GetPrivateProfileSection(secname, ac_Result1, 2000, (LPCTSTR)filename);
		char * p = ac_Result1;
		for (int j = 0; j < m_lRetValue1; j++)
		{
			if (*p == '\0')
				break;
			char strtmp[256];
			sprintf(strtmp, p);
			char * strtmp1 = strtok(strtmp, "=");
			char * strtmp2 = strtok(NULL, "=");
			if (strtmp2 == NULL)
				strtmp2 = strnull;
			printf("%s,%s", strtmp1, strtmp);
			char keyname[256];
			sprintf(keyname, "%s_%s", secname, strtmp1);

			mt->insert(pair<string, string>(keyname, strtmp2));
			printf("init map :%s = %s \n", keyname, strtmp2);

			//load_param_value(secname,p, filename, mt);
			p = p + strlen(p) + 1;
		}
	}
	void load_param_sections(const char * filename, map<string, string> * mt)
	{
		char ac_Result[2000];

		int m_lRetValue;
		m_lRetValue = GetPrivateProfileSectionNames(ac_Result, 2000, filename);
		char * p = ac_Result;

		for (int i = 0; i < m_lRetValue; i++)
		{
			load_param_section(p, filename, mt);

			p = p + strlen(p) + 1;
		};
	}

	char szFilePath[MAX_PATH + 1];
	const char * get_current_exe()
	{
		GetModuleFileName(NULL, szFilePath, MAX_PATH);
		return szFilePath;
	};

	void load_param()
	{
		char param_file[256];
		get_parent_dir(get_current_exe(), param_file);
		strcat(param_file, deffilename);
		load_param_sections(param_file, &msbinfo);

	};
	void free_param()
	{
		msbinfo.empty();
	};
private:

	const char * read_csinfo(const char * appname, const char * prop_key, const char * default_value)
	{
		string strkey = appname;
		strkey += "_";
		strkey += prop_key;
		map<string, string>::const_iterator iter = msbinfo.find(strkey);
		if (iter != msbinfo.end())
		{
			return iter->second.c_str();
		}
		else
		{
			msbinfo.insert(pair<string, string>(strkey, default_value));
			write_param(appname, prop_key, default_value);
			return default_value;
		};
	};
private:
	//基本信息
	string sbid;
	string sj;
	string cs;
	string hphm;
	string hpys;
	string hplx;
	string cllx;
	string fxid;
	string cdid;
	string datatype; //数据类型
	string tpmc;
	string wfdm;
	string fxdm;
public:
	//testdata:20150427111212999_0_70_陕A12345_黄_01_01_4_3_6109001_hik.jpg
	char sdatainfo[256];
	int read_baseinfo(char * datainfo)
	{

		char ext[10];
		_splitpath(datainfo, 0, 0, sdatainfo, ext);
		int iresult = 0;

		sj = strtok(sdatainfo, "_");
		datatype = strtok(NULL, "_");
		cs = strtok(NULL, "_");
		hphm = strtok(NULL, "_");
		hpys = strtok(NULL, "_");
		hplx = strtok(NULL, "_");
		cllx = strtok(NULL, "_");
		fxid = strtok(NULL, "_");
		cdid = strtok(NULL, "_");
		sbid = strtok(NULL, "_");
		wfdm = strtok(NULL, "_");
		tpmc = datainfo;
		return iresult;
	}
public:
	int get_datatype()
	{
		return atoi(datatype.c_str());
	}

private:
	char kkid[256];
	const char * get_kkid()
	{

		//return "371600000001";
		sprintf(kkid,read_csinfo(sbid.c_str(), "kkid", sbid.c_str()));
		return kkid;

	}
	//	<!--号牌号码[不能为空]-->
	//	<hphm>京A12345</hphm>
	const char *get_hphm()
	{
		return hphm.c_str();
	};

	const char *get_hpys(char * hpys)
	{
		//<!--号牌颜色[不能为空，输入：黄，蓝，白，黑，其它]-->
		//<hpys>黄< / hpys>
		//return hpys.c_str();
		//return "1";
		for (int i = 0; i < 5; i++)
		{
			if (strcmp(strtmpdef[i][0], hpys) == 0)
			{
				return strtmpdef[i][1];
			}
		}
		return "4";
	};

	char gethpzl[5];
	const char * get_hpzl(char * hpzl)
	{
		if (strcmp(hpzl, "黄") == 0)
		{
			sprintf(gethpzl, "01");
		}else
		if (strcmp(hpzl, "蓝") == 0)
		{
			sprintf(gethpzl, "02");
		}else
		if (strcmp(hpzl, "白") == 0)
		{
			sprintf(gethpzl, "23");
		}
		else
		{
			sprintf(gethpzl, "99");
		}
		return gethpzl;
	}
	//	<!--车辆类型[不可空，符合GA24.4的要求]输入编码：01 - 大型车辆，02 - 小型车辆，03 - 外籍车辆，23 - 公安警用车辆，99 - 其它-->
	//	<cllx>01< / cllx>

	//	1 其他车辆 0 ----
	//		2 小型车 1 ----
	//		3 大型车 2 ----
	//		4 行人 3 ----
	//		5 二轮车 4 ----
	//		6 三轮车 5 ----

	const char * get_default_cllx(const char * hpys)
	{
		for (int i = 0; i < 5; i++)
		{
			if (strcmp(strtmpdef[i][0], hpys))
			{
				return strtmpdef[i][1];
			}
		}
		return "4";
	}
	char ffid_cdid[256];
	map<string, int> unregkkcdlist;
	const char * get_ffid_cdid()
	{
		sprintf(ffid_cdid, "%s_%s", get_fxbh(), get_cdbh());
		return ffid_cdid;
	}
	
	

	char wfsj[256];
	const char *get_gwsj()
	{
		sprintf(wfsj,"%c%c%c%c-%c%c-%c%c %c%c:%c%c:%c%c"
			, sj.at(0), sj.at(1), sj.at(2), sj.at(3), sj.at(4), sj.at(5), sj.at(6)
			, sj.at(7), sj.at(8), sj.at(9), sj.at(10), sj.at(11), sj.at(12), sj.at(13));
		return wfsj;
	};

	//	<!--方向编号[不能为空，输入（1 - 4）。定义如下：南向北1：东向西2：北向南3:西向东4-->
	//	<fxbh>1</fxbh>
	char fx[20];
	char fxhm[3];
	const char * get_fxbh()
	{
		sprintf(fx, "%s_%s", "99", "1");
		strncpy(fxhm, read_csinfo(sbid.c_str(), fxid.c_str(), fx), 2);
		return fxhm;
		//return read_csinfo("fxbh", fxid.c_str(), fxid.c_str());
		//return fxid.c_str();
	};
	const char * get_do()
	{
		char * p = (char *)strstr(read_csinfo(sbid.c_str(), fxid.c_str(), "99_1"), "_");
		printf("%s\n", read_csinfo(sbid.c_str(), fxid.c_str(), "991"));
		printf("sbid = %s\n", sbid.c_str());
		printf("fxid = %s\n", fxid.c_str());
		printf("\n  p = %p", p);
		return p + 1;
	}
	//	<!--车道编号[不能为空，输入：1 - 99]-->
	//	<cdbh>2< / cdbh>

	const char * get_cdbh()
	{
		//return cdid.c_str();
		return read_csinfo("cdbh", cdid.c_str(), cdid.c_str());
	};
	//	<!--车辆速度[不能为空，输入：1 - 300]-->
	//	<clsd>76< / clsd>
	const char * get_clsd()
	{
		return cs.c_str();
	};
	//	<!--证据文件1[不能为空，全景图片，输入有效的http路径-->
	//	<zjwj1>http://10.2.61.209/capture/2009/09/10120420070204150006_1.jpg</zjwj1>
	string strimg1;
	const char * get_zjwj1()
	{
		strimg1 = read_csinfo("imgurl", "server", "http://10.176.102.4/image/");
		strimg1.append(tpmc.c_str());
		return strimg1.c_str();
	};
	//<!--证据文件2[能空，特写图片，输入有效的http路径-->
	//	<zjwj2>http ://10.2.61.209/capture/2009/09/10120420070204150006_2.jpg</zjwj2>
	const char * get_zjwj2()
	{
		return "";
	};
	//	<!--证据文件3[可空，号牌图片，输入有效的http路径]-->
	//	<zjwj3>http ://10.2.61.209/capture/2009/09/10120420070204150006_3.jpg</zjwj3>
	const char * get_zjwj3()
	{
		return "";
	};
	//	<!--证据文件4[[可空，录像文件，输入有效的http路径-->
	//	<zjwj4>http ://10.2.61.209/capture/2009/09/10120420070204150006_4.MP4</zjwj4>
	const char * get_zjwj4()
	{
		return "";
	};
	//	<!--记录类型 : [不能为空, 0：正常车辆1：黑名单车辆2：特殊勤务车辆3：布控车辆4：占道车辆5：历史违法车辆6：畅行车辆7：区间超速车辆8：超速违法车辆9：逆行 10：转入违法  11.闯红灯违法 12.不按车道行驶 13.违法停车  14.压黄线] -->
	//	<jllx>0< / jllx>
	const char * get_jllx()
	{
		return datatype.c_str();
	};
	//	<!--车速毫秒[可空, 车速的毫秒部分，3位]-->
	//	<cshm>869< / cshm>
	char cshm[256];
	const char * get_cshm()
	{
		sprintf(cshm, "%c%c%c"
			, sj.at(14), sj.at(15), sj.at(16));
		return cshm;
	};

	//<!--数据来源[不能为空，数据来源“1 - 闯红灯设备，2 - 公路卡口设备，3 - 测速设备，4 - 闭路电视（违法抓拍），5 - 移动摄像，6 - 行车记录仪，9 - 其它电子设备]-->
	//	<sjly>2< / sjly>
	const char * get_sjly()
	{
		return read_csinfo(sbid.c_str(), "sjlx", "2");
	};

	//	<!--号牌种类[不能为空，符合GA标准] 输入编码：01 - 大型车辆，02 - 小型车辆，03 - 外籍车辆，23 - 公安警用车辆，99 - 其它-->
	//	<hpzl>01< / hpzl>

	const char * get_hpzl()
	{
		return hplx.c_str();
	};

	//	<!--违法时间[不能为空，时间格式：yyyy - MM - dd hh24 : mm : ss]-->
	//	<wfsj>2009 - 09 - 23 04:11 : 55< / wfsj>
	const char * get_wfsj()
	{
		return get_gwsj();
	};
	//	<!--违法行为[不能为空，符合GA标准]-->
	//	<wfxw>1030</wfxw>
	string strwfxw;
	const char * get_wfxw()
	{
		
		
		/*
		int ispeed = atoi(cs.c_str());
		int ilimitspeed = atoi(get_clxs());
		int csb = (ispeed * 100) / ilimitspeed;
		if (csb < 20)
		{
			strtmp = read_csinfo(get_cllx(), "csb20", "1020");
		}
		else
		{
			if (csb > 50)
			{
				strtmp = read_csinfo(get_cllx(), "csb50", "1020");
			}
			else
			{
				strtmp = read_csinfo(get_cllx(), "csb205050", "1020");
			}
		};
		return strtmp.c_str();
		*/
		strwfxw = read_csinfo("wfdm", wfdm.c_str(), wfdm.c_str());
		return strwfxw.c_str();
	};

	//	<!--违法内容[可空，对违反行为简单的描述]-->
	//	<wfnr>超速50%< / wfnr>
	string strwfnr;
	const char * get_wfnr()
	{
		strwfnr = read_csinfo("wfnr", wfdm.c_str(), "");
		return strwfnr.c_str();
	};
	//<!--违法地点[不可空]-->
	//	<wfdd>208300315500</wfdd>
	string strwfdd;
	const char * get_wfdd()
	{
		 strwfdd = read_csinfo(sbid.c_str(), "wfdd", sbid.c_str());
		 return strwfdd.c_str();
	};

	//<!--违法地址[不可空，违法地点的描述]-->
	//<wfdz>北京路—天津路< / wfdz>
	const char * get_wfdz()
	{
		return read_csinfo(sbid.c_str(), "wfdz", "十天高速");
	};



	//<!--车辆限速[可空，输入：1 - 300]-->
	//<clxs>70< / clxs>
	const char * get_clxs()
	{
		string strtmp = "speedlimit_";
		strtmp += cllx.c_str();
		return  read_csinfo(sbid.c_str(), strtmp.c_str(), "80");
	};
	//<!--最低限速[可空，输入：1 - 300]-->
	//<zdxs>60< / zdxs>
	const char * get_zdxs()
	{
		return "1";
	};
	//<!--采集机关[不能为空，-->
	//<cjjg>371600000001< / cjjg>
	const char * get_cjjg()
	{
		return  read_csinfo(sbid.c_str(), "cjjg", "371600000001");
	};
	//<!--采集用户[可空，-->
	//<cjyh>抓拍员< / cjyh>
	const char * get_cjyh()
	{
		return read_csinfo(sbid.c_str(), "cjyh", "9999");
	};
	//<!--采集时间[可空，-->
	//<cjsj>2009 - 09 - 23 04:11 : 55< / cjsj>
	const char * get_cjsj()
	{
		return get_wfsj();
	};
	//<!--执勤名警[不可空] -->
	//<zqmj>012321< / zqmj>
	const char * get_zqmj()
	{
		return read_csinfo(sbid.c_str(), "cjyh", "9999");
	};
	//<!--备注信息[可空]-->
	//<bz>< / bz>
	const char * get_bz()
	{
		return read_csinfo(sbid.c_str(), "bz", "");
	};

public:
	//void plug_proc_viodata(int itype, const wchar_t * datastr, wchar_t * proc_msg)
	



	int inittype;	//初始化未完成


	int init_wxdata(string kkbh, string fxlx, long cdh)
	{

		char refxml[254];
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		char tmp[30] = { 0 };
		sprintf(tmp, "%4d-%02d-%02d %02d:%02d:%02d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);
		string jkid = read_csinfo("wxjkinfo", "jkid", "62C01");
		string jkxlh = read_csinfo("wxjkinfo", "jkxlh", "791F09090306170309158E9882DB8B8E8FEEF681FF8B9AEB92CFD0A1A1C5A5D3CCA8");
		string info = "<info><jkid>";
		string hostip = read_csinfo("wxjkinfo", "hostip", "10.176.100.185");
		info += jkid;
		info += "</jkid><jkxlh>";
		info += jkxlh;
		info += "</jkxlh><time>";
		info += tmp;
		info += "</time></info>";
	
		printf("卡口注册信息：%s,%s,%d\n", kkbh.c_str(), fxlx.c_str(), cdh);
		printf("info:%s\n", info.c_str());
		//		return -1;
		
		return call_wx_upload_data_inittrans(kkbh, fxlx, cdh, info, hostip);
	}
	int valdate = 0;



	int put_wxdata()
	{

		//卡口数据直接跳过
		if (!datatype.compare("1"))
			return 1;
		/*
		if (isreg() == false)
		{
			WriteLog("卡口注册失败！");
			return -1;
		};
		*/

		char refxml[512] = {0};
//		string ikkbh(get_kkid());	//卡口编号不变，直接用
//		string ifxlx = "1";			//不能为空（1-上行/进城/环线顺时针、2-下行/出城/环线逆时针）
		//string ifxlx = read_csinfo(ikkbh.c_str(), "fxlx", "1");
		string ifxlx = get_fxbh();
		LONG64 icdh = atol(get_cdbh());	//车道号，1位的东西干嘛定义了long64
		string ihphm(get_hphm());	//号牌号码，直接用
		
	
		string dcodeOther(get_do());
		string igcsj(get_gwsj());	//过车时间，格式一致，直接用
		LONG64 iclsd = atol(get_clsd());//车辆速度，最长三位
		LONG64 iclxs = atol(get_clxs());		//车辆限速
		string iwfdm(get_wfxw());		//违法代码
		LONG64 icwkc = 0;				//车外廓长
		string ihpys = get_hpys((char *)hpys.c_str());				//0-白色，1-黄色，2-蓝色，3-黑色，4-绿色，9-其它颜色
		string fplateType(get_hpzl((char *)hpys.c_str()));
		string icllx(read_csinfo("cllx", hpys.c_str(), "X99"));		//参考GA24.4（K11-大型普通客车，K21-中型普通客车，K31-小型普通客车，K33-轿车，H11-重型普通客车，H21-中型普通客车，M22-轻便二轮摩托车）
////////////////////////////////////////////////////////////////////////
		string devCode(get_kkid());
		string dcode(get_fxbh());

		char url[256] = {0};
		char pic[128] = {0};
		char ext[128] = {0};
		_splitpath(get_zjwj1(), NULL, url,pic , ext);
		string itplj(url);				//文件路径
		string itp1(pic);				//图片文件名1
		itp1 += ext;
		memset(pic, 0, 128);
		
		_splitpath(get_zjwj2(), NULL, NULL, pic, ext);
		string itp2(pic);				//图片文件名2
		memset(pic, 0, 128);
		_splitpath(get_zjwj3(), NULL, NULL, pic, ext);
		string itp3(pic);				////图片文件名3
		string itztp = "";				//特征图片	扩展预留，为空
		
		/*
		int call_wx_upload_data_inpasscar(std::string kkbh, std::string fxlx, long long cdh, char * refxml, std::string hphm, std::string hpzl
			, std::string gcsj, long long clsd, long long clxs, std::string wfdm, long long cwkc, std::string hpys, std::string cllx
			, std::string fzhpzl, std::string fzhphm, std::string fzhpys, std::string clpp, std::string clwx, std::string csys
			, std::string tplj, std::string tp1, std::string tp2, std::string tp3, std::string tztp);
			*/

		c_time passtime(get_gwsj());
		c_time nowtime;
		nowtime.get_local_time();
		if (++valdate >= 10000)
		{
			valdate = 1;
		}
		char sp[4] = { 0 };
		sprintf(sp, "%d", iclsd);
		char cdid[3] = { 0 };
		sprintf(cdid, "%d", icdh);
		char vld[5] = { 0 };
		sprintf(vld, "%04d", valdate);
		printf("/n passtime = %s currtime = %s", passtime.get_datetime(), nowtime.get_datetime());
		long secbetween = passtime.sec_between(nowtime, passtime);
		printf("/n secbetween = %ld/n", secbetween);
		long overtime = atoi(read_csinfo("upload_wxs", "overtime(sec)", "300"));
		string datainfo;
		string datainfoutf = "";
		datainfo = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
		datainfo += "<rootInfo>";
			datainfo += "<headInfo>";
				datainfo += "<devCode>";
				datainfo += devCode;
					//datainfo += "610900000013021116";
				datainfo += "</devCode>";
				datainfo += "<license>";
					datainfo += "gaojiao";
				datainfo += "</license>";
				datainfo += "<comName>";
					datainfo += "haikang";
				datainfo += "</comName>";
				datainfo += "<comWord>";
					datainfo += "haikang";
				datainfo += "</comWord>";
				datainfo += "<sendTime>";
					datainfo += nowtime.get_datetime(true, false);
				datainfo += "</sendTime>";
				datainfo += "<ver>";
				datainfo += "1.2</ver>";
			datainfo += "</headInfo>";
			datainfo += "<listInfo>";
			datainfo += "<roadcode>";
			datainfo += devCode;
			//datainfo += "610900000013021116";
			datainfo += "</roadcode>";
			datainfo += "<dcode>";
			datainfo += dcode;
			datainfo += "</dcode>";
			datainfo += "<dcodeOther>";
			datainfo += dcodeOther;
			datainfo += "</dcodeOther>";
			datainfo += "<lcode>";
			datainfo += cdid;
			datainfo += "</lcode>";
			datainfo += "<plateTime>";
			datainfo += igcsj;
			datainfo += "</plateTime>";
			datainfo += "<plateFlag>";
			datainfo += "1";
			datainfo += "</plateFlag>";
			datainfo += "<fplateNum>";
			datainfo += ihphm;
			datainfo += "</fplateNum>";
			datainfo += "<fplateColor>";
			datainfo += ihpys;
			datainfo += "</fplateColor>";
			datainfo += "<fplateType>";
			datainfo += fplateType;
			datainfo += "</fplateType>";
			datainfo += "<carType>";
			datainfo += icllx;
			datainfo += "</carType>";
			datainfo += "<img1_path>";
			datainfo += itplj;
			datainfo += itp1;
			datainfo += "</img1_path>";
			if (itp2.length() > 0)
			{
				datainfo += "<img2_path>";
				datainfo += itplj;
				datainfo += itp2;
				datainfo += "</img2_path>";
			}
			
			datainfo += "<img1_s_path>";
			datainfo += "</img1_s_path>";
			/*
			datainfo += "<img2_s_path>";
			datainfo += itplj;
			datainfo += itztp;
			datainfo += "</img2_s_path>";
			*/
			datainfo += "<img_flag>";
			datainfo += "2</img_flag>";
			datainfo += "<ftp_flag>2</ftp_flag>";
			datainfo += "<carSpeed>";
			datainfo += sp;
			datainfo += "</carSpeed>";
			datainfo += "<valdate>";
			datainfo += vld;
			datainfo += "</valdate>";
			datainfo += "<isUpFile>";
			datainfo += "0</isUpFile>";
			datainfo += "</listInfo>";
		datainfo += "</rootInfo>\n";
		CChineseCode::GB2312ToUTF_8(datainfoutf, (char *)datainfo.c_str(), datainfo.length());
		
		int iresult = 1;
		if (ihphm.length() == 6)
		{
			return iresult;
		}
		if (secbetween < overtime)
		{
			/*
			iresult = call_wx_upload_data_inpasscar(ikkbh, ifxlx, icdh,  ihphm, ihpzl
			, igcsj, iclsd, iclxs, iwfdm, icwkc, ihpys, icllx
			, ifzhpzl, ifzhphm, ifzhpys, iclpp, iclwx, icsys
			, itplj, itp1, itp2, itp3, itztp);
			*/
			if (IsConnect == 1)
			{
				int t = SendN(&sockClient, (char *)datainfoutf.c_str(), datainfoutf.size());
				if (t < 0)
				{
					ConnectServer();
				}
				else
				{
					printf("\nsocket send ok!!\n");
					WriteLog((char *)datainfo.c_str());
				}
			}
		}
		else
		{
			char errmsg[256];
			sprintf(errmsg,"--errmsg:overtime data = %s", tpmc.c_str());
			WriteLog(errmsg);
		}
		/*
		if (iresult < 1)
		{
			isreg();
		}
		*/
		return iresult;
	};
	
} ;



c_data_convert_hkfilename g_datafile_convert_hkfilename;
#include ".\lib\write_log.h"
class waitproc_info
{

private:
	list<string> m_waitproc_info;
public:
	int waitcount()
	{
		return m_waitproc_info.size();
	}
	void add_proc(const char * datainfo)
	{
		gmux.lock();
		m_waitproc_info.push_back(datainfo);
		gmux.unlock();
	};
private:
	/*
	< ? xml version = "1.0" encoding = "GB2312" ? >
	<message System = "ATMS" Ver = "1.0">
	<systemtype>TGS< / systemtype>
	<messagetype>FEEDBACK< / messagetype>
	<sourceIP>3.0< / sourceIP>
	<targetIP>< / targetIP>
	<user>< / user>
	<password>< / password>
	<executeresult>
		<!- 如果成功，返回执行成功后记录编号]-->
		<recordid>< / recordid>
		<!- 错误描述]-->
		<errordescription>不是正确的xml格式！< / errordescription>
		<!- 成功所影响的条数]-->
		<resultnum>0< / resultnum>
		<!- 成功后的描述]-->
		<resultdescription>< / resultdescription>
		<!- 错误代码]-->
		<errorcode>0< / errorcode>
	< / executeresult>
	< / message>
	*/
	int check_result_xml(const char * xml)
	{
		string str = "";
		
		char * p = (char *)strstr((const char *)xml, ((const char *)"<errorcode>"));
		p += strlen("<errorcode>");
		while (*p != '<' && *p != '\0')
		{
			if (*p >= '0' && *p <= '9')
				str += *p;
			p++;
		};
		return atoi(str.c_str());
	};
public:
	void proc_info(const char * datastr,int * iresult_code,char * reset_msg)
	{
		g_datafile_convert_hkfilename.read_baseinfo((char *)datastr);
		
		char stmp[256];
		//strxml.append(g_datafile_convert_hkfilename.sdatainfo);
		int iresult = 0;
		iresult = g_datafile_convert_hkfilename.put_wxdata();
		/*
		if (g_datafile_convert_hkfilename.get_datatype() == 0)
		{
			//tracert car
			 iresult = g_datafile_convert_hkfilename.get_passcar_xml(strxml);
			 printf(strxml.c_str());
			 if (iresult == 0)
				 iresult = upload_data_inpasscar(strxml.c_str(), reset_msg);
		}
		else
		{
			//vio car
			 iresult = g_datafile_convert_hkfilename.get_viodata_xml(strxml);
			 printf(strxml.c_str());
			 if (iresult == 0)
				 iresult = upload_data_inviocar(strxml.c_str(), reset_msg);
		};
		*/
		//WriteLog((char *)strxml.c_str());
		
		/*
		if (iresult == 0)
		{
			
			if (reset_msg != "\0")
			{
				
				WriteLog((char *)reset_msg);
				iresult = check_result_xml(reset_msg);
			}
			else
			{
				iresult = 100;
			}
			
		}
	   */

		if (iresult<0)
		{
	
			*iresult_code = iresult;
			sprintf(stmp, "error[%d]:web service upload data fail!", iresult);
			WriteLog(stmp);
		};
	
	}
	bool gproc_flag;
	char buf[512];
	void proc_recinfo()
	{
		while (1)
		{
			if (g_datafile_convert_hkfilename.IsConnect == 0)
			{
				Sleep(2000);
				continue;
			}
			memset(buf, 0, 512);
			int ret = g_datafile_convert_hkfilename.RecvN(&g_datafile_convert_hkfilename.sockClient, buf, 512);
			printf("%s\n", buf);
			WriteLog(buf);
		}
	}
	void proc_info()
	{
		while (gproc_flag)
		{
			gmux.lock();
			list<string>::const_iterator iter = m_waitproc_info.begin();
			gmux.unlock();
			if (iter != m_waitproc_info.end())
			{
				proc_info((char *)iter->c_str(),0,0);
				gmux.lock();
				m_waitproc_info.remove(*iter);
				gmux.unlock();
				Sleep(1);
			}
			else
			{
				Sleep(100);
			}
		}
	}
} gwaitproc_info;



static void __cdecl thread_proc_datainfo(void * param)
{
	gwaitproc_info.proc_info();
};

static void _cdecl thread_proc_recdata(void * param)
{
	gwaitproc_info.proc_recinfo();
}
/*
static void __cdecl thread_regkkinfo(void * param)
{
	while(1)
	{
		Sleep(500);
		if(g_datafile_convert_hkfilename.inittype == 1)
			g_datafile_convert_hkfilename.reinit_kkinfo();
	}
	
};
*/

#include <process.h>
class c_work
{
private:
	uintptr_t thread;
	uintptr_t threadrec;
public:
	c_work()
	{
		WriteLog("thread work begin!");
		gwaitproc_info.gproc_flag = true;
		thread = _beginthread(thread_proc_datainfo, NULL, NULL);
		WriteLog("\n-----------beginthread(thread_proc_datainfo--------------\n");
		Sleep(5000);
		threadrec = _beginthread(thread_proc_recdata, NULL, NULL);
	};
	~c_work()
	{
		gwaitproc_info.gproc_flag = false; 
		//SignalObjectAndWait()
	}
} g_work;

void __stdcall plug_proc_data(int itype, const char * datastr, int * irescode, char * proc_msg)
{
	if (itype == 0)
	{
		WriteLog("\n-----------gwaitproc_info.proc_info--------------\n");
		gwaitproc_info.proc_info(datastr, irescode, proc_msg);
		
	}
	else
	{
		gwaitproc_info.add_proc(datastr);
	}
		//sprintf(proc_msg, "process [*s]", strxml);
		sprintf(proc_msg, "process [%s],wait_proc[%d]", datastr,gwaitproc_info.waitcount());
 };

void  __stdcall init_work_mode(int workmode /*= 0*/, const char * webserviceurl /*= "http://localhost/Ehl.Atms.Tgs.WebService/OpenInterface.asmx"*/)
{
//	sprintf(soap_endpoint, webserviceurl);
}


int __cdecl main(int argc, char * argv[])
{

	printf(argv[1]);
	char reftest[1024];
	//void  plug_proc_data(int itype, const char * datastr, char * proc_msg)
	plug_proc_data(0, (const char *)argv[1],NULL, reftest);
	printf("\n");
	printf(reftest);
	system("PAUSE");

	return 0;
}


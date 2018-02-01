#include "public.h"
#include "file_ctrl.h"
#include <vector>
using namespace std;

int findstr(const char *str,const char * substr)
{
	int pos = 0;
	char * pstr = (char *)str;
	int lenstr = strlen(str);
	int lensub = strlen(substr);
	bool bresult = false;

	for(int i=0;i<lenstr;i++)
	{
		pstr = (char *)str+i;
		if(*pstr == *substr)
		{
			bresult = true;
			for(int i=0;i<lensub;i++)
			{

				if(*(pstr+i) != *(substr+i))
				{
					bresult = false;
					break;
				}
			}

			if(bresult)
			{
				pos = i+1;
				break;
			}

		}

	}

		return pos;
}

int find_file(vector<string> * filelist,string mspath,string sub_path="")
{
	/***************************�����������**************************
	
	filelist->push_back("20160607130000000_0_76_��A12345_��_0_2_�ɱ�����_02_6109010015_0_hik.jpg");//��ï����K1166+900
	filelist->push_back("20160607130325000_0_76_��A12345_��_0_2_�ɱ�����_02_6109010016_0_hik.jpg");//��ï����K1174+600

	filelist->push_back("20160607130100000_0_76_��A12345_��_0_2_�ɱ�����_02_6109010013_0_hik.jpg");//��ï����K1094+600
	filelist->push_back("20160607130334000_0_76_��A12345_��_0_2_�ɱ�����_02_6109010014_0_hik.jpg");//��ï���� K1100+900_������

	filelist->push_back("20160607130200000_0_76_��A12345_��_0_2_������_02_6109010017_0_hik.jpg");//��ï����K1186+200
	filelist->push_back("20160607130342000_0_76_��A12345_��_0_2_������_02_6109010018_0_hik.jpg");//��ï����K1182+200

	filelist->push_back("20160607130300000_0_76_��A12345_��_0_2_������_02_6109010019_0_hik.jpg");//ʮ�����K185+100
	filelist->push_back("20160607130900000_0_76_��A12345_��_0_2_������_02_6109010020_0_hik.jpg");//ʮ�����K171+500

	filelist->push_back("20160607130400000_0_76_��A12345_��_0_2_������_02_6109010021_0_hik.jpg");//ʮ�����K94+800
	filelist->push_back("20160607131000000_0_76_��A12345_��_0_2_������_02_6109010022_0_hik.jpg");//ʮ�����K80+300

	filelist->push_back("20160607130500000_0_76_��A12345_��_0_2_�ɶ�����_02_6109010023_0_hik.jpg");//ʮ�����K79+300
	filelist->push_back("20160607130917000_0_76_��A12345_��_0_2_�ɶ�����_02_6109010024_0_hik.jpg");//ʮ�����K89+100

	filelist->push_back("20160607130600000_0_76_��A12345_��_0_2_������_02_6109010001_0_hik.jpg");//����������������������
	filelist->push_back("20160607131114000_0_76_��A12345_��_0_2_������_02_6109010002_0_hik.jpg");//����������������������

	filelist->push_back("20160607130700000_0_76_��A12345_��_0_2_�ɱ�����_02_6109010012_0_hik.jpg");//�������ٰ�������˱�ƺ
	filelist->push_back("20160607131123000_0_76_��A12345_��_0_2_�ɱ�����_02_6109010011_0_hik.jpg");//�������ٰ�����������ƺ���

	filelist->push_back("20160607130800000_0_76_��A12345_��_0_2_�ɱ�����_02_6109010006_0_hik.jpg");//��ï���ٰ����������ɽ
	filelist->push_back("20160607132528000_0_76_��A12345_��_0_2_�ɱ�����_02_6109010007_0_hik.jpg");//��ï���ٰ�����������

	filelist->push_back("20160607130900000_0_76_��A12345_��_0_2_������_02_6109010040_0_hik.jpg");//��ƽ����K644
	filelist->push_back("20160607131032000_0_76_��A12345_��_0_2_������_02_6109010039_0_hik.jpg");//��ƽ����K648

	filelist->push_back("20160607131100000_0_76_��A12345_��_0_2_������_02_6109010036_0_hik.jpg");//��ƽ����K655
	filelist->push_back("20160607131232000_0_76_��A12345_��_0_2_������_02_6109010034_0_hik.jpg");//��ƽ����K651

	filelist->push_back("20160607131200000_0_76_��A12345_��_0_2_������_02_6109010028_0_hik.jpg");//��������K256
	filelist->push_back("20160607131511000_0_76_��A12345_��_0_2_������_02_6109010029_0_hik.jpg");//��������K249

	filelist->push_back("20160607131300000_0_76_��A12345_��_0_2_�ɶ�����_02_6109010026_0_hik.jpg");//��������K223
	filelist->push_back("20160607131820000_0_76_��A12345_��_0_2_�ɶ�����_02_6109010027_0_hik.jpg");//��������K235

	filelist->push_back("20160607131400000_0_76_��A12345_��_0_2_������_02_6109010008_0_hik.jpg");//��ï����������������
	filelist->push_back("20160607132819000_0_76_��A12345_��_0_2_������_02_6109010009_0_hik.jpg");//��ï���������������ɽ
	
	filelist->push_back("20160607131400000_0_76_��A12345_��_0_2_�ɱ�����_02_6109010003_0_hik.jpg");//�����������·�����
	filelist->push_back("20160607132604000_0_76_��A12345_��_0_2_�ɱ�����_02_6109010004_0_hik.jpg");//�������ٴ�Ӱ�
	return (int)filelist->size();
	***************************�����������**************************/



	int  findHandle;                                 // �����ļ����
	char fileSpec[256];                              // �����ļ�����
	struct _finddata_t  fileInfo;                    // �ļ���Ϣ
	sprintf(fileSpec, "%s/%s%/*.*", mspath.c_str(), sub_path.c_str()); 

	/*   _sFileName.c_str()��ʾ����ƥ����ļ��������磺*.txt
	*   sPath.c_str() ���ļ���·��
	*/

	findHandle = _findfirst(fileSpec, &fileInfo);   // ��ʼ����
	if (findHandle != -1)                            // �ҵ��ļ�
	{
		do
		{
			if (fileInfo.attrib & _A_SUBDIR)
			{
				//cout << (strcmp(fileInfo.name,"."))<< endl;
				if((strcmp(fileInfo.name,"."))&&strcmp(fileInfo.name,".."))
				{
					string tmpsub_path = sub_path;
					int len = tmpsub_path.length();
					if(len != 0)
					{
						len -= 1;
					}
					if(tmpsub_path[len] != '/')
					{
						tmpsub_path += "/";
					}
					tmpsub_path += fileInfo.name;
					find_file(filelist,mspath,tmpsub_path);
				};
			};
			if (fileInfo.attrib & _A_ARCH)  // ������ĵ�������ļ���            
			{ 

				//int ipos = findstr(fileInfo.name,"*.jpg");
				int ipos = 1;
				if(ipos)
				{
					
					cout<<"�ļ���:"     << fileInfo.name << endl;				
					string strfilename = sub_path;
					strfilename += "/";
					strfilename += fileInfo.name;
					filelist->push_back(strfilename);
					if(filelist->size()>= max_find_count)
					{
						break;
					}
				}
			}
		}while(_findnext(findHandle, &fileInfo) != -1);    // ������һ���ļ�
	};

	_findclose(findHandle);                          // �رղ���
	return (int)filelist->size();	
};

//bool send_datafile
#include "xxlib\plugin\user_plug_prcdata.h"
bool send_datafile(const char * m_path)
{

	//find datafile ÿ��50��
	vector<string> file_list;
	int ifilecount = find_file(&file_list,m_path);
	//for
	for(vector<string>::iterator iter = file_list.begin();
		iter != file_list.end();
		iter ++)
	{
		char strdata[256];
		char strref[1024];

		string strtmp = m_path;
		strtmp += "/";
		strtmp += iter->c_str();

		string ftpfile = g_local_dir.m_backdir;
		ftpfile += "/";
		ftpfile += iter->c_str();

		string ftpfail = g_local_dir.m_faildir;
		ftpfail += "/";
		ftpfail += iter->c_str();


		sprintf(strdata, "%s", iter->c_str());
		int iresult_code = 0;
		int ipos = findstr(strdata, "_hik.jpg");
		if (ipos)
		{
			plug_proc_data(strdata,&iresult_code, strref);
		}
		continue;
		if (iresult_code == 0)
		{
			string strdir = ftpfile.c_str();
			get_parent_dir(strdir);
			check_file_path(strdir.c_str());

			for (int i = 0; i < 100; i++)
			{
				BOOL bmove =
					MoveFile(strtmp.c_str(), ftpfile.c_str());
				if (bmove)
				{
					break;
				}
				else
				{

					DWORD errno1 = GetLastError();
						printf("move file is fail!,error(%d)", errno1);
						if (errno1 == 183)
						{
							DeleteFile(strtmp.c_str());
							break;
						}
						if (errno1 == 123)
							break;
						

					Sleep(1000);
				};

			}

		}	   
		else
		{
			string strdir = ftpfail.c_str();
			get_parent_dir(strdir);
			check_file_path(strdir.c_str());

			check_file_path(strtmp.c_str());

			for (int i = 0; i < 100; i++)
			{
				BOOL bmove =
					MoveFile(strtmp.c_str(), ftpfail.c_str());
				if (bmove)
				{

					break;
				}
				else
				{
					DWORD errno1 = GetLastError();
					printf("move file is fail!,error(%d)", errno1);
					if (errno1 == 183)
					{
						DeleteFile(strtmp.c_str());
						break;
					}
					if (errno1 == 123)
						break;
					Sleep(1000);
				};

			}


		}
		
		/*
		//send datafile	
		string strtmp = m_path;
		strtmp += "/";
		strtmp += iter->c_str();

		//ftp datafile
		string ftpfile = g_ftp_dir.m_datafile;
		ftpfile += "/";
		ftpfile += iter->c_str();
		//

		cout << "send file ..." << strtmp;
		if(g_ftp_info.send_ftp_file(strtmp.c_str(),ftpfile.c_str()))
		{
			cout << "succer" << endl;
			remove(strtmp.c_str());
		}
		else
		{
			cout << "fail" << endl;
		}
		*/


	};
	return true;
};

//ͨ���̵߳��÷���
//datafile
unsigned int __stdcall thread_send_datafile(void * param)
{
	_run_station * p_run_station = (_run_station *)param;
	while (p_run_station->get_work())
		{
			send_datafile(g_local_dir.m_datafile.c_str());
			Sleep(1000);
		}
	return 1;
}


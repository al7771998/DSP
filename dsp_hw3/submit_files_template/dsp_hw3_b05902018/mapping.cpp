#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

int zhu_in_big5(char a, char b)
{
	int zhu_in;
	switch(b)
	{
		case 116: //ㄅ
			zhu_in = 0;
			break;
		case 117://ㄆ
			zhu_in = 1;
			break;
		case 118://ㄇ
			zhu_in = 2;
			break;
		case 119://ㄈ
			zhu_in = 3;
			break;
		case 120://ㄉ
			zhu_in = 4;
			break;
		case 121://ㄊ
			zhu_in = 5;
			break;
		case 122://ㄋ
			zhu_in = 6;
			break;
		case 123://ㄌ
			zhu_in = 7;
			break;
		case 124://ㄍ
			zhu_in = 8;
			break;
		case 125://ㄎ
			zhu_in = 9;
			break;
		case 126://ㄏ
			zhu_in = 10;
			break;
		case -95://ㄐ
			zhu_in = 11;
			break;
		case -94://ㄑ
			zhu_in = 12;
			break;
		case -93://ㄒ
			zhu_in = 13;
			break;
		case -92://ㄓ
			zhu_in = 14;
			break;
		case -91://ㄔ
			zhu_in = 15;
			break;
		case -90://ㄕ
			zhu_in = 16;
			break;
		case -89://ㄖ
			zhu_in = 17;
			break;
		case -88://ㄗ
			zhu_in = 18;
			break;
		case -87://ㄘ
			zhu_in = 19;
			break;
		case -86://ㄙ
			zhu_in = 20;
			break;
		case -85://ㄚ
			zhu_in = 21;
			break;
		case -84://ㄛ
			zhu_in = 22;
			break;
		case -83://ㄜ
			zhu_in = 23;
			break;
		case -82://ㄝ
			zhu_in = 24;
			break;
		case -81://ㄞ
			zhu_in = 25;
			break;
		case -80://ㄟ
			zhu_in = 26;
			break;
		case -79://ㄠ
			zhu_in = 27;
			break;
		case -78://ㄡ
			zhu_in = 28;
			break;
		case -77://ㄢ
			zhu_in = 29;
			break;
		case -76://ㄣ
			zhu_in = 30;
			break;
		case -75://ㄤ
			zhu_in = 31;
			break;
		case -74://ㄥ
			zhu_in = 32;
			break;
		case -73://ㄦ
			zhu_in = 33;
			break;
		case -72://ㄧ
			zhu_in = 34;
			break;
		case -71://ㄨ
			zhu_in = 35;
			break;
		case -70://ㄩ
			zhu_in = 36;
			break;
		default:
			zhu_in = -1;
	}
	return zhu_in;
}
int main(int argc, char *argv[])
{
	//char filename[] = "Big5-ZhuYin.map";
	fstream fp,fp2;
	fp.open(argv[1],ios::in);
	fp2.open(argv[2],ios::out);
	char strin[200];
	char zhu_in[40][10000][3], bpm[40][3];
	short int used[40] = {}, get_zhu;
	//int T = 10;
	int length;
	if(!fp && !fp2)
		cout << "cannot open the file\n";
	else
	{
		//============讀檔==============
		while(!fp.eof())
		//while(T--)
		{
			fp.getline(strin,sizeof(strin));
			length = strlen(strin);
			int i;
			for (i = 0;i < length; i++)
			{
				if(i == 3)
				{
					get_zhu = zhu_in_big5(strin[i],strin[i+1]);
					if(get_zhu >= 0)
					{
						zhu_in[get_zhu][used[get_zhu]][0] = strin[0];
						zhu_in[get_zhu][used[get_zhu]][1] = strin[1];
						if(used[get_zhu] == 0) 
						{
							bpm[get_zhu][0] = strin[i];
							bpm[get_zhu][1] = strin[i+1];
						}
						used[get_zhu]++;	
					}
				}
				else if(strin[i-1] == '/')
				{
					get_zhu = zhu_in_big5(strin[i],strin[i+1]);
					if(get_zhu >= 0)
					{
						zhu_in[get_zhu][used[get_zhu]][0] = strin[0];
						zhu_in[get_zhu][used[get_zhu]][1] = strin[1];
						if(used[get_zhu] == 0)
						{
							bpm[get_zhu][0] = strin[i];
							bpm[get_zhu][1] = strin[i+1];
						}
						used[get_zhu]++;	
					}
				}
			}
			//fp2 << strin << "\n";

		}	
		fp.close();

		//=============寫檔===============
		int i,j;
		for(i = 0; i < 37;i++)
		{
			//cout << "第" << i << "個用了" << used[i] << endl;
			if(used[i] != 0){
				fp2 << bpm[i] << "\t";
				for (j = 0; j < used[i]; j++)
				{
					fp2 << zhu_in[i][j] << " ";
				}
				fp2 << "\n";
				for (j = 0; j < used[i]; j++)
				{
					fp2 << zhu_in[i][j] << "\t" << zhu_in[i][j] << "\n";
				}
			}
		}
		fp2.close();
		chmod("ZhuYin-Big5.map",S_IRUSR|S_IWUSR|S_IXUSR|S_IXGRP|S_IRGRP|S_IROTH|S_IXOTH);
	}
	//system("pause");
	return 0;
}


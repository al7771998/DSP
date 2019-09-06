#include <cstdio>
#include "Ngram.h"
#include <fstream>
#include <cstring>
#include <stdlib.h>

int ZhuYin_big5(char b)
{
	int ZhuYin;
	if (b <= 126 && b>=116)
	{
		return (b-116);
	}
	else if (b >= -95 && b <= -83)
	{
		return (b+106);
	}
	else if (b == -81) return 24;
	else if (b >= -79 && b <= -70)
	{
		return (b+104);
	}
	else return -1;
}
int main(int argc, char *argv[])
{
	fstream ftext,fmap;
	if(!strcmp(argv[1],"-text"))
	{
		ftext.open(argv[2],ios::in);
	}
	if(!strcmp(argv[3],"-map"))
	{
		fmap.open(argv[4],ios::in);
	}
	int ngram_order;
	if(!strcmp(argv[7],"-order"))
	{
		ngram_order = atoi(argv[8]);
	}
	Vocab voc;
	Ngram lm( voc, ngram_order );
	if(!strcmp(argv[5],"-lm"))
	{
        File lmFile(argv[6], "r" );
		lm.read(lmFile);
        lmFile.close();
	}
	char w_in[50000];
	char ZhuYin[40][3000][5];
	int i = 0,j,k,Zhu_count[40], length;
	//int T = 25;
	while(!fmap.eof())//while(T--)
	{
		fmap.getline(w_in, sizeof(w_in));
		length = strlen(w_in);
		Zhu_count[i] = (length - 3) / 3;
		for (j = 0; j < Zhu_count[i]; j++)
		{
			fmap.getline(w_in, sizeof(w_in));
			ZhuYin[i][j][0] = w_in[0];
			ZhuYin[i][j][1] = w_in[1];
		}
		//cout << Zhu_count[i] << endl;
		i++;
	}
	fmap.close();
	double delta[2000][101], word_prob;
	int last_state[2000][101], count, Zhu_now, state_num[101];
	int state_final[101];
	char state_word[2000][101][3];
	VocabIndex wid, context;
	int max_state;
	double max_prob;
	while(!ftext.eof())//while(T--)
	{
		ftext.getline(w_in,sizeof(w_in));
		length = strlen(w_in);
		count = 0;
		for (i = 0; i < length; i++)
		{
			if(w_in[i] == ' ' || w_in[i] == '\n') continue;
			else if(w_in[i] == -93)
			{
				if((Zhu_now = ZhuYin_big5(w_in[i+1])) >= 0)
				{
					state_num[count] = Zhu_count[Zhu_now];
					for(j = 0; j < Zhu_count[Zhu_now]; j++)
					{
						if(count == 0)
						{
							context = voc.getIndex("<s>");
							//cout << "context = " << context << ",";
							wid = voc.getIndex(ZhuYin[Zhu_now][j]);
							if (wid == Vocab_None) wid = voc.getIndex(Vocab_Unknown);
							//cout << "wid = " << wid << endl;
							state_word[j][count][0] = ZhuYin[Zhu_now][j][0];
							state_word[j][count][1] = ZhuYin[Zhu_now][j][1];
							delta[j][count] = lm.wordProb(wid, &context);
						}
						else
						{
							max_prob = -999999.0;
							max_state = -1;
							wid = voc.getIndex(ZhuYin[Zhu_now][j]);
							if (wid == Vocab_None) wid = voc.getIndex(Vocab_Unknown);
							
							for(k = 0; k < state_num[count - 1]; k++)
							{
								context = voc.getIndex(state_word[k][count - 1]);
								word_prob = lm.wordProb(wid,&context);
								if(delta[k][count - 1] + word_prob > max_prob)
								{
									max_state = k;
									max_prob = delta[k][count - 1] + word_prob; 
								}
							}
							last_state[j][count] = max_state;
							state_word[j][count][0] = ZhuYin[Zhu_now][j][0];
							state_word[j][count][1] = ZhuYin[Zhu_now][j][1];
							delta[j][count] = max_prob;
						}
					}
				}
				count++;
				i++;
			}
			else
			{
				state_num[count] = 1;	
				if(count == 0)
				{
					context = voc.getIndex("<s>");
					state_word[0][count][0] = w_in[i];
					state_word[0][count][1] = w_in[i+1];
					wid = voc.getIndex(state_word[0][count]);
					if(wid == Vocab_None)
					{
						delta[j][count] = lm.wordProb(voc.getIndex(Vocab_Unknown), &context);
					}
					else
						delta[j][count] = lm.wordProb(wid, &context);
				}
				else
				{
					max_prob = -999999.0;
					max_state = -1;
					state_word[0][count][0] = w_in[i];
					state_word[0][count][1] = w_in[i+1];
					wid = voc.getIndex(state_word[0][count]);
					if (wid == Vocab_None) wid = voc.getIndex(Vocab_Unknown);
					for(k = 0; k < state_num[count - 1]; k++)
					{
						context = voc.getIndex(state_word[k][count - 1]);
						word_prob = lm.wordProb(wid,&context);
						if(delta[k][count - 1] + word_prob > max_prob)
						{
							max_state = k;
							max_prob = delta[k][count - 1] + word_prob; 
						}
					}
					last_state[0][count] = max_state;
					delta[0][count] = max_prob;
				}
				
				
				count++;
				i++;
			}
		}
		if(count > 0)
		{
			state_num[count] = 1;	
			max_prob = -999999.0;
			max_state = -1;
			wid = voc.getIndex("</s>");
			if (wid == Vocab_None) wid = voc.getIndex(Vocab_Unknown);
			for(k = 0; k < state_num[count - 1]; k++)
			{
				context = voc.getIndex(state_word[k][count - 1]);
				word_prob = lm.wordProb(wid,&context);
				if(delta[k][count - 1] + word_prob > max_prob)
				{
					max_state = k;
					max_prob = delta[k][count - 1] + word_prob; 
				}
			}
			last_state[0][count] = max_state;
			delta[0][count] = max_prob;
			count++;
		}
		max_state = -1;
		max_prob = -999999.0;
		for(i = 0; i < state_num[count - 1]; i++)
		{
			if(delta[i][count - 1] > max_prob)
			{
				max_state = i;
				max_prob = delta[i][count - 1];
			}
		}
		state_final[count - 1] = max_state;
		for(i = count - 2;i >= 0; i--)
		{
			state_final[i] = last_state[state_final[i+1]][i+1];
		}
		if (count > 0) cout << "<s> ";
		for(i = 0; i < count - 1; i++)
		{
			cout << state_word[state_final[i]][i] << " ";
		}
		if (count > 0)cout << "</s>\n";
	}
	ftext.close();
	//VocabIndex wid = voc.getIndex("囧");
	//if(wid == Vocab_None) {
	//	printf("No word with wid = %d\n", wid);
    //    printf("where Vocab_None is %d\n", Vocab_None);	    
	//}
	//printf("此處為bug01\n");
	//wid = voc.getIndex("患者");
	//printf("此處為Bug02\n");
	//VocabIndex context[] = {voc.getIndex("癮") , voc.getIndex("毒"), Vocab_None};
	//printf("此處為Bug03\n");
	//printf("log Prob(患者|毒-癮) = %f\n", lm.wordProb(wid, context));
	//printf("此處為Bug03\n");
	return 0;
}

#include "hmm.h"
#include <math.h>

int main(int argc, char *argv[])
{

	HMM hmms[5];
	load_models( argv[1], hmms, 5);
	double delta[MAX_SEQ][MAX_STATE];
	int count0, count1, i, j, k, l;
	FILE *fp = open_or_die(argv[2],"r");
	FILE *fpw = open_or_die(argv[3],"w");
	char seq_test[2500][MAX_SEQ];
	for(count0 = 0; count0 < 2500; count0++)
	{
		fscanf(fp, "%s", seq_test[count0]);
	}
	fclose(fp);
	
	double prob[5], max_delta, max_prob;
	int max_prob_index;
	int len = strlen(seq_test[0]);
	for(count1 = 0; count1 < 2500; count1++)
	{
		for(count0 = 0; count0 < 5; count0++)
		{
			for(i = 0; i < len; i++)
			{
				for(j = 0; j < hmms[count0].state_num; j++)
				{
					if(i == 0)
					{
						delta[i][j] = hmms[count0].initial[j] * hmms[count0].observation[seq_test[count1][i] - 'A'][j];
					}
					else
					{
						max_delta = 0.0;
						for(k = 0; k < hmms[count0].state_num; k++)
						{
							if(delta[i - 1][k] * hmms[count0].transition[k][j] > max_delta)
							{
								max_delta = delta[i - 1][k] * hmms[count0].transition[k][j];
							}
						}
						delta[i][j] = max_delta * hmms[count0].observation[seq_test[count1][i] - 'A'][j];
					}
				}
			}
			prob[count0] = 0.0;
			for(i = 0; i < hmms[count0].state_num; i++)
			{
				if(delta[len - 1][i] > prob[count0])
				{
					prob[count0] = delta[len - 1][i];
				}
			}
		}
		max_prob_index = 0;
		max_prob = prob[0];
		for(count0 = 1; count0 < 5; count0++){
			if(prob[count0] > max_prob)
			{
				max_prob = prob[count0];
				max_prob_index = count0;
			}
		}
		fprintf(fpw, "%s %e\n", hmms[max_prob_index].model_name, max_prob);
	}
	//dump_models( hmms, 5);
	fclose(fpw);
	if(!strcmp(argv[3],"result1.txt"))
	{
		double correct = 0.0;
		fp = open_or_die(argv[3],"r");
		fpw = open_or_die("../testing_answer.txt","r");
		char str1[MAX_SEQ], str2[MAX_SEQ];
		for(count0 = 0; count0 < 2500; count0++)
		{
			fscanf(fp, "%s %le", str1, &max_prob);
			fscanf(fpw, "%s", str2);
			if(!strcmp(str1,str2))
			{
				correct += 1.0;
			}
		}
		correct /= 2500;
		fprintf(stderr, "%.5lf\n", correct);
		fpw = open_or_die("acc.txt","w");
		fprintf(fpw, "%.5lf\n", correct);
		fclose(fpw);
	}
	
	return 0;
}











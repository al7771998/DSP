#include "hmm.h"
#include <math.h>

int main(int argc, char *argv[])
{
	HMM hmm_train;
	int iter = atoi(argv[1]);
	int i,j,k,l,count0,count1;
	//for(i = 0; i < 5; i++) 
	//{
	loadHMM( &hmm_train, argv[2]);
	//}
	//dumpHMM( stderr, &hmm_initial );
	
	double alpha[MAX_SEQ][MAX_STATE];
	double beta[MAX_SEQ][MAX_STATE];
	double gamma[MAX_SEQ][MAX_STATE];
	double epsilon[MAX_SEQ][MAX_STATE][MAX_STATE];
	FILE *fp = open_or_die(argv[3],"r");
	char seq_model[10000][MAX_SEQ];
	double sum_pi[MAX_STATE];
	double sum_a_up[MAX_STATE][MAX_STATE], sum_a_down[MAX_STATE][MAX_STATE];
	double sum_b_up[MAX_OBSERV][MAX_STATE], sum_b_down[MAX_OBSERV][MAX_STATE];
	double sig_alpha_a, sig_ab_beta, sig_alpha_beta, sig_a_a_b_b;
	int len;
	//for(count0 = 0; count0 < 5;count0++)
	//{
	for(i = 0; i < 10000; i++)
	{
		fscanf(fp, "%s", seq_model[i]);
		//if(i % 100 == 0){
		//	fprintf(stderr, "%s\n", seq_model[i]);
		//} 
		
	}
	len = strlen(seq_model[0]);
	fclose(fp);
	for(i = 0; i < iter; i++)
	{
		//initialize
		for(count1 = 0; count1 < hmm_train.state_num; count1++)
		{
			sum_pi[count1] = 0.0;
			for(j = 0; j < hmm_train.state_num; j++)
			{
				sum_a_down[count1][j] = 0.0;
				sum_a_up[count1][j] = 0.0;
			}
			for(j = 0; j < hmm_train.observ_num; j++)
			{
				sum_b_down[j][count1] = 0.0;
				sum_b_up[j][count1] = 0.0;
			}
		}
		//Each sample calculates alpha, beta, gamma and epsilon first 
		for(count1 = 0;count1 < 10000; count1++)
		{
			for(j = 0; j < len; j++)
			{
				for(k = 0; k < hmm_train.state_num; k++)
				{
					if(j == 0)
					{
						alpha[j][k] = hmm_train.initial[k] * hmm_train.observation[seq_model[count1][j] - 'A'][k];
						beta[len-j-1][k] = 1.0;
					}
					else
					{
						sig_alpha_a = 0.0;//The sum of alpha(t,i) * a(i,j) for each i
						sig_ab_beta = 0.0;//The beta with t < T
						for(l = 0; l < hmm_train.state_num; l++)
						{
							sig_alpha_a += alpha[j-1][l] * hmm_train.transition[l][k];
							sig_ab_beta += hmm_train.transition[k][l] * hmm_train.observation[seq_model[count1][len - j] - 'A'][l] * beta[len - j][l];
						}
						alpha[j][k] = sig_alpha_a * hmm_train.observation[seq_model[count1][j] - 'A'][k];
						beta[len - j - 1][k] = sig_ab_beta;
					}

				}
				
			}
			/*for(j = 0; j < len; j++)
			{
				for(k = 0; k < hmm_train.state_num; k++)
				{
					fprintf(stderr, "%e ", alpha[j][k]);
				}
				fprintf(stderr, "\n");
			}*/
			//fprintf(stderr, "--------------------------------------\n");
			for(j = 0; j < len; j++)
			{
				sig_alpha_beta = 0.0;//Denominator of gamma
				sig_a_a_b_b = 0.0;//Denominator of epsilon
				for(k = 0; k < hmm_train.state_num; k++)
				{
					sig_alpha_beta += alpha[j][k] * beta[j][k];
					for(l = 0; l < hmm_train.state_num; l++)
					{
						sig_a_a_b_b += alpha[j][k] * hmm_train.transition[k][l] * hmm_train.observation[seq_model[count1][j + 1] - 'A'][l] * beta[j + 1][l];
					}
				}
				for(k = 0; k < hmm_train.state_num; k++)
				{
					gamma[j][k] = alpha[j][k] * beta[j][k] / sig_alpha_beta;
					for(l = 0; l < hmm_train.state_num; l++)
					{
						epsilon[j][k][l] = alpha[j][k] * hmm_train.transition[k][l] * hmm_train.observation[seq_model[count1][j + 1] - 'A'][l] * beta[j + 1][l];
						epsilon[j][k][l] /= sig_a_a_b_b;
					}
				}
			}
			//The following add all samples of gammas and epsilons for updating new hmm
			//sum_pi calculates new initial of hmm
			for(j = 0; j < hmm_train.state_num; j++)
			{
				sum_pi[j] += gamma[0][j];
			}
			//sum_a_down is Denominator of new transition(a) of hmm
			//sum_a_up is numerator of new transition(a) of hmm
			for(j = 0; j < hmm_train.state_num; j++)
			{
				for(k = 0; k < hmm_train.state_num; k++)
				{
					for(l = 0; l < len - 1; l++)
					{
						sum_a_down[j][k] += gamma[l][j];
						sum_a_up[j][k] += epsilon[l][j][k];
					}
				}
			}
			//sum_b_down is Denominator of new observation(b) of hmm
			//sum_b_up is numerator of new observation(b) of hmm
			for(j = 0; j < hmm_train.state_num; j++)
			{
				for(k = 0; k < hmm_train.observ_num; k++)
				{
					for(l = 0; l < len; l++)
					{
						if(seq_model[count1][l] - 'A' == k)
						{
							sum_b_up[k][j] += gamma[l][j];
						}
						sum_b_down[k][j] += gamma[l][j];
					}
				}
			}
		}
		//update for each iteration
		for(j = 0; j < hmm_train.state_num; j++)
		{
			hmm_train.initial[j] = sum_pi[j] / 10000.0;
			for(k = 0; k < hmm_train.state_num; k++)
			{
				hmm_train.transition[j][k] = sum_a_up[j][k] / sum_a_down[j][k];
			}
			for(k = 0; k < hmm_train.observ_num; k++)
			{
				hmm_train.observation[k][j] = sum_b_up[k][j] / sum_b_down[k][j];
			}
		}
		//dumpHMM( stderr, &hmm_train);
		//fprintf(stderr, "==================================================\n");
	}
	//}
	//dump_models(hmm_train,5);
	dumpHMM( stderr, &hmm_train);
	fp = open_or_die(argv[4],"w");
	dumpHMM(fp, &hmm_train);
	fclose(fp);
	return 0;
}










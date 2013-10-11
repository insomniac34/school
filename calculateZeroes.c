//written by Tyler Raborn

#include "math.h"
#include "stdio.h"
#include "stdlib.h"

float* calculateZeroes(float, float, float); 

int main(int argc, char** argv)
{
	if (argc!=4) 
	{
		printf("Invalid Number of Args! Usage: /.zeroes 1 2 3\n");
		return EXIT_FAILURE;
	}

	float* ans = (float*)calculateZeroes(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
	if (ans[2] == 1) printf("Zeroes: {%g, 0} and {%g, 0} \n", ans[0], ans[1]);
	else printf("ERROR: Undefined or Invalid Result!\n");
	free(ans);
	return EXIT_SUCCESS;
}

float* calculateZeroes(float a, float b, float c)
{
	float* ret = (float*)malloc(sizeof(float)*3);
	float temp_pos, temp_neg;

	if ((2*a)!=0)
	{
		if (pow(b, 2) - (4*a*c) > 0)
		{
			temp_pos = ((-1 * b) + sqrt(pow(b, 2) - (4*a*c)));
			temp_neg = ((-1 * b) - sqrt(pow(b, 2) - (4*a*c)));

			ret[0] = temp_pos / (2*a);
			ret[1] = temp_neg / (2*a);
			ret[2] = 1; //flag = valid
		}
		else
		{
			ret[0] = 0.0;
			ret[1] = 0.0;
			ret[2] = 3; //flag = undefined 
		}
	}
	else
	{
		ret[0] = 0.0;
		ret[1] = 0.0;
		ret[2] = 0; //flag = dne
	}

	return ret;
}

/*
 * Monte Carlo simulator that simulates block discovery without 
 * computing a cryptopuzzle
 */

#include <stdio.h>
#include <stdlib.h>

/* determines how many samples are in x-axis */
#define DATA_SIZE 1000

/* difference of private chain (both published and unpublished blocks
   included) and public chain */
static unsigned pool_lead; 
/* length of private branch; if only one branch exists then 0 */
static unsigned privateBranchLen;  
/* number of private blocks included in main chain */
static unsigned revenue_pool; 
/* number of public blocks included in main chain */
static unsigned revenue_others; 


static void runSim(int iter, float gamma_param, float data[][5], int i);
static void init();
static void pool_found();
static void others_found(float gamma_param);
static void comp_g_param(float gamma_param);
static void plot_data(float data[][5]);

int main() 
{
    float (*data)[5] = (float (*)[5])malloc(sizeof(float)*DATA_SIZE*5);
    if (!data) {
        fprintf(stderr, "Error allocating mem for data\n"); 
        exit(1);
    }
    for (int i = 0; i < DATA_SIZE; i++) {
        float alpha_param = i * (0.5/DATA_SIZE);
        data[i][0] = alpha_param;
        data[i][1] = alpha_param; // honest mining
    }

    for (int i = 0; i < 3; i++) {
        float gamma_param = 0.5 * i;
        runSim(1000000, gamma_param, data, i+2);
    } 
    plot_data(data);
    free(data);
}

/* 
 * @fun runSim: compute relative pool revenue; store results in data arr
 * 
 * @param iter: number of blocks to simulate
 * @param gamma_param: ratio of honest miners that received pool's 
 *                     block first to the total honest mining pool 
 * @param data: array to store relative pool's revenue
 * @param idx: second index in data array where to store data 
 */
static void 
runSim(int iter, float gamma_param, float data[][5], int idx)
{
    for (int j = 0; j < DATA_SIZE; j += 1) {
        init();
        float alpha_param = j * (0.5/DATA_SIZE);
        float r; 
        for (int i = 0; i < iter; i++) {
            r = ((float) rand())/RAND_MAX;
            if (r < alpha_param) 
                pool_found();
            else
                others_found(gamma_param);
        }
        /* store relative pool revenue */
        data[j][idx] = 
            ((float) revenue_pool)/(revenue_pool+revenue_others);
    }
}

/* initialize global vars */
static void init()
{
    pool_lead = 0;
    privateBranchLen = 0;
    revenue_pool = 0;
    revenue_others = 0;
}

/* selfish pool finds a new block */
static void pool_found()
{
    privateBranchLen++;
    if (pool_lead == 0 && privateBranchLen == 2) { 
        /* was tie, pool wins due to lead of 1
        publish all private blocks; profit of 2 to pool */ 
        revenue_pool += 2;
        privateBranchLen = 0;
    }
    else {
        pool_lead++;
    }
    return;
}

/* others find a new block */
void others_found(float gamma_param)
{
    if (pool_lead == 0) {
        if (privateBranchLen == 0) {
            /* no private chain
               profit of 1 to others */
            revenue_others++;
        }
        else {
            /* two competing branches
               check for gamma_param
               determine which branch others build on */
            comp_g_param(gamma_param);
            privateBranchLen = 0;
        }
    } 
    else if (pool_lead == 1) {
        /* was lead 1, now same length; profit determined later */
        pool_lead--; 
    }
    else if (pool_lead == 2) {
        /* publish all private chain
           profit of 2 to pool due to lead of 1 */
        revenue_pool += 2;
        pool_lead = 0;
        privateBranchLen = 0; 
    }
    else if (pool_lead > 2){
        /* publish one private block, lead remains >= 2
           profit of 1 to pool since pool wins eventually */   
        revenue_pool++;
        pool_lead--;   
    }
    return;
}

/* computes profit in case of two competing branches of same length */
void comp_g_param(float gamma_param)
{
    float r = ((float)rand())/RAND_MAX;
    if (r < gamma_param) {
        /* others find a block after pool head
           profit of 1 to others and pool */
        revenue_others++;
        revenue_pool++;
    }
    else {
        /* others find a block after others' head
           profit of 2 to others */
        revenue_others += 2;
    }
}

/* writes data to data.txt */
static void plot_data(float data[][5])
{
    FILE *gp = popen("gnuplot -persist", "w");
    if (!gp) {
        fprintf(stderr, "Error opening GNUplot\n");
        exit(1);
    }
    FILE *fp = fopen("data.txt", "w");
    if (!fp) {
        fprintf(stderr, "Error opening data file\n");
        exit(1);
    }
    for (int i = 0; i < DATA_SIZE; i++) {
        fprintf(fp, "%f %f %f %f %f\n", 
            data[i][0], data[i][1], data[i][2], data[i][3], data[i][4]);
    }
    fclose(fp);

    fprintf(gp, "set term pngcairo enhanced font 'Arial,14'\n");
    fprintf(gp, "set output 'plot.png'\n");
    fprintf(gp, "set size 1,1\n");
    fprintf(gp, "set xlabel 'Pool size'\n");
    fprintf(gp, "set ylabel 'Relative pool revenue'\n");
    fprintf(gp, "set xtics 0.1\n");
    fprintf(gp, "set ytics 0.2\n");
    fprintf(gp, "set grid lw 1.5\n");
    fprintf(gp, "set key left top\n");
    fprintf(gp, "plot 'data.txt' u 1:2 with lines lw 2 lc rgb 'grey' \
                title 'Honest mining', \
                'data.txt' u 1:3 with lines lw 2 lc rgb 'red' \
                title 'gamma=0.0', \
                'data.txt' u 1:4 with lines lw 2 lc rgb 'green' \
                title 'gamma=0.5', \
                'data.txt' u 1:5 with lines lw 2 lc rgb 'blue' \
                title 'gamma=1.0'\n");

    fflush(gp);
    pclose(gp);
}

/*
 * Selfish Mining Algorithm proposed by Eyal & Sirer: 
 * Monte Carlo simulator of state transitions 
 */

#include <stdio.h>
#include <stdlib.h>

/* change if you want even finer sampling of x-axis 
   memory is allocated through malloc */
#define MAX_DATA_S 1000000

/* chaneg default parameters here */
#define DATA_SIZE 1000
#define BLOCKS 1000000

/* difference of private chain (both published and unpublished blocks
   included) and public chain */
static unsigned pool_lead; 
/* length of private branch; if only one branch exists then 0 */
static unsigned privateBranchLen;  
/* number of private blocks included in main chain */
static unsigned revenue_pool; 
/* number of public blocks included in main chain */
static unsigned revenue_others; 

/* default number of samples in x-axis */
int data_size = DATA_SIZE;

static void runSim
    (int blocks, float gamma_param, float (*data)[data_size]);
static void init();
static void pool_found();
static void others_found(float gamma_param);
static void comp_g_param(float gamma_param);
static void plot_data(float (*data)[data_size], float gamma, int mode);

int main(int argc, char *argv[]) 
{
    float gamma_param = 0;
    int blocks = BLOCKS; /* default number of blocks to simulate */
    int mode = 1;   /* default mode */

    if (argc != 1 && argc != 4) {
        fprintf(stderr, 
            "Usage: %s <gamma param> <data size> <blocks>\n", argv[0]);
        return 0;
    }
    if (argc == 4) {
        gamma_param = atof(argv[1]);
        data_size = atoi(argv[2]);
        blocks = atoi(argv[3]);
        mode = 0;
    }
    if (gamma_param < 0 || gamma_param > 1 || 
        data_size <= 0 || data_size > MAX_DATA_S || blocks < 0) {
            fprintf(stderr, "gamma param: float, 0 <= g <= 1\n");
            fprintf(stderr, "data size  : int, O < ds <= 1000000 "
                "(maximum data size can be changed in source code)\n");
            fprintf(stderr, "blocks     : int, > 0\n");
        return 0;
    }   
    
    /* allocate memory to store data */
    float (*data)[data_size];
    if (mode) {
        data = (float (*)[data_size])malloc(sizeof(float)*data_size*5);
    }
    else {
        data = (float (*)[data_size])malloc(sizeof(float)*data_size*3);
    }
    if (!data) {
        fprintf(stderr, "Error allocating mem for data\n"); 
        exit(1);
    }
    
    /* initialize x-axis and honest mining plot */
    for (int i = 0; i < data_size; i++) {
        float alpha_param = i * (0.5/data_size);
        data[0][i] = alpha_param;
        data[1][i] = alpha_param; // honest mining
    }

    if (mode) {
        for (int i = 0; i < 3; i++) {
            float gamma_param = 0.5 * i;
            runSim(blocks, gamma_param, data);
        } 
    }
    else {
        runSim(blocks, gamma_param, data);
    }
    plot_data(data, gamma_param, mode);
    free(data);
}

/*------------------ Discrete-event simulator engine -----------------*/

/* 
 * @fun runSim: compute relative pool revenue; store results in data arr
 * 
 * @param iter: number of blocks to simulate
 * @param gamma_param: ratio of honest miners that received pool's 
 *                     block first to the total honest mining pool 
 * @param data: array to store relative pool revenue values
 */
static void 
runSim(int iter, float gamma_param, float (*data)[data_size])
{
    static int idx = 2;
    for (int j = 0; j < data_size; j += 1) {
        init();
        float alpha_param = j * (0.5/data_size);
        float r; 
        for (int i = 0; i < iter; i++) {
            r = ((float) rand())/RAND_MAX;
            if (r < alpha_param) 
                pool_found();
            else
                others_found(gamma_param);
        }
        /* store relative pool revenue */
        data[idx][j] = 
            ((float) revenue_pool)/(revenue_pool+revenue_others);
    }
    idx++;
}


/*----------------------- Selfish Mine Logic -------------------------*/

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

/*-------------------------- Plot the graph --------------------------*/

/* writes data to data.txt and plots to plot.png */
static void 
plot_data(float (*data)[data_size], float gamma, int mode)
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
    if (mode) {
        for (int i = 0; i < data_size; i++) {
            fprintf(fp, "%f %f %f %f %f\n", 
            data[0][i], data[1][i], data[2][i], data[3][i], data[4][i]);
        }
    }
    else {
        for (int i = 0; i < data_size; i++) {
            fprintf(fp, "%f %f %f\n", 
            data[0][i], data[1][i], data[2][i]);
        }
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
    if (mode) {
        fprintf(gp, 
        "plot 'data.txt' u 1:2 with lines lw 2 lc rgb 'grey' \
        title 'Honest mining', \
        'data.txt' u 1:3 with lines lw 2 lc rgb 'red' \
        title 'gamma=0.0', \
        'data.txt' u 1:4 with lines lw 2 lc rgb 'green' \
        title 'gamma=0.5', \
        'data.txt' u 1:5 with lines lw 2 lc rgb 'blue' \
        title 'gamma=1.0'\n");
    }
    else {
        fprintf(gp, 
        "plot 'data.txt' u 1:2 with lines lw 2 lc rgb 'grey' \
        title 'Honest mining', \
        'data.txt' u 1:3 with lines lw 2 lc rgb 'red' \
        title 'gamma=%.2f'\n", gamma);
    }
    fflush(gp);
    pclose(gp);
}

/*--------------------------------------------------------------------*/

/* initialize global vars */
static void init()
{
    pool_lead = 0;
    privateBranchLen = 0;
    revenue_pool = 0;
    revenue_others = 0;
}
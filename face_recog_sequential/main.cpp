
/*
title: face recognition with openmpi
author: Raikibul Hasan
course:Parallel and Grid computing
date: 18 dec 2020
*/
#include <bits/stdc++.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <math.h>
#include <limits.h>
#include <sys/time.h>

using namespace std;

int **alloc_2d_matrix(int r, int c)
{
    int ** a;
    int i;
    a = (int **)malloc(sizeof(int *) * r);
    if (a == NULL) {
        perror("Failed to allocate Memory!");
        exit(0);
    }
    for (i = 0; i < r; ++i) {
        a[i] = (int *)malloc(sizeof(int) * c);
        if (a[i] == NULL) {
            perror("Failed to allocate Memory!");
            exit(EXIT_FAILURE);
        }
    }
    return a;
}

void dealloc_2d_matrix(int **a, int r, int c)
{
    	int i;
   	for (i = 0; i < r; ++i)
        	free(a[i]);
    	free(a);
}

int ** read_pgm_file(char * file_name, int h, int w)
{
	FILE * file;
    int i,j, int_tmp;
    int** data=alloc_2d_matrix(h,w);

    if ((file = fopen(file_name, "r+")) == NULL)
	{
	    printf("ERROR:failed to Open file\n");
	    return(NULL);
	}

	for (i = 0; i < (h); i++)
	{
        for (j = 0; j < (w); j++)
        {
            fscanf(file,"%d", &int_tmp);
            data[i][j] = int_tmp;
        }
    }
    fclose(file);
    return data;
}



void create_hist(int * hist, int **img, int num_rows, int num_cols) {

    for (int i = 0; i < 255; i++) {
        hist[i] = 0;
    }

    int model_img[8];

    // pixels
    for(int r = 1; r < num_rows - 2; r++) {
        for(int c = 1; c < num_cols - 2; c++) {

            int center = img[r][c]; //
            model_img[0] = (center < img[r-1][c-1] ? 1 : 0);
            model_img[1] = (center < img[r-1][c] ? 1 : 0);
            model_img[2] = (center < img[r-1][c+1] ? 1 : 0);
            model_img[3] = (center < img[r][c+1] ? 1 : 0);
            model_img[4] = (center < img[r+1][c+1] ? 1 : 0);
            model_img[5] = (center < img[r+1][c] ? 1 : 0);
            model_img[6] = (center < img[r+1][c-1] ? 1 : 0);
            model_img[7] = (center < img[r][c-1] ? 1 : 0);

            //convert binary string to decimal
            int index = 0;

            for(int i = 0; i <= 7; i++) {
                index += pow(2,(7 - i)) * model_img[i];
            }

            hist[index]++;
        }
    }



    for (int c = 1; c < num_cols - 1; c++) {
        int center = img[0][c];
        model_img[0] = 0;
        model_img[1] = 0;
        model_img[2] = 0;
        model_img[3] = (center < img[0][c+1] ? 1 : 0);
        model_img[4] = (center < img[1][c+1] ? 1 : 0);
        model_img[5] = (center < img[1][c] ? 1 : 0);
        model_img[6] = (center < img[1][c-1] ? 1 : 0);
        model_img[7] = (center < img[0][c-1] ? 1 : 0);

        //convert binary string to decimal
        int index = 0;

        for(int i = 0; i <= 7; i++) {
            index += pow(2,(7 - i)) * model_img[i];
        }

        hist[index]++;
    }

    for (int c = 1; c < num_cols - 1; c++) {
        int center = img[num_rows - 1][c];
        model_img[0] = (center < img[num_rows - 2][c-1] ? 1 : 0);
        model_img[1] = (center < img[num_rows - 2][c] ? 1 : 0);
        model_img[2] = (center < img[num_rows - 2][c+1] ? 1 : 0);
        model_img[3] = (center < img[num_rows - 1][c+1] ? 1 : 0);
        model_img[4] = 0;
        model_img[5] = 0;
        model_img[6] = 0;
        model_img[7] = (center < img[num_rows - 1][c-1] ? 1 : 0);

        //convert binary string to decimal
        int index = 0;

        for(int i = 0; i <= 7; i++) {
            index += pow(2,(7 - i)) * model_img[i];
        }

        hist[index]++;
    }

    // process c = 0 case
    for (int r = 1; r < num_rows - 1; r++) {
        int center = img[r][0];
        model_img[0] = 0;
        model_img[1] = (center < img[r-1][0] ? 1 : 0);
        model_img[2] = (center < img[r-1][1] ? 1 : 0);
        model_img[3] = (center < img[r][1] ? 1 : 0);
        model_img[4] = (center < img[r+1][1] ? 1 : 0);
        model_img[5] = (center < img[r+1][0] ? 1 : 0);
        model_img[6] = 0;
        model_img[7] = 0;

        //convert binary string to decimal
        int index = 0;

        for(int i = 0; i <= 7; i++) {
            index += pow(2,(7 - i)) * model_img[i];
        }

        hist[index]++;
    }

    // process c = img_width - 1 case
    for (int r = 1; r < num_rows - 1; r++) {
        int center = img[r][num_cols - 1];
        model_img[0] = (center < img[r-1][num_cols - 2] ? 1 : 0);
        model_img[1] = (center < img[r-1][num_cols - 1] ? 1 : 0);
        model_img[2] = 0;
        model_img[3] = 0;
        model_img[4] = 0;
        model_img[5] = (center < img[r+1][num_cols - 1] ? 1 : 0);
        model_img[6] = (center < img[r+1][num_cols - 2] ? 1 : 0);
        model_img[7] = (center < img[r][num_cols - 2] ? 1 : 0);

        //convert binary string to decimal
        int index = 0;

        for(int i = 0; i <= 7; i++) {
            index += pow(2,(7 - i)) * model_img[i];
        }

        hist[index]++;
    }

    // process corners of the image

    // r = 0 and c = 0
    int center = img[0][0];
    model_img[0] = 0;
    model_img[1] = 0;
    model_img[2] = 0;
    model_img[3] = (center < img[0][1] ? 1 : 0);
    model_img[4] = (center < img[1][1] ? 1 : 0);
    model_img[5] = (center < img[1][0] ? 1 : 0);
    model_img[6] = 0;
    model_img[7] = 0;

    // convert binary string to decimal
    int index = 0;

    for(int i = 0; i <= 7; i++) {
        index += pow(2,(7 - i)) * model_img[i];
    }

    hist[index]++;

    // r = img_height - 1 and c = 0
    center = img[num_rows - 1][0];
    model_img[0] = 0;
    model_img[1] = (center < img[num_rows - 2][0] ? 1 : 0);
    model_img[2] = (center < img[num_rows - 2][1] ? 1 : 0);
    model_img[3] = (center < img[num_rows - 1][1] ? 1 : 0);
    model_img[4] = 0;
    model_img[5] = 0;
    model_img[6] = 0;
    model_img[7] = 0;

    //convert binary string to decimal
    index = 0;

    for(int i = 0; i <= 7; i++) {
        index += pow(2,(7 - i)) * model_img[i];
    }

    hist[index]++;

    // r = img_height - 1 and c = img_width - 1
    center = img[num_rows - 1][num_cols - 1];
    model_img[0] = (center < img[num_rows - 2][num_cols - 2] ? 1 : 0);
    model_img[1] = (center < img[num_rows - 2][num_cols - 1] ? 1 : 0);
    model_img[2] = 0;
    model_img[3] = 0;
    model_img[4] = 0;
    model_img[5] = 0;
    model_img[6] = 0;
    model_img[7] = (center < img[num_rows - 1][num_cols - 2] ? 1 : 0);

    //convert binary string to decimal
    index = 0;

    for(int i = 0; i <= 7; i++) {
        index += pow(2,(7 - i)) * model_img[i];
    }

    hist[index]++;

    // r = 0 and c = img_width - 1
    center = img[0][num_cols - 1];
    model_img[0] = 0;
    model_img[1] = 0;
    model_img[2] = 0;
    model_img[3] = 0;
    model_img[4] = 0;
    model_img[5] = (center < img[1][num_cols - 1] ? 1 : 0);
    model_img[6] = (center < img[1][num_cols - 2] ? 1 : 0);
    model_img[7] = (center < img[0][num_cols - 2] ? 1 : 0);

    //convert binary string to decimal
    index = 0;

    for(int i = 0; i <= 7; i++) {
        index += pow(2,(7 - i)) * model_img[i];
    }

    hist[index]++;
}

double distance(int *a, int *b, int size) {

    double distance = 0.0;
    double running = 0.0;

    for(int i = 0; i < size; i++) {
        running = pow(a[i] - b[i], 2) * 0.5;
        running = (a[i] + b[i] == 0) ? 0 : running / (a[i] + b[i]);
        distance += running;
    }

    return distance;
}

int find_closest(int ***training_set, int num_persons, int num_training, int size, int *test_image) {

    int min_dist =  INT_MAX;
    int min_pid = -1;
    int temp_dist = 0;

    for (int i = 0; i < num_persons; i++) {
        for (int j = 0; j < num_training; j++) {
            temp_dist = distance(training_set[i][j], test_image, size);
            if(temp_dist < min_dist) {
                min_dist = temp_dist;
                min_pid = i+1;
            }
        }
    }

    return min_pid;

}

int main(int * argc, char **argv) {

    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    int trainset=5;

    int k = atoi(argv[trainset]); // k is the number of training set images, and 20 > k > 0

    // following variables are self-explanatory
    int num_person = 18;
    int img_per_person = 20;
    int img_width = 200;
    int img_height = 180;
    int bin_count = 255;

    char * img_dir = "./images";
    char * img_name;

    // allocate a 3d array to hold histograms for training and test set images.
    int *** training_set_histograms = (int***)malloc(num_person * sizeof(int**));
    int *** test_set_histograms = (int***)malloc(num_person * sizeof(int**));
    for(int i = 0; i < num_person; i++) {
        training_set_histograms[i] = alloc_2d_matrix(k, bin_count);
        test_set_histograms[i] = alloc_2d_matrix(20 - k, bin_count);
    }

    //--------------------------- Creating Training Step & Test Step Histogram -----------------------------------------------

    // read image files in the "images/"" directory and generate corresponding histograms
    int cur_person_id;
    int cur_img_id;
    int ** cur_img;
    char * abs_file_path =(char*) malloc (19 * sizeof(char));

    for (cur_person_id = 1; cur_person_id <= num_person; cur_person_id++) {
        for (cur_img_id = 1; cur_img_id <= img_per_person; cur_img_id++ ) {


            sprintf(abs_file_path, "%s/%d.%d.txt", img_dir, cur_person_id, cur_img_id);
            cur_img = read_pgm_file(abs_file_path, img_height, img_width);


            if(cur_img_id > k) {
                // the image is in test set i.e. the images with ids between k + 1 and 20
                create_hist(test_set_histograms[cur_person_id - 1][cur_img_id - 1 - k], cur_img, img_height, img_width);
            }
            else {
                // the image is in training set i.e. the images with ids between 1 and k
                create_hist(training_set_histograms[cur_person_id - 1][cur_img_id - 1], cur_img, img_height, img_width);
            }

            dealloc_2d_matrix(cur_img, img_height, img_width);
        }

    }

    // now histograms for the training and test set are computed.
    // generate test result s and compare them with correct values.
    int correct_count = 0;
    int pred = -1;
    for( int i = 0; i < num_person; i++) {
        for( int j = 0; j < 20 - k; j++) {
            // find closest training hist.
            pred = find_closest(training_set_histograms, num_person, k,  bin_count, test_set_histograms[i][j]);
            printf("%d.%d.txt %d %d\n", (i+1), (j + k + 1), pred, i+1);
            if(pred == (i+1)){
                correct_count++;
            }
        }
    }

    gettimeofday(&end, NULL);
    double seq_time = (end.tv_sec - begin.tv_sec) * 1000 + (end.tv_usec - begin.tv_usec)/1000;
    cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl;
    cout<<"*****Face Recognition system*******"<<endl;
    cout<<"*****LOCAL BINARY PATTERN********"<<endl<<endl;
    cout<<"Number of Traning set:"<<trainset<<endl;
    printf("Execution Time for Sequential Implementation: %f ms\n", seq_time);
    //printf("Parallel time: 00.00 ms\n");

    return 0;
}




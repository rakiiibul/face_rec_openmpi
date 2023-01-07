
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
#include <omp.h>

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
      #pragma omp parallel for shared(hist)
    for (int i = 0; i < 255; i++) {
        hist[i] = 0;
    }

    int model_img[8];
    int r;

    #pragma omp parallel for shared(num_rows, num_cols, hist, img), private(model_img), default(none)
    // pixels
    for(r = 1; r < num_rows - 2; r++) {
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
            #pragma omp atomic
            hist[index]++;
        }
    }

    int c;
    #pragma omp parallel for private(c,model_img),shared (img,num_cols,num_rows,hist),default(none)

    for (c = 1; c < num_cols - 1; c++) {
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
        #pragma omp atomic
        hist[index]++;
    }


    #pragma omp parallel for private(c,model_img),shared (img,num_cols,num_rows,hist),default(none)
    for ( c = 1; c < num_cols - 1; c++) {
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
        #pragma omp atomic
        hist[index]++;
    }
    #pragma omp parallel for private(r,model_img),shared (img,num_cols,num_rows,hist),default(none)
    // process c = 0 case
    for (r = 1; r < num_rows - 1; r++) {
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
        #pragma omp atomic
        hist[index]++;
    }
     #pragma omp parallel for private(r,model_img),shared (img,num_cols,num_rows,hist),default(none)
    // process c = img_width - 1 case
    for ( r = 1; r < num_rows - 1; r++) {
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
        #pragma omp atomic
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
    #pragma omp atomic
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
    #pragma omp atomic
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
    #pragma omp atomic
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
    #pragma omp atomic
    hist[index]++;
}

double distance(int *a, int *b, int size) {

    double distance = 0.0;
    double running = 0.0;
    int i;

    #pragma omp parallel for shared(distance, a, b, size), private(running), default(none)

    for(i = 0; i < size; i++) {
        running = pow(a[i] - b[i], 2) * 0.5;
        running = (a[i] + b[i] == 0) ? 0 : running / (a[i] + b[i]);

        #pragma omp critical
        {

        distance += running;
        }
    }

    return distance;
}

int find_closest(int ***training_set, int num_persons, int num_training, int size, int *test_image) {

    int min_dist =  INT_MAX;
    int min_pid = -1;
    int temp_dist = 0;
    int i,j;

    #pragma omp parallel for shared(min_dist, min_pid, training_set, test_image, size, num_persons, num_training), private(temp_dist, j), default(none)

    for ( i = 0; i < num_persons; i++) {
        for (j = 0; j < num_training; j++) {
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

    struct timeval parallel_begin_1, parallel_begin_2, parallel_end_1, parallel_end_2, sequential_begin_1, sequential_begin_2, sequential_begin_3, sequential_end_1, sequential_end_2, sequential_end_3;

    //------------------------------------------ beginning of first sequential segment
    gettimeofday(&sequential_begin_1, NULL);

    int k = atoi(argv[1]); // k is the number of training set images, and 20 > k > 0

    // following variables are self-explanatory
    int num_person = 18;
    int img_per_person = 20;
    int img_width = 200;
    int img_height = 180;
    int bin_count = 255;


    char * img_dir = "./images";

    // allocate a 3d array to hold histograms for training and test set images.
    int *** training_set_histograms = (int***)malloc(num_person * sizeof(int**));
    int *** test_set_histograms =(int***) malloc(num_person * sizeof(int**));
    int i;
    //int set_thr= 14;
    //------------------------------------------ end of first sequential segment
    gettimeofday(&sequential_end_1, NULL);
    //omp_set_num_threads(set_thr);
    //------------------------------------------ beginning of first parallel segment
    gettimeofday(&parallel_begin_1, NULL);
    #pragma omp parallel for shared(num_person, training_set_histograms, test_set_histograms, k, bin_count), default(none)


    for( i = 0; i < num_person; i++) {
        training_set_histograms[i] = alloc_2d_matrix(k, bin_count);
        test_set_histograms[i] = alloc_2d_matrix(20 - k, bin_count);
    }
    //------------------------------------------ end of first parallel segment
    gettimeofday(&parallel_end_1, NULL);

    //------------------------------------------ beginning of second sequential segment
    gettimeofday(&sequential_begin_2, NULL);

    //--------------------------- Training Step & Test Step Histogram Creation -----------------------------------------------

    // read image files in the "images/"" directory and generate corresponding histograms
    int cur_person_id;
    int cur_img_id;
    int ** cur_img;
    char abs_file_path[19];

    //------------------------------------------ end of second sequential segment
    gettimeofday(&sequential_end_2, NULL);


    //------------------------------------------ beginning of second parallel segment
    //test set generation
    gettimeofday(&parallel_begin_2, NULL);
    #pragma omp parallel for shared(img_dir, img_height, img_width, k, num_person, img_per_person,training_set_histograms, test_set_histograms), private(cur_img_id , cur_img, abs_file_path), default(none)
    for (cur_person_id = 1; cur_person_id <= num_person; cur_person_id++) {
        for (cur_img_id = k+1; cur_img_id <= img_per_person; cur_img_id++ ) {

            sprintf(abs_file_path, "%s/%d.%d.txt", img_dir, cur_person_id, cur_img_id);
            cur_img = read_pgm_file(abs_file_path, img_height, img_width);


            // the image is in test set i.e. the images with ids between k + 1 and 20
            create_hist(test_set_histograms[cur_person_id - 1][cur_img_id - 1 - k], cur_img, img_height, img_width);

            dealloc_2d_matrix(cur_img, img_height, img_width);
        }

    }

    // training set generation
    #pragma omp parallel for shared(img_dir, img_height, img_width, k, num_person, img_per_person,training_set_histograms, test_set_histograms), private(cur_img_id , cur_img, abs_file_path), default(none)
    for (cur_person_id = 1; cur_person_id <= num_person; cur_person_id++) {
        for (cur_img_id = 1; cur_img_id <= k; cur_img_id++ ) {

            sprintf(abs_file_path, "%s/%d.%d.txt", img_dir, cur_person_id, cur_img_id);
            cur_img = read_pgm_file(abs_file_path, img_height, img_width);

            // the image is in training set i.e. the images with ids between 1 and k
            create_hist(training_set_histograms[cur_person_id - 1][cur_img_id - 1], cur_img, img_height, img_width);

            dealloc_2d_matrix(cur_img, img_height, img_width);
        }

    }
    // now histograms for the training and test set are computed.
    // generate test result s and compare them with correct values.
    int correct_count = 0;
    int pred = -1;
    for( int i = 0; i < num_person; i++) {
        for( int j = 0; j < 20 - k; j++) {

            pred = find_closest(training_set_histograms, num_person, k,  bin_count, test_set_histograms[i][j]);
            printf("%d.%d.txt %d %d\n", (i+1), (j + k + 1), pred, i+1);
            if(pred == (i+1)){
                correct_count++;
            }
        }
    }
    gettimeofday(&parallel_end_2, NULL);
    gettimeofday(&sequential_begin_3, NULL);

    double parallel_time = (parallel_end_1.tv_sec - parallel_begin_1.tv_sec) * 1000 + (parallel_end_1.tv_usec - parallel_begin_1.tv_usec)/1000;
    parallel_time += (parallel_end_2.tv_sec - parallel_begin_2.tv_sec) * 1000 + (parallel_end_2.tv_usec - parallel_begin_2.tv_usec)/1000;
    double sequential_time = (sequential_end_1.tv_sec - sequential_begin_1.tv_sec) * 1000 + (sequential_end_1.tv_usec - sequential_begin_1.tv_usec)/1000;
    sequential_time += (sequential_end_2.tv_sec - sequential_begin_2.tv_sec) * 1000 + (sequential_end_2.tv_usec - sequential_begin_2.tv_usec)/1000;
    gettimeofday(&sequential_end_3, NULL);
    sequential_time += (sequential_end_3.tv_sec - sequential_begin_3.tv_sec) * 1000 + (sequential_end_3.tv_usec - sequential_begin_3.tv_usec)/1000;



    //cout<<"Totall number of threads use for complete all of the partS:"<<x<<endl;
    cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl;
    cout<<"*****Parallel Implementation*******"<<endl;
    cout<<"*****Face Recognition system*******"<<endl;
    cout<<"*****LOCAL BINARY PATTERN********"<<endl<<endl;
    cout<<"Number of Traning set: 1"<<endl;
    //cout<<"Number of threads:"<<set_thr<<endl;
    printf("Parallel time: %f ms\n", parallel_time);
    //printf("Sequential time: %f ms\n", sequential_time);



    return 0;
}

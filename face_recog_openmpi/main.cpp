#include <bits/stdc++.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <math.h>
#include <limits.h>
#include <sys/time.h>

/*
title: face recognition with openmpi
author: Raikibul Hasan
course:Parallel and Grid computing
date: 18 dec 2020
*/

using namespace std;


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

            //manually convert binary string to decimal
            int index = 0;

            for(int i = 0; i <= 7; i++) {
                index += pow(2,(7 - i)) * model_img[i];
            }

            hist[index]++;
        }
    }
}
//calculate distance



int main()
{
    //create_hist()

    cout << "Hello world!" << endl;
    return 0;
}

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <limits.h>
#include <sstream>
#include <assert.h>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <vector>
#include <unordered_map>

#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))
#endif

#define XY_TO_INT(x, y) (((y)<<12)|(x))
#define INT_TO_X(v) ((v)&((1<<12)-1))
#define INT_TO_Y(v) ((v)>>12)

#define _DEBUG

using namespace cv;
using namespace std;


struct Box {
    int xmin, xmax, ymin, ymax;
};

struct CMap {
    unordered_map<int, vector<pair<int, int> > > constraint_map;
    vector<int> constraint_ids;
};

class PatchMatch {
    class BITMAP {
    public:
        int w, h;
        int* data;
        BITMAP(int w_, int h_);
        BITMAP(BITMAP* bm);
        ~BITMAP();
        int* operator[](int y) { return &data[y * w]; }
    };
private:
    int patch_w = 8;
    int pm_iters = 5;
    int rs_max = INT_MAX; // random search
    int sigma = 1 * patch_w * patch_w;

    Mat _resultImg;

private:
    void getCMap(Mat constraint, CMap* cmap);
    Box getBox(Mat mask);
    bool inBox(int x, int y, Box box);
    int dist(Mat a, Mat b, int ax, int ay, int bx, int by, int cutoff);
    void improve_guess(Mat a, Mat b, int ax, int ay, int& xbest, int& ybest, int& dbest, int bx, int by, int type);
    void patchmatch(Mat a, Mat b, BITMAP*& ann, BITMAP*& annd, Mat dilated_mask, Mat constraint, CMap* cmap);

public:
    PatchMatch();
    ~PatchMatch();
    void image_complete(Mat im_orig, Mat mask, Mat constraint);
    Mat getResult() { return _resultImg; }
};



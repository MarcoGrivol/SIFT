#include <iomanip>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define Octave vector<Mat>
#define Pyramid vector<Octave>

// params
typedef struct params {
    int num_octaves;
    int scales_per_octave;
    float k;
    vector<float> sigmas;
} Params;

Params defineParams(Size img_size, int scales_per_octave, float sigma) {
    Params P;
    P.num_octaves = cvRound(log((float)min(img_size.width, img_size.height)) / log(2.) - 2);
    P.scales_per_octave = scales_per_octave + 3;
    P.k = pow(2, 1. / scales_per_octave);
    for (int i = 0; i < P.scales_per_octave; i++) {
        P.sigmas.push_back(pow(P.k, i) * sigma);
    }
    return P;
}

Octave buildOctave(Mat img, Params P) {
    // same img size with increasing sigma in gaussian blur
    Octave oct;
    for (int i = 0; i < P.scales_per_octave; i++) {
        Mat img_blur;
        GaussianBlur(img, img_blur, Size(0, 0), P.sigmas[i]);
        oct.push_back(img_blur);
    }
    return oct;
}

Pyramid buildPyramid(Mat img, Params P) {
    // build pyramid with downsapling in between each octave (scale)
    vector<Octave> pyr;
    for (int i = 0; i < P.num_octaves; i++) {
        pyr.push_back(buildOctave(img, P));
        resize(img, img, Size(img.cols / 2, img.rows / 2));
    }
    return pyr;
}

int main(int argc, char **argv) {
    Mat img = imread(argv[1]);
    if (img.empty()) {
        cout << "Error: image is empty." << endl;
        return EXIT_FAILURE;
    }

    Params P = defineParams(img.size(), 3, 1.6);
    cout << "Defined Params: " << endl;
    cout << "|  num_octaves = " << P.num_octaves << endl;
    cout << "|  scales_per_octave = " << P.scales_per_octave << endl;
    cout << "|  k = " << P.k << endl;
    cout << "|  sigmas = ";
    for (int i = 0; i < P.sigmas.size(); i++) {
        cout << fixed << setprecision(1) << P.sigmas[i] << " ";
    } cout << endl;


    Pyramid pyr = buildPyramid(img, P);
    cout << "Pyramid has " << pyr.size() << " octaves" << endl;
    cout << "Octave has " << pyr[0].size() << " scales" << endl;
    for (int i = 0; i < pyr.size(); i++) {
        cout << "Octave " << i << " = " << pyr[i][0].size() << endl;
    }

    return EXIT_SUCCESS;
}
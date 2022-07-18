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
    P.num_octaves = cvRound(log2((double)min(img_size.height, img_size.width)) - 5) + 1;
    P.scales_per_octave = scales_per_octave + 3;
    P.k = pow(2, 1. / (scales_per_octave + 2));
    P.sigmas.push_back(sigma);
    for (int i = 1; i < P.scales_per_octave; i++) {
        // float sigma_prev = pow(P.k, i - 1) * sigma;
        // float sigma_next = P.k * sigma_prev;
        // float sig = sqrt(sigma_next*sigma_next - sigma_prev*sigma_prev);
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
    resize(img, img, Size(img.cols * 2, img.rows * 2));
    GaussianBlur(img, img, Size(0, 0), P.sigmas[0]);    

    vector<Octave> pyr;
    for (int i = 0; i < P.num_octaves; i++) {
        pyr.push_back(buildOctave(img, P));
        img = pyr[i][pyr[i].size() - 2];
        resize(img, img, Size(img.cols / 2, img.rows / 2));
    }
    return pyr;
}

Pyramid buildDoG(Pyramid pyr, Params P) {
    Pyramid dog_pyr;
    for (int oct = 0; oct < pyr.size(); oct++) {
        Octave dog_oct;
        for (int i = 0; i < pyr[oct].size() - 1; i++) {
            Mat curr_img = pyr[oct][i];
            Mat next_img = pyr[oct][i + 1];
            Mat img_diff;
            absdiff(next_img, curr_img, img_diff);
            dog_oct.push_back(img_diff);
        }
        dog_pyr.push_back(dog_oct);
    }
    return dog_pyr;
}

void plotHist(Mat img) {

}

int main(int argc, char **argv) {
    Mat img = imread(argv[1], IMREAD_GRAYSCALE);
    if (img.empty()) {
        cout << "Error: image is empty." << endl;
        return EXIT_FAILURE;
    }
    double min, max;
    minMaxLoc(img, &min, &max);
    cout << "after read" << min << " " << max << endl;

    normalize(img, img, 0., 1., NORM_MINMAX, CV_32F);

    minMaxLoc(img, &min, &max);
    cout << "after norm" << min << " " << max << endl;

    namedWindow("Lena.png", WINDOW_AUTOSIZE);
    imshow("Lena.png", img);


    Mat img_blur1;
    Mat img_blur2;
    GaussianBlur(img, img_blur1, Size(0, 0), 1.6);
    GaussianBlur(img, img_blur2, Size(0, 0), 1.8);
    Mat img_diff;
    absdiff(img_blur2, img_blur1, img_diff);
    namedWindow("Lena DoG", WINDOW_AUTOSIZE);
    imshow("Lena DoG", img_diff);

    waitKey(0); destroyAllWindows();

    minMaxLoc(img_diff, &min, &max);
    cout << "after diff" << min << " " << max << endl;
    normalize(img_diff, img_diff, 0, 255, NORM_MINMAX, CV_32F);

    minMaxLoc(img_diff, &min, &max);
    cout << "after norm" << min << " " << max << endl;

    imwrite("/home/marco/cs/repos/SIFT/Lena_diff.png", img_diff);
    return EXIT_SUCCESS;

    Params P = defineParams(img.size(), 4, 1.6);
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

    Pyramid dog = buildDoG(pyr, P);
    cout << "DoG Pyramid has " << dog.size() << " octaves" << endl;
    cout << "DoG Octave has " << dog[0].size() << " scales" << endl;
    namedWindow("DoG img0", WINDOW_NORMAL);
    imshow("DoG img0", dog[0][0]);
    namedWindow("DoG img4", WINDOW_NORMAL);
    imshow("DoG img4", dog[0][dog[0].size()-1]);
    waitKey(0); destroyAllWindows();

    return EXIT_SUCCESS;
}
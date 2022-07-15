#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "usage: main.out < ImagePath" << endl;
        return EXIT_FAILURE;
    }

    Mat img = imread(argv[1], 1);
    if (!img.data) {
        cout << "No image data!" << endl;
        return EXIT_FAILURE;
    }
    cvtColor(img, img, COLOR_BGR2GRAY);

    namedWindow("img color", WINDOW_NORMAL);
    imshow("img color", img);

    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            if (i == j)
                img.at<uchar>(i, j) = 255;
        }
    }

    namedWindow("img2", WINDOW_NORMAL);
    imshow("img2", img);
    waitKey(0); destroyAllWindows();

    // kernel
    int ksize = 256;
    Mat kernel = Mat::zeros(Size(ksize, ksize), CV_8U);
    kernel.at<uchar>(ksize/2, ksize/2) = 255;
    namedWindow("img", WINDOW_NORMAL);
    imshow("img", kernel);

    Mat gauss = getGaussianKernel(25, 3.5);
    Mat new_kernel;
    sepFilter2D(img, new_kernel, CV_8U, gauss, gauss.t());
    double min, max;
    minMaxLoc(new_kernel, &min, &max);
    printf("Gauss (%lf %lf)\n", min, max);

    namedWindow("img2", WINDOW_NORMAL);
    imshow("img2", new_kernel);
    waitKey(0); destroyAllWindows();

    return EXIT_SUCCESS;
}
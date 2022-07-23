// #include <stdio.h>
#include <iostream>
/* SIFT3D headers */
#include "immacros.h"
#include "imutil.h"
#include "sift.h"

using namespace std;

const string folder = "./out";
const string out_path = folder + "/1dicom";
const string keys_path = folder + "/1_keys.csv.gz";
const string desc_path = folder + "/1_desc.csv.gz";
const string draw_path = folder + "/1_keys.nii.gz";

bool feature(char *im_path) {
/* This illustrates how to use images within a function, and free all memory
* afterwards. */
	Image im, draw;
        Mat_rm keys;
	SIFT3D sift3d;
	Keypoint_store kp;
	SIFT3D_Descriptor_store desc;

    // Initialize the intermediates
    init_Keypoint_store(&kp);
    init_SIFT3D_Descriptor_store(&desc);
    init_im(&im);
    init_im(&draw);
    if (init_Mat_rm(&keys, 0, 0, SIFT3D_DOUBLE, SIFT3D_FALSE))
        return EXIT_FAILURE; 
    if (init_SIFT3D(&sift3d)) {
        cleanup_Mat_rm(&keys);
        return EXIT_FAILURE;
    }
    // Read the image
    if (im_read(im_path, &im))
        goto demo_quit;
    // Detect keypoints
    if (SIFT3D_detect_keypoints(&sift3d, &im, &kp))
        goto demo_quit;
    // Write the keypoints to a file
    if (write_Keypoint_store(keys_path.c_str(), &kp))
        goto demo_quit;
    cout << "Keypoints written to " << keys_path << endl;
    // Extract descriptors
    if (SIFT3D_extract_descriptors(&sift3d, &kp, &desc))
        goto demo_quit;
    // Write the descriptors to a file
    if (write_SIFT3D_Descriptor_store(desc_path.c_str(), &desc))
        goto demo_quit;
    cout << "Descriptors written to " << desc_path << endl;
    // Convert the keypoints to a matrix 
    if (Keypoint_store_to_Mat_rm(&kp, &keys))
        goto demo_quit;
    // Draw the keypoints
    if (draw_points(&keys, SIFT3D_IM_GET_DIMS(&im), 1, &draw))
        goto demo_quit;
    // Write the drawn keypoints to a file
    if (im_write(draw_path.c_str(), &draw))
        goto demo_quit;
    cout << "Keypoints drawn in " << draw_path << endl;

    // Clean up
    im_free(&im);
    im_free(&draw);
    cleanup_Mat_rm(&keys);
    cleanup_SIFT3D(&sift3d);
    cleanup_Keypoint_store(&kp);
    cleanup_SIFT3D_Descriptor_store(&desc);
    return EXIT_SUCCESS;

demo_quit:
    // Clean up and return an error
    im_free(&im);
    im_free(&draw);
    cleanup_Mat_rm(&keys);
    cleanup_SIFT3D(&sift3d);
    cleanup_Keypoint_store(&kp);
    cleanup_SIFT3D_Descriptor_store(&desc);
    return EXIT_FAILURE;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "Error: args[1] := input image, REQUIRED." << endl;
        return EXIT_FAILURE;
    }
    bool ret = feature(argv[1]);
    if (ret != 0) {
        fprintf(stderr, "Fatal demo error, code %d. \n", ret);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

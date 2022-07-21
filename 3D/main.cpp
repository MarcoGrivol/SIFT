// #include <stdio.h>
#include <iostream>
/* SIFT3D headers */
#include "immacros.h"
#include "imutil.h"
#include "sift.h"

using namespace std;

/* Example file paths */
// const char in_path[] = "1.nii.gz";
const char out_path[] = "./out/1dicom";
const char *keys_path = "./out/1_keys.csv.gz";
const char *desc_path = "./out/1_desc.csv.gz";
const char *draw_path = "./out/1_keys.nii.gz";

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
        return 1; 

    if (init_SIFT3D(&sift3d)) {
            cleanup_Mat_rm(&keys);
        return 1;
    }

    // Read the image
    if (im_read(im_path, &im))
        goto demo_quit;

    // Detect keypoints
if (SIFT3D_detect_keypoints(&sift3d, &im, &kp))
            goto demo_quit;

    // Write the keypoints to a file
    if (write_Keypoint_store(keys_path, &kp))
            goto demo_quit;
    printf("Keypoints written to %s. \n", keys_path);

    // Extract descriptors
    if (SIFT3D_extract_descriptors(&sift3d, &kp, &desc))
            goto demo_quit;

    // Write the descriptors to a file
    if (write_SIFT3D_Descriptor_store(desc_path, &desc))
            goto demo_quit;
    printf("Descriptors written to %s. \n", desc_path);

    // Convert the keypoints to a matrix 
    if (Keypoint_store_to_Mat_rm(&kp, &keys))
            goto demo_quit;

    // Draw the keypoints
    if (draw_points(&keys, SIFT3D_IM_GET_DIMS(&im), 1, &draw))
            goto demo_quit;

    // Write the drawn keypoints to a file
    if (im_write(draw_path, &draw))
            goto demo_quit;
    printf("Keypoints drawn in %s. \n", draw_path);

    // Clean up
    im_free(&im);
    im_free(&draw);
    cleanup_Mat_rm(&keys);
    cleanup_SIFT3D(&sift3d);
    cleanup_Keypoint_store(&kp);
    cleanup_SIFT3D_Descriptor_store(&desc);

    return 0;

demo_quit:
    // Clean up and return an error
    im_free(&im);
    im_free(&draw);
    cleanup_Mat_rm(&keys);
    cleanup_SIFT3D(&sift3d);
    cleanup_Keypoint_store(&kp);
    cleanup_SIFT3D_Descriptor_store(&desc);

    return 1;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "Error: args[1] := input image, REQUIRED." << endl;
        return EXIT_FAILURE;
    }

    int ret;

    // Do the demo
    ret = feature(argv[1]);

    // Check for errors
    if (ret != 0) {
            fprintf(stderr, "Fatal demo error, code %d. \n", ret);
            return 1;
    }

    return 0;
}

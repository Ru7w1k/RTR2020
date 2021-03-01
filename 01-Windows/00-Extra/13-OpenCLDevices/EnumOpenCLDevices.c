// headers
#include <stdio.h>
#include <stdlib.h>
#include <cl/opencl.h>

cl_int ret_ocl;
cl_int platform_count;
cl_platform_id *cl_platform_ids = NULL;
cl_platform_id oclPlatformID_required;

cl_int device_count;
cl_device_id *cl_device_ids = NULL;
cl_device_id oclDeviceID;
cl_device_type device_type;

char info[512];

int main()
{
    // get the number of supported platforms
    ret_ocl = clGetPlatformIDs(0, NULL, &platform_count);
    if (platform_count < 1)
    {
        printf("No OpenCL supported platform!\n");
        exit(EXIT_FAILURE);
    }

    // allocate memory according to the number of platforms
    cl_platform_ids = (cl_platform_id *) malloc(platform_count * sizeof(cl_platform_id));
    if (cl_platform_ids == NULL)
    {
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    // call clGetPlatformIDs() with proper platform count
    ret_ocl = clGetPlatformIDs(platform_count, cl_platform_ids, NULL);

    // enumerate platforms to get the devices
    for (int i = 0; i < platform_count; i++)
    {
        clGetPlatformInfo(cl_platform_ids[i], CL_PLATFORM_NAME, 512, info, NULL);
        printf("OpenCL Platform is %s\n", info);
    }

    free(cl_platform_ids);
    return(0);
}

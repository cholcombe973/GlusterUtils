/* 
 * File:   main.c
 * Author: chris
 *
 * Created on May 8, 2015, 12:39 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "api/glfs.h"
#include <string.h>
/*
 * 
 */
int mkdir(glfs_t *fs) {
    int ret = 0;

    ret = glfs_mkdir(fs, "", 0);

    if (ret != 0) {
        fprintf(stderr, "%s: (%d) %s\n", "", ret, strerror(errno));
    }
}
int get_usage(glfs_t *fs) {
    int ret = 0;
    //Read the extended attributes
    //ssize_t glfs_getxattr (glfs_t *fs, const char *path, const char *name,
    //                   void *value, size_t size) __THROW;
}
int main(int argc, char** argv) {
    int ret = 0;
    glfs_t *fs = NULL;
    glfs_fd_t *fd = NULL;

    if (argc != 4) {
        printf("Expected the following arguments\n\t%s <volname> <hostname> <username>\n", argv[0]);
        return (EXIT_FAILURE);
    }

    fs = glfs_new(argv[1]);
    if (!fs) {
        fprintf(stderr, "glfs_new: returned NULL\n");
        fprintf(stderr, "Unable to connect to the Gluster Cluster\n");
        return (EXIT_FAILURE);
    }
    ret = glfs_set_volfile_server(fs, "tcp", argv[2], 24007);
    ret = glfs_set_logging(fs, "/dev/stderr", 7);

    ret = glfs_init(fs);

    if (ret != 0) {
        fprintf(stderr, "glfs_init failed\n");
    }

    ret = get_usage(fs);

    if (ret != 0) {
        fprintf(stderr, "%s: (%d) %s\n", "", ret, strerror(errno));
    }

    ret = glfs_fini(fs);
    return (EXIT_SUCCESS);
}


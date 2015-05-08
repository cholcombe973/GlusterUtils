/* 
 * File:   main.c
 * Author: Chris Holcombe
 *
 * Created on May 8, 2015, 12:39 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "api/glfs.h"
#include <string.h>
#include <getopt.h>
#include <alloca.h>
#include <getopt.h>
#include <stdbool.h>

/*
 * Gets the usage information or creates a user folder for deep mounting
 * Gluster's quota translator is used to track this information.
 */

static struct option longopts[] = {
    {
        .name = "help",
        .has_arg = 0,
        .flag = NULL,
        .val = (int) 'h',
    },
    {
        .name = "all",
        .has_arg = 0,
        .flag = NULL,
        .val = (int) 'a',
    },
    {
        .name = "username",
        .has_arg = 1,
        .flag = NULL,
        .val = (int) 'u',
    },
    {
        .name = "volumename",
        .has_arg = 1,
        .flag = NULL,
        .val = (int) 'l',
    },
    {
        .name = "servername",
        .has_arg = 1,
        .flag = NULL,
        .val = (int) 's',
    },
    {
        .name = "create",
        .has_arg = 0,
        .flag = NULL,
        .val = (int) 'c',
    },
    {
        .name = "get",
        .has_arg = 0,
        .flag = NULL,
        .val = (int) 'g',
    },
    { 0, 0, 0, 0},
};

const char *user;
const char *volume;
const char *server;

bool create_user;
bool get_usage;
bool get_all_usage;

void printusage(FILE *file, const char *argv0) {
    fprintf(file, "usage: %s [-h] [-v]\n", argv0);
    fprintf(file, "\nrequired arguments:\n");
    fprintf(file, "  --username USERNAME\n");
    fprintf(file, "  --volumename VOLUME\n");
    fprintf(file, "  --host HOSTNAME\n");
    fprintf(file, "\noptional arguments:\n");
    fprintf(file, "  --create\n");
    fprintf(file, "  --get\n");
    fprintf(file, "  --all\n");
    fprintf(file, "  -h, --help\n");
    fprintf(file, "  -v, --verbose\n");
}

int parsecmdargs(int argc, const char **argv) {
    int opt;
    int ret;

    if (NULL == argv) {
        errno = EINVAL;
        return -1;
    }

    if (argc < 2) {
        errno = EINVAL;
        goto out;
    }

    while ((opt = getopt_long_only(argc, (char* const*) argv, "", longopts, NULL)) != -1) {
        switch (opt) {
            case 'u': /* user name */
                user = strdup(optarg);
                if (!user) {
                    fprintf(stderr,
                            "[!] failed to save username \"%s\"\n",
                            optarg);
                    errno = EINVAL;
                    goto out;
                }
                break;
            case 'l': /* volume name */
                volume = strdup(optarg);
                if (!volume) {
                    fprintf(stderr,
                            "[!] failed to save volumename \"%s\"\n",
                            optarg);
                    errno = EINVAL;
                    goto out;
                }
                break;
            case 's': /* volume name */
                server = strdup(optarg);
                if (!server) {
                    fprintf(stderr,
                            "[!] failed to save server \"%s\"\n",
                            optarg);
                    errno = EINVAL;
                    goto out;
                }
            case 'c': /* create the user */
                create_user = true;
            case 'g': /* get the user's usage */
                get_usage = true;
            case 'a': /* get the entire volume usage*/
                get_all_usage = true;
            case 'h': /* help */
                printusage(stdout, argv[0]);
                return 1;
            default:
                errno = EINVAL;
                goto out;
        }
    }
    return 0;
out:
    printusage(stderr, argv[0]);
    return -1;
}

int mkdir(glfs_t *fs, const char *path) {
    int ret = 0;

    ret = glfs_mkdir(fs, path, 0);

    if (ret != 0) {
        fprintf(stderr, "%s: (%d) %s\n", "", ret, strerror(errno));
    }
}

int get_usage(glfs_t *fs, const char *user) {
    int ret = 0;
    //Read the extended attributes
    //ssize_t glfs_getxattr (fs, const char *path, const char *name,
    //                   void *value, size_t size) __THROW;
}

int main(int argc, char** argv) {
    int ret = 0;
    glfs_t *fs = NULL;
    glfs_fd_t *fd = NULL;

    parsecmdargs(argc, argv);
    /*
    if (argc != 4) {
        printf("Expected the following arguments\n\t%s <volname> <hostname> <username>\n", argv[0]);
        return (EXIT_FAILURE);
    }
     */

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

cleanup:
    free(user);
    free(volume);
    free(server);
    glfs_fini(fs); //finalize the Gluster handle

    return (EXIT_SUCCESS);
}
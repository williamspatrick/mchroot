#define _GNU_SOURCE

#include <sched.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>

void print_usage();
void do_mounts(const char* path);

int main(int argc, char** argv, char** envp)
{
    int rc = 0;

    if (0 != geteuid())
    {
        printf("ERROR: Must be ran setuid.\n");
        exit(-1);
    }
    if (0 == getuid())
    {
        printf("ERROR: Should not be ran as root.\n");
        exit(-1);
    }

    if (3 > argc)
    {
        printf("ERROR: Insufficient arguments.\n");
        print_usage();
        exit(-1);
    }

    rc = unshare(CLONE_NEWNS);
    if (0 != rc)
    {
        printf("ERROR: unshare call failed with %s\n", strerror(errno));
        exit(-1);
    }

    do_mounts(argv[1]);

    rc = chroot(argv[1]);
    if (0 != rc)
    {
        printf("ERROR: chroot call failed with %s\n", strerror(errno));
        exit(-1);
    }

    rc = chdir(get_current_dir_name());
    if (0 != rc)
    {
        printf("ERROR: chdir call failed with %s\n", strerror(errno));
        exit(-1);
    }

    rc = setuid(getuid());
    if (0 != rc)
    {
        printf("ERROR: setuid call failed with %s\n", strerror(errno));
    }

    rc = execve(argv[2], &argv[2], envp);
    if (0 != rc)
    {
        printf("ERROR: execve call failed with %s\n", strerror(errno));
    }

    return 0;
}

void print_usage()
{
    printf("mchroot chroot_path cmd args\n");
    printf("    Executes a command under a mounted chroot environment.\n");
}

void do_mounts(const char* path)
{
    int rc = 0;

    char* configFile = (char*) malloc(PATH_MAX);
    strncpy(configFile, path, PATH_MAX);
    strncat(configFile, "/.mchroot.cfg", PATH_MAX-1);

    if(PATH_MAX <= strnlen(configFile, PATH_MAX))
    {
        printf("ERROR: path length too long.\n");
        exit(-1);
    }

    int file = open(configFile, O_RDONLY | O_NOFOLLOW);
    if (-1 == file)
    {
        printf("ERROR: Failed to open %s\n", configFile);
        exit(-1);
    }

    char* data = (char*) malloc(4096);
    memset(data, '\0', 4096);
    ssize_t size = read(file, data, 4096);

    if (size >= 4096)
    {
        printf("ERROR: Config file %s is much too big.\n", configFile);
        exit(-1);
    }

    do
    {
        data = strtok(data, " \t\n");

        if (NULL != data)
        {
            if (strlen(data))
            {
                char* sub_path = (char*) malloc(PATH_MAX);

                strncpy(sub_path, path, PATH_MAX);
                strncat(sub_path, data, PATH_MAX-1);

                int rc = mkdir(sub_path, 0777);
                if (rc != 0)
                {
                    if ((errno != EEXIST) &&
                        (errno != EACCES) &&
                        (errno != EPERM) &&
                        (errno != EROFS))
                    {
                        printf("ERROR: Creating %s : %s\n", sub_path,
                               strerror(errno));
                        exit(-1);
                    }
                }

                rc = mount(data, sub_path, "", MS_BIND|MS_REC, "");
                if (rc != 0)
                {
                    printf("ERROR: Mounting %s : %s\n", data,
                           strerror(errno));
                    exit(-1);
                }

                free(sub_path);
            }

            data = &data[strlen(data)+1];
        }


    } while (data != NULL);

    return;
}

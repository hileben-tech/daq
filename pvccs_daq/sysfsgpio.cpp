#include "sysfsgpio.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>

//static
const char *SysfsGpio::SYSFS_GPIO_DIR = "/sys/class/gpio";

//static
const int SysfsGpio::MAX_BUF = 40;

SysfsGpio::SysfsGpio(unsigned int io)
{
    gpio = io;

    exportGpio();

    assert(isExported());
}

SysfsGpio::~SysfsGpio()
{
    unexportGpio();
}

int SysfsGpio::setDirection(char *direct)
{
    assert((strcmp(direct, "in") == 0) || (strcmp(direct, "out") == 0));

    char buf[MAX_BUF] = {0, };
    snprintf(buf, sizeof(buf), "%s/gpio%d/direction", SYSFS_GPIO_DIR, gpio);

    int fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio/set-direction");

        return fd;
    }

    int len = write(fd, direct, strlen(direct) + 1);
    if (len < 0) {
        perror("gpio/set-direction --");

        return len;
    }

    close(fd);

    return 0;
}

int SysfsGpio::getDirection(char *direct)
{
    char buf[MAX_BUF] = {0, };
    snprintf(buf, sizeof(buf), "%s/gpio%d/direction", SYSFS_GPIO_DIR, gpio);

    int fd = open(buf, O_RDONLY);
    if (fd < 0) {
        perror("gpio/get-direction");

        return fd;
    }

    bzero(buf, sizeof(buf));
    int len = read(fd, &buf, MAX_BUF);
    if (len < 0) {
        perror("gpio/get-direction --");

        return len;
    }

    strcpy(direct, buf);

    close(fd);

    return 0;
}

int SysfsGpio::setValue(char *value)
{
    assert((strcmp(value, "0") == 0) || (strcmp(value, "1") == 0));

    char buf[MAX_BUF] =  {0, };
    snprintf(buf, sizeof(buf), "%s/gpio%d/value", SYSFS_GPIO_DIR, gpio);

    int fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio/set-value");

        return fd;
    }

    int len = write(fd, value, strlen(value) + 1);
    if (len < 0) {
        perror("gpio/set-value --");

        return len;
    }

    close(fd);

    return 0;
}

int SysfsGpio::getValue(char *value)
{
    char buf[MAX_BUF] = {0, };
    snprintf(buf, sizeof(buf), "%s/gpio%d/value", SYSFS_GPIO_DIR, gpio);

    int fd = open(buf, O_RDONLY);
    if (fd < 0) {
        perror("gpio/get-value");

        return fd;
    }

    bzero(buf, sizeof(buf));
    int len = read(fd, &buf, MAX_BUF);
    if (len < 0) {
        perror("gpio/get-value --");

        return len;
    }

    strcpy(value, buf);

    close(fd);

    return 0;
}

int SysfsGpio::setEdge(char *edge)
{
    assert((strcmp(edge, "none") == 0) || (strcmp(edge, "falling") == 0) || (strcmp(edge, "rising") == 0) || (strcmp(edge, "both") == 0));

    char buf[MAX_BUF] = {0, };
    snprintf(buf, sizeof(buf), "%s/gpio%d/edge", SYSFS_GPIO_DIR, gpio);

    int fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio/set-edge");

        return fd;
    }

    int len = write(fd, edge, strlen(edge) + 1);
    if (len < 0) {
        perror("gpio/set-edge --");

        return len;
    }

    close(fd);

    return 0;
}

int SysfsGpio::getEdge(char *edge)
{
    char buf[MAX_BUF] = {0, };
    snprintf(buf, sizeof(buf), "%s/gpio%d/edge", SYSFS_GPIO_DIR, gpio);

    int fd = open(buf, O_RDONLY);
    if (fd < 0) {
        perror("gpio/get-edge");

        return fd;
    }

    bzero(buf, sizeof(buf));
    int len = read(fd, &buf, MAX_BUF);
    if (len < 0) {
        perror("gpio/get-edge --");

        return len;
    }

    strcpy(edge, buf);

    close(fd);

    return 0;
}

int SysfsGpio::openValueIO()
{
    char buf[MAX_BUF] = {0, };
    snprintf(buf, sizeof(buf), "%s/gpio%d/value", SYSFS_GPIO_DIR, gpio);

    int fd = open(buf, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("gpio/open-value-io");
    }

    return fd;
}

//static
int SysfsGpio::closeIO(int io)
{
    return close(io);
}

int SysfsGpio::exportGpio()
{
    char buf[MAX_BUF] = {0, };
    snprintf(buf, sizeof(buf), "%s/export", SYSFS_GPIO_DIR);

    int fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio/export");

        return fd;
    }

    bzero(buf, sizeof(buf));
    int len = snprintf(buf, sizeof(buf), "%d", gpio);

    //printf("[DEBUG] gpio %s\n", buf);

    len = write(fd, buf, len);
    if (len < 0) {
        perror("gpio/export --");

        return len;
    }

    close(fd);

    return 0;
}

int SysfsGpio::unexportGpio()
{
    char buf[MAX_BUF] = {0, };
    snprintf(buf, sizeof(buf), "%s/unexport", SYSFS_GPIO_DIR);

    int fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio/unexport");

        return fd;
    }

    bzero(buf, sizeof(buf));
    int len = snprintf(buf, sizeof(buf), "%d", gpio);

    len = write(fd, buf, len);
    if (len < 0) {
        perror("gpio/unexport --");

        return len;
    }

    close(fd);

    return 0;
}

bool SysfsGpio::isExported()
{
    char buf[MAX_BUF] = {0, };
    snprintf(buf, sizeof(buf), "%s/gpio%d", SYSFS_GPIO_DIR, gpio);

    struct stat st;
    int result = stat(buf, &st);
    if (result < 0) {
        perror("gpio/is-exported");

        return false;
    }

    if ((st.st_mode & S_IFDIR) == 0) {
        perror("gpio/is-exported --");

        return false;
    }

    return true;
}

#ifndef SYSFSGPIO_H
#define SYSFSGPIO_H

class SysfsGpio
{
    static const char *SYSFS_GPIO_DIR;
    static const int MAX_BUF;

public:
    explicit SysfsGpio(unsigned int io);
    ~SysfsGpio();

    int setDirection(char *direct);
    int getDirection(char *direct);
    int setValue(char *value);
    int getValue(char *value);
    int setEdge(char *edge);
    int getEdge(char *edge);

    int openValueIO();

    static int closeIO(int io);

private:
    unsigned int gpio;

    int exportGpio();
    int unexportGpio();
    bool isExported();
};

#endif // SYSFSGPIO_H

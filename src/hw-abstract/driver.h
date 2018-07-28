
#ifndef __DRIVER_H_
#define __DRIVER_H_

class Driver
{
public:
    Driver();
    virtual ~Driver();

    virtual void activate();
    virtual void deactivate();
    virtual int reset();
};

class DriverManager
{
public:
    DriverManager();

    void appendDriver(Driver *);

    void activateAll();

private:
    Driver *drivers[255];
    int driverCount;
};

#endif // __DRIVER_H_
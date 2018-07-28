
#include "driver.h"

Driver::Driver()
{

}

void Driver::activate()
{

}

void Driver::deactivate()
{

}

int Driver::reset()
{
    return 0;
}

DriverManager::DriverManager()
    : driverCount(0)
{
}

void DriverManager::appendDriver(Driver *driver)
{
    drivers[driverCount++] = driver;
}

void DriverManager::activateAll()
{
    for (int i(0); i != driverCount; ++i)
        drivers[i]->activate();
}
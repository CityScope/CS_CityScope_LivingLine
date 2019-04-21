#pragma once
#include <string>

/*

Thomas Sanchez Lengeling
March, 2019

Living Line

*/

class SystemVars{
public:

    static SystemVars& getInstance()
    {
        static SystemVars    instance;
        // Instantiated on first use.
        return instance;
    }

    //add here config parameters
    int width;
    int height;
    std::string projectName;

private:

    //default values
    SystemVars() {
        width  = 1920;
        height =  1080;
        projectName = "";
    }

    SystemVars(SystemVars const&) = delete;
    void operator=(SystemVars const&) = delete;

};

#pragma once

class IConfig {
    protected:
        IConfig() = default;
    
    public:
        virtual ~IConfig() = default;

        virtual int readParam(const char param[100], float &val) = 0;
        virtual int writeParam(const char param[100], float newValue) = 0;
};


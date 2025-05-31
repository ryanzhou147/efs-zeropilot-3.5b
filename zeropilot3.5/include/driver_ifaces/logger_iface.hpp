#pragma once

class ILogger {
    protected:
        ILogger() = default;
    
    public:
        virtual ~ILogger() = default;

        virtual int log(const char message[100]) = 0;
        virtual int log(const char message[][100], int count) = 0;
        virtual int read(char *valueBuf, size_t bufSize, const char *key) = 0;
};

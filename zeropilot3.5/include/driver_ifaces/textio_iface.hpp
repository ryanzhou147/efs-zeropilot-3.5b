#pragma once

class ITextIO {
    protected:
        ITextIO() = default;

    public:
        ~ITextIO() = default;

        virtual int open(const char *filename) = 0;
        virtual int close() = 0;
        virtual char* read(char *buffer, size_t bufferSize) = 0;
        virtual int write(const char *buffer) = 0;
        virtual int seek(int offset) = 0;
}
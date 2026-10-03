#ifndef PAGE_H
#define PAGE_H

class Page
{
    int pageNumber;
    int frameNumber;
    bool present;
    bool referenced;
    bool dirty;
    unsigned long long lastAccess;

public:
    explicit Page(int number=0):
        pageNumber(number),
        frameNumber(-1),
        present(false),
        referenced(false),
        dirty(false),
        lastAccess(0)
    {
    }

    int getPageNumber() const
    {
        return pageNumber;
    }

    int getFrameNumber() const
    {
        return frameNumber;
    }

    bool isPresent() const
    {
        return present;
    }

    bool isReferenced() const
    {
        return referenced;
    }

    bool isDirty() const
    {
        return dirty;
    }

    unsigned long long getLastAccess() const
    {
        return lastAccess;
    }

    void load(int frame,unsigned long long time)
    {
        frameNumber=frame;
        present=true;
        referenced=true;
        lastAccess=time;
    }

    void access(unsigned long long time)
    {
        referenced=true;
        lastAccess=time;
    }

    void write(unsigned long long time)
    {
        referenced=true;
        dirty=true;
        lastAccess=time;
    }

    void unload()
    {
        frameNumber=-1;
        present=false;
        referenced=false;
        dirty=false;
        lastAccess=0;
    }

    void clearReference()
    {
        referenced=false;
    }
};

#endif
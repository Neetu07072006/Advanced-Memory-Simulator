#ifndef PAGINGMANAGER_H
#define PAGINGMANAGER_H

#include <vector>
#include <string>
#include "../memory/PhysicalFrameManager.h"
#include "PageTable.h"

class PagingManager
{
    PageTable pageTable;
    PhysicalFrameManager* frameManager;
    int processId;
    unsigned long long clock;

public:
    PagingManager(
        int numberOfPages=0,
        PhysicalFrameManager* manager=nullptr,
        int pid=-1
    ):
        pageTable(numberOfPages),
        frameManager(manager),
        processId(pid),
        clock(0)
    {
    }

    void attachFrameManager(
        PhysicalFrameManager* manager,
        int pid
    )
    {
        frameManager=manager;
        processId=pid;
    }

    void resize(int numberOfPages)
    {
        releaseAll();
        pageTable.resize(numberOfPages);
        clock=0;
    }

    int getNumberOfPages() const
    {
        return pageTable.size();
    }

    int getResidentPageCount() const
    {
        return pageTable.getResidentPageCount();
    }

    int getNonResidentPageCount() const
    {
        return pageTable.getNonResidentPageCount();
    }

    bool isPagePresent(int pageNumber) const
    {
        return pageTable.isPresent(pageNumber);
    }

    int getFrameNumber(int pageNumber) const
    {
        return pageTable.getFrameNumber(pageNumber);
    }

    int loadPage(int pageNumber)
    {
        if(
            pageNumber<0 ||
            pageNumber>=pageTable.size()
        )
            return -1;

        if(pageTable.isPresent(pageNumber))
        {
            pageTable.accessPage(
                pageNumber,
                ++clock
            );

            return pageTable.getFrameNumber(
                pageNumber
            );
        }

        if(!frameManager)
            return -1;

        int frame=
            frameManager->allocateFrame(
                processId,
                pageNumber
            );

        if(frame==-1)
            return -1;

        pageTable.loadPage(
            pageNumber,
            frame,
            ++clock
        );

        return frame;
    }

    bool accessPage(int pageNumber)
    {
        if(
            pageNumber<0 ||
            pageNumber>=pageTable.size()
        )
            return false;

        if(!pageTable.isPresent(pageNumber))
        {
            if(loadPage(pageNumber)==-1)
                return false;
        }
        else
        {
            pageTable.accessPage(
                pageNumber,
                ++clock
            );

            int frame=
                pageTable.getFrameNumber(
                    pageNumber
                );

            if(frameManager)
                frameManager->touchFrame(frame);
        }

        return true;
    }

    bool writePage(int pageNumber)
    {
        if(
            pageNumber<0 ||
            pageNumber>=pageTable.size()
        )
            return false;

        if(!pageTable.isPresent(pageNumber))
        {
            if(loadPage(pageNumber)==-1)
                return false;
        }

        pageTable.writePage(
            pageNumber,
            ++clock
        );

        int frame=
            pageTable.getFrameNumber(
                pageNumber
            );

        if(frameManager)
            frameManager->touchFrame(frame);

        return true;
    }

    bool unloadPage(int pageNumber)
    {
        if(
            pageNumber<0 ||
            pageNumber>=pageTable.size()
        )
            return false;

        if(!pageTable.isPresent(pageNumber))
            return false;

        int frame=
            pageTable.getFrameNumber(
                pageNumber
            );

        if(frameManager)
            frameManager->releaseFrame(frame);

        pageTable.unloadPage(pageNumber);

        return true;
    }

    void releaseAll()
    {
        if(frameManager)
        {
            const auto frames=
                pageTable.getFrameNumbers();

            for(int frame:frames)
                frameManager->releaseFrame(frame);
        }

        for(int page=0;page<pageTable.size();++page)
            pageTable.unloadPage(page);
    }

    int findFreeGlobalFrame() const
    {
        if(!frameManager)
            return -1;

        const auto& frames=
            frameManager->getFrames();

        for(const auto& frame:frames)
        {
            if(!frame.occupied)
                return frame.frameNumber;
        }

        return -1;
    }

    const PageTable& getPageTable() const
    {
        return pageTable;
    }

    PageTable& getPageTable()
    {
        return pageTable;
    }

    std::vector<int> getResidentPages() const
    {
        return pageTable.getResidentPages();
    }

    std::vector<int> getNonResidentPages() const
    {
        return pageTable.getNonResidentPages();
    }

    std::vector<int> getFrameNumbers() const
    {
        return pageTable.getFrameNumbers();
    }

    int getProcessId() const
    {
        return processId;
    }

    unsigned long long getClock() const
    {
        return clock;
    }

    void clearReferences()
    {
        pageTable.clearReferences();
    }

    void display() const
    {
        pageTable.display();
    }
};

#endif
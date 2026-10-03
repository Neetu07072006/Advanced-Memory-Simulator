#ifndef PHYSICALFRAMEMANAGER_H
#define PHYSICALFRAMEMANAGER_H

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

struct PhysicalFrame
{
    int frameNumber;
    int processId;
    int pageNumber;
    bool occupied;
    unsigned long long lastUsed;
    unsigned long long loadTime;

    PhysicalFrame(
        int number=0,
        int pid=-1,
        int page=-1,
        bool used=false
    ):
        frameNumber(number),
        processId(pid),
        pageNumber(page),
        occupied(used),
        lastUsed(0),
        loadTime(0)
    {
    }
};

class PhysicalFrameManager
{
    int totalFrames;
    int usedFrames;
    unsigned long long clock;
    std::vector<PhysicalFrame> frames;

public:
    explicit PhysicalFrameManager(
        int memoryKB=1000,
        int pageSizeKB=4
    ):
        totalFrames(
            memoryKB>0 && pageSizeKB>0
                ? memoryKB/pageSizeKB
                : 1
        ),
        usedFrames(0),
        clock(0),
        frames()
    {
        if(totalFrames<=0)
            totalFrames=1;

        frames.reserve(totalFrames);

        for(int i=0;i<totalFrames;++i)
            frames.emplace_back(i);
    }

    void reset()
    {
        usedFrames=0;
        clock=0;

        for(auto& frame:frames)
        {
            frame.processId=-1;
            frame.pageNumber=-1;
            frame.occupied=false;
            frame.lastUsed=0;
            frame.loadTime=0;
        }
    }

    int allocateFrame(
        int processId,
        int pageNumber
    )
    {
        for(auto& frame:frames)
        {
            if(!frame.occupied)
            {
                frame.occupied=true;
                frame.processId=processId;
                frame.pageNumber=pageNumber;
                frame.lastUsed=++clock;
                frame.loadTime=clock;
                ++usedFrames;
                return frame.frameNumber;
            }
        }

        return -1;
    }

    bool releaseFrame(int frameNumber)
    {
        if(
            frameNumber<0 ||
            frameNumber>=totalFrames
        )
            return false;

        auto& frame=frames[frameNumber];

        if(!frame.occupied)
            return false;

        frame.occupied=false;
        frame.processId=-1;
        frame.pageNumber=-1;
        frame.lastUsed=0;
        frame.loadTime=0;

        if(usedFrames>0)
            --usedFrames;

        return true;
    }

    int findFrame(
        int processId,
        int pageNumber
    ) const
    {
        for(const auto& frame:frames)
        {
            if(
                frame.occupied &&
                frame.processId==processId &&
                frame.pageNumber==pageNumber
            )
                return frame.frameNumber;
        }

        return -1;
    }

    std::vector<int> getProcessFrames(
        int processId
    ) const
    {
        std::vector<int> result;

        for(const auto& frame:frames)
        {
            if(
                frame.occupied &&
                frame.processId==processId
            )
                result.push_back(
                    frame.frameNumber
                );
        }

        return result;
    }

    int getUsedFrames() const
    {
        return usedFrames;
    }

    int getFreeFrames() const
    {
        return totalFrames-usedFrames;
    }

    int getTotalFrames() const
    {
        return totalFrames;
    }

    double getUtilization() const
    {
        return totalFrames>0
            ? 100.0*usedFrames/totalFrames
            : 0.0;
    }

    const std::vector<PhysicalFrame>&
    getFrames() const
    {
        return frames;
    }

    void touchFrame(int frameNumber)
    {
        if(
            frameNumber<0 ||
            frameNumber>=totalFrames
        )
            return;

        if(frames[frameNumber].occupied)
            frames[frameNumber].lastUsed=++clock;
    }

    int findLRUVictim() const
    {
        int victim=-1;
        unsigned long long oldest=0;

        for(const auto& frame:frames)
        {
            if(!frame.occupied)
                continue;

            if(
                victim==-1 ||
                frame.lastUsed<oldest
            )
            {
                victim=frame.frameNumber;
                oldest=frame.lastUsed;
            }
        }

        return victim;
    }

    int findFIFOVictim() const
    {
        int victim=-1;
        unsigned long long oldest=0;

        for(const auto& frame:frames)
        {
            if(!frame.occupied)
                continue;

            if(
                victim==-1 ||
                frame.loadTime<oldest
            )
            {
                victim=frame.frameNumber;
                oldest=frame.loadTime;
            }
        }

        return victim;
    }

    void display() const
    {
        std::cout
            << "\n====================================================\n"
            << "GLOBAL PHYSICAL FRAME MANAGER\n"
            << "====================================================\n";

        std::cout
            << std::left
            << std::setw(10) << "Frame"
            << std::setw(12) << "PID"
            << std::setw(12) << "Page"
            << std::setw(14) << "Status"
            << std::setw(14) << "Last Used"
            << std::setw(14) << "Load Time"
            << "\n";

        std::cout
            << "----------------------------------------------------\n";

        for(const auto& frame:frames)
        {
            std::cout
                << std::left
                << std::setw(10)
                << frame.frameNumber
                << std::setw(12)
                << (
                    frame.occupied
                        ? std::to_string(frame.processId)
                        : "-"
                )
                << std::setw(12)
                << (
                    frame.occupied
                        ? std::to_string(frame.pageNumber)
                        : "-"
                )
                << std::setw(14)
                << (
                    frame.occupied
                        ? "OCCUPIED"
                        : "FREE"
                )
                << std::setw(14)
                << frame.lastUsed
                << std::setw(14)
                << frame.loadTime
                << "\n";
        }

        std::cout
            << "----------------------------------------------------\n"
            << "Total Frames : "
            << totalFrames
            << "\n"
            << "Used Frames  : "
            << usedFrames
            << "\n"
            << "Free Frames  : "
            << getFreeFrames()
            << "\n"
            << "Utilization  : "
            << std::fixed
            << std::setprecision(2)
            << getUtilization()
            << "%\n"
            << "====================================================\n";
    }
};

#endif
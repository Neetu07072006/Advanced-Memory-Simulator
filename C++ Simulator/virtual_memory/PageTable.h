#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <vector>
#include <iostream>
#include <iomanip>
#include "Page.h"

class PageTable
{
    std::vector<Page> pages;

public:
    explicit PageTable(int numberOfPages=0)
    {
        resize(numberOfPages);
    }

    void resize(int numberOfPages)
    {
        if(numberOfPages<0)
            numberOfPages=0;

        pages.clear();
        pages.reserve(numberOfPages);

        for(int i=0;i<numberOfPages;++i)
            pages.emplace_back(i);
    }

    int size() const
    {
        return static_cast<int>(pages.size());
    }

    bool empty() const
    {
        return pages.empty();
    }

    Page& getPage(int pageNumber)
    {
        return pages.at(pageNumber);
    }

    const Page& getPage(int pageNumber) const
    {
        return pages.at(pageNumber);
    }

    bool isPresent(int pageNumber) const
    {
        if(pageNumber<0||pageNumber>=size())
            return false;

        return pages[pageNumber].isPresent();
    }

    int getFrameNumber(int pageNumber) const
    {
        if(pageNumber<0||pageNumber>=size())
            return -1;

        return pages[pageNumber].getFrameNumber();
    }

    void loadPage(
        int pageNumber,
        int frameNumber,
        unsigned long long time
    )
    {
        if(pageNumber<0||pageNumber>=size())
            return;

        pages[pageNumber].load(
            frameNumber,
            time
        );
    }

    void accessPage(
        int pageNumber,
        unsigned long long time
    )
    {
        if(pageNumber<0||pageNumber>=size())
            return;

        pages[pageNumber].access(time);
    }

    void writePage(
        int pageNumber,
        unsigned long long time
    )
    {
        if(pageNumber<0||pageNumber>=size())
            return;

        pages[pageNumber].write(time);
    }

    void unloadPage(int pageNumber)
    {
        if(pageNumber<0||pageNumber>=size())
            return;

        pages[pageNumber].unload();
    }

    int getResidentPageCount() const
    {
        int count=0;

        for(const auto& page:pages)
            if(page.isPresent())
                ++count;

        return count;
    }

    int getNonResidentPageCount() const
    {
        return size()-getResidentPageCount();
    }

    std::vector<int> getResidentPages() const
    {
        std::vector<int> result;

        for(const auto& page:pages)
        {
            if(page.isPresent())
                result.push_back(
                    page.getPageNumber()
                );
        }

        return result;
    }

    std::vector<int> getNonResidentPages() const
    {
        std::vector<int> result;

        for(const auto& page:pages)
        {
            if(!page.isPresent())
                result.push_back(
                    page.getPageNumber()
                );
        }

        return result;
    }

    std::vector<int> getFrameNumbers() const
    {
        std::vector<int> result;

        for(const auto& page:pages)
        {
            if(page.isPresent())
                result.push_back(
                    page.getFrameNumber()
                );
        }

        return result;
    }

    const std::vector<Page>& getPages() const
    {
        return pages;
    }

    void clearReferences()
    {
        for(auto& page:pages)
            page.clearReference();
    }

    void display() const
    {
        std::cout
            << "\n====================================================\n"
            << "PAGE TABLE\n"
            << "====================================================\n"
            << std::left
            << std::setw(10) << "Page"
            << std::setw(12) << "Frame"
            << std::setw(15) << "Present"
            << std::setw(15) << "Referenced"
            << std::setw(12) << "Dirty"
            << std::setw(15) << "Last Access"
            << "\n";

        std::cout
            << "----------------------------------------------------\n";

        for(const auto& page:pages)
        {
            std::cout
                << std::left
                << std::setw(10)
                << page.getPageNumber()
                << std::setw(12)
                << (
                    page.isPresent()
                        ? std::to_string(page.getFrameNumber())
                        : "-"
                )
                << std::setw(15)
                << (
                    page.isPresent()
                        ? "YES"
                        : "NO"
                )
                << std::setw(15)
                << (
                    page.isReferenced()
                        ? "YES"
                        : "NO"
                )
                << std::setw(12)
                << (
                    page.isDirty()
                        ? "YES"
                        : "NO"
                )
                << std::setw(15)
                << page.getLastAccess()
                << "\n";
        }

        std::cout
            << "----------------------------------------------------\n"
            << "Total Pages      : "
            << size()
            << "\n"
            << "Resident Pages   : "
            << getResidentPageCount()
            << "\n"
            << "Non-Resident     : "
            << getNonResidentPageCount()
            << "\n"
            << "====================================================\n";
    }
};

#endif
#ifndef RENDER_TILE_JOB_HPP
#define RENDER_TILE_JOB_HPP

#include "Render/Job.hpp"
#include "Render/Framebuffer.hpp"

#include <mutex>
#include <condition_variable>

namespace Render {
    class TileJob : public Job
    {
    public:
        TileJob(Framebuffer &framebuffer);

        std::unique_ptr<Job::Task> getNextTask() override;
        void stop() override;

    protected:
        virtual void renderPixel(unsigned int x, unsigned int y, Job::ThreadLocal &threadLocal) = 0;
        virtual bool needRepeat();
        Framebuffer &framebuffer();

    private:
        void taskDone();

        Framebuffer &mFramebuffer;
        unsigned int mWidthInTiles;
        unsigned int mHeightInTiles;
        unsigned int mNextTile;
        unsigned int mOutstandingTasks;
        std::mutex mMutex;
        std::condition_variable mCondVar;
        bool mStop;
    };
}
#endif

#define NOMINMAX
#include "Render/Engine.hpp"

#include "Render/RenderJob.hpp"

#include "Lighter/UniPath.hpp"

#include <algorithm>
#include <memory>

namespace Render {
    Engine::Engine(const Object::Scene &scene)
        : mScene(scene)
    {
        mRendering = false;
        mRenderFramebuffer = std::make_unique<Framebuffer>(0, 0);
        mSampleStatusFramebuffer = std::make_unique<Framebuffer>(0, 0);
    }

    Engine::~Engine()
    {
        stop();
    }

    void Engine::stop()
    {
        {
            std::unique_lock<std::mutex> lock(mMutex);
            mStopThreads = true;
            if (mCurrentJob) {
                mCurrentJob->stop();
            }
            mConditionVariable.notify_all();
        }

        for (std::unique_ptr<std::thread> &thread : mThreads) {
            thread->join();
        }
        mThreads.clear();
        mCurrentJob.reset();
        mJobs.clear();

        if(mRendering) {
            renderDone();
        }
    }

    const Object::Scene &Engine::scene() const
    {
        return mScene;
    }

    void Engine::startRender(Listener *listener)
    {
        mListener = listener;
        mRendering = true;
        mListener->onRenderStatus("");

        mStartTime = GetTickCount();

        if (mSettings.lighting) {
            mLighter = std::make_unique<Lighter::UniPath>(mSettings.lighterSettings);
            std::vector<std::unique_ptr<Job>> prerenderJobs = mLighter->createPrerenderJobs(mScene, *mRenderFramebuffer);
            for (std::unique_ptr<Job> &job : prerenderJobs) {
                addJob(std::move(job));
            }
        }

        std::unique_ptr<Job> renderJob = std::make_unique<RenderJob>(mScene, mSettings, *mLighter, *mRenderFramebuffer, *mSampleStatusFramebuffer);
        renderJob->setDoneHandler([=]() { renderDone(); } );
        addJob(std::move(renderJob));

        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        unsigned int numThreads = static_cast<unsigned int>(sysinfo.dwNumberOfProcessors);

        mNumRunningThreads = 0;
        mStopThreads = false;
        for (unsigned int i = 0; i < numThreads; i++) {
            mThreads.push_back(std::make_unique<std::thread>([=]() { runThread(); }));
        }
    }

    bool Engine::rendering() const
    {
        return mRendering;
    }

    const Settings &Engine::settings() const
    {
        return mSettings;
    }

    void Engine::setSettings(const Settings &settings)
    {
        if (settings.width != mSettings.width || settings.height != mSettings.height) {
            mRenderFramebuffer = std::make_unique<Framebuffer>(settings.width, settings.height);
            mSampleStatusFramebuffer = std::make_unique<Framebuffer>(settings.width, settings.height);
        }
        mSettings = settings;
    }

    Framebuffer &Engine::renderFramebuffer()
    {
        return *mRenderFramebuffer;
    }

    Framebuffer &Engine::sampleStatusFramebuffer()
    {
        return *mSampleStatusFramebuffer;
    }

    Object::Color Engine::toneMap(const Object::Radiance &radiance)
    {
        float red = radiance.red() / (radiance.red() + 1);
        float green = radiance.green() / (radiance.green() + 1);
        float blue = radiance.blue() / (radiance.blue() + 1);

        return Object::Color(red, green, blue);
    }

    void Engine::addJob(std::unique_ptr<Job> job)
    {
        std::unique_lock<std::mutex> lock(mMutex);

        if (!mCurrentJob && mNumRunningThreads == 0) {
            mCurrentJob = std::move(job);
            mConditionVariable.notify_all();
        }
        else {
            mJobs.push_back(std::move(job));
        }
    }

    void Engine::runThread()
    {
        while (true) {
            std::unique_lock<std::mutex> lock(mMutex);
            if (mStopThreads) {
                return;
            }

            mNumRunningThreads++;

            bool block = false;

            if (mCurrentJob) {
                std::unique_ptr<Job::ThreadLocal> threadLocal = mCurrentJob->createThreadLocal();

                while (true) {
                    if (mStopThreads) {
                        return;
                    }
                    std::unique_ptr<Job::Task> task = mCurrentJob->getNextTask();

                    if (!task) {
                        block = true;
                        break;
                    }

                    lock.unlock();
                    (*task)(*threadLocal);
                    lock.lock();
                }
            }

            mNumRunningThreads--;

            if (mNumRunningThreads == 0) {
                if (mCurrentJob) {
                    mCurrentJob->done();
                    mCurrentJob.reset();
                }

                if (mJobs.size() > 0) {
                    mCurrentJob = std::move(mJobs.front());
                    mJobs.pop_front();
                    mConditionVariable.notify_all();
                    block = false;
                }
                else {
                    block = true;
                }
            }

            if (block && !mStopThreads) {
                mConditionVariable.wait(lock);
            }
        }
    }

    void Engine::renderDone()
    {
        DWORD endTime = GetTickCount();
        char buf[256];
        double seconds = static_cast<float>((endTime - mStartTime) / 1000.0f);
        int hours = static_cast<unsigned int>(seconds) / 3600;
        seconds -= hours * 3600;
        int minutes = static_cast<unsigned int>(seconds) / 60;
        seconds -= minutes * 60;
        if (hours > 0) {
            sprintf_s(buf, sizeof(buf), "Render time: %ih %im %is", hours, minutes, static_cast<unsigned int>(seconds));
        }
        else if (minutes > 0) {
            sprintf_s(buf, sizeof(buf), "Render time: %im %is", minutes, static_cast<unsigned int>(seconds));
        }
        else {
            sprintf_s(buf, sizeof(buf), "Render time: %.3fs", seconds);
        }
        mRendering = false;
        mLighter.reset();

        mListener->onRenderStatus(buf);
        mListener->onRenderDone();
    }
}

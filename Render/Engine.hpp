#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP

#include "Object/Color.hpp"
#include "Object/Scene.hpp"
#include "Object/Radiance.hpp"

#include "Render/Framebuffer.hpp"
#include "Render/Job.hpp"
#include "Render/Settings.hpp"

#include "Lighter/Base.hpp"

#include <set>
#include <thread>
#include <mutex>
#include <random>
#include <deque>

#include <windows.h>

namespace Render {
    class Engine
    {
    public:
        class Listener {
        public:
            virtual void onRenderDone() = 0;
            virtual void onRenderStatus(const char *message) = 0;
        };

        Engine(const Object::Scene &scene);
        ~Engine();

        void stop();

        const Object::Scene &scene() const;

        bool rendering() const;

        void startRender(Listener *listener);
        void setSettings(const Settings &settings);

        const Settings &settings() const;
        Framebuffer &renderFramebuffer();
        Framebuffer &sampleStatusFramebuffer();

        static Object::Color toneMap(const Object::Radiance &radiance);

    private:
        void addJob(std::unique_ptr<Job> job);
        void runThread();

        void renderDone();

        const Object::Scene &mScene;
        Settings mSettings;
        Listener *mListener;
        std::unique_ptr<Framebuffer> mRenderFramebuffer;
        std::unique_ptr<Framebuffer> mSampleStatusFramebuffer;
        bool mRendering;
        DWORD mStartTime;
        std::unique_ptr<Lighter::Base> mLighter;

        std::mutex mMutex;
        std::condition_variable mConditionVariable;
        std::vector<std::unique_ptr<std::thread>> mThreads;
        std::unique_ptr<Job> mCurrentJob;
        std::deque<std::unique_ptr<Job>> mJobs;
        int mNumRunningThreads;
        bool mStopThreads;
    };
}

#endif

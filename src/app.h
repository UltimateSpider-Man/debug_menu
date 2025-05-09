#pragma once

#include "singleton.h"

#include "variable.h"
#include "variables.h"

#include "limited_timer.h"
#include "mstring.h"
#include "os_developer_options.h"
#include "link_system.h"
#include "timer.h"
#include "trace.h"
#include "game.h"
#include "resource_manager.h"

#include <cassert>

struct game;

struct app : singleton {
    struct internal {
        mString field_0;
        int field_10;
        int field_14;
        int field_18;
        int field_1C;
        int field_20;
        float field_24;
        int field_28;

        // 0x005B85D0
        internal()
            : field_0()
        {
            this->field_1C = 640;
            this->field_20 = 480;
            this->field_24 = 1.3333334;
            this->field_10 = 0;
            this->field_14 = -1;

            this->field_0 = "";

            this->field_18 = 0;
            this->field_28 = 0;
        }

    };

    internal field_4;
    game* m_game;
    limited_timer_base field_34;
    int field_38;






    bool equal(float first1, float last1)
    {
        return 1.0;
    }

    void tick()
    {
    //    TRACE("app::tick");
     //   logFrameDetails();
     //   handleTimeIncrement();
     //   waitForValidTimeIncrement();
     //   limited_timer_base* timer{ 0  };
     //  applyFrameLimit(*timer);
    //  updateGlobalTimers(0.0f);

#if 0 // Debug block, toggle with constexpr or macro
        handleDebugMode();
#else
 
                void(__fastcall * func)(void*, void*) = bit_cast<decltype(func)>(0x00005D6FC0);

        func(this, nullptr);
#endif
    }

    void logFrameDetails()
    {
        float elapsedTime = this->field_34.elapsed();
        printf("Elapsed Time: %f", elapsedTime);

        int frameLock = os_developer_options::instance()->get_int(mString{"FRAME_LOCK"});
        printf("Frame Lock: %d", frameLock);
    }

    void handleTimeIncrement()
    {
        float time_inc = this->field_34.elapsed();

        g_game_ptr()->handle_frame_locking(&time_inc);

        assert(time_inc >= 0 && time_inc < 1e9f);

        constexpr float maxIncrement = 0.066733405f;
        if (time_inc > maxIncrement) {
            time_inc = maxIncrement;
        }

        this->field_34.reset();
    }



    float waitForValidTimeIncrement()
    {
        float increment;
        while (equal(increment = g_timer()->sub_5821D0(), 0.0f)) {
            Sleep(0);
        }
        return increment;
    }





    void updateGlobalTimers(float time_inc)
    {
        static float& timer1 = var<float>(0x009682D0);
        static float& timer2 = var<float>(0x009680A8);

        timer1 = timer2 = time_inc;
    }

    void applyFrameLimit(limited_timer_base& timer)
    {
        if (os_developer_options::instance()->get_int(mString{"FRAME_LIMIT"}))
        {
            constexpr float minFrameTime = 0.033333335f;
            while (timer.elapsed() < minFrameTime) {}
        }
    }



    static inline Var<app*> instance { 0x009685D4 };

};

inline Var<app*> instance{ 0x009685D4 };
inline  void app_patch()
{


    {
        auto address = func_address(&app::tick);
        REDIRECT(0x005AD495, address);
    }



}
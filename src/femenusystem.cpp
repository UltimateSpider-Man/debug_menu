#include "femenusystem.h"

#include "common.h"
#include "femenu.h"
#include "func_wrapper.h"
#include "input_mgr.h"
#include "trace.h"
#include "utility.h"
#include "vtbl.h"
#include "fetext.h"

#include <cassert>

VALIDATE_SIZE(FEMenuSystem, 0x2C);

VALIDATE_SIZE(FEMenuSystem2, 0x30);

bool getButtonState(int a2, int a3);

FEMenuSystem::FEMenuSystem(int a2, font_index a3)
{
    if constexpr (1) {
        this->field_10 = a2;
        this->m_vtbl = 0x00893A98;
        auto *v4 = static_cast<FEMenu **>(operator new(4 * a2));

        this->field_8 = a3;
        this->field_4 = v4;
        this->m_count = 0;
        this->m_index = -1;
        this->field_28 = 0;
        this->field_29 = 1;

        this->UpdateButtonDown();

        for (int i = 0; i < this->m_count; ++i) {
            auto *menu = this->field_4[i];
            menu->Init();
        }
    } else {


        void(__fastcall * func)(void*, void*, int, font_index) = bit_cast<decltype(func)>(0x00618760);

        func(this, nullptr, a2, a3);
    }
}

void FEMenuSystem::MakeDeactive(int idx) {
    if constexpr (1) {
        auto idx = this->m_index;
        if (idx != -1) {
            FEMenu *v4 = this->field_4[idx];

            if (idx < 0) {
                v4 = nullptr;
            }

            auto *v5 = this->field_4[idx];
            v5->OnDeactivate(v4);
        }

        this->m_index = idx;
        if (idx >= 0) {
            auto *v6 = this->field_4[idx];
            v6->OnActivate();
        }

        {
            auto *vtbl = bit_cast<std::intptr_t(*)[1]>(this->m_vtbl);
            auto func = (*vtbl)[7];

            assert(func == 0x00629960);
            this->UpdateButtonDown();
        }
    } else {


        void(__fastcall * func)(void*, void*, int) = bit_cast<decltype(func)>(0x0060B610);

        func(this, nullptr, idx);
    }
}

void FEMenuSystem::MakeActive(int idx) {


        void(__fastcall * func)(void*, void*,int) = bit_cast<decltype(func)>(0x0060B610);

        func(this, nullptr, idx);
    
}

void FEMenuSystem::UpdateButtonDown()
{
    if constexpr (1) {
        auto *v2 = this->field_18;

        for (int v1 = 0; v1 < 8; ++v1) {
            for (int i = 1; i < 0x4000; i *= 2) {
                auto v4 = getButtonState(i, v1);

                if (v4) {
                    v2[0] |= i;
                } else {
                    v2[0] &= ~(int16_t) i;
                }
            }
            ++v2;
        }
    } else {
        void(__fastcall * func)(void*, void*) = bit_cast<decltype(func)>(0x00629960);

        func(this, nullptr);
    }
}

char FEMenuSystem::GetDefaultColorScheme()
{
    char (__fastcall *func)(void *) = CAST(func, get_vfunc(m_vtbl, 0x2C));
    return func(this);
}

bool FEMenuSystem::GetSingleInput()
{
    bool (__fastcall *func)(void *) = CAST(func, get_vfunc(m_vtbl, 0x34));
    return func(this);
}

void FEMenuSystem::OnButtonPress(int a2, int a3)
{
    if constexpr (1)
	{
        const auto idx = this->m_index;

        if (idx >= 0)
        {

            {
                auto *v5 = this->field_4[idx];
                v5->OnAnyButtonPress(a3, a2);
            }

            if (!this->GetSingleInput() || a3 == input_mgr::instance->field_58 - 1000000)
			{
                if (a2 > 128)
				{
                    if (a2 > 2048)
					{
                        if (a2 == 4096)
						{
                            auto *v19 = this->field_4[this->m_index];
                            v19->OnL2(a3);
                        } else if (a2 == 8192) {
                            auto *v18 = this->field_4[this->m_index];
                            v18->OnR2(a3);
                        }

                    } else {
                        switch (a2) {
                        case 2048: {
                            auto *v17 = this->field_4[this->m_index];
                            v17->OnR1(a3);
                            break;
                        }
                        case 256: {
                            auto *v16 = this->field_4[this->m_index];
                            v16->OnSquare(a3);
                            break;
                        }
                        case 512: {
                            auto *v15 = this->field_4[this->m_index];
                            v15->OnCircle(a3);
                            break;
                        }
                        case 1024: {
                            auto *v14 = this->field_4[this->m_index];
                            v14->OnL1(a3);
                            break;
                        }
                        }
                    }

                } else if (a2 == 128) {
                    auto *v13 = this->field_4[this->m_index];
                    v13->OnTriangle(a3);
                } else {
                    switch (a2) {
                    case 1: {
                        auto *v6 = this->field_4[this->m_index];
                        v6->OnSelect(a3);
                        break;
                    }
                    case 2: {
                        auto *v7 = this->field_4[this->m_index];
                        v7->OnStart(a3);
                        break;
                    }
                    case 4: {
                        auto *v8 = this->field_4[this->m_index];
                        v8->OnUp(a3);
                        break;
                    }
                    case 8: {
                        auto *v9 = this->field_4[this->m_index];
                        v9->OnDown(a3);
                        break;
                    }
                    case 16: {
                        auto *v10 = this->field_4[this->m_index];

                        v10->OnLeft(a3);
                        break;
                    }
                    case 32: {
                        auto *v11 = this->field_4[this->m_index];
                        v11->OnRight(a3);
                        break;
                    }
                    case 64: {
                        auto *v12 = this->field_4[this->m_index];
                        v12->OnCross(a3);
                        break;
                    }
                    default:
                        return;
                    }
                }
            }
        }
    } else {

        void(__fastcall* func)(void*, void*, int, int) = bit_cast<decltype(func)>(0x006187D0);

        func(this, nullptr, a2, a3);


    }
}

void FEMenuSystem::OnButtonRelease(int a2, int a3)
{
    if constexpr (1)
    {
        auto idx = this->m_index;
        if (idx >= 0)
        {
            auto **v4 = this->field_4;
            if (auto *menu = v4[idx]; menu != nullptr) {
                menu->OnButtonRelease(a3, a2);
            }
        }
    } else {

        void(__fastcall * func)(void*, void*, int, int) = bit_cast<decltype(func)>(0x0060B6B0);

        func(this, nullptr, a2, a3);
    }
}

int sub_618A40(int a2, int a3)
{


        int (*func)(int a2, int a3) = CAST(func, 0x00618A40);
        return func(a2, a3);
    
}

bool getButtonState(int a2, int a3)
{
 
        bool (*func)(int a2, int a3) = CAST(func, 0x00618A90);
        return func(a2, a3);
    }


void FEMenuSystem::UpdateButtonPresses()
{
    TRACE("FEMenuSystem::UpdateButtonPresses():");

    if constexpr (0) {
        int v2 = 0;
        for (const auto &v3 : this->field_18) {
            for (int i = 1; i < 16384; i *= 2) {
                auto *vtbl = bit_cast<fastcall_call(*)[16]>(this->m_vtbl);

                if (v3 & i && !getButtonState(i, v2)) {
                    this->field_18[v2] &= ~static_cast<uint16_t>(i);

                    this->OnButtonRelease(i, v2);
                    return;
                }

                if (!(v3 & i) && getButtonState(i, v2)) {
                    this->field_18[v2] |= static_cast<uint16_t>(i);

                    auto *func = (*vtbl)[14];
                    if (bit_cast<std::intptr_t>(func) == 0x006187D0) {
                        this->OnButtonPress(i, v2);
                    } else {
                        func(this, i, v2);
                    }

                    return;
                }
            }

            ++v2;
        }
    }
    else
    {

        void(__fastcall * func)(void*, void*) = bit_cast<decltype(func)>(0x006298D0);

        func(this, nullptr);
    }
}

void FEMenuSystem_patch() {
    {
        FUNC_ADDRESS(address, &FEMenuSystem::OnButtonPress);
        //set_vfunc(0x00893AD0, address);
    }

    {
        FUNC_ADDRESS(address, &FEMenuSystem::UpdateButtonPresses);
        //set_vfunc(0x00893E18, address);
    }
}

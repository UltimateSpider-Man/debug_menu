#pragma once
struct rtdt_replay_mgr {
    int field_4;
    rtdt_replay_mgr();

    static  rtdt_replay_mgr* get_instance();


    int sub_69AD90();


    void* sub_67CE03();


    static int function_instance(rtdt_replay_mgr* thisPtr, int a2);


    void sub_685B0C(void* v3);


    void sub_685742();


    void* sub_6A6613();

    void sub_689978(void* v4);


    int sub_6975F0();


    void sub_66212F(void* v5);


    int sub_67996F();


    static inline rtdt_replay_mgr* m_instance;

};
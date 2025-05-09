#pragma once

#include "mstring.h"

#include <fstream>

static_assert(sizeof(HANDLE) == sizeof(std::ifstream *));

struct os_file {
    enum mode_flags
    {
        FILE_READ = 1,
        FILE_WRITE = 2,
        FILE_MODIFY = 3,
        FILE_APPEND = 4
    };

    enum filepos_t {
        FP_BEGIN,
        FP_CURRENT,
        FP_END
    };

    mString m_path;
    mString field_0;
    uint32_t flags;
    bool opened;
    bool field_15;

    HANDLE io;

    uint32_t field_1C;
    uint32_t m_fileSize;
    bool field_24;
    uint32_t m_offset;
    uint32_t field_2C;
    uint32_t field_30;

    //0x0058DEA0
    inline      os_file() {
        CDECL_CALL(0x0058DEA0);
    }

    //0x0059EAA0
    inline    explicit os_file(const mString &a2, int dwShareMode) {
        CDECL_CALL(0x0059EAA0,a2, dwShareMode);
    }

    //0x0059B6F0
    ~os_file() {
        CDECL_CALL(0x0059B6F0);
    }

    bool is_open() const {
        return this->opened;
    }

    inline    void sub_58DEF0() {
        CDECL_CALL(0x0058DEF0);
    }

    inline     size_t sub_58DF50() {
        CDECL_CALL(0x0058DF50);
    }

    //0x0058DF90
    inline     int get_size() {
        return (int) CDECL_CALL(0x0058DF90);
    }

    //0x0059B740
    inline    void open(const mString &path, int shareMode) {
        CDECL_CALL(0x0059B740, path, shareMode);
    }

 //0x00598C30
    inline    int write(const void *lpBuffer, int nNumberOfBytesToWrite) {
      return (int)  CDECL_CALL(0x00598C30, lpBuffer, nNumberOfBytesToWrite);
    }

    //0x00598AD0
inline    int read(LPVOID data, int bytes) {
   return (int) CDECL_CALL(0x00598AD0,data, bytes);
}

    //0x0058E020
 inline   void set_fp(uint32_t lDistanceToMove, filepos_t a3) {
     CDECL_CALL(0x0058E020, lDistanceToMove, a3);
 }

    //0x00598A60
 inline   void close() {
     CDECL_CALL(0x00598A60);
 }

   static Var<bool> system_locked;
};

//0x00519F40
extern void add_str(char *Dest, const char *Source, size_t a3);

//0x00519F10
extern void copy_str(char *Dest, const char *Source, size_t Count);

extern void os_file_patch();

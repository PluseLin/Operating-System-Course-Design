#include"DiskDevice.h"
#include"File.h"
#include"FileManager.h"
#include"FileSystem.h"
#include"header.h"
#include"User.h"

const char* disk_fp = "mydisk.img";
DiskDevice* g_Disk = nullptr;

FileSystem* g_fs = nullptr;

OpenFileTable* g_ftable = nullptr;
InodeTable* g_itable = nullptr;
UserTable* g_utable = nullptr;
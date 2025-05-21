// mod_loader.h
#ifndef MOD_LOADER_H
#define MOD_LOADER_H

void* load_module(const char* path);
void run_device_command(void* handle, const char* cmd);

#endif

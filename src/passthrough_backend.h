// passthrough_backend.h
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int StartPassthroughBackend(const wchar_t *sourcePath, const wchar_t *mountPoint);
void StopPassthroughBackend(void);

#ifdef __cplusplus
}
#endif

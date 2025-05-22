#include <winfsp/winfsp.h>
#include <strsafe.h>


// extern "C" {
// FspServiceRun;
// NT_SUCCESS;
// FspLoad;
static NTSTATUS SvcStart(FSP_SERVICE *Service, ULONG argc, PWSTR *argv);
static NTSTATUS SvcStop(FSP_SERVICE *Service);

// };
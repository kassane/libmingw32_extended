#include <sys/mman.h>
#include <windows.h>
#include <errno.h>

static int __map_mman_error(const DWORD err, const int deferr)
{
    if (err == 0)
        return deferr;

    int result;
    switch (err) {
    case ERROR_FILE_NOT_FOUND:
        result = ENOENT;
        break;
    case ERROR_ACCESS_DENIED:
        result = EACCES;
        break;
    case ERROR_NOT_ENOUGH_MEMORY:
        result = ENOMEM;
        break;
    // Add more cases as needed
    default:
        result = EINVAL;
        break;
    }

    return result;
}

static DWORD __map_mmap_prot_page(const int prot)
{
    DWORD protect = 0;
    
    if (prot == PROT_NONE)
        return protect;
        
    if ((prot & PROT_EXEC) != 0)
    {
        protect = ((prot & PROT_WRITE) != 0) ? 
                    PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ;
    }
    else
    {
        protect = ((prot & PROT_WRITE) != 0) ?
                    PAGE_READWRITE : PAGE_READONLY;
    }
    
    return protect;
}

int mprotect(void *addr, size_t len, int prot)
{
    DWORD newProtect = __map_mmap_prot_page(prot);
    DWORD oldProtect = 0;
    
    if (VirtualProtect(addr, len, newProtect, &oldProtect))
        return 0;
    
    errno =  __map_mman_error(GetLastError(), EPERM);
    
    return -1;
}

#include <lib/x86.h>

#include "import.h"

#define PAGESIZE      4096
#define PDIRSIZE      (PAGESIZE * 1024)
#define VM_USERLO     0x40000000
#define VM_USERHI     0xF0000000
#define VM_USERLO_PDE (VM_USERLO / PDIRSIZE)
#define VM_USERHI_PDE (VM_USERHI / PDIRSIZE)

/**
 * For each process from id 0 to NUM_IDS - 1,
 * set up the page directory entries so that the kernel portion of the map is
 * the identity map, and the rest of the page directories are unmapped.
 */
void pdir_init(unsigned int mbi_addr)
{
    unsigned int i, j;

    idptbl_init(mbi_addr);

    for (i = 0; i < NUM_IDS; i++) {
        for (j = 0; j < 1024; j++) {
            if (j < VM_USERLO_PDE || j >= VM_USERHI_PDE) {
                set_pdir_entry_identity(i, j);   // kernel part: identity map
            } else {
                rmv_pdir_entry(i, j);            // user part: empty
            }
        }
    }
}

/**
 * Allocates a page (with container_alloc) for the page table,
 * and registers it in the page directory for the given virtual address,
 * and clears (set to 0) all page table entries for this newly mapped page table.
 * It returns the page index of the newly allocated physical page.
 * In the case when there's no physical page available, it returns 0.
 */
unsigned int alloc_ptbl(unsigned int proc_index, unsigned int vaddr)
{
    unsigned int page_index, i;
    page_index = container_alloc(proc_index);
    if (page_index == 0) {
        return 0;
    }
    set_pdir_entry_by_va(proc_index, vaddr, page_index);
    for (i = 0; i < 1024; i++) {
        rmv_ptbl_entry(proc_index, vaddr >> 22, i);
    }
    return page_index;
}

// Reverse operation of alloc_ptbl.
// Removes corresponding the page directory entry,
// and frees the page for the page table entries (with container_free).
void free_ptbl(unsigned int proc_index, unsigned int vaddr)
{
    unsigned int page_index;
    page_index = get_pdir_entry_by_va(proc_index, vaddr) >> 12;
    rmv_pdir_entry_by_va(proc_index, vaddr);
    container_free(proc_index, page_index);
}

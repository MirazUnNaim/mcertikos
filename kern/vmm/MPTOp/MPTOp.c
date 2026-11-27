#include <lib/x86.h>

#include "import.h"

#define PAGESIZE      4096
#define PDIRSIZE      (PAGESIZE * 1024)
#define VM_USERLO     0x40000000
#define VM_USERHI     0xF0000000
#define VM_USERLO_PDE (VM_USERLO / PDIRSIZE)
#define VM_USERHI_PDE (VM_USERHI / PDIRSIZE)

/**
 * Returns the page table entry corresponding to the virtual address,
 * according to the page structure of process # [proc_index].
 * Returns 0 if the mapping does not exist.
 */
unsigned int get_ptbl_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
    unsigned int pde = vaddr >> 22;
    unsigned int pte = (vaddr >> 12) & 0x3FF;
    if (get_pdir_entry(proc_index, pde) != 0) {
        return get_ptbl_entry(proc_index, pde, pte);
    }
    return 0;
}

// Returns the page directory entry corresponding to the given virtual address.
unsigned int get_pdir_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
    return get_pdir_entry(proc_index, vaddr >> 22);
}

// Removes the page table entry for the given virtual address.
void rmv_ptbl_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
    unsigned int pde = vaddr >> 22;
    unsigned int pte = (vaddr >> 12) & 0x3FF;
    if (get_pdir_entry(proc_index, pde) != 0) {
        rmv_ptbl_entry(proc_index, pde, pte);
    }
}

// Removes the page directory entry for the given virtual address.
void rmv_pdir_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
    rmv_pdir_entry(proc_index, vaddr >> 22);
}

// Maps the virtual address [vaddr] to the physical page # [page_index] with permission [perm].
// You do not need to worry about the page directory entry. just map the page table entry.
void set_ptbl_entry_by_va(unsigned int proc_index, unsigned int vaddr,
                          unsigned int page_index, unsigned int perm)
{
    set_ptbl_entry(proc_index, vaddr >> 22, (vaddr >> 12) & 0x3FF, page_index, perm);
}

// Registers the mapping from [vaddr] to physical page # [page_index] in the page directory.
void set_pdir_entry_by_va(unsigned int proc_index, unsigned int vaddr,
                          unsigned int page_index)
{
    set_pdir_entry(proc_index, vaddr >> 22, page_index);
}

// Initializes the identity page table.
// The permission for the kernel memory should be PTE_P, PTE_W, and PTE_G,
// While the permission for the rest should be PTE_P and PTE_W.
void idptbl_init(unsigned int mbi_addr)
{
    unsigned int i, j;

    container_init(mbi_addr);

    for (i = 0; i < 1024; i++) {
        for (j = 0; j < 1024; j++) {
            if (i < VM_USERLO_PDE || i >= VM_USERHI_PDE) {
                set_ptbl_entry_identity(i, j, PTE_P | PTE_W | PTE_G);  // kernel part
            } else {
                set_ptbl_entry_identity(i, j, PTE_P | PTE_W);          // user part
            }
        }
    }
}

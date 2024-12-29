#include <mach/mach.h>

function OS_SystemInfo os_get_system_info(void)
{
	return (OS_SystemInfo){vm_page_size, MB(2)};
}

function void *os_reserve(U64 size)
{
	void * result = 0;

	AssertAlways(vm_allocate(
		mach_task_self(),
		(vm_address_t *)&result,
		size,
		VM_FLAGS_ANYWHERE
	) == KERN_SUCCESS);

	return result;
}

function B32 os_commit(void *ptr, U64 size)
{
	return 1;
}

function void os_decommit(void *ptr, U64 size)
{
	// no-op
}

function void os_release(void *ptr, U64 size)
{
	AssertAlways(vm_deallocate(
		mach_task_self(),
		(vm_address_t)ptr,
		size
	) == KERN_SUCCESS);
}

function void *os_reserve_large(U64 size)
{
	void * result = 0;

	AssertAlways(vm_allocate(
		mach_task_self(),
		(vm_address_t *)&result,
		size,
		VM_FLAGS_ANYWHERE | VM_FLAGS_SUPERPAGE_SIZE_2MB
	) == KERN_SUCCESS);

	return result;
}

function B32 os_commit_large(void *ptr, U64 size)
{
	return 1;
}

int main(int argc, char *argv[])
{
	local_persist TCTX tctx;
	tctx_init_and_equip(&tctx);
	main_thread_base_entry_point(argc, argv);
	tctx_release();
	return 0;
}

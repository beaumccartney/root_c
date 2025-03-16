internal void r_window_close(OS_Window os_window)
{
	r_window_unequip(os_window);
	os_window_close(os_window);
}

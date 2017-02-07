#
# Regular cron jobs for the imageio package
#
0 4	* * *	root	[ -x /usr/bin/imageio_maintenance ] && /usr/bin/imageio_maintenance

#
# Regular cron jobs for the moviepy package
#
0 4	* * *	root	[ -x /usr/bin/moviepy_maintenance ] && /usr/bin/moviepy_maintenance

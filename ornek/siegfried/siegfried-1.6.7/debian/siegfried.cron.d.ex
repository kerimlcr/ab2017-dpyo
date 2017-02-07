#
# Regular cron jobs for the siegfried package
#
0 4	* * *	root	[ -x /usr/bin/siegfried_maintenance ] && /usr/bin/siegfried_maintenance

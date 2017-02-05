#
# Regular cron jobs for the lollypop package
#
0 4	* * *	root	[ -x /usr/bin/lollypop_maintenance ] && /usr/bin/lollypop_maintenance

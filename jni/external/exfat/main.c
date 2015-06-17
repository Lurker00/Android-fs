/*
	exFAT file system multi-call binary

	Copyright (C) 2011-2013  Andrew Nayenko
	Copyright (C) 2013 The CyanogenMod Project

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>

extern int mkexfatfs_main(int argc, char* argv[]);
extern int exfatfsck_main(int argc, char* argv[]);
extern int mount_exfat_main(int argc, char* argv[]);
extern int dumpexfat_main(int argc, char* argv[]);
extern int exfatlabel_main(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	printf("exFAT multi-call binary, mod/build 1.0.4 by Lurker\n");
	fflush(stdout);
    if (strstr(argv[0], "mkfs.exfat") != NULL || strstr(argv[0], "mkexfatfs") != NULL)
        return mkexfatfs_main(argc, argv);
    if (strstr(argv[0], "fsck.exfat") != NULL || strstr(argv[0], "exfatfsck") != NULL)
        return exfatfsck_main(argc, argv);
    if (strstr(argv[0], "dumpexfat"))
        return dumpexfat_main(argc, argv);
    if (strstr(argv[0], "exfatlabel"))
        return exfatlabel_main(argc, argv);
    return mount_exfat_main(argc, argv);
}

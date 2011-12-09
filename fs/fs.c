/*
 * kernel/fs/fs.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <types.h>
#include <string.h>
#include <sizes.h>
#include <malloc.h>
#include <hash.h>
#include <stdio.h>
#include <list.h>
#include <fs/fs.h>

/*
 * the list of filesystem
 */
static struct fs_list __fs_list = {
	.entry = {
		.next	= &(__fs_list.entry),
		.prev	= &(__fs_list.entry),
	},
};
struct fs_list * fs_list = &__fs_list;

/*
 * search filesystem by name
 */
struct filesystem * filesystem_search(const char * name)
{
	struct fs_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&fs_list->entry)->next; pos != (&fs_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct fs_list, entry);
		if(strcmp(list->fs->name, name) == 0)
			return list->fs;
	}

	return NULL;
}

/*
 * register a filesystem into fs_list
 */
bool_t filesystem_register(struct filesystem * fs)
{
	struct fs_list * list;

	list = malloc(sizeof(struct fs_list));
	if(!list || !fs)
	{
		free(list);
		return FALSE;
	}

	if(!fs->name || filesystem_search(fs->name))
	{
		free(list);
		return FALSE;
	}

	list->fs = fs;
	list_add(&list->entry, &fs_list->entry);

	return TRUE;
}

/*
 * unregister a filesystem from fs_list
 */
bool_t filesystem_unregister(struct filesystem * fs)
{
	struct fs_list * list;
	struct list_head * pos;

	if(!fs || !fs->name)
		return FALSE;

	for(pos = (&fs_list->entry)->next; pos != (&fs_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct fs_list, entry);
		if(list->fs == fs)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

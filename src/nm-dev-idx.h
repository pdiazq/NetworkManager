/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* NetworkManager -- Network link manager
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Copyright (C) 2017 Red Hat, Inc.
 */

#ifndef __NM_DEV_IDX_H__
#define __NM_DEV_IDX_H__

#include "nm-utils/c-list.h"

typedef struct _NMDevIdxIndex NMDevIdxIndex;

struct _NMDevIdxNodeDevBase {
	NMDevice *device;
};

struct _NMDevIdxNodeConBase {
	NMSettingsConnection *connection;
};

typedef struct {
	struct _NMDevIdxNodeDevBase base;
	CList lst_dc_dev_head;
	NMDevIdxIndex *_self;
	guint _ref_count;
	CList lst_dev;
} NMDevIdxNodeDev;

typedef struct {
	struct _NMDevIdxNodeConBase base;
	CList lst_dc_con_head;
	NMDevIdxIndex *_self;
	guint _ref_count;
	CList lst_con;
} NMDevIdxNodeCon;

struct _NMDevIdxNodeDCBase {
	union {
		struct _NMDevIdxNodeDevBase *node_dev_base;
		NMDevIdxNodeDev *node_dev;
	};
	union {
		struct _NMDevIdxNodeConBase *node_con_base;
		NMDevIdxNodeCon *node_con;
	};
};

typedef struct {
	struct _NMDevIdxNodeDCBase base;
	CList lst_dc_dev;
	CList lst_dc_con;
} NMDevIdxNodeDC;

/*****************************************************************************/

static inline void
nm_dev_idx_node_con_ref (NMDevIdxNodeCon *node_con) {
	node_con->_ref_count++;
}

static inline void
nm_dev_idx_node_dev_ref (NMDevIdxNodeDev *node_dev) {
	node_dev->_ref_count++;
}

void nm_dev_idx_node_con_unref (NMDevIdxNodeCon *node_con);
void nm_dev_idx_node_dev_unref (NMDevIdxNodeDev *node_dev);

/*****************************************************************************/

NMDevIdxIndex *nm_dev_idx_index_new (void);
void nm_dev_idx_index_ref (NMDevIdxIndex *self);
void nm_dev_idx_index_unref (NMDevIdxIndex *self);

CList *nm_dev_idx_index_dev_get_all (NMDevIdxIndex *self);
CList *nm_dev_idx_index_con_get_all (NMDevIdxIndex *self);

NMDevIdxNodeDev *nm_dev_idx_index_dev_create (NMDevIdxIndex *self,
                                              NMDevice *device);

NMDevIdxNodeDev *nm_dev_idx_index_dev_lookup (NMDevIdxIndex *self,
                                              NMDevice *device);

NMDevIdxNodeCon *nm_dev_idx_index_con_create (NMDevIdxIndex *self,
                                              NMSettingsConnection *connection);

NMDevIdxNodeCon *nm_dev_idx_index_con_lookup (NMDevIdxIndex *self,
                                              NMSettingsConnection *connection);

NMDevIdxNodeDC *nm_dev_idx_index_dc_access (NMDevIdxIndex *self,
                                            NMDevice *device,
                                            NMSettingsConnection *connection,
                                            gboolean allow_create,
                                            gboolean remove);

#endif /* __NM_DEV_IDX_H__ */

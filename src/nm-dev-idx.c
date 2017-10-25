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
 * Copyright (C) 2017 Novell, Inc.
 */

#include "nm-default.h"

#include "nm-dev-idx.h"

#include "nm-utils/nm-hash-utils.h"
#include "devices/nm-device.h"
#include "settings/nm-settings-connection.h"

/*****************************************************************************/

typedef struct _NMDevIdxNodeDevBase NMDevIdxNodeDevBase;
typedef struct _NMDevIdxNodeConBase NMDevIdxNodeConBase;
typedef struct _NMDevIdxNodeDCBase NMDevIdxNodeDCBase;

struct _NMDevIdxIndex {
	guint _ref_count;
	GHashTable *idx_con;
	GHashTable *idx_dev;
	GHashTable *idx_data;
	CList lst_dev_head;
	CList lst_con_head;
};

#define NM_IS_DEV_IDX_INDEX(self) ((self) != NULL)

/*****************************************************************************/

#define nm_assert_index(self) \
	G_STMT_START { \
		if (NM_MORE_ASSERTS > 0) { \
			const NMDevIdxIndex *_self = (self); \
			\
			nm_assert (NM_IS_DEV_IDX_INDEX (_self)); \
			nm_assert (_self->_ref_count > 0); \
		} \
	} G_STMT_END

#define nm_assert_node_dev_base(node_dev) \
	G_STMT_START { \
		if (NM_MORE_ASSERTS > 0) { \
			const NMDevIdxNodeDevBase *_node_dev = (node_dev); \
			\
			nm_assert (_node_dev); \
			nm_assert (NM_IS_DEVICE (_node_dev->device)); \
		} \
	} G_STMT_END

#define nm_assert_node_dev(node_dev) \
	G_STMT_START { \
		if (NM_MORE_ASSERTS > 0) { \
			const NMDevIdxNodeDev *_node_dev = (node_dev); \
			\
			nm_assert (_node_dev); \
			nm_assert_index (_node_dev->_self); \
			nm_assert (NM_IS_DEVICE (_node_dev->base.device)); \
			nm_assert (!c_list_is_empty (&node_dev->lst_dev)); \
			nm_assert (_node_dev->_ref_count > 0); \
		} \
	} G_STMT_END

#define nm_assert_node_con_base(node_con) \
	G_STMT_START { \
		if (NM_MORE_ASSERTS > 0) { \
			const NMDevIdxNodeConBase *_node_con = (node_con); \
			\
			nm_assert (_node_con); \
			nm_assert (NM_IS_SETTINGS_CONNECTION (_node_con->connection)); \
		} \
	} G_STMT_END

#define nm_assert_node_con(node_con) \
	G_STMT_START { \
		if (NM_MORE_ASSERTS > 0) { \
			const NMDevIdxNodeCon *_node_con = (node_con); \
			\
			nm_assert (_node_con); \
			nm_assert_index (_node_con->_self); \
			nm_assert (NM_IS_SETTINGS_CONNECTION (_node_con->base.connection)); \
			nm_assert (!c_list_is_empty (&node_con->lst_con)); \
			nm_assert (_node_con->_ref_count > 0); \
		} \
	} G_STMT_END

#define nm_assert_node_dc_base(node_dc) \
	G_STMT_START { \
		if (NM_MORE_ASSERTS > 0) { \
			const NMDevIdxNodeDCBase *_node_dc = (node_dc); \
			\
			nm_assert (_node_dc); \
			nm_assert_node_dev_base (_node_dc->node_dev_base); \
			nm_assert_node_con_base (_node_dc->node_con_base); \
			nm_assert (_node_dc->node_con->_self == _node_dc->node_dev->_self); \
		} \
	} G_STMT_END

#define nm_assert_node_dc(node_dc) \
	G_STMT_START { \
		if (NM_MORE_ASSERTS > 0) { \
			const NMDevIdxNodeDC *_node_dc = (node_dc); \
			\
			nm_assert (_node_dc); \
			nm_assert_node_dev (_node_dc->base.node_dev); \
			nm_assert_node_con (_node_dc->base.node_con); \
			nm_assert (_node_dc->base.node_con->_self == _node_dc->base.node_dev->_self); \
		} \
	} G_STMT_END

/*****************************************************************************/

static guint
_idx_data_fcn_hash (gconstpointer datax)
{
	const NMDevIdxNodeDCBase *node_dc = datax;
	NMHashState h;

	nm_assert_node_dc_base (node_dc);

	nm_hash_init (&h, 1052530169u);
	nm_hash_update_vals (&h,
	                     node_dc->node_dev->base.device,
	                     node_dc->node_con->base.connection);
	return nm_hash_complete (&h);
}

static gboolean
_idx_data_fcn_equal (gconstpointer datax_a, gconstpointer datax_b)
{
	const NMDevIdxNodeDCBase *node_dc_a = datax_a;
	const NMDevIdxNodeDCBase *node_dc_b = datax_b;

	nm_assert_node_dc_base (node_dc_a);
	nm_assert_node_dc_base (node_dc_b);

	return    node_dc_a->node_dev->base.device     == node_dc_b->node_dev->base.device
	       && node_dc_a->node_con->base.connection == node_dc_b->node_con->base.connection;
}

static guint
_idx_head_fcn_hash (gconstpointer ptr_headx)
{
	const void *const*ptr_head = ptr_headx;

	nm_assert (ptr_head
	           && (   NM_IS_DEVICE (*ptr_head)
	               || NM_IS_SETTINGS_CONNECTION (*ptr_head)));

	return nm_hash_ptr (*ptr_head);
}

static gboolean
_idx_head_fcn_equal (gconstpointer ptr_headx_a, gconstpointer ptr_headx_b)
{
	const void *const*ptr_head_a = ptr_headx_a;
	const void *const*ptr_head_b = ptr_headx_b;

	nm_assert (ptr_head_a
	           && (   NM_IS_DEVICE (*ptr_head_a)
	               || NM_IS_SETTINGS_CONNECTION (*ptr_head_a)));

	nm_assert (ptr_head_b
	           && (   NM_IS_DEVICE (*ptr_head_b)
	               || NM_IS_SETTINGS_CONNECTION (*ptr_head_b)));

	return *ptr_head_a == *ptr_head_b;
}

static void
_idx_data_fcn_free (gpointer datax)
{
	NMDevIdxNodeDC *node_dc = datax;

	nm_assert_node_dc (node_dc);

	c_list_unlink (&node_dc->lst_dc_dev);
	c_list_unlink (&node_dc->lst_dc_con);

	nm_dev_idx_node_dev_unref (node_dc->base.node_dev);
	nm_dev_idx_node_con_unref (node_dc->base.node_con);

	g_slice_free (NMDevIdxNodeDC, node_dc);
}

/*****************************************************************************/

void
nm_dev_idx_node_dev_unref (NMDevIdxNodeDev *node_dev)
{
	nm_assert_node_dev (node_dev);

	if (--node_dev->_ref_count == 0) {
		nm_assert (c_list_is_empty (&node_dev->lst_dc_dev_head));
		c_list_unlink (&node_dev->lst_dev);
		g_hash_table_remove (node_dev->_self->idx_dev, node_dev);
		g_object_unref (node_dev->base.device);
		g_slice_free (NMDevIdxNodeDev, node_dev);
	}
}

void
nm_dev_idx_node_con_unref (NMDevIdxNodeCon *node_con)
{
	nm_assert_node_con (node_con);

	if (--node_con->_ref_count == 0) {
		nm_assert (c_list_is_empty (&node_con->lst_dc_con_head));
		c_list_unlink (&node_con->lst_con);
		g_hash_table_remove (node_con->_self->idx_con, node_con);
		g_object_unref (node_con->base.connection);
		g_slice_free (NMDevIdxNodeCon, node_con);
	}
}

/*****************************************************************************/

static NMDevIdxNodeDev *
_dev_create (NMDevIdxIndex *self,
             NMDevice *device,
             gboolean allow_create,
             gboolean exclusive)
{
	NMDevIdxNodeDev *node_dev;

	nm_assert_index (self);
	nm_assert (NM_IS_DEVICE (device));

	node_dev = g_hash_table_lookup (self->idx_dev, &device);
	if (!node_dev) {
		if (!allow_create)
			return NULL;
		node_dev = g_slice_new0 (NMDevIdxNodeDev);
		node_dev->_self = self;
		node_dev->_ref_count = 1;
		node_dev->base.device = g_object_ref (device);
		c_list_init (&node_dev->lst_dc_dev_head);
		g_hash_table_add (self->idx_dev, node_dev);
		c_list_link_tail (&self->lst_dev_head, &node_dev->lst_dev);
	} else {
		if (!allow_create)
			return node_dev;
		if (exclusive)
			return NULL;
		nm_dev_idx_node_dev_ref (node_dev);
	}

	return node_dev;
}

NMDevIdxNodeDev *
nm_dev_idx_index_dev_create (NMDevIdxIndex *self,
                             NMDevice *device,
                             gboolean exclusive)
{
	return _dev_create (self, device, TRUE, exclusive);
}

NMDevIdxNodeDev *
nm_dev_idx_index_dev_lookup (NMDevIdxIndex *self,
                             NMDevice *device)
{
	return _dev_create (self, device, FALSE, FALSE);
}

static NMDevIdxNodeCon *
_con_create (NMDevIdxIndex *self,
             NMSettingsConnection *connection,
             gboolean allow_create,
             gboolean exclusive)
{
	NMDevIdxNodeCon *node_con;

	nm_assert_index (self);
	nm_assert (NM_IS_SETTINGS_CONNECTION (connection));

	node_con = g_hash_table_lookup (self->idx_con, &connection);
	if (!node_con) {
		if (!allow_create)
			return NULL;
		node_con = g_slice_new0 (NMDevIdxNodeCon);
		node_con->_self = self;
		node_con->_ref_count = 1;
		node_con->base.connection = g_object_ref (connection);
		c_list_init (&node_con->lst_dc_con_head);
		g_hash_table_add (self->idx_con, node_con);
		c_list_link_tail (&self->lst_con_head, &node_con->lst_con);
	} else {
		if (!allow_create)
			return node_con;
		if (exclusive)
			return NULL;
		nm_dev_idx_node_con_ref (node_con);
	}

	return node_con;
}

NMDevIdxNodeCon *
nm_dev_idx_index_con_create (NMDevIdxIndex *self,
                             NMSettingsConnection *connection,
                             gboolean exclusive)
{
	return _con_create (self, connection, TRUE, exclusive);
}

NMDevIdxNodeCon *
nm_dev_idx_index_con_lookup (NMDevIdxIndex *self,
                             NMSettingsConnection *connection)
{
	return _con_create (self, connection, FALSE, FALSE);
}

/*****************************************************************************/

NMDevIdxNodeDC *
nm_dev_idx_index_dc_access (NMDevIdxIndex *self,
                            NMDevice *device,
                            NMSettingsConnection *connection,
                            gboolean allow_create,
                            gboolean remove)
{
	const NMDevIdxNodeDCBase needle_data_base = {
		.node_dev_base = (NMDevIdxNodeDevBase *) &device,
		.node_con_base = (NMDevIdxNodeConBase *) &connection,
	};
	NMDevIdxNodeDC *node_dc;
	NMDevIdxNodeDev *node_dev;
	NMDevIdxNodeCon *node_con;

	nm_assert (NM_IS_DEV_IDX_INDEX (self));
	nm_assert (NM_IS_DEVICE (device));
	nm_assert (NM_IS_SETTINGS_CONNECTION (connection));

	node_dc = g_hash_table_lookup (self->idx_data, &needle_data_base);
	if (node_dc) {
		nm_assert_node_dc (node_dc);

		if (!remove)
			return node_dc;

		g_hash_table_remove (self->idx_data, node_dc);
		return NULL;
	}

	if (remove || !allow_create)
		return NULL;

	node_dev = _dev_create (self, device, TRUE, FALSE);
	node_con = _con_create (self, connection, TRUE, FALSE);

	node_dc = g_slice_new0 (NMDevIdxNodeDC);
	node_dc->base.node_dev = node_dev;
	node_dc->base.node_con = node_con;
	c_list_link_tail (&node_dev->lst_dc_dev_head, &node_dc->lst_dc_dev);
	c_list_link_tail (&node_con->lst_dc_con_head, &node_dc->lst_dc_con);
	g_hash_table_add (self->idx_data, node_dc);

	return node_dc;
}

/*****************************************************************************/

CList *
nm_dev_idx_index_dev_get_all (NMDevIdxIndex *self)
{
	nm_assert_index (self);

	return &self->lst_dev_head;
}

CList *
nm_dev_idx_index_con_get_all (NMDevIdxIndex *self)
{
	nm_assert_index (self);

	return &self->lst_con_head;
}

/*****************************************************************************/

NMDevIdxIndex *
nm_dev_idx_index_new (void)
{
	NMDevIdxIndex *self;

	self = g_slice_new0 (NMDevIdxIndex);

	self->_ref_count = 1;
	c_list_init (&self->lst_dev_head);
	c_list_init (&self->lst_con_head);
	self->idx_data = g_hash_table_new_full (_idx_data_fcn_hash,
	                                        _idx_data_fcn_equal,
	                                        _idx_data_fcn_free,
	                                        NULL);
	self->idx_dev = g_hash_table_new (_idx_head_fcn_hash,
	                                  _idx_head_fcn_equal);
	self->idx_con = g_hash_table_new (_idx_head_fcn_hash,
	                                  _idx_head_fcn_equal);
	return self;
}

void
nm_dev_idx_index_ref (NMDevIdxIndex *self)
{
	nm_assert_index (self);

	self->_ref_count++;
}

void
nm_dev_idx_index_unref (NMDevIdxIndex *self)
{
	nm_assert_index (self);

	g_hash_table_remove_all (self->idx_data);

	/* we must end up with no remaining nodes, otherwise it means
	 * somebody else kept an additional reference. */
	g_assert (g_hash_table_size (self->idx_con) == 0);
	g_assert (g_hash_table_size (self->idx_dev) == 0);
	g_assert (c_list_is_empty (&self->lst_dev_head));
	g_assert (c_list_is_empty (&self->lst_con_head));

	g_hash_table_unref (self->idx_data);
	g_hash_table_unref (self->idx_con);
	g_hash_table_unref (self->idx_dev);

	g_slice_free (NMDevIdxIndex, self);
}

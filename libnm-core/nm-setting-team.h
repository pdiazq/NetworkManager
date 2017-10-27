/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 * Copyright 2013 Jiri Pirko <jiri@resnulli.us>
 */

#ifndef __NM_SETTING_TEAM_H__
#define __NM_SETTING_TEAM_H__

#if !defined (__NETWORKMANAGER_H_INSIDE__) && !defined (NETWORKMANAGER_COMPILATION)
#error "Only <NetworkManager.h> can be included directly."
#endif

#include "nm-setting.h"

G_BEGIN_DECLS

#define NM_TYPE_SETTING_TEAM            (nm_setting_team_get_type ())
#define NM_SETTING_TEAM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NM_TYPE_SETTING_TEAM, NMSettingTeam))
#define NM_SETTING_TEAM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NM_TYPE_SETTING_TEAM, NMSettingTeamClass))
#define NM_IS_SETTING_TEAM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NM_TYPE_SETTING_TEAM))
#define NM_IS_SETTING_TEAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NM_TYPE_SETTING_TEAM))
#define NM_SETTING_TEAM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), NM_TYPE_SETTING_TEAM, NMSettingTeamClass))

#define NM_SETTING_TEAM_SETTING_NAME "team"

#define NM_SETTING_TEAM_CONFIG                     "config"
#define NM_SETTING_TEAM_NOTIFYPEERS_COUNT          "notifypeers-count"
#define NM_SETTING_TEAM_NOTIFYPEERS_INTERVAL       "notifypeers-interval"
#define NM_SETTING_TEAM_MCASTREJOIN_COUNT          "mcastrejoin-count"
#define NM_SETTING_TEAM_MCASTREJOIN_INTERVAL       "mcastrejoin-interval"
#define NM_SETTING_TEAM_RUNNER                     "runner"
#define NM_SETTING_TEAM_RUNNER_HWPOLICY            "runner-hwpolicy"
#define NM_SETTING_TEAM_RUNNER_TXHASH              "runner-txhash"
#define NM_SETTING_TEAM_RUNNER_TXBALANCER          "runner-txbalancer"
#define NM_SETTING_TEAM_RUNNER_TXBALANCER_INTERVAL "runner-txbalancer-interval"
#define NM_SETTING_TEAM_RUNNER_ACTIVE              "runner-active"
#define NM_SETTING_TEAM_RUNNER_FASTRATE            "runner-fastrate"
#define NM_SETTING_TEAM_RUNNER_SYSPRIO             "runner-sysprio"
#define NM_SETTING_TEAM_RUNNER_MINPORTS            "runner-minports"
#define NM_SETTING_TEAM_RUNNER_AGGSELECTPOLICY     "runner-aggselectpolicy"

#define NM_SETTING_TEAM_RUNNER_BROADCAST    "broadcast"
#define NM_SETTING_TEAM_RUNNER_ROUNDROBIN   "roundrobin"
#define NM_SETTING_TEAM_RUNNER_ACTIVEBACKUP "activebackup"
#define NM_SETTING_TEAM_RUNNER_LOADBALANCE  "loadbalance"
#define NM_SETTING_TEAM_RUNNER_LACP         "lacp"

/**
 * NMSettingTeam:
 *
 * Teaming Settings
 */
struct _NMSettingTeam {
	NMSetting parent;
};

typedef struct {
	NMSettingClass parent;

	/*< private >*/
	gpointer padding[4];
} NMSettingTeamClass;

GType nm_setting_team_get_type (void);

NMSetting *  nm_setting_team_new                (void);

const char * nm_setting_team_get_config (NMSettingTeam *setting);
NM_AVAILABLE_IN_1_10
gint nm_setting_team_get_notifypeers_count (NMSettingTeam *setting);
NM_AVAILABLE_IN_1_10
gint nm_setting_team_get_notifypeers_interval (NMSettingTeam *setting);
NM_AVAILABLE_IN_1_10
gint nm_setting_team_get_mcastrejoin_count (NMSettingTeam *setting);
NM_AVAILABLE_IN_1_10
gint nm_setting_team_get_mcastrejoin_interval (NMSettingTeam *setting);
NM_AVAILABLE_IN_1_10
const char * nm_setting_team_get_runner (NMSettingTeam *setting);
NM_AVAILABLE_IN_1_10
const char * nm_setting_team_get_runner_hwpolicy (NMSettingTeam *setting);
NM_AVAILABLE_IN_1_10
const char * nm_setting_team_get_runner_txbalancer (NMSettingTeam *setting);
NM_AVAILABLE_IN_1_10
gint nm_setting_team_get_runner_txbalancer_interval (NMSettingTeam *setting);
NM_AVAILABLE_IN_1_10
gboolean nm_setting_team_get_runner_active (NMSettingTeam *setting);
NM_AVAILABLE_IN_1_10
gboolean nm_setting_team_get_runner_fastrate (NMSettingTeam *setting);
NM_AVAILABLE_IN_1_10
gint nm_setting_team_get_runner_sysprio (NMSettingTeam *setting);
NM_AVAILABLE_IN_1_10
gint nm_setting_team_get_runner_minports (NMSettingTeam *setting);
NM_AVAILABLE_IN_1_10
const char * nm_setting_team_get_runner_aggselectpolicy (NMSettingTeam *setting);
NM_AVAILABLE_IN_1_10
gboolean nm_setting_team_remove_runner_txhash_by_value (NMSettingTeam *setting, const char *txhash);
NM_AVAILABLE_IN_1_10
guint nm_setting_team_get_num_runner_txhash (NMSettingTeam *setting);
NM_AVAILABLE_IN_1_10
void nm_setting_team_remove_runner_txhash (NMSettingTeam *setting, int idx);
NM_AVAILABLE_IN_1_10
gboolean nm_setting_team_add_runner_txhash (NMSettingTeam *setting, const char *txhash);
G_END_DECLS

#endif /* __NM_SETTING_TEAM_H__ */

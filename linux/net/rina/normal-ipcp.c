/*
 *  IPC Processes layer
 *
 *    Leonardo Bergesio     <leonardo.bergesio@i2cat.net>
 *    Francesco Salvestrini <f.salvestrini@nextworks.it>
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/list.h>

#define IPCP_NAME "normal-ipc"
#define RINA_PREFIX IPCP_NAME

#include "logs.h"
#include "normal-ipcp.h"
#include "common.h"
#include "debug.h"
#include "utils.h"
#include "kipcm.h"
#include "ipcp-utils.h"
#include "ipcp-factories.h"
#include "du.h"
#include "kfa.h"
#include "rnl-utils.h"

/*  FIXME: To be removed ABSOLUTELY */
extern struct kipcm * default_kipcm;

struct ipcp_instance_data {
        /* FIXME add needed attributes */
        int dummy_attr;
};

struct ipcp_factory_data {
        struct list_head instances;
};

static struct ipcp_factory_data normal_data;

static int normal_init(struct ipcp_factory_data * data) 
{
        ASSERT(data);

        bzero(&normal_data, sizeof(normal_data));
        INIT_LIST_HEAD(&data->instances);

        return 0;
}

static int normal_fini(struct ipcp_factory_data * data)
{
        ASSERT(data);
        ASSERT(list_empty(&data->instances));
        return 0;
}

static struct ipcp_instance * normal_create(struct ipcp_factory_data * data,
                                            const struct name *        name,
                                            ipc_process_id_t          id)
{
        LOG_MISSING;
        return 0;
}

static int normal_destroy(struct ipcp_factory_data * data,
                          struct ipcp_instance *     instance)
{
        LOG_MISSING;
        return 0;
}

static struct ipcp_factory_ops normal_ops = {
        .init    = normal_init,
        .fini    = normal_fini,
        .create  = normal_create,
        .destroy = normal_destroy,
};

struct ipcp_factory * normal = NULL;

static int __init mod_init(void)
{
        LOG_DBG("RINA IPCP loading");

        if (normal) {
                LOG_ERR("RINA normal IPCP already initialized, bailing out");
                return -1; 
        }   

        normal = kipcm_ipcp_factory_register(default_kipcm,
                                             IPCP_NAME,
                                             &normal_data,
                                             &normal_ops);
        ASSERT(normal != NULL);

        if (!normal) {
                LOG_CRIT("Could not register %s factory, bailing out",
                         IPCP_NAME);
                return -1;
        }

        LOG_DBG("RINA normal IPCP loaded successfully");

        return 0;
}

static void __exit mod_exit(void)
{

        ASSERT(normal);

        if (kipcm_ipcp_factory_unregister(default_kipcm, normal)) {
                LOG_CRIT("Could not unregister %s factory, bailing out",
                         IPCP_NAME);
                return;
        }   

        LOG_DBG("RINA normal IPCP unloaded successfully");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_DESCRIPTION("RINA normal IPC Process");

MODULE_LICENSE("GPL");

MODULE_AUTHOR("Francesco Salvestrini <f.salvestrini@nextworks.it>");
MODULE_AUTHOR("Miquel Tarzan <miquel.tarzan@i2cat.net>");
MODULE_AUTHOR("Sander Vrijders <sander.vrijders@intec.ugent.be>");
MODULE_AUTHOR("Leonardo Bergesio <leonardo.bergesio@i2cat.net>");

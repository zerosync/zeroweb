#include <nopoll.h>
#include <stdlib.h>
#include <stdio.h>

nopoll_bool on_connection_opened (noPollCtx * ctx, noPollConn * conn, noPollPtr user_data)
{
    if (! nopoll_conn_set_sock_block (nopoll_conn_socket (conn), nopoll_false)) {
         printf ("ERROR: failed to configure non-blocking state to connection..\n");
         return nopoll_false;
    } 
            
    /* check to reject */
    if (nopoll_cmp (nopoll_conn_get_origin (conn), "http://deny.aspl.es"))  {
        printf ("INFO: rejected connection from %s, with Host: %s and Origin: %s\n",
                    nopoll_conn_host (conn), nopoll_conn_get_host_header (conn), nopoll_conn_get_origin (conn));
        return nopoll_false;
    } 

    /* notify connection accepted */
    /* printf ("INFO: connection received from %s, with Host: %s and Origin: %s\n"
                   nopoll_conn_host (conn), nopoll_conn_get_host_header (conn), nopoll_conn_get_origin (conn)); */
    printf ("Connection established!\n");
    return nopoll_true;
}


void handleMsg (noPollCtx *ctx, noPollConn *con, noPollMsg *msg, noPollPtr *user_data)
{
    printf("Server received (size: %d) (msg:%s)\n", nopoll_msg_get_payload_size(msg),
                                                    (const  char*) nopoll_msg_get_payload(msg));
   
    nopoll_conn_send_text (con, "Message received!", 17); 
}

int main (int argc, char **argv) 
{
    printf("Server started!\n");
    
    noPollCtx *ctx = nopoll_ctx_new();
    if (!ctx){
        printf("Could not create context. Shutting down!\n");
        return EXIT_FAILURE;
    } 

    // NULL ist the default port: 80
    noPollConn *listener = nopoll_listener_new(ctx, "localhost", "1234");
    if (!nopoll_conn_is_ok(listener))  {
        printf("The listener wasn´t initilialized correctly.\n");
        return EXIT_FAILURE;
    }  
    nopoll_ctx_set_on_msg(ctx, handleMsg, NULL);
    nopoll_ctx_set_on_open(ctx, on_connection_opened, NULL);

    // Wait for Client Action
    nopoll_loop_wait(ctx, 0);

    nopoll_conn_close(listener);
    nopoll_ctx_unref(ctx);
    return EXIT_SUCCESS;
}

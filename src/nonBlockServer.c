#include "NBS.h"

void startRTK ()
{
    int    i, len, rc, on = 1;
    int    listen_sd, max_sd, new_sd;
    int    desc_ready, end_server = FALSE;
    int    close_conn;
    char   buffer[1024];
    struct sockaddr_in6 addr;
    struct timeval      timeout;
    fd_set              master_set, working_set;
    
    int keyRTK;


    listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
    if (listen_sd < 0)
    {
        perror("socket() failed");
        exit(-1);
    }

    rc = setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR,
                    (char *)&on, sizeof(on));
    if (rc < 0)
    {
        perror("setsockopt() failed");
        close(listen_sd);
        exit(-1);
    }

    rc = ioctl(listen_sd, FIONBIO, (char *)&on);
    if (rc < 0)
    {
        perror("ioctl() failed");
        close(listen_sd);
        exit(-1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin6_family      = AF_INET6;
    memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
    addr.sin6_port        = htons(SERVER_PORT);
    rc = bind(listen_sd,
                (struct sockaddr *)&addr, sizeof(addr));
    if (rc < 0)
    {
        perror("bind() failed");
        close(listen_sd);
        exit(-1);
    }

    rc = listen(listen_sd, 32);
    if (rc < 0)
    {
        perror("listen() failed");
        close(listen_sd);
        exit(-1);
    }

    FD_ZERO(&master_set);
    max_sd = listen_sd;
    FD_SET(listen_sd, &master_set);

    timeout.tv_sec  = 3 * 60;
    timeout.tv_usec = 0;

    do
    {
        memcpy(&working_set, &master_set, sizeof(master_set));

        rc = select(max_sd + 1, &working_set, NULL, NULL, &timeout);

        if (rc < 0)
        {
            perror("  select() failed");
            break;
        }

        if (rc == 0)
        {
            printf("  select() timed out.  End program.\n");
            break;
        }

        desc_ready = rc;
        for (i=0; i <= max_sd  &&  desc_ready > 0; ++i)
        {
            if (FD_ISSET(i, &working_set))
            {
                desc_ready -= 1;
                if (i == listen_sd)
                {
                    do
                    {
                        new_sd = accept(listen_sd, NULL, NULL);
                        if (new_sd < 0)
                        {
                            if (errno != EWOULDBLOCK)
                            {
                                perror("  accept() failed");
                                end_server = TRUE;
                            }
                            break;
                        }

                        printf("  New incoming connection - %d\n", new_sd);
                        FD_SET(new_sd, &master_set);
                        if (new_sd > max_sd)
                            max_sd = new_sd;

                    } while (new_sd != -1);
                }

                else
                {
                printf("  Descriptor %d is readable\n", i);
                close_conn = FALSE;

                do
                {

                    rc = recv(i, buffer, sizeof(buffer), 0);
                    if (rc < 0)
                    {
                        if (errno != EWOULDBLOCK)
                        {
                            perror("  recv() failed");
                            close_conn = TRUE;
                        }
                        break;
                    }

                    if (rc == 0)
                    {
                        printf("  Connection closed\n");
                        close_conn = TRUE;
                        break;
                    }
                    char subStr[1024];
    
                    solMB[0] = atof(strncpy(subStr, buffer+26, 12));
                    solMB[1] = atof(strncpy(subStr, buffer+41, 12));
                    solMB[2] = atof(strncpy(subStr, buffer+56, 12));

                    errMB[0] = atof(strncpy(subStr, buffer+80, 6));
                    errMB[1] = atof(strncpy(subStr, buffer+86, 6));
                    errMB[2] = atof(strncpy(subStr, buffer+98, 6));
                    
                    /*fprintf(stderr, "%f, %f, %f, %f, %f, %f\n",solMB[0], solMB[1], solMB[2], errMB[0], errMB[1], errMB[2]);*/
                    /*len = rc;
                    printf("  %s\n", buffer);*/

                } while (TRUE);

                if (close_conn)
                {
                    close(i);
                    FD_CLR(i, &master_set);
                    if (i == max_sd)
                    {
                        while (FD_ISSET(max_sd, &master_set) == FALSE)
                            max_sd -= 1;
                    }
                }
                } /* End of existing connection is readable */
            } /* End of if (FD_ISSET(i, &working_set)) */
        } /* End of loop through selectable descriptors */

    } while (end_server == FALSE);

    for (i=0; i <= max_sd; ++i)
    {
        if (FD_ISSET(i, &master_set))
            close(i);
    }
}
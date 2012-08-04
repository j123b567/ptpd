/**
 * @file        MgmtMsgClient.cpp
 * @author      Tomasz Kleinschmidt
 * 
 * @brief       Main file for the PTPd Management Message Client.
 * 
 * Used as an interface between a user and the application.
 */

#include "MgmtMsgClient.h"

#include <cstdlib>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "OptBuffer.h"

#include "client.h"
#include "constants.h"
#include "datatypes_dep.h"

using namespace std;

/* Flag set by ‘--verbose’. */
int verbose_flag = 0;

/**
 * @brief Main function.
 * 
 * The main function is responsible for parsing arguments passed by a user
 * and passing them on.
 */
int main(int argc, char** argv) {
    int c;
    OptBuffer* optBuf = new OptBuffer(argv[0]);
    
    while (1)
    {
        static struct option long_options[] =
        {
            /* These options set a flag. */
            {"verbose", no_argument,    &verbose_flag, 1},
            {"brief",   no_argument,    &verbose_flag, 0},
            
            /* These options don't set a flag.
             * We distinguish them by their indices. */
            {"action",          required_argument, 0, 'c'},
            {"address",         required_argument, 0, 'a'},
            {"help",            no_argument,       0, 'h'},
            {"interface",       required_argument, 0, 'i'},
            {"message",         required_argument, 0, 'm'},
            {"port",            required_argument, 0, 'p'},
            {"timeout",         required_argument, 0, 't'},
            {"value",           required_argument, 0, 'v'},
            {0, 0, 0, 0}
        };
           
        /* getopt_long stores the option index here. */
        int option_index = 0;
     
        c = getopt_long (argc, argv, "a:c:hi:m:p:t:v:", long_options, &option_index);
        
        /* Detect the end of the options. */
        if (c == -1)
            break;
     
        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                DBG("option %s", long_options[option_index].name);
                
                if (optarg)
                    DBG(" with arg %s\n", optarg);
                
               break;
     
            case 'a':
                DBG("option -a with value `%s'\n", optarg);
                memcpy(optBuf->u_address, optarg, MAX_ADDR_STR_LEN + 1);
                break;
                
            case 'c':
                DBG("option -c with value `%s'\n", optarg);
                optBuf->mgmtActionTypeParser(optarg);
                break;
                
            case 'h':
                DBG("option -h\n");
                optBuf->help_print = true;
                break;
                
            case 'i':
                DBG("option -i with value `%s'\n", optarg);
                optBuf->interface = (char*)calloc(strlen(optarg), sizeof(char));
                memcpy(optBuf->interface, optarg, strlen(optarg));
                optBuf->interface_set = true;
                break;
     
            case 'm':
                DBG("option -m with value `%s'\n", optarg);
                
                if (strcmp(optarg, "print") == 0)
                    optBuf->msg_print = true;
                else
                    optBuf->mgmtIdParser(optarg);
                
                break;
     
            case 'p':
                DBG("option -p with value `%s'\n", optarg);
                memcpy(optBuf->u_port, optarg, MAX_PORT_STR_LEN + 1);
                break;
                
            case 't':
                DBG("option -t with value `%s'\n", optarg);
                sscanf(optarg, "%u", &(optBuf->timeout));
                break;
                
            case 'v':
                DBG("option -v with value `%s'\n", optarg);
                optBuf->value.lengthField = strlen(optarg);
                optBuf->value.textField = (Octet*)calloc(strlen(optarg), sizeof(Octet));
                memcpy(optBuf->value.textField, optarg, strlen(optarg));
                optBuf->value_set = true;
                break;
     
            case '?':
                /* getopt_long already printed an error message. */
                break;
     
            default:
                abort ();
        }
    }
    
    /* Instead of reporting ‘--verbose’
     * and ‘--brief’ as they are encountered,
     * we report the final status resulting from them. */
    if (verbose_flag)
        DBG("verbose flag is set\n");
     
    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
    }
    
    mainClient(optBuf);

    return 0;
}

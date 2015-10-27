#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>
#define SLAVENAME "slave_hist"
int main(int argc, char *argv[])
{

    // Test the number of slaves to start
    printf ("Number of target slaves: \%s\n",argv[2]);
    printf("INPUT: %s\n", argv[1]);

    int mytid; // my task id
    int tids[32]; // slave task id
    int n, nproc, numt, i, nhost, narch, startP, endP;
       int length=0;
    int histogram[256], temp[256];
    int buff[100], imageVector[500];
    int nbOfElems=0;
    struct pvmhostinfo *hostp;

    // enroll in pvm
    mytid = pvm_mytid();

    // Set number of slaves to start
    pvm_config(&nhost, &narch, &hostp);
    nproc = nhost * atoi(argv[2]);
    if ( nproc > 32 ) nproc = 32;
        printf("Spawning %d worker tasks ...", nproc );

    // Start up slave tasks
    numt = pvm_spawn(SLAVENAME, (char**)0, 0, "", nproc, tids );
    if ( numt < nproc )
    {
        printf("\n Trouble spawning slaves. Aborting. Error codes are: \n");
        for ( i=numt; i<nproc; i++)
        {
            printf("TID %d %d\n", i, tids[i]);
        }
        for( i=0; i<numt; i++)
        {
            pvm_kill(tids[i]);
        }
        pvm_exit();
        exit(1);
    }
    printf("SUCCESSFUL\n");



    // Begin user Program
    char *imageFile;
    imageFile=argv[1];

    FILE *image;
    image = fopen(imageFile, "r");
    if (image==NULL)
    {
       fprintf(stderr, "[master]Can't open input file %s", argv[1]);
       exit(1);
    }
    else
    {
        int num;
        while(fscanf(image, "%d", &num) > 0)
        {
            printf("%d",num);
            printf("\n");
            imageVector[length]=num;
            length ++;
        }
    }
    printf ("\nLENGTccccH=%d\n", length);

    int offset = length / nproc;
    printf("OFFSET:%d\n", offset);


    for (i = 0; i < 256; i++)
    {
      histogram[i]=temp[i]=0;
    }

    for (i=0; i<nproc; i++)
    {
        startP=i*offset;
        if (i==nproc-1)
        {
            endP = length;
        } else
        {
            endP = startP+offset;
        }

        nbOfElems=endP-startP;

        int ii = 0, jj=0 ;
        for(ii = startP, jj=0; ii < endP; ii++, jj++)
        {
            buff[jj]=imageVector[ii];
        }

        printf("\n\nBUFFER CONTENT:\n\n");
        int var =0;
        for (var = 0; var < nbOfElems; ++var) {
            printf("BUFF[%d]=%d\n", var, buff[var]);

        }

        printf ("Sending hist part to slave %d\n", i);
        pvm_initsend(PvmDataDefault);
        pvm_pkint(&startP, 1, 1);
        pvm_pkint(&nbOfElems, 1, 1);
        pvm_pkint(buff, nbOfElems, 1);
        pvm_send(tids[i], 1);

    }


    for (i=0; i<nproc; i++)
    {
        printf("[master] Waiting for slave %d to do work\n", i);

        pvm_recv(tids[i], 1);
        pvm_upkint(&startP, 1, 1);
        pvm_upkint(&nbOfElems, 1, 1);
        pvm_upkint(temp, 256, 1);
        printf ("[master] Received from slave %d values %d, %d\n", i, startP, nbOfElems);

        int j = 0;
        for(j=0; j<256; j++)
        {
            histogram[j]+=temp[j];
        }
    }

    int j=0;
    printf("HISTOGRAM:\n\n");
    for(j=0; j<256; j++)
    {
        printf("HISTOGRAM[%d]=%d\n", j, histogram[j]);
    }

}
















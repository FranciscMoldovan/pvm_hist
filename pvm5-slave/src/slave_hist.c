#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>
int main()
{
printf("[slave] My tid=%x\n", pvm_mytid());

    int ptid = pvm_parent();
    int myNumber = 3;
    int startP, endP, nbOfElems;
    int buff[100];
    int histogram[256]={0};

    pvm_recv(ptid, 1);
    pvm_upkint(&startP, 1, 1);
    pvm_upkint(&nbOfElems, 1, 1);
    pvm_upkint(buff, nbOfElems, 1);
    printf("[slave] I got start from master\n");


    ////// Histogram creation
    ///
          int i=0;
          for(i=0; i<nbOfElems; i++)
          {
              histogram[buff[i]]++;
          }
    ///
    ///////////////////////////

    printf("[slave] Sending data back to master\n");

    pvm_initsend(PvmDataDefault);
    pvm_pkint(&startP, 1, 1);
    pvm_pkint(&nbOfElems, 1, 1);
    pvm_pkint(histogram, 256, 1);
    pvm_send(ptid, 1);


    pvm_exit();
    exit(0);
}




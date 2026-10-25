#include "generated/job.h"

void JobChange(int job) {
    JobMode = job;
}

void JobInit(void) {
    JobMode = 1;
}

void JobNext(void) {
    JobMode++;
}

void JobMain(void) {
    jobfunc[JobMode]();
}

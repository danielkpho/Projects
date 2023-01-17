#include <pthread.h>
int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock,
                        const pthread_rwlockattr_t *restrict attr);
pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;

void *downtime() {
    int i;
    Node *rootBalanced = NULL;
    for (i = 0; i < 3; i++) {
        sleep(2);
        pthread_rwlock_wrlock(&lock);
        rootBalanced = balanceTree(root);
        root = freeSubtree(root);
        root = rootBalanced;
        pthread_rwlock_unlock(&lock);
    }
    return 0;
}

void *ServeClient(char *clientCommands) {
    FILE * fp = fopen(clientCommands, "r");
    if (fp == NULL) {
        printf("Error opening file %s", clientCommands);
        return NULL;
    }
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char * command;
        char * token;
        char * rest = line;
        token = strtok_r(rest, " ", &rest);
        command = token;
        int data;
        if (strcmp(command, "addNode") == 0) {
            token = strtok_r(rest, " ", &rest);
            data = atoi(token);
            pthread_rwlock_wrlock(&lock);
            root = addNode(root, data);
            pthread_rwlock_unlock(&lock);
            printf("[%s]addNode %d\n", clientCommands, data);
        } else if (strcmp(command, "removeNode") == 0) {
            token = strtok_r(rest, " ", &rest);
            data = atoi(token);
            pthread_rwlock_wrlock(&lock);
            root = removeNode(root, data);
            pthread_rwlock_unlock(&lock);
            printf("[%s]removeNode %d\n", clientCommands, data);
        } else if (strcmp(command, "countNodes\n") == 0) {
            pthread_rwlock_rdlock(&lock);
            int count = countNodes(root);
            pthread_rwlock_unlock(&lock);
            printf("[%s]countNodes = %d\n", clientCommands, count);
        } else if (strcmp(command, "avgSubtree\n") == 0) {
            pthread_rwlock_rdlock(&lock);
            float sum = avgSubtree(root);
            pthread_rwlock_unlock(&lock);
            printf("[%s]avgSubtree = %f\n", clientCommands, sum);
        }
    }
    fclose(fp);
    return NULL;
}




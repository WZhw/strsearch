#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "strsearch.h"

FILE *pattern_file, *string_file, *result_file;

Pa Patterns[MAXPaN];
char read_pattern_buff[MAXPa];
char read_obj_buff[MAXObj];

int main(int argc, char* argv[]){
    clock_t start, finish;
    double  duration;
    start = clock();

    if(argc != 4){
        printf("Arguments don't agree.\n");
        return -1;
    }

    pattern_file = fopen(argv[1], "r");
    if(pattern_file == NULL){
        printf(argv[1]);
        return -1;
    }
    printf("Pattern file read successful.\n");

    AC_STRUCT *ac_tree = ac_alloc();
    if(ac_tree == NULL){
        printf("AC_TREE allocate unsuccessful.\n");
        return -1;
    }
    printf("AC_TREE allocate successful.\n");

    //ac_construct_by_file(ac_tree, pattern_file);
    int id = 1, posi = 0, patternLength;
    while(!feof(pattern_file)){
        memset(read_pattern_buff, 0, MAXPa * sizeof(char));
        fgets(read_pattern_buff, MAXPa, pattern_file);
        patternLength = strlen(read_pattern_buff) - 1;
        Pa p = malloc(sizeof(Pattern));
        for(posi = 0; posi < patternLength; posi++){
            p->P[posi] = read_pattern_buff[posi];
        }
        p->P[patternLength] = '\0';
        p->length = patternLength;
        Patterns[id] = p;

        ac_add_string(ac_tree, read_pattern_buff, patternLength, id);
        id++;
    }
    ac_prep(ac_tree);
    printf("Construct ac successful.\n");



    string_file = fopen(argv[2], "r");
    if(string_file == NULL){
        printf(argv[2]);
        return -1;
    }
    printf("String file read successful.\n");

    result_file = fopen(argv[3], "w");
    if(result_file == NULL){
        printf(argv[3]);
    }
    printf("Result file create successful.\n");

    //ac_search_by_file(ac_tree, result_file, string_file);
    while(!feof(string_file)){
        memset(read_obj_buff, 0, MAXObj * sizeof(char));
        fgets(read_obj_buff, MAXObj, string_file);
        ac_search_init(ac_tree, read_obj_buff, strlen(read_obj_buff));
        ac_search(ac_tree);
    }

    ac_free(ac_tree);

    if(fclose(result_file) != 0){
        printf("fclose error in result file.\n");
    }

    if(fclose(string_file) != 0){
        printf("fclose error in string file.\n");
    }

    if(fclose(pattern_file) != 0){
        printf("fclose error in pattern file.\n");
    }

    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    printf( "Processed in %f seconds\n", duration );

    return 0;
}


/*
 * ac_alloc
 *
 * Creates a new AC_STRUCT structure and initializes its fields.
 *
 * Parameters:    none.
 *
 * Returns:  A dynamically allocated AC_STRUCT structure.
 */
AC_STRUCT *ac_alloc(void){
    AC_STRUCT *node;

    if ((node = malloc(sizeof(AC_STRUCT))) == NULL){
        return NULL;
    }

    memset(node, 0, sizeof(AC_STRUCT));

    if ((node->root = malloc(sizeof(ACTREE_NODE))) == NULL) {
        free(node);
        return NULL;
    }

    memset(node->root, 0, sizeof(ACTREE_NODE));

    return node;
}


/*
 * ac_add_string
 *
 * Adds a string to the AC_STRUCT structure's keyword tree.
 *
 * NOTE:  The `id' value given must be unique to any of the strings
 *        added to the tree, and must be a small integer greater than
 *        0 (since it is used to index an array holding information
 *        about each of the strings).
 *
 *        The best id's to use are to number the strings from 1 to K.
 *
 * Parameters:   node      -  an AC_STRUCT structure
 *               P         -  the sequence
 *               M         -  the sequence length
 *               id        -  the sequence identifier
 *
 * Returns:  non-zero on success, zero on error.
 */
int ac_add_string(AC_STRUCT *node, char *P, int M, int id){
    int i = 0, flag = 0;
    AC_TREE preNode, currentNode, newNode;

    if (id == 0 || id <= node->patternNum){
        return 0;
    }

    if(node->patternNum == 0){
        currentNode = node->root;
        for(; i < M; i++){
            if((newNode = malloc(sizeof(ACTREE_NODE))) == NULL){
                return -1;
            }
            memset(newNode, 0, sizeof(ACTREE_NODE));
            newNode->data = P[i];
            if(i == M-1){
                newNode->stateId = id;
            }
            else{
                newNode->stateId = 0;
            }
            currentNode->children = newNode;
            currentNode = currentNode->children;
        }
        node->patternNum++;
        return 1;
    }

    currentNode = preNode = node->root->children;
    while(currentNode != NULL && i < M){
        preNode = currentNode;
        if(P[i] == currentNode->data){
            i++;
            currentNode = currentNode->children;
            flag = 0;
        }
        else{
            currentNode = currentNode->sibling;
            flag = 1;
        }
    }

    if(i == M){
        return 0;
    }

    if((newNode = malloc(sizeof(ACTREE_NODE))) == NULL){
        return -1;
    }
    memset(newNode, 0, sizeof(ACTREE_NODE));
    newNode->data = P[i];
    if(i == M-1){
        newNode->stateId = id;
    }
    else{
        newNode->stateId = 0;
    }

    currentNode = preNode;
    if(flag == 0){
        currentNode->children = newNode;
        currentNode = currentNode->children;
    }
    else{
        currentNode->sibling = newNode;
        currentNode = currentNode->sibling;
    }
    i++;

    for(; i < M; i++){
        if((newNode = malloc(sizeof(ACTREE_NODE))) == NULL){
            return -1;
        }
        memset(newNode, 0, sizeof(ACTREE_NODE));
        newNode->data = P[i];
        if(i == M-1){
            newNode->stateId = id;
        }
        else{
            newNode->stateId = 0;
        }
        currentNode->children = newNode;
        currentNode = currentNode->children;
    }
    node->patternNum++;
    return 1;
}

/*
 * ac_prep
 *
 * Compute the failure and output links for the keyword tree.
 *
 * Parameters:  node  -  an AC_STRUCT structure
 *
 * Returns: non-zero on success, zero on error.
 */
int ac_prep(AC_STRUCT *node){
    printf("Add pattern strings successful.\n");
    AC_TREE root, currentNode, preNode, stateNode, faillinkNode;
    root = node->root;
    int flag;
    Queue *travQueue, *siblingQueue, *faillinkQueue;
    travQueue = queue_init();
    siblingQueue = queue_init();
    faillinkQueue = queue_init();
    if(root->children != NULL){
        currentNode = root->children;
        if(!enQueue(siblingQueue, currentNode)){
            return 0;
        }
        while(!isEmpty(siblingQueue)){
            currentNode = deQueue(siblingQueue);
            currentNode->faillink = root;
            enQueue(travQueue, currentNode);
            if(currentNode->sibling != NULL){
                enQueue(siblingQueue, currentNode->sibling);
            }
        }

        while(!isEmpty(travQueue)){
            preNode = deQueue(travQueue);
            if(preNode->children != NULL){
                enQueue(siblingQueue, preNode->children);
                while(!isEmpty(siblingQueue)){
                    currentNode = deQueue(siblingQueue);
                    if(currentNode->sibling != NULL){
                        enQueue(siblingQueue, currentNode->sibling);
                    }
                    enQueue(travQueue, currentNode);

                    flag = 1;
                    stateNode = preNode;
                    while(flag){
                        stateNode = stateNode->faillink;
                        if(stateNode->children != NULL){
                            enQueue(faillinkQueue, stateNode->children);
                            while(!isEmpty(faillinkQueue)){
                                faillinkNode = deQueue(faillinkQueue);
                                if(faillinkNode->sibling != NULL){
                                    enQueue(faillinkQueue, faillinkNode->sibling);
                                }
                                if(faillinkNode->data == currentNode->data){
                                    currentNode->faillink = faillinkNode;
                                    if(faillinkNode->stateId != 0){
                                        currentNode->outlink = faillinkNode;
                                    }
                                    flag = 0;
                                }
                            }
                        }
                        if(stateNode==root && flag ==1){
                            currentNode->faillink = root;
                            break;
                        }
                    }//while(flag)
                }//while(!empty(siblingQueue))
            }//if(preNode->children != NULL)
        }//while(!empty(travQueue))
    }
    return 1;
}

/** ac_construct_by_file
 *
 * \param
 * \param
 * \return
 *
 */
inline void ac_construct_by_file(AC_STRUCT *node, FILE *pattern){
    int id = 1, posi = 0, patternLength;
    while(!feof(pattern)){
        memset(read_pattern_buff, 0, MAXPa * sizeof(char));
        fgets(read_pattern_buff, MAXPa, pattern);
        patternLength = strlen(read_pattern_buff) - 1;
        Pa p = malloc(sizeof(Pattern));
        for(posi = 0; posi < patternLength; posi++){
            p->P[posi] = read_pattern_buff[posi];
        }
        p->P[patternLength] = '\0';
        p->length = patternLength;
        Patterns[id] = p;

        ac_add_string(node, read_pattern_buff, patternLength, id);
        id++;
    }
    ac_prep(node);
    printf("Construct ac successful.\n");
}



/*
 * ac_search_init
 *
 * Initializes the variables used during an Aho-Corasick search.
 * See ac_search for an example of how it should be used.
 *
 * Parameters:  node  -  an AC_STRUCT structure
 *              T     -  the sequence to be searched
 *              N     -  the length of the sequence
 *
 * Returns:  nothing.
 */
void ac_search_init(AC_STRUCT *node, char *objString, long long int objStringLength){
    node->topOfLine = node->topOfLine + node->objStringLength;
    node->objStringLength = objStringLength;
    node->objString = objString;
    node->posInLine = 0;
}


/*
 * ac_search
 * Parameters:  node           -  a preprocessed AC_STRUCT structure
 *              length_out     -  where to store the new match's length
 *              id_out         -  where to store the identifier of the
 *                                pattern that matched
 *
 * Returns:  the left end of the text that matches a pattern, or NULL
 */
void ac_search(AC_STRUCT *node){
    long long int pos;
    char *objString = node->objString;
    AC_TREE currentStateNode, childNode;
    if(node->topOfLine == 0){
        node->currentState = node->root;
    }
    currentStateNode = node->currentState;
    for(pos = 0; pos < node->objStringLength; pos++){
        childNode = currentStateNode->children;
        while(currentStateNode != node->root || childNode != NULL){
            if(childNode == NULL){
                currentStateNode = currentStateNode->faillink;
                childNode = currentStateNode->children;
                continue;
            }
            if(childNode->data == objString[pos]){
                currentStateNode = childNode;
                node->currentState = childNode;
                node->posInLine = pos + 1;
                if(childNode->stateId != 0){
                    output(node);
                }
                break;
            }
            else{
                childNode = childNode->sibling;
            }
        }
    }
}

/** ac_search_by_file
 *
 * \param
 * \param
 * \return
 *
*/

inline void ac_search_by_file(AC_STRUCT *node, FILE *obj_string, FILE *result){
    while(!feof(obj_string)){
        memset(read_obj_buff, 0, MAXObj * sizeof(char));
        fgets(read_obj_buff, MAXObj, obj_string);
        ac_search_init(node, read_obj_buff, strlen(read_obj_buff));
        ac_search(node);
    }
}


/** output
 *
 * \param
 * \param
 * \return
 *
*/
void output(AC_STRUCT *node){
    AC_TREE currentNode;
    currentNode = node->currentState;
    while(currentNode != NULL){
        fprintf(result_file, "%s  %I64d\n", \
                Patterns[currentNode->stateId]->P, \
                node->topOfLine + node->posInLine - Patterns[currentNode->stateId]->length
                );
        currentNode = currentNode->outlink;
    }
}




/*
 * ac_free
 *
 * Free up the allocated AC_STRUCT structure.
 *
 * Parameters:   node  -  a AC_STRUCT structure
 *
 * Returns:  nothing.
 */
void ac_free(AC_STRUCT *node){
    AC_TREE front, back, next;
    if (node == NULL){return;}
    if (node->root != NULL) {
        front = back = node->root;
        while (front != NULL) {
            back->sibling = front->children;
            while (back->sibling != NULL){
                back = back->sibling;
            }
            next = front->sibling;
            free(front);
            front = next;
        }
    }
    if (node->currentState != NULL){
        free(node->currentState);
    }
    if  (node->objString != NULL){
        free(node->objString);
    }
    free(node);
}



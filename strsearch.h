#ifndef STRSEARCH_H_INCLUDED
#define STRSEARCH_H_INCLUDED

#define MAXN 2560000
#define MAXPa 256
#define MAXObj 25600
#define MAXPaN 2560000

typedef struct actreenode {
    char data;
    int stateId;
    struct actreenode *outlink, *faillink;
    struct actreenode *children, *sibling;
} ACTREE_NODE, *AC_TREE;


typedef struct {
    AC_TREE root;
    AC_TREE currentState;
    long long int topOfLine, posInLine, objStringLength, patternNum;
    char *objString;
} AC_STRUCT;

typedef struct {
    char P[MAXPa];
    int length;
}Pattern, *Pa;

AC_STRUCT *ac_alloc(void);
int ac_add_string(AC_STRUCT *node, char *P, int M, int id);
int ac_prep(AC_STRUCT *node);
inline void ac_construct_by_file(AC_STRUCT *node, FILE *pattern);
void ac_search_init(AC_STRUCT *node, char *objString, long long int objStringLength);
void ac_search(AC_STRUCT *node);
inline void ac_search_by_file(AC_STRUCT *node, FILE *obj_string, FILE *result);
void output(AC_STRUCT *node);
void ac_free(AC_STRUCT *node);

typedef struct{
    AC_TREE queue[MAXN];
    int head, tail;
}Queue;

Queue *queue_init(){
    Queue *q;
	if((q = malloc(sizeof(Queue))) == NULL){
		return NULL;
	}
	memset(q, 0, sizeof(Queue));
	q->head = 0;
	q->tail = 0;
	return q;
}

int isEmpty(Queue *q){
    return ((q->head - q->tail) == 0) ? 1 : 0;
}

int isFull(Queue *q){
    return ((q->head - q->tail + MAXN)%MAXN == 1);
}

int enQueue(Queue *q, AC_TREE node){
    if(isFull(q)){
        return 0;
    }
    else{
        q->queue[q->tail] = node;
        q->tail = (q->tail + 1) % MAXN;
        return 1;
    }
}

AC_TREE deQueue(Queue *q){
    if(isEmpty(q)){
        return NULL;
    }
    else{
        int temp = q->head;
        q->head = (q->head + 1) % MAXN;
        return q->queue[temp];
    }
}

#endif // STRSEARCH_H_INCLUDED

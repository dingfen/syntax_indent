#include "json.h"

void initJSON(JSONobj *pj)
{
    pj->key = "Head Of List";
    if (pj->next)
        pj->next = NULL;
}

void addJSON(JSONobj *pj, const char* key, Value val, State s)
{
    JSONobj *p;
    if (!pj)
        return ;
    
    while(pj->next)
        pj = pj->next;
    p = pj->next = (JSONobj *)malloc(sizeof(JSONobj));
    p->key = key;
    p->state = s;
    if (s == V_JSON) {
        p->value.obj = (JSONobj *)malloc(sizeof(JSONobj));
        memcpy(p->value.obj, val.obj, sizeof(JSONobj));
    }
    else 
        p->value = val;
    p->next = NULL;
}

void printJSON(JSONobj *obj)
{
    obj = obj->next;
    while (obj)
    {
        printf("\"%s\" : ", obj->key);
        switch (obj->state)
        {
        case V_CHAR:
            printf("\'%c\'\n", obj->value.ch);
            break;
        case V_INT:
            printf("%d\n", obj->value.data);
            break;
        case V_STR:
            printf("\"%s\"\n", obj->value.str);
            break;
        case V_JSON:
            printf(" {\n");
            printJSON(obj->value.obj); 
            printf("}\n");
            break;
        }
        obj = obj->next;
    }
}

void destroyJSON(JSONobj *pj)
{
    JSONobj *p, *q;
    p = pj->next;
    while (p)
    {
        q = p->next;
        if (p->state == V_JSON) {
            destroyJSON(p->value.obj);
            free(p->value.obj);
        }
        free(p);
        p = q;
    }
    return ;
}

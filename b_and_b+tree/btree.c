#include<stdio.h>
#include<stdlib.h>
#define M 6
#define DEGREE (M/2)
static long zero;
typedef int KEY_TYPE;
typedef struct btree_node{
    KEY_TYPE *keys;
    //指针的数组而不是二维数组
    struct btree_node **childrens;//malloc 好处在于sizeof不会特别大
    int num;
    int isleaf;//是否叶子节点
                //事实上int和bool是一样的，因为边界对齐
} btree_node;

struct btree{
    struct btree_node *root;
    //这个T存储M/2
    int t;
};

struct btree_node* btree_create_node(int t,int leaf){
    struct btree_node *node = (struct btree_node *) calloc(1,sizeof(struct btree_node));
    if(node == NULL){
        return NULL;
    }
    node->num = 0;
    node->keys = (KEY_TYPE *)calloc(1,(2*t -1)*sizeof(KEY_TYPE));
    node->childrens = (struct btree_node**)calloc(1,2*t*sizeof(struct btree_node *));
    node->isleaf = leaf;
    return node;
}

void btree_destory_node(struct btree_node *node){
    if(node){
        if(node->keys){
            free(node->keys);
        }
        if(node->childrens){
            free(node->childrens);
        }
        free(node);
    }
}


void btree_split_child(struct btree *T,struct btree_node *x ,int son_number){
    struct btree_node *y = x->childrens[son_number];
    //1
    btree_node *z = btree_create_node(T->t,y->isleaf);
    for(int j = 0 ; j < T->t -1;++j){
        z->keys[j] = y->keys[j + T->t];
    }
    if(!y->isleaf)
    {
        for(int j = 0;j < T->t;++j){
            z->childrens[j] = y->childrens[j + T->t];
        }
    }
    //num存储有几个key了
    y->num = T->t - 1;
    //为向上插入的索引节点空出一个位置来
    for(int j = x->num;j >= son_number+1;--j ){
        x->childrens[j + 1] = x->childrens[j];
    }
    x->childrens[son_number + 1] = z;
}

//插入一个不满的节点的情况
void btree_insert_nonfull(struct btree * T,btree_node *x,KEY_TYPE value){
    int i = x->num;
    if(x->isleaf){
        //递归出口
        while(i>=0 && x->keys[i] > value){
            x->keys[i + 1] = x->keys[i];
            i--;
        }
        x->keys[i+1] = value;
        x->num++;
    }
    else{
        //找到子树对应位置
        while(i>= 0 && x->keys[i]>value){
            i--;
        }
        //子树满的情况
        //进行分裂
        if(x->childrens[i + 1]->num == 2*T->t - 1){
            btree_split_child(T,x,i+1);
            if(value > x->keys[i+1]){
                i++;
            }
        }
        btree_insert_nonfull(T,x->childrens[i],value);
    }
}


//所有的节点全部插入到叶子节点
//先找节点
//两种情况，根节点以及非根节点
void btree_insert(struct btree * T,KEY_TYPE key)
{
    struct btree_node *r = T->root;
    if(r->num == T->t *2 -1){
        struct btree_node *newNode = btree_create_node(T->t,0);
        T->root = newNode;
        newNode->childrens[0] = r;
        btree_split_child(T,newNode,0);
        int i = 0;
        if(newNode->keys[0] < key){
            i++;
        }
        btree_insert_nonfull(T,newNode->childrens[i],key);
    }
    else{
        btree_insert_nonfull(T,r,key);
    }
}

void btree_delete_key(struct btree *T,btree_node *x ,KEY_TYPE value){
    int idx = 0;
    while(idx < x->num && x->keys[idx] ){
        idx++;
    }
    if(value == x->keys[idx]){

    }
}

void btree_create(struct btree * T,int t){
    T->t = t;
    T->root = btree_create_node(t,1);
}


int main(){
    return 0;
}


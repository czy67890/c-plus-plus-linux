#include<linux/sysctl.h>
#define M 6
#define DEGREE (M/2)
static long zero;
typedef int KEY_VALUE;
struct btree_node{
    KEY_VALUE *keys;
    //ָ�����������Ƕ�ά����
    struct btree_node **childrens;//malloc �ô�����sizeof�����ر��
    int num;
    bool isleaf;//�Ƿ�Ҷ�ӽڵ�
                //��ʵ��int��bool��һ���ģ���Ϊ�߽����
};

struct btree{
    struct btree_node *root;
};

struct btree_node *btree_create_node(struct btree * T){

}
void btree_split_child(struct btree *T,struct btree_node *x ,int son_number){
    struct btree_node *y = x->childrens[son_number];
    //1
    btree_node *z = (btree_node *)malloc( sizeof(btree_node ) * DEGREE);
    for(int j = 0 ; j < DEGREE - 1;++j){
        z->keys[j] = y->keys[j];
    }
    if(!y->isleaf)
    {
        for(int j = 0;j < DEGREE;++j){
            z->childrens[j] = y->childrens[j];
        }
    }
    y->num = DEGREE;

}

void btree_insert_nonfull(struct btree * T,btree_node *X,KEY_VALUE value){
    int i = x->num;
    if(x->isleaf){

    }
    else{
        //�ҵ�������Ӧλ��
        while(i>= 0 && x->keys[i]>k){
            i--;
        }
        //�����������
        //���з���
        if(x->childrens[i + 1]->num == M - 1){
            btree_split_child(T,x,i+1);
        }
        btree_insert_nonfull(T,x->childrens[i],value);
    }
}

void btree_insert(struct btree * T,KEY_VALUE key)
{
    struct btree_node *r = T->root;
    if(r->num == M-1){
        struct btree_node *newNode = btree_create_node(T);
        T->root = newNode;
        newNode->childrens[0] = r;
        btree_split_child(T,newNode,0);
    }
    else{

    }
}

void btree_delete_key(struct btree *T,btree_node *x KEY_VALUE value){
    int idx = 0;
    while(idx <node->num && node->keys[idx] ){
        idx++;
    }
    if(key == node->keys[]){
        
    }
}







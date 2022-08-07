typedef int KEY_TYPE;

int key_compare(KEY_TYPE a,KEY_TYPE b){
    return a<b;
}
#define RED 0
#define BLACK 1

typedef struct _rb_node{
    unsigned char color;
    struct _rb_node *left;
    struct _rb_node *right;
    struct _rb_node *parent;
    KEY_TYPE key;
    void *value;
}rbtree_node;

typedef struct _rb_tree{
    rbtree_node *root;
    rbtree_node *nil;
}rb_tree;


void _left_rotate(rb_tree* T,rbtree_node *x){
    //����������������ָ��ı�
    rbtree_node * y = x->right;
    //��һ������
    x->right = y->left;
    //y������������Ҷ�ڵ�����
    if(y->left != T->nil){
        y->left->parent = x;
    }
    y->parent = x->parent;
    //xΪ�������
    if(x->parent == T->nil){
        T->root = y;
    }
    else if(x == x->parent->left){
        x->parent->left = y;
    }
    else{
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void _right_rotate(rb_tree*T,rbtree_node *y){
    rbtree_node * x = y->left;
    y->left = x->right;
    //y������������Ҷ�ڵ�����
    if(x->right != T->nil){
        x->right->parent = y;
    }
    x->parent = y->parent;
    //xΪ�������
    if(x->parent == T->nil){
        T->root = x;
    }
    else if(y == y->parent->left){
        y->parent->left = x;
    }
    else{
        y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
}
//���Ҳ��뵱ǰ�ڵ�ǰ�϶���һ�ź����
//����ֻ���ĸ��ڵ���游�ڵ��Լ���ڵ�
//�ȴӸ��ڵ�����������ʼ˼��
//��������������ǶԳƵ�
void rbtree_insert_fixup(rb_tree* T,rbtree_node *z){
    //ֻ�и��ڵ��Ǻ�ɫ�ſ��ܻ����Υ����������ʵ����
    //���ʱ���游�ڵ�һ���Ǻ�ɫ���ʲ���Ҫ����
    //���ϵĵ������ϵݹ�
    while(z->parent->color == RED){
        //�常�ڵ��Ǻ�ɫ
        //�常�ڵ��Ǻ�ɫ(��Ҷ�ڵ�)����ǰ�ڵ㸸�ڵ�������
        //�常�ڵ��Ǻ�ɫ����ǰ�ڵ㸸�ڵ�������
        if(z->parent == z->parent->parent->left){
            rbtree_node *y = z->parent->parent->right;
            //�常Ϊ������
            if(y->color == RED){
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                //�������ϲ�ѯ
                z = z->parent->parent;
            }
            else{
                //ͳһ��ֻ��һ���нڵ����ʽ
                if(z == z->parent->right){
                    z = z->parent;
                    _left_rotate(T,z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                _right_rotate(T,z->parent);
            }
        }
        else{
            rbtree_node * y = z->parent->parent->left;
            if(y->color == RED){
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else{
                if(z == z->parent->left){
                    z = z->parent;
                    _right_rotate(T,z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                _rleft_rotate(T,z->parent);
            }
        }  
    }
}


void rbtree_insert(rb_tree* T,rbtree_node *z){
    //yʼ����x�ĸ��ڵ�
    rbtree_node *y = T->nil;
    rbtree_node *x = T->root;
    while(x != T->nil){
        if(z->key < x->key){
            y = x;
            x = x->left;
        }
        else if(z->key > x->key){
            y = x;
            x = x->right;
        }
        else{
            //��ȵ������������
            return;
        }
    }
    z->parent = y;
    //����ڵ��Ǹ��ڵ�������������ǰ��Ϊ�յ������
    if(y == T->nil){
        T->root = z;
    }
    else if(z->key < y->key){
        y->left = z;
    }
    else{
        y->right = z;
    }
    z->left = T->nil;
    z->right = T->nil;
    //�Ϻ�ɫ
    z->color = RED;
    rbtree_insert_fixup(T,z);
}






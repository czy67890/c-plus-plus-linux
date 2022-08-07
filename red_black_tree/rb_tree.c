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
    //左旋三个方向，六个指针改变
    rbtree_node * y = x->right;
    //第一个方向
    x->right = y->left;
    //y的左子树不是叶节点的情况
    if(y->left != T->nil){
        y->left->parent = x;
    }
    y->parent = x->parent;
    //x为根的情况
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
    //y的左子树不是叶节点的情况
    if(x->right != T->nil){
        x->right->parent = y;
    }
    x->parent = y->parent;
    //x为根的情况
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
//并且插入当前节点前肯定是一颗红黑树
//调整只关心父节点和祖父节点以及叔节点
//先从父节点是左子树开始思考
//是右子树的情况是对称的
void rbtree_insert_fixup(rb_tree* T,rbtree_node *z){
    //只有父节点是红色才可能会出现违背红黑树性质的情况
    //这个时候祖父节点一定是黑色，故不需要考虑
    //不断的迭代往上递归
    while(z->parent->color == RED){
        //叔父节点是红色
        //叔父节点是黑色(即叶节点)，当前节点父节点左子树
        //叔父节点是黑色，当前节点父节点右子树
        if(z->parent == z->parent->parent->left){
            rbtree_node *y = z->parent->parent->right;
            //叔父为红的情况
            if(y->color == RED){
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                //继续向上查询
                z = z->parent->parent;
            }
            else{
                //统一成只有一边有节点的形式
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
    //y始终是x的父节点
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
            //相等的情况不做处理
            return;
        }
    }
    z->parent = y;
    //插入节点是根节点的情况（即插入前树为空的情况）
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
    //上红色
    z->color = RED;
    rbtree_insert_fixup(T,z);
}






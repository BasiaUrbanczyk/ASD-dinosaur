#include <iostream>
#include <cstdio>
#include <array>
#include <ctime>
using namespace std;

struct item {
    int prior, value, cnt;
    int max, leftCount, rightCount; //info about max substring in subtree, predecessor and successor
    int leftValue, rightValue; //values of the predecessor and successor
    int farLeftCount, farRightCount; //lenght of the maxleft substring with identical values
    int farRightValue, farLeftValue; //farest value on the left/on the right
    item *l, *r;
    bool rev;
    item () = default;
    explicit item (int value) : value(value), cnt(1), rev(false), prior(rand()), l(NULL), r(NULL) { }
    explicit item (int value, int cnt) : value(value), cnt(cnt), rev(false), prior(rand()), l(NULL), r(NULL) { }
};

typedef item* pitem;


int cnt (pitem it) {
    return it ? it->cnt : 0;
}

void upd_cnt (pitem it) {
    if (it)
        it->cnt = cnt(it->l) + cnt(it->r) + 1;
}

void heapify (pitem t) {
    if (!t) return;
    pitem max = t;
    if (t->l != NULL && t->l->prior > max->prior)
        max = t->l;
    if (t->r != NULL && t->r->prior > max->prior)
        max = t->r;
    if (max != t) {
        swap (t->prior, max->prior);
        heapify (max);
    }
}

pitem build (int * a, int n) {
    if (n == 0) return NULL;
    int mid = n / 2;
    pitem t = new item (a[mid]);
    t->l = build (a, mid);
    t->r = build (a + mid + 1, n - mid - 1);
    heapify (t);
    upd_cnt(t);
    return t;
}

int maxCount(pitem t){ //counting the max consistent subarray in my subtree
    if(t == NULL){
        return 0;
    }
    if(t->l == NULL && t->r == NULL){
        return 1;
    }

    int mySubstring = 0;
    int maxTemp;

    if(t->l == NULL){
        if(t->rightValue == t->value){
            maxTemp = max(t->rightCount + 1, t->r->max);
            return maxTemp;
        }
        return(max(t->r->max, 1));
    }

    if(t->r == NULL){
        if(t->leftValue == t->value){
            maxTemp = max(t->leftCount + 1, t->l->max);
            return maxTemp;
        }
        return(max(t->l->max, 1));
    }


    if(t->value == t->rightValue){ //checking if the value of my successor is the same as mine, if so adding the number of next elements with the same value
        mySubstring += t->rightCount;
    }

    if(t->value == t->leftValue){
        mySubstring += t->leftCount;
    }

    maxTemp = max(t->l->max, t->r->max);
    return max(maxTemp, mySubstring + 1); //adding 1 to mySubstring, bc we have to add me
}


int farRightCountFunc(pitem t){ //counting the farRightCount based on my childrens values

    if(t->r->rev){
        if(t->r->l == NULL){
            if(t->r->leftValue == t->r->value){
                return t->r->leftCount + 1;
            }
            return 1;
        }
    }
    else{
        if(t->r->r == NULL){ //right child doesnt have its right child, so its the last element in this subtree
            if(t->r->leftValue == t->r->value){
                return t->r->leftCount + 1;
            }
            return 1;
        }
    }

    int helpme;
    if(!t->r->rev){
        helpme = t->r->r->cnt;
    }
    else if(t->r->l != NULL){
        helpme = t->r->l->cnt;
    }
    else{
        helpme = 0;
    }

    if(t->r->farRightCount == helpme && t->r->value == t->r->farRightValue){ //the whole right subtree od the right child have the same value
        if(t->r->value == t->r->leftValue){
            return t->r->farRightCount + 1 + t->r->leftCount;
        }
        return t->r->farRightCount + 1;
    }

    return t->r->farRightCount;
}

int farLeftCountFunc(pitem t){ //counting the farLeftCount based on my childrens values

    if(t->l->rev){
        if(t->l->r == NULL){
            if(t->l->rightValue == t->l->value){
                return t->l->rightCount + 1;
            }
            return 1;
        }
    }
    else{
        if(t->l->l == NULL){
            if(t->l->rightValue == t->l->value){
                return t->l->rightCount + 1;
            }
            return 1;
        }
    }

    int helpme;
    if(!t->l->rev){
        helpme = t->l->l->cnt;
    }
    else if(t->l->r != NULL){
        helpme = t->l->r->cnt;
    }
    else{
        helpme = 0;
    }

    if(t->l->farLeftCount == helpme && t->l->value == t->l->farLeftValue){
        if(t->l->value == t->l->rightValue){
            return t->l->farLeftCount + 1 + t->l->rightCount;
        }
        return t->l->farLeftCount + 1;
    }
    return t->l->farLeftCount;
}


int rightCountFunction(pitem t){ //counting the rightCount value
    int value = t->r->farLeftValue;

    if(t->r->rev){
        if(t->r->r == NULL){
            if(t->r->rightValue == value){
                return 1 + t->r->rightCount;
            }
            return 1;
        }
    }
    else{
        if(t->r->l == NULL){ //right child doesnt have its left child
            if(t->r->rightValue == value){
                return 1 + t->r->rightCount;
            }
            return 1;
        }
    }

    int helpme;
    if(!t->r->rev){
        helpme = t->r->l->cnt;
    }
    else if(t->r->r != NULL){
        helpme = t->r->r->cnt;
    }
    else{
        helpme = 0;
    }
    if(t->r->value == value && helpme == t->r->farLeftCount){
        if(t->r->rightValue == value){
            return t->r->farLeftCount + 1 + t->r->rightCount;
        }
        return t->r->farLeftCount + 1;
    }

    return t->r->farLeftCount;
}


int leftCountFunction(pitem t){ //counting the value of leftValue based on childrens values
    int value = t->l->farRightValue;

    if(t->l->rev){
        if(t->l->l == NULL){
            if(t->l->leftValue == value){
                return 1 + t->l->leftCount;
            }
            return 1;
        }
    }
    else{
        if(t->l->r == NULL){ //left child doesnt have its right child
            if(t->l->leftValue == value){
                return 1 + t->l->leftCount;
            }
            return 1;
        }
    }

    int helpme;
    if(!t->l->rev){
        helpme = t->l->r->cnt;
    }
    else if(t->l->l != NULL){
        helpme = t->l->l->cnt;
    }
    else{
        helpme = 0;
    }
    if(t->l->value == value && helpme == t->l->farRightCount){
        if(t->l->leftValue == value){
            return t->l->farRightCount + 1 + t->l->leftCount;
        }
        return t->l->farRightCount + 1;
    }

    return t->l->farRightCount;
}

void valueSwap(pitem t){ //swapping the additional values
    if(t == NULL){
        return;
    }
    swap(t->leftValue, t->rightValue);
    swap(t->leftCount, t->rightCount);
    swap(t->farLeftValue, t->farRightValue);
    swap(t->farLeftCount, t->farRightCount);
}

void valueUpd(pitem t){ //updating the additional values
    if (t == NULL) {
        return;
    }
    if (t->l != NULL&& t->l->rev) {
        valueSwap(t->l);
    }
    if (t->r != NULL && t->r->rev) {
        valueSwap(t -> r);
    }



    if(t->l  == NULL && t->r == NULL){ //we are in the leaf, not used while creating the tree
        t->leftValue = t->value;
        t->rightValue = t->value;
        t->leftCount = 0;
        t->rightCount = 0;
        t->farRightValue = t->value;
        t->farLeftValue = t->value;
        t->farLeftCount = 0;
        t->farRightCount = 0;
        t->max = 1;
        return;
    }


    if(t->l == NULL){
        t->leftValue = t->value;
        t->leftCount = 0;
        t->rightValue = t->r->farLeftValue;
        t->rightCount = rightCountFunction(t);
        t->farLeftValue = t->value;
        t->farLeftCount = 0;
        t->farRightValue = t->r->farRightValue;
        t->farRightCount = farRightCountFunc(t);
        t->max = maxCount(t);
    }

    else if(t->r == NULL){
        t->rightValue = t->value;
        t->rightCount = 0;
        t->leftValue = t->l->farRightValue;
        t->leftCount = leftCountFunction(t);
        t->farLeftValue = t->l->farLeftValue;
        t->farLeftCount = farLeftCountFunc(t);
        t->farRightValue = t->value;
        t->farRightCount = 0;
        t->max = maxCount(t);
    }

    else{
        t->leftValue = t->l->farRightValue;
        t->leftCount = leftCountFunction(t);
        t->rightValue = t->r->farLeftValue;
        t->rightCount = rightCountFunction(t);
        t->farLeftValue = t->l->farLeftValue;
        t->farLeftCount = farLeftCountFunc(t);
        t->farRightValue = t->r->farRightValue;
        t->farRightCount = farRightCountFunc(t);
        t->max = maxCount(t);
    }


    if (t->l != NULL&& t->l->rev) {
        valueSwap(t->l);
    }
    if (t->r != NULL && t->r->rev) {
        valueSwap(t -> r);
    }
}


void valueUpdBeggining(pitem t){ //used only while creating the tree
    if(t == NULL){
        return;
    }

    if(t->l  == NULL && t->r == NULL){ //we are in the leaf
        t->leftValue = t->value;
        t->rightValue = t->value;
        t->leftCount = 0;
        t->rightCount = 0;
        t->farRightValue = t->value;
        t->farLeftValue = t->value;
        t->farLeftCount = 0;
        t->farRightCount = 0;
        t->max = 1;
        return;
    }

    valueUpdBeggining(t->l);
    valueUpdBeggining(t->r);

    valueUpd(t);
}

void push (pitem it) {
    if (it && it->rev) {
        it->rev = false;
        swap (it->l, it->r);
        valueSwap(it);
        if (it->l)  it->l->rev ^= true;
        if (it->r)  it->r->rev ^= true;
    }
}


void merge (pitem & t, pitem l, pitem r) {
    push (l);
    push (r);
    if (!l || !r)
        t = l ? l : r;
    else if (l->prior > r->prior){
        merge (l->r, l->r, r);
        t = l;
    }
    else{
        merge (r->l, l, r->l);
        t = r;
    }
    upd_cnt (t);
    valueUpd(t);
}


void split (pitem t, pitem & l, pitem & r, int key, int add = 0) {
    if (!t)
        return void( l = r = 0 );
    push (t);
    int cur_key = add + cnt(t->l);
    if (key <= cur_key)
        split (t->l, l, t->l, key, add),  r = t;
    else
        split (t->r, t->r, r, key, add + 1 + cnt(t->l)),  l = t;
    upd_cnt (t);
    valueUpd(t);
}

void reverse (pitem t, int l, int r) {
    if (r <= l) {
        return;
    }
    pitem t1, t2, t3;
    split (t, t1, t2, l);
    split (t2, t2, t3, r-l+1);
    if(t2 != NULL){
        t2->rev ^= true;
    }
    merge (t, t1, t2);
    merge (t, t, t3);
}

void output (pitem t) {
    if (!t)  return;
    push (t);
    output (t->l);
    printf ("%d ", t->value);
    output (t->r);
}

void shift2(pitem& treap, int j, int k, int l) { // inserting the substring from j to k between l-1 and l
    if(j == l){
        return;
    }
    pitem t1, t2, temp;
    split(treap, t1, t2, j);
    split(t2, t2, temp, k - j + 1);
    merge(treap, t1, temp);
    split(treap, t1, temp, l);
    merge(treap, t1, t2);
    merge(treap, treap, temp);
}

int count(pitem& treap, int j, int k){
    pitem t1, t2, t3;
    split (treap, t1, t2, j);
    split (t2, t2, t3, k-j+1);
    int res = t2->max;
    merge (treap, t1, t2);
    merge (treap, treap, t3);
    return res;
}



int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    srand(time(nullptr));

    int n, m;
    cin >> n >> m;
    int tab[n]; //A - 1, G - 2; C - 3, T - 4
    string help;
    cin >> help;
    for(int i = 0; i < n; i++){ //wczytywanie slowa do tablicy
        tab[i] = (int) help[i];
        switch (tab[i]){
            case 65:
                tab[i] = 1;
                break;
            case 71:
                tab[i] = 2;
                break;
            case 67:
                tab[i] = 3;
                break;
            case 84:
                tab[i] = 4;
                break;
        }
    }

    pitem treap = build(tab, n); //budowa treapa ehh
    valueUpdBeggining(treap); //updating the additional values of the enriched treap

    int j, k, l;
    for(int i = 0; i < m; i++){
        string plz;
        cin >> plz;
        int operation = (int) plz[0];

        if(operation == 79){ // O
            cin >> j >> k;
            j--; // zmniejszam bo indeksy sie nie licza od 0 tylko od 1
            k--;
            reverse(treap, j, k);
        }

        else if(operation == 80){ // P
            cin >> j;
            cin >> k;
            cin >> l;
            j--;
            k--;
            l--;
            shift2(treap, j, k, l);
        }

        else{ // N
            cin >> j;
            cin >> k;
            j--;
            k--;
            int res;
            if(j == k){
                res = 1;
            }
            else{
                res = count(treap, j, k);
            }
            cout << res << "\n";
        }
    }

    return 0;
}

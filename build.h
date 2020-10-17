#ifndef BUILD_H
#define BUILD_H
#include<stdio.h>
#include<stdlib.h>

#include<fcntl.h>
#include<string.h>


typedef struct _Node {
	
	char* str;
	int occ;
	struct _Node * left;
	struct _Node * right;

} heapnode;

heapnode * createNode( int occ, char*str){
	int size = strlen(str);
	heapnode * new = malloc(sizeof(heapnode));
	new->str = malloc((size*sizeof(heapnode)+1));
	new->str[size] = '\0';
	new->str = str;
	new-> occ = occ;
	new->left = NULL;
	new-> right = NULL;
	return new;

}




char *createBuff( char* file){
	
	int fp1 = open(file, O_RDONLY);
	//int fsize;
	off_t fsize = lseek(fp1, 0 ,SEEK_END ); //use lseek to find size of file
	close(fp1);

   	char *buff = malloc(fsize*sizeof(char) + 1); 
//	buff[fsize] = '\0';
	memset(buff, '\0', fsize);
	int readIn = 0 ;
	int status =0;
	int fd = open(file, O_RDONLY);
	
	do{
		status = read(fd, buff + readIn , fsize - readIn);
		readIn += status;

	}while (status > 0 && readIn < fsize);
	
	int i =0;

	return buff;

}


// An AVL tree node c\also used for huff
struct Node 
{ 
	int occ; 
	struct Node *left; 
	struct Node *right; 
    char * str;
	int height; 
}; 

// A utility function to get maximum of two integers 
int max(int a, int b); 

// A utility function to get the height of the tree 
int height(struct Node *N) 
{ 
	if (N == NULL) 
		return 0; 
	return N->height; 
} 

// A utility function to get maximum of two integers 
int max(int a, int b) 
{ 
	return (a > b)? a : b; 
} 

/* Helper function that allocates a new node with the given key and 
	NULL left and right pointers. */
struct Node* newNode( char* str) 
{ 

    int size = strlen(str);	
    struct Node* node = (struct Node*) malloc(sizeof(struct Node)); 

	node->str = malloc((size* sizeof(char))+1);
	node->str[size] = '\0';
	node->str = str;
//	node->key = key; 
	node->occ = 1;
	node->left = NULL; 
	node->right = NULL; 
	node->height = 1; // new node is initially added at leaf 
	return(node); 
} 

// A utility function to right rotate subtree rooted with y 
// See the diagram given above. 
struct Node *rightRotate(struct Node *y) 
{ 
	struct Node *x = y->left; 
	struct Node *T2 = x->right; 

	// Perform rotation 
	x->right = y; 
	y->left = T2; 

	// Update heights 
	y->height = max(height(y->left), height(y->right))+1; 
	x->height = max(height(x->left), height(x->right))+1; 

	// Return new root 
	return x; 
} 

// A utility function to left rotate subtree rooted with x 
// See the diagram given above. 
struct Node *leftRotate(struct Node *x) 
{ 
	struct Node *y = x->right; 
	struct Node *T2 = y->left; 

	// Perform rotation 
	y->left = x; 
	x->right = T2; 

	// Update heights 
	x->height = max(height(x->left), height(x->right))+1; 
	y->height = max(height(y->left), height(y->right))+1; 

	// Return new root 
	return y; 
} 

// Get Balance factor of node N 
int getBalance(struct Node *N) 
{ 
	if (N == NULL) 
		return 0; 
	return height(N->left) - height(N->right); 
} 

int search(struct Node *root, char*str) 
{ 
    struct Node * rootPtr = root;
	if(root != NULL) 
	{ 
//		printf("%d ", root->key); 
        if(strcmp(root->str, str) == 0){
                (root->occ)++;
		//		printf("same node\n");
               	return 1;
        }
		search(root->left, str); 
		search(root->right, str); 
	} 
	return 0;
} 


// Recursive function to insert a key in the subtree rooted 
// with node and returns the new root of the subtree. 
struct Node* insert(struct Node* node, char* str) 
{ 


	/* 1. Perform the normal BST insertion */
	if (node == NULL){
		return(newNode(str)); 
	}

 	if(strcmp(str, node->str) < 0)
		node->left = insert(node->left, str); 
	else if(strcmp(str, node->str) > 0)
		node->right = insert(node->right, str); 
	else if ((strcmp(node->str,str)== 0)){

		(node->occ)++;
	} else
		return node; 

	/* 2. Update height of this ancestor node */
	node->height = 1 + max(height(node->left), 
						height(node->right)); 

	/* 3. Get the balance factor of this ancestor 
		node to check whether this node became 
		unbalanced */
	int balance = getBalance(node); 

	// If this node becomes unbalanced, then 
	// there are 4 cases 

	// Left Left Case 

	if (balance > 1 && (strcmp(str, node->left->str) < 0))
		return rightRotate(node); 

	// Right Right Case 
	if (balance < -1 && (strcmp(str, node->right->str) > 0))
		return leftRotate(node); 

	// Left Right Case 
	if (balance > 1 && (strcmp(str, node->left->str) > 0))
	{ 
		node->left = leftRotate(node->left); 
		return rightRotate(node); 
	} 

	// Right Left Case 
	if (balance < -1 && (strcmp(str, node->right->str) < 0))
	{ 
		node->right = rightRotate(node->right); 
		return leftRotate(node); 
	} 

	/* return the (unchanged) node pointer */
	return node; 
} 

// A utility function to print preorder traversal 
// of the tree. 
// The function also prints height of every node 
int preOrder(struct Node *root, int AVLsize) 
{ 
	int size = AVLsize;
	if(root != NULL) 
	{ 
//		printf("%d ", root->key); 
     //   printf("occurence is: %d\t word is: %s\n", root->occ, root->str); 
		size++;
//		printf("AVL size is %d\n" , size);
		size = preOrder(root->left, size); 
		size = preOrder(root->right, size); 
	} 
} 
void AVLsearch(struct Node *root, char* str, int occ) 
{ 
	if(root != NULL) 
	{ 
		if((strcmp(str, root->str)== 0)){
			root-> occ = occ;
		}

		AVLsearch(root->left, str, occ); 
		AVLsearch(root->right, str , occ); 
	} 
} 

//------------------------------------------------------------ -----------AVL above, Min heap below

void swap(heapnode **x, heapnode **y, heapnode ** minheap){
	
	heapnode *temp = *x;
	*x = *y;
	*y = temp;   

}
// To heapify a subtree rooted with node i which is 
// an index in arr[]. N is size of heap 
void heapifydown(heapnode ** minheap, int n, int i) 
{ 
    int smallest = i; // Initialize smallest as root 
    int l = 2 * i + 1; // left = 2*i + 1 
    int r = 2 * i + 2; // right = 2*i + 2 
  
    // If left child is larger than root 
    if (l < n && minheap[l]->occ < minheap[smallest]-> occ) 
        smallest = l; 
  
    // If right child is larger than largest so far 
    if (r < n && minheap[r]->occ < minheap[smallest]-> occ) 
        smallest = r; 
  
    // If largest is not root 
    if (smallest != i) { 

		swap( &minheap[i], &minheap[smallest], minheap); 

        // Recursively heapify the affected sub-tree 
        heapifydown(minheap, n, smallest); 
    } 
} 
  
// Function to delete the root from Heap 
int deleteRoot(heapnode ** minheap, int n) 
{ 
    // Get the last element 
    heapnode *lastElement = minheap[n - 1]; 
  
    // Replace root with first element 
    minheap[0] = lastElement; 
  
    // Decrease size of heap by 1 
    n = n - 1; 
  
    // heapify the root node 
    heapifydown(minheap, n, 0); 
	return n;
} 
// Function to heapify ith node in a Heap 
// of size n following a Bottom-up approach 
void heapifyUp( heapnode ** minheap, int n, int i) 
{ 
	// Find parent 
	int parent = (i - 1) / 2; 
//	printf("parent: %d\t curr: %d\n" , parent, i);

//	if (minheap[parent] > 0) { 
			//swap if curr node is smaller than its parent
			if ( (minheap[i]-> occ) < (minheap[parent]-> occ)) { 

				swap( &minheap[i], &minheap[parent], minheap); 

			// Recursively heapify the parent node 
			heapifyUp(minheap, n, parent); 
		} 
//	} 
} 

// Function to insert a new node to the Heap 
int insertNode(heapnode **minheap, int no, heapnode * hn) 
{ 

	// Increase the size of Heap by 1 
	int n = no;
//	printf("n is %d\n" ,n);

	n = n + 1; 
//	printf("n increased to %d\n" ,n);

	// Insert the element at end of Heap 
	minheap[n - 1] = hn;
//	printf("last elem: %s\t" , minheap[n-1]->str);

	// Heapify the new node following a 
	// Bottom-up approach 
	
	heapifyUp(minheap, n, n - 1); 
	return n;
} 

// A utility function to print array of size n 
void printHeap(heapnode ** minheap, int n) 
{ 
	int i = 0;
	for (i; i < n; ++i) {
	//	printf(" min heap: %d\t word: %s\n", minheap[i]-> occ, minheap[i]->str);
	}
//	printf("\n");
} 


int copy(struct Node *root, heapnode** minheap , int AVLsize, int heapsize) 
{ 
	heapnode** h = minheap;
	int hsize  = heapsize;
	int hsize2;
	int hsize3;
	int count = 0;
	if(root != NULL) 
	{ 

		heapnode *new = createNode( root->occ, root->str);


		hsize = insertNode(h, heapsize, new);

		hsize = copy(root->left, h, AVLsize , hsize); 
		hsize = copy(root->right, h, AVLsize, hsize); 
	} 
	return hsize;
} 


heapnode ** buildMinheap ( struct Node * Avl, int AVLsize ){
	heapnode ** heap =  malloc(AVLsize*sizeof(heapnode));
	heap = NULL;
	int n = 0;




	
	return heap;

}

//-----------------------------------------------------------------------huff is below

//  huff node
typedef struct _huffnode 
{ 
	int occ; 
	struct _huffnode *left; 
	struct _huffnode *right; 
    char * str;
	int height; 
}huffnode; 

huffnode* newHuff( int occ, char* str) 
{ 

    int size = strlen(str);	
    huffnode* huff = (huffnode*) malloc(sizeof(huffnode)); 
	huff->str = malloc((size* sizeof(char))+1);
	huff->str[size] = '\0';
	huff->str = str;
//	node->key = key; 
	huff->occ = occ;
	huff->left = NULL; 
	huff->right = NULL; 
//	node->height = 1; // new node is initially added at leaf 
	return huff; 
} 



int buildhuff(heapnode ** minheap, int heapsize){
	int hsize = heapsize;
	heapnode **heap = minheap;
	
	while(hsize > 1){	
	
		//heapnode *new1 = createNode(heap[0]->occ, heap[0]-> str); //cant just create new node need to copy node
		heapnode *new1 = heap[0];
		hsize = deleteRoot(heap, hsize );
	//	heapnode *new2 = createNode(heap[0]->occ, heap[0]-> str);
		heapnode *new2 = heap[0];
		hsize = deleteRoot(heap, hsize );

		heapnode *new3 = createNode( new1->occ + new2-> occ , "");  //parent of two lowest nodes in min heap
		new3->left = new1;
		new3->right = new2;
	//	printf("str is %s occ is %d\n"  , new3->left->str , new3->left->occ);
		hsize = insertNode(heap, hsize, new3);
	}
	return hsize;
	

}

void printHuff(heapnode * root, int arr[]){
	heapnode *r = root;
	if(r->left != NULL){

	}




}

void printArr(int arr[], int n, int fp) 
{ 
   	int i; 
	//char str[n+1];
    for (i = 0; i < n; ++i){
	//	char c = arr[i];
	//	write(fp, c  , 1);
	//	str[i] = c;
		if(arr[i] == 0){
			write(fp, "0" , 1);
		}
		if(arr[i] == 1){
			write(fp, "1" , 1);
		}
 
     //   printf("%d", arr[i]); 
	}
//	str[n] = '\0';
//	write(fp, str , n);
//	int s;
 //  printf("\n"); 
} 

void printCodes(heapnode* root, int arr[], int top, int fp) 
  
{ 
  
    // Assign 0 to left edge and recur 
    if (root->left) { 
  
        arr[top] = 1; 
        printCodes(root->left, arr, top + 1, fp); 
    } 
  
    // Assign 1 to right edge and recur 
    if (root->right) { 
  
        arr[top] = 0; 
        printCodes(root->right, arr, top + 1, fp); 
    } 
  
    // If this is a leaf node, then 
    // it contains one of the input 
    // characters, print the character 
    // and its code from arr[] 
    if (root->right == NULL && root->left == NULL) { 
  
		write(fp, "\n" , 1);
	//	printf("\n");
		
		printArr(arr, top, fp); 
	//	printf("\t%s", root->str); 
		write(fp, "\t" , 1);
		
		write(fp, root->str , strlen(root->str));


    } 
} 



struct Node* finalAVL(struct Node * AVLroot, char * buff ){
	int buffSize = strlen(buff);
	int numAVLnodes;
//	node *root = NULL;
//	printf("%s" , buff);
	char * ptr = buff;
	char * strPtr = buff;
	int i = 0;
	int j = 0;
	int strSize; 
	int strIndex = 0;
	char * str;
	int tocc =0;
	int nocc = -1;
	int socc = 0;
	while (ptr[i] != '\0'){
		strSize = 0; 

		while( strPtr[j]!=  '\0'){		// traverse token to find size
			
			char c = strPtr[j];

			if (c == '\t'){
//				printf("token is !t\n");
				tocc++;
				break;

			}else if ( c == '\n'){
//				printf("token is !n\n");
				if(nocc > 0 ){

				}
				nocc++;
				break;
			}else if (c == ' '){
//				printf("token is space\n");
				socc++;
				break;
			}else {
				strSize++;
				j++;
				continue;
			}		
		}
		
		if( j >= 0){
			if ((strPtr[j] == ' ') || (strPtr[j] == '\n') || (strPtr[j] == '\t')) {
				if(j == 0){
					j++;
					i = j;
					continue;
				}
				if ( ((strPtr[j-1] == ' ') || (strPtr[j-1] == '\n') || (strPtr[j-1] == '\t'))){
//					printf(" strj = %c str(j-1) = %c \n", strPtr[j] , strPtr[j-1]);
					j++;
					i = j;
					continue;
				}
			}
		}
		j++;
//		printf("token size is %d\n",strSize );
		str = malloc(strSize*sizeof(char)+1);
		memset(str, '\0' , strSize );
		strIndex = 0;
		while(strIndex  < strSize){
			str[strIndex] = ptr[i];
			i++;
			strIndex++;

		}
		i++;	
		AVLroot = insert(AVLroot, str);
		numAVLnodes++;

		strSize = 0;
	}
	//printf("spaces: %d\t tabs: %d\t newLines: %d\n", socc, tocc, nocc);
	if(socc > 0){
	
		//char arrSpace[2] = {' ', '\0'};
		AVLroot = insert(AVLroot, " "); 
		AVLsearch(AVLroot, " " , socc);
	
	/*
		AVLroot = insert(AVLroot, "space"); 
		AVLsearch(AVLroot, "space" , socc);
	*/
		numAVLnodes++;
	
	}
	if(nocc > 0){
	
	//	char arrNL[2] = {'\n', '\0'};
		AVLroot = insert(AVLroot, "\n"); 
		AVLsearch(AVLroot, "\n" , nocc);
	
	/*	
		AVLroot = insert(AVLroot, "!n");
		AVLsearch(AVLroot, "!n" , nocc);
	*/
		numAVLnodes++;
	
	}
	if(tocc > 0){
	
	//	char arrT[2] = {'\t', '\0'};
		AVLroot = insert(AVLroot, "\t"); 
		AVLsearch(AVLroot, "\t" , tocc);

/*	
		AVLroot = insert(AVLroot, "!t");
		AVLsearch(AVLroot, "!t" , tocc);
	*/
		numAVLnodes++;
	}
	return AVLroot;



}

heapnode ** finalhuff(struct Node* AVLroot, int AVLsize, heapnode ** minheap){
	if(AVLsize == 0){
		int fp2 = open("./HuffmanCodebook", O_RDWR | O_CREAT, 00600);
		write(fp2, "\\" , 1);
		write(fp2, "\n\n" , 2);
		//write(fp2, "\n" , 1);

		close(fp2);
		return minheap;

	}
	int nsize = copy(AVLroot, minheap, AVLsize, 0);
//	printf("n size is %d\n", nsize);
	printHeap(minheap,AVLsize);
//	heapnode * huff = malloc(nsize*sizeof(heapnode));
	nsize = buildhuff(minheap, nsize);
	//printHeap(minheap,nsize);
//	printf("final size is : %d\n" , nsize);
//	printf("n occ  is %d   string is %s\n", minheap[0]->right->occ, minheap[0]->left->right->right->right->right->str);
	int arr[AVLsize];
	int top = 0;
	int hi;
	int fp2 = open("./HuffmanCodebook", O_RDWR | O_CREAT, 00600);
//	printf("\\");
	write(fp2, "\\" , 1);
	if((minheap[0]-> left == NULL)&& (minheap[0]-> left == NULL)){
		write(fp2, "\n" , 1);
		write(fp2, "0\t" , 2);
		write(fp2, minheap[0]->str , 1);
	}else{
		printCodes(minheap[0] , arr, top, fp2);
	}
	close(fp2);
	

}



#endif



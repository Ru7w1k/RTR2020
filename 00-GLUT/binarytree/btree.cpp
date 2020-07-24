#include<GL/glut.h>
#include<stdio.h>
#include<stdlib.h>

typedef struct Node{
	
	struct Point{
		float p1;
		float p2;
	}Vertices;
	float xx;
	float yy;
	float height;
	int data;
	struct Node* lChild;
	struct Node* rChild;
	
}NODE,*PNODE,**PPNODE;

PNODE Root = NULL;


int main(int argc,char* argv[]){
	
	void InsertNode(PPNODE,int);
	void Display(PNODE,int);
	void PreOrder(PNODE);
	void InOrder(PNODE);
	void PostOrder(PNODE);
	void DeleteTree(PNODE);
	
	void display(void);
	void init(void);
	void reshape(int,int);

	int Data = 0;
	int Option;
	int Running = 1;
	int i = 0;
	
	while(Running){
		printf("Enter 1 to Insert Key\n");
		printf("Enter -1 to stop Inserting Node\n");

		printf("Enter Your Choice\n");
		scanf("%d",&Option);
		
		switch(Option){
			case 1:
			for(;;){
				printf("Enter Data\n");
				scanf("%d",&Data);
				if(Data == -1){
					Running = 0;
					break;
				}
				InsertNode(&Root,Data);
				
			}
			break;
			default:
				printf("Wrong Option Entered\n");
			break;
		}
		
	}
	
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500,500);
	glutInitWindowPosition(100,100);
	glutCreateWindow(argv[0]);
	
	init();
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	DeleteTree(Root);
	return(0);
}

void init(void){
	
	glClearColor(0.0,0.0,0.0,0.0);
	
}

void DrawTree(PNODE root){
	
	
	static float x1,y1,x2,y2;
	
		
	x1 = root->Vertices.p1;
	y1 = root->Vertices.p2;
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
			
	glColor3f(0.0,1.0,0.0);
	glTranslatef(root->Vertices.p1,root->Vertices.p2,0.0);
	glutSolidSphere(1,180,180);
	glFlush();

	glEnable(GL_LINE_SMOOTH);
	glLineWidth(0.5);
		if(root->lChild != NULL){
			 x2 = root->lChild->Vertices.p1;
			 y2 = root->lChild->Vertices.p2;
	
			 glMatrixMode(GL_MODELVIEW);
			 glLoadIdentity();
			 glBegin(GL_LINES);
				glColor3f(1.0,1.0,1.0);
				glVertex2f(x1,y1);
				glVertex2f(x2,y2);
			 glEnd();
			 glFlush();
			DrawTree(root->lChild);
			x1 = root->Vertices.p1;
			y1 = root->Vertices.p2;
		}
		if(root->rChild != NULL){
			x2 = root->rChild->Vertices.p1;
			y2 = root->rChild->Vertices.p2;

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glBegin(GL_LINES);
			glColor3f(1.0,1.0,1.0);
				glVertex2f(x1,y1);
				glVertex2f(x2,y2);
			glEnd();
			glFlush();
			DrawTree(root->rChild);
			x1 = root->Vertices.p1;
			y1 = root->Vertices.p2;
		}


}

void reshape(int w,int h){
	
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-20.0,20.0,-20.0,20.0,-10.0,10.0);
	glMatrixMode(GL_MODELVIEW);	
	
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT);
	
	DrawTree(Root);
}

void DeleteTree(PNODE root){

		if(root->lChild != NULL){
			DeleteTree(root->lChild);
		}
		if(root->rChild != NULL){
			DeleteTree(root->rChild);
		}
		free(root);
	
}

void InsertNode(PPNODE root,int value){
	
	PNODE newn = NULL;
	PNODE temp = NULL;
	float x,y;
	float Height;
	float xx,yy;
	
	newn = (PNODE)malloc(sizeof(NODE));

	newn->xx = 9.0;
	newn->yy = 9.0;
	newn->Vertices.p1 = 0.0;
	newn->Vertices.p2 = 5.0;
	newn->data = value;
	newn->height = 0;
	newn->lChild = NULL;
	newn->rChild = NULL;
	
	if(*root == NULL){
		*root = newn;
	}
	else{
		temp = *root;
		
		while(1){
			if(value > (temp)->data){
				x = temp->Vertices.p1;
				y = temp->Vertices.p2;
				Height = temp->height;
				xx = temp->xx;
				yy = temp->yy;
				
				if((temp)->rChild == NULL){
					newn->Vertices.p1 = x + xx - Height;
					printf("Height : %f \n",temp->height);
					newn->Vertices.p2 = y - 3.0;
					newn->height = Height + 4.0;
					newn->xx = xx + 0.4;
					printf("x : %f \n",temp->Vertices.p1);
					printf("xx : %f \n",xx);
					printf("X : %f \n",newn->Vertices.p1);
					
					(temp)->rChild = newn;
					break;
				}
				temp = (temp)->rChild;
			}
			else{
				x = temp->Vertices.p1;
				y = temp->Vertices.p2;
				Height = temp->height;
				xx = temp->xx;
				yy = temp->yy;				
				if((temp)->lChild == NULL){
					newn->Vertices.p1 = x - yy + Height;
					printf("Height : %f \n",temp->height);
					newn->Vertices.p2 = y - 3.0;
					newn->height = Height + 4.0;
					newn->yy = yy + 0.4;
					printf("x : %f \n",temp->Vertices.p1);
					printf("xx : %f \n",yy);
					printf("X : %f \n",newn->Vertices.p1);
					
					(temp)->lChild = newn;
					break;
				}
				temp = (temp)->lChild;				
			}
		}
	}
	
	
}



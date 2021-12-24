#include <curses.h>
#include <stdlib.h>		
#include <pthread.h>	// pthread 函数调用的使用
#include <unistd.h>		// sleep 函数调用的使用
#include <stdio.h>
typedef struct snake
{
	int Hang;
	int Lie;
	struct snake *next;
}sna,*psna;

psna head = NULL;
psna tail = NULL;
#define UP  	 1
#define DOWN 	-1
#define RIGHT 	-2
#define LEFT 	 2  
int key;
int dir;
sna food;	// 定义贪吃蛇食物

void initFood()		//贪吃蛇食物函数
{
	int x = rand()%18+1;	// rand() 食物随机函数
	int y = rand()%18+1;
	food.Hang = x;
	food.Lie = y;
	food.next = NULL;
}

void initNcurse()	//	贪吃蛇身体节点判定函数
{
	int key;
 	initscr();
	keypad(stdscr,1);
	return;
}
int hadSnakeNode(int hang,int lie)
{
	psna p = head;
	while(p != NULL)
	{
		if(hang == p->Hang && lie == p->Lie)
		{
			return 1;
		}
		p = p->next;
	}
	return 0;
}

void gamePic()   //	游戏地图的打印
{
	int hang;
	int lie;
	move(0,0);		// 移动光标到（0，0）的位置
	for(hang = 0;hang < 20;hang++)
	{
		if(hang == 0 || hang == 19)
		{
			for(lie = 0;lie < 20;lie++)
			{
				printw("==");	
			}
			printw("\n");
		}
		else
		{
			for(lie = 0;lie < 20;lie++)
			{
				if(lie == 0 || lie == 19)
				{
					if(lie == 0)
					{
						printw("||");
					}
					else
					{
						printw("||");
					}
				}
				else if(hadSnakeNode(hang,lie))		// 检测是否有贪吃蛇身体的节点
				{
					printw("[]");
				}
				else if(food.Hang == hang && food.Lie == lie)
				{
					printw("##");
				}
				else
				{
					printw("  ");
				}
			}
			printw("\n");
		}
	}
	printw("xiaopan made, I wish you a happy game!\n");	// 没啥用，声明游戏作者和寄语
//	printw("key = %d   dir = %d\n",key,dir);
	return;
}

void addNode()	// 贪吃蛇增加节点函数
{
	psna new = (psna)malloc(sizeof(sna));
	new->next = NULL;
	switch(dir)
	{
		case UP:
			new->Hang = tail->Hang-1;
			new->Lie = tail->Lie;
			break;
		case DOWN:
			new->Hang = tail->Hang+1;
			new->Lie = tail->Lie;
			break;
		case RIGHT:
			new->Hang = tail->Hang;
			new->Lie = tail->Lie+1;
			break;
		case LEFT:
			new->Hang = tail->Hang;
			new->Lie = tail->Lie-1;
			break;
	}
	tail->next = new;
	tail = new;
	return;	
}

void initSnake()
{
	dir = RIGHT;
	psna p = NULL;
	while(head != NULL)		
	{
		p = head;
		head = head->next;
		free(p); 
		p = NULL;
	}	//	蛇撞墙时候重新初始化，并且对产生的 malloc 产生的结构体空间进行释放
	initFood();
	head = (psna)malloc(sizeof(sna));
	head->Hang = 1;
	head->Lie = 1;
	head->next = NULL;
	tail = head;
	addNode();
	addNode();
	addNode();
	addNode();
	return;
}

void deleteNode()	// 贪吃蛇移动删除链表的头节点
{
	psna p = head;
	head = head->next;
	free(p);
	p = NULL;
	return;
}

int ifSnakeDie()
{
	psna p = head;
	if(tail->Hang == 0 || tail->Hang == 19 || tail->Lie == 0 || tail->Lie == 19)
	{
		return 1;
	}
	while(p->next != NULL)
	{
		if(p->Hang == tail->Hang && p->Lie == tail->Lie)
		{
			return 1;
		}
		p = p->next;
	}
	return 0;
}

void moveSnake()	
{
/*	psna new = (psna)malloc(sizeof(sna));
	new->Hang = tail->Hang;
	new->lie = tail->Lie + 1;
	tail->next = new;
	tail = new;
*/
	addNode();
	if(tail->Hang == food.Hang && tail->Lie == food.Lie)
	{
		initFood();
	}
	else
	{	
		deleteNode();
	}
	if(ifSnakeDie())	//	判定蛇头是否撞墙
	{
		initSnake();
	}
	return;
}
void* refreshJieMian()		// 游戏刷新界面函数
{

	while(1)
	{
		moveSnake();
		gamePic();
		refresh();		// refresh 函数刷新函数，没有参数
		usleep(150000);		// usleep 函数的参数单位是微秒，sleep 函数的参数单位是秒
	}

}

void turn(int direction)
{
	if(dir + direction != 0)
	{
		dir = direction;
	}
}

void* changeDir()	// 游戏方向键响应的函数
{
	while(1)
	{
		key = getch();
		switch(key)
		{
			case KEY_UP:
				turn(UP);
				break;
			case KEY_DOWN:
				turn(DOWN);
				break;
			case KEY_RIGHT:
				turn(RIGHT);
				break;
			case KEY_LEFT:
				turn(LEFT);
				break;
		}
	}
}

int main()
{
	int sr;		// sr 第一次指引蛇向右走的时候，使用的参数
	initNcurse();
	initSnake();	
	gamePic();
	pthread_t th1;
	pthread_t th2;
	pthread_create(&th1,NULL,refreshJieMian,NULL);
	pthread_create(&th2,NULL,changeDir,NULL);
	while(1);

/*	while(1)
	{

		sr = getch();
		if(sr == KEY_RIGHT)
		{
			moveSnake();
			gamePic();
		} // 按下向右方向键移动蛇
		
		moveSnake();
		gamePic();
		refresh();
		usleep(100000);

	}
*/
	getch();
	endwin();
	return 0;
}


#include<ncurses.h>
#include<stdlib.h>
#include<fstream>
#include<iostream>
#include"winner.cpp"
using namespace std;

//initializing functions
void level(int &maxrow,int &maxcol,int MainMatrix[100][100], char filename[11]);
void printmatrix(int maxrow,int maxcol, int MainMatrix[100][100]);
void printhelp();
void printwinscr(char name[10],char player[10]);
void assumematrix(int MainMatrix[100][100]);
void gethighscore(char name[10],int &marks);
void savehighscore(char name[10],int marks);
void startgame(char player[10]);
void winner(char name[10],int score);

//define global variabales
int maxrow=0,maxcol=0, setlevel=1,score=0,marks=0,co=0;
int MainMatrix[100][100];
char filename[11], name[10], player[10],tempch;

int main(){
	char ch='n';
start:
	if(setlevel==5)
		setlevel=1;
	startgame(player);
	assumematrix(MainMatrix);
	gethighscore(name,marks);

		if (setlevel==1){
level1:
			level( maxrow, maxcol, MainMatrix, "data/level1.dat");
		}else if(setlevel==2){
level2:
			level( maxrow, maxcol, MainMatrix, "data/level2.dat");
		}else if(setlevel==3){
level3:
			level( maxrow, maxcol, MainMatrix, "data/level3.dat");
		}else if(setlevel==4){
level4:			if(score>marks){
				winner(player,score);
				savehighscore(player,score);
			}else{
				winner(name, marks);
			}
			tempch='q';
			setlevel=5;
		}
		co=0;
		if(tempch=='q'){
			score=0;
			endwin();
			endwin();
			initscr();
			clear();
			start_color();
			init_pair(1,COLOR_RED,0);
			attrset(COLOR_PAIR(1));
			do{
				printw("\nDo you want to continue...[y]\\[n] : ");	
				ch=getch();
			}while(ch!='y'&& ch!='n');
		}
		if(ch=='n'){
			if(setlevel==1)
				goto level1;
			else if(setlevel==2)
				goto level2;
			else if(setlevel==3)
				goto level3;
			else if(setlevel==4)
				goto level4;
			else if(setlevel==5)
				goto start;
		}
	endwin();
	cout<<"Bye\n";
//	printmatrix( maxrow, maxcol, MainMatrix);
	return 0;
}//main

void level(int &maxrow, int &maxcol, int MainMatrix[100][100],char filename[11]){
	int x=0,y=0,i=17,j=1;
	bool trapperr;
	double matno,tempscore;
	initscr();
	keypad(stdscr, TRUE);
	erase();
	echo();
        start_color();
        init_pair(1,COLOR_RED,COLOR_BLACK);
	init_pair(2,COLOR_GREEN,COLOR_BLACK);
	init_pair(3,COLOR_BLUE,COLOR_BLACK);
	init_pair(4,COLOR_YELLOW,COLOR_BLACK);
	init_pair(5,0,COLOR_YELLOW);
	init_pair(6,0,0);
	//open file
        ifstream infile(filename);
        if(!infile){
                move(10,30);
                attrset(COLOR_PAIR(1));
                printw("Input Error...!");
		getch();
		exit(1);
        }
	move(1,30);
	attrset(COLOR_PAIR(1));
	printw("MULA WEE (Version 0.0.1)");
	move(2,3);
	printw("Level : %d",setlevel);
	//get row and column sizes
	infile >> maxrow;
	infile >> maxcol;
	//get values to array matrix
	for(x=0;x<maxrow;x++){
		for(y=0;y<maxcol;y++){
			infile >> tempch;
			if(tempch=='|' || tempch=='%'){
				MainMatrix[x][y]=0;
			}else if (tempch=='*'){
				MainMatrix[x][y]=1;
			}else if(tempch=='$'){
				MainMatrix[x][y]=2;
			}
			move(x+3,y+3);
			attrset(COLOR_PAIR(2));
			if(tempch=='*')
				printw(" ");
			else if (tempch=='$'){
				attrset(COLOR_PAIR(5));
				printw("$");
			}
			else
				printw("%c",tempch);
		}
	}
//	move(maxrow+14,0);
//	attrset(COLOR_PAIR(4));
//	printhelp();
//	move(maxrow+8,11);
	x=20;
	y=4;
	attrset(COLOR_PAIR(6));
	move(x,y);
	//get charctar and move curser
	tempch=getch();
	move(x,y);
	while(tempch!='q'){
		co++;
		if(tempch=='a'){
			i=y--;
			matno=MainMatrix[x-3][y-3];
		}else if(tempch=='d'){
			i=y++;
			matno=MainMatrix[x-3][y-3];
		}else if(tempch=='w'){
			j=x--;
			matno=MainMatrix[x-3][y-3];
		}else if(tempch=='s'){
			j=x++;
			matno=MainMatrix[x-3][y-3];
		}else{
			beep();
			move(maxrow+3,3);
			attrset(COLOR_PAIR(1));
			printw("%c is Invalid Key....", tempch);
			move(x,y);
			trapperr=0;
		}
		if((matno!=0) && (trapperr)){
			move(x,y);
			attrset(COLOR_PAIR(4));
			printw("*");
		}
		if(matno==0){
			beep();
			if(tempch=='a'|| tempch=='d')
				y=i;
			else if(tempch=='w' || tempch=='s')
				x=j;
			move(x,y);
			attrset(COLOR_PAIR(4));
			printw("*");
		}
		move(maxrow+4,3);
		attrset(COLOR_PAIR(3));
		printw("Row No. %d Column No. %d",x-3,y-3); 
		move(maxrow+5,3);
		printw("No. of Turns : %d", co);
		attrset(COLOR_PAIR(6));
		move(x,y);
		if(tempch=='q')
			exit(1);
		tempch=getch();
		trapperr=1;
		
		if(matno==2){
			tempscore=0;
			if(setlevel==1){
				tempscore=((154+(154-co))*154)/100;
				score+=tempscore;
			}else if(setlevel==2){
				tempscore=((253+(253-co))*253)/100;
				score+=tempscore;
			}else if(setlevel==3){
				tempscore=((212+(212-co))*212)/100;
				score+=tempscore;
			}
			clear();
			endwin();
			printwinscr(name,player);
			endwin();
			clear();
			initscr();
			return;
		}
	
	}
	endwin();
		
}//levelOne

void printhelp(){
	printw("ATTENTION! ATTENTION! ATTENTION!\n\tKey Pad\n\tLeft : a\n\tRight : d\n\tUp : w\n\tDown : s\n\tQuit : q\n\n\tIn this game you have move curser to yellow box... \n\tthen you can win\n\ttry it... you can do it easily..\n\n\tCopyright 2004 Nipuna Perera");	
	move(9,10);
}//printhelp

void printwinscr(char name[10],char player[10]){
	long double i;
	char tempch;
	short int temp=0;
	endwin();
	initscr();
	start_color();
	init_pair(1,COLOR_MAGENTA,COLOR_BLACK);
	init_pair(2,COLOR_RED,COLOR_BLACK);
	move(5,25);
	attrset(COLOR_PAIR(1));
	printw("%s\n\t\t\tlevel %d Complete\n\t\t\tYou Are The Winner....\n\t\t\tYour Score : %d",player,setlevel,score);
	move(25,4);
	attrset(COLOR_PAIR(2));
	printw("Press any key to Continue...");
	getch();
	clear();
	if(setlevel!=4){
		move(5,25);
		attrset(COLOR_PAIR(1));
		printw("-----------Level %d----------",setlevel+1);
		move(25,4);
		attrset(COLOR_PAIR(2));
		printw("Press any key to continue........");
		getch();
		setlevel++;
	}
		endwin();
		return;

}//printwinscr

void assumematrix(int MainMatrix[100][100]){
	int x,y;
	for(x=0;x<100;x++){
		for(y=0;y<100;y++){
			MainMatrix[x][y]=0;
		}
	}

}//assumematrix

void gethighscore(char name[10], int &marks){
	ifstream infile("score.dat");
	if(!infile){
		endwin();
		cout << "Input Error...!";
		exit(1);
	}
	infile>>name;
	infile>>marks;
}//gethighscore

void savehighscore(char name[10], int marks){
	ofstream outfile("score.dat");
	if(!outfile){
		endwin();
		cout << "Output Error.....!";
		exit(1);
	}
	outfile<<player<<" ";
	outfile<<marks;
}//savehighscore

void printmatrix(int maxrow, int maxcol,int MainMatrix[100][100]){
	int i,j;
	for(i=0;i<maxrow;i++){
		for(j=0;j<maxcol;j++){
			cout << MainMatrix[i][j];
		}
		cout << endl;
	}

}//printmatrix*/

void startgame(char player[10]){
	int i,j;
	initscr();
	start_color();
	init_pair(1,COLOR_RED,0);
	init_pair(2,COLOR_YELLOW,0);
	attrset(COLOR_PAIR(2));
	move(6,8);
	printhelp();
	attrset(COLOR_PAIR(1));
	for(j=0;j<4;j++){
		for(i=0;i<71;i++){
			if(j==0){
				move(2,i+2);
				printw("*");
			}else if(j==1){
				move(22,i+2);
				printw("*");
			}else if(j==2){
				move(i+2,2);
				printw("*");
				if(i>19)
					break;
			}else if(j==3){
				move(i+2,73);
				printw("*");
				if(i>19)
					break;
			}
		}	
	}
	move(3,30);
	attrset(COLOR_PAIR(2));
	printw("MULA VEE");
	move(4,28);
	printw("Version 0.0.1");
//	move(10,25);
//	printw("Created By : Nipuna Perera");
//	move(11,25);
//	printw("Copyright 2004");
	move(20,10);
	printw("Enter Your Name Here [Maximum 10 charcters only] : ");
	scanw("%s",player);
	endwin();
}


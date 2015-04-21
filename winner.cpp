

void winner(char name[10], int score){
	int i,j;
	endwin();
	initscr();
	start_color();
	init_pair(1,0,0);
	init_pair(2,5,0);
	init_pair(3,COLOR_RED,0);
	init_pair(4,COLOR_GREEN,0);
	init_pair(5,COLOR_BLUE,0);
	
	
	for(i=2;i<71;i++){
		for(j=2;j<21;j++){
			attrset(COLOR_PAIR(1));
			move(j,i);
			printw(" ");
		}
	}
	attrset(COLOR_PAIR(3));
	for(i=2;i<71;i++){
		for(j=2;j<21;j++){
			if((i>4 || i<71)&&(j==2 || j==20)){
				move(j,i);
				printw("*");
			}else if((i==2 || i==70) && (j<19 || j>3)){
				move(j,i);
				printw("*");
			}
			
		}
	
	}	
	move(5,32);
	attrset(COLOR_PAIR(2));
	printw("---MULA WEE---");
	move(7,20);
	printw("........YOU ARE THE WINNER........");
	attrset(COLOR_PAIR(4));
	move(10,20);
	printw("HIGHEST SCORE......");
	move(11,20);
	printw("%s\t %d", name, score);
	attrset(COLOR_PAIR(5));
	move(16,10);
	printw("Created By :");
	move(17,10);
	printw("Nipuna Perea");
	move(18,10);
	printw("Copyright 2004");
	getch();
	endwin();
	return;
}

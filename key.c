// //https://stackoverflow.com/questions/10463201/getch-and-arrow-codes
// #include <stdio.h>

// int main()
// {
// 	int c,n = 0;

// 	while (( c=getchar()) != 'Q')
// 	{
// 		if (c == '\033')
// 		{
// 			//these two strange prints are from:
// 			//https://itnext.io/overwrite-previously-printed-lines-4218a9563527
// 			//and they delete the previous line
// 			printf("\033[1A");//line up
// 			printf("\x1b[2K");//delete line
// 			printf("got an arrow ");
// 			getchar(); //skip the [
// 			switch(getchar()) { // the real value
// 			case 'A':
// 			    // code for arrow up
// 			    printf("up\n");
// 			    break;
// 			case 'B':
// 			    // code for arrow down
// 			    printf("down\n");
// 			    break;
//     			}

// 		}
// 		else
// 		{
// 			printf("char %3d is %c code %d\n", n++,c,c);
// 		}
// 	}

// 	return 0;
// }

// //to get more info , you can read teh book chapter 5,page 151 (172 in pdf)
// //also take a look here: https://dev.to/napicella/linux-terminals-tty-pty-and-shell-192e

#include <stdio.h>
#include <ncurses.h>

int main()
{
	printf("hello: ");
	initscr(); // Initialize ncurses
	// timeout(0);

	int ch;
	while (1)
	{
		ch = getch(); // Get the pressed key
		if (ch != ERR) { // Check if a key is pressed
            if (ch == KEY_UP) { // Arrow up
                printw("Arrow up pressed. Behave differently.\n");
            } else if (ch == KEY_DOWN) { // Arrow down
                printw("Arrow down pressed. Behave differently.\n");
            }
        }
		refresh(); // Refresh the screen to update the display
		printf("\033[1A"); // line up
		printf("\x1b[2K"); // delete line
		// getch() == '\033'
		//  // these two strange prints are from:
		// // https://itnext.io/overwrite-previously-printed-lines-4218a9563527
		// // and they delete the previous line
		// printf("got an arrow ");
		// getchar(); // skip the [
		// switch (getchar())
		// { // the real value
		// case 'A':
		// 	// code for arrow up
		// 	printf("up");
		// 	break;
		// case 'B':
		// 	// code for arrow down
		// 	printf("down");
		// 	break;
		// }
	}
	printf("not what you think\n");
	endwin();
}
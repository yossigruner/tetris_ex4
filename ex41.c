/*
 * Tetris operations systems ex 4
 * Yossi Gruner - 300907946
 * Litsan Lavie - 301447082
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>

#define EXEC_PROC "draw.out"
#define RUN_FOREVER 1

/**
 * get the char for the user
 *
 * return: the selected char
 *
 *
 */
char getch() {
	char buf = 0;
	struct termios old = { 0 };
	if (tcgetattr(0, &old) < 0) {
		perror("tcsetattr()");
	}
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old) < 0) {
		perror("tcsetattr ICANON");
	}
	if (read(0, &buf, 1) < 0) {
		perror("read()");
	}
	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if (tcsetattr(0, TCSADRAIN, &old) < 0) {
		perror("tcsetattr ~ICANON");
	}
	return (buf);
}

int main(int argc, char* argv[]) {

	pid_t pid;
	char selectedChar;
	int fd[2];
	int dup;

	// make the pipe
	pipe(fd);

	pid = fork();
	if (pid == 0) {
		dup = dup2(fd[0], STDIN_FILENO);
		if (dup < 0) {
			perror("Duplication fault !\n");
			exit(0);
		}
		//close in pipe for child proccess
		close(fd[0]);
		// run draw proces
		execv(EXEC_PROC, (char** ) { NULL });
	} else {
		while (RUN_FOREVER) {
			selectedChar = getch();

			switch (selectedChar) {
			case 'q':
				close(fd[1]);
				kill(pid, SIGKILL);
				return EXIT_SUCCESS;
			default:
				write(fd[1], &selectedChar, 1);
				kill(pid, SIGUSR2);

				break;
			}
		}
	}
	return EXIT_SUCCESS;
}

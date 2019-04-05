/*
 * Copyright (c) 2008, Nico Leidecker
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the organization nor the names of its contributors 
 *       may be used to endorse or promote products derived from this software 
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <sys/poll.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#include "utils.h"
#include "phrasendrescher.h"
#include "worker.h"

int verbose = 0;

static struct pollfd pfd[1];
static struct termios old_tty;

int terminate = 0;

void
prepare_tty()
{
	struct termios tty;

	tcgetattr(0, &old_tty);
	tcgetattr(0, &tty);

	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO;

	tcsetattr(0, TCSANOW, &tty);

	pfd[0].fd = 0;    
	pfd[0].events = POLLIN;

}

void 
reset_tty()
{
	tcsetattr(0, TCSANOW, &old_tty);
}

void
handle_user_input(int worker_num)
{
	int c;
	
	while(terminate < worker_num) {
		if (poll(pfd, 1, 100) > 0) {
			c = getchar();
			if (c != -1) {
				switch(c) {
					case 'q':
						kill(0, SIG_TERMINATE);
					default:
						kill (0, SIG_WORKER_STATS);
						break;
				}
			}
		}
	}
}

void
set_verbose()
{
    verbose = 1;
}

void
verbose_printf(const char *fmt, ...)
{
    va_list ap;
    if (verbose) {
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
    }
}

void
error_printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
 }

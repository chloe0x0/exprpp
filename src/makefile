# Builds Expr
CC = g++
CFLAGS = -Wall -Wextra -Wunused
FILES = token.cxx lexer.cxx expr_tree.cxx parser.cxx

# Builds the REPL executable
repl:
	$(CC) $(CFLAGS) -o repl repl.cxx $(FILES)

clean:
	rm *.exe
	rm *.o
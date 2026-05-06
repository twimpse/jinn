# Compiler and linker settings
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99
LDFLAGS = 
LDLIBS = 

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Target executable name
TARGET = $(BINDIR)/jinn

# Source files (add all your .c files here)
SOURCES = $(SRCDIR)/jinn.c \
          $(SRCDIR)/jinn-server.c \
          $(SRCDIR)/jinn-client.c

# Object files (derived from sources)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Default target
all: $(TARGET)

# Link object files into final executable
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compile C files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create necessary directories
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Clean and rebuild
rebuild: clean all

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: clean all

# Phony targets (not actual files)
.PHONY: all clean rebuild run debug

# Print variables (useful for debugging)
print:
	@echo "CC = $(CC)"
	@echo "CFLAGS = $(CFLAGS)"
	@echo "SOURCES = $(SOURCES)"
	@echo "OBJECTS = $(OBJECTS)"
	@echo "TARGET = $(TARGET)"

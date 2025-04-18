#define HISTORY_SIZE 50

char* history[HISTORY_SIZE];
int history_count = 0;

// Add a command to history
void add_to_history(const char* cmd) {
    if (history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(cmd);
    } else {
        // Remove oldest and shift
        free(history[0]);
        for (int i = 1; i < HISTORY_SIZE; i++) {
            history[i - 1] = history[i];
        }
        history[HISTORY_SIZE - 1] = strdup(cmd);
    }
}

// Display history
void show_history() {
    for (int i = 0; i < history_count; i++) {
        printf("[%d]: %s\n", i + 1, history[i]);
    }
}
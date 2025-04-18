#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_USERS 500
#define MAX_NAME 100
#define MAX_COUNTRY 50
#define MAX_LINE 1024
#define MAX_CONNECTIONS 50
#define INF 99999

// ANSI escape codes for text formatting
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_GREEN   "\033[32m"

// Struct to store each user's data
typedef struct {
    int id;
    char name[MAX_NAME];
    int followers;
    float activity_score;
    char country[MAX_COUNTRY];
    int connections[MAX_CONNECTIONS];
    int connection_count;
} User;

User users[MAX_USERS];
int user_count = 0;

int adj_matrix[MAX_USERS][MAX_USERS];

// Removes the newline character from the end of a string.
void trim_newline(char *str) {
    str[strcspn(str, "\n")] = 0;
}

// Parses a string of space-separated integers into an integer array.
void parse_connections(char *conn_str, int *connections, int *count) {
    char *token = strtok(conn_str, " ");
    *count = 0;
    while (token != NULL && *count < MAX_CONNECTIONS) {
        connections[(*count)++] = atoi(token);
        token = strtok(NULL, " ");
    }
}

// Loads user data from a CSV file into the 'users' array.
void load_users(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file!\n");
        exit(1);
    }

    char line[MAX_LINE];
    fgets(line, sizeof(line), file); // skip header

    while (fgets(line, sizeof(line), file)) {
        char *token;
        token = strtok(line, ",");
        users[user_count].id = atoi(token);

        token = strtok(NULL, ",");
        strcpy(users[user_count].name, token);

        token = strtok(NULL, ",");
        users[user_count].followers = atoi(token);

        token = strtok(NULL, ",");
        users[user_count].activity_score = atof(token);

        token = strtok(NULL, ",");
        strcpy(users[user_count].country, token);

        token = strtok(NULL, "\n");
        parse_connections(token, users[user_count].connections, &users[user_count].connection_count);

        user_count++;
    }
    fclose(file);
}

// Finds the index of a user in the 'users' array by their name (case-insensitive).
int find_user_by_name(const char *name) {
    for (int i = 0; i < user_count; i++) {
        if (strcasecmp(users[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Finds the index of a user in the 'users' array by their ID.
int find_user_by_id(int id) {
    for (int i = 0; i < user_count; i++) {
        if (users[i].id == id) return i;
    }
    return -1;
}

// Builds an adjacency matrix representing user connections.
void build_adj_matrix() {
    for (int i = 0; i < user_count; i++) {
        for (int j = 0; j < user_count; j++) {
            adj_matrix[i][j] = 0;
        }
    }
    for (int i = 0; i < user_count; i++) {
        for (int j = 0; j < users[i].connection_count; j++) {
            int conn_id = users[i].connections[j];
            int conn_index = find_user_by_id(conn_id);
            if (conn_index != -1)
                adj_matrix[i][conn_index] = 1;
        }
    }
}

// Recursive helper function for Depth-First Search.
void dfs_util(int v, bool *visited) {
    visited[v] = true;
    printf("%d -> ", users[v].id);
    for (int i = 0; i < user_count; i++) {
        if (adj_matrix[v][i] && !visited[i])
            dfs_util(i, visited);
    }
}

// Performs Depth-First Search starting from a given user.
void dfs(int start_index) {
    bool visited[MAX_USERS] = { false };
    printf(COLOR_YELLOW "\nDFS traversal starting from user ID %d: " COLOR_RESET, users[start_index].id);
    dfs_util(start_index, visited);
    printf("\n");
}

// Performs Breadth-First Search starting from a given user.
void bfs(int start_index) {
    bool visited[MAX_USERS] = { false };
    int queue[MAX_USERS], front = 0, rear = 0;
    visited[start_index] = true;
    queue[rear++] = start_index;
    printf(COLOR_CYAN "\nBFS traversal starting from user ID %d: " COLOR_RESET, users[start_index].id);
    while (front < rear) {
        int v = queue[front++];
        printf("%d -> ", users[v].id);
        for (int i = 0; i < user_count; i++) {
            if (adj_matrix[v][i] && !visited[i]) {
                visited[i] = true;
                queue[rear++] = i;
            }
        }
    }
    printf("\n");
}

// Implements the Floyd-Warshall algorithm to find all-pairs shortest paths.
void floyd_warshall() {
    int dist[MAX_USERS][MAX_USERS];
    for (int i = 0; i < user_count; i++) {
        for (int j = 0; j < user_count; j++) {
            if (i == j) dist[i][j] = 0;
            else if (adj_matrix[i][j]) dist[i][j] = 1;
            else dist[i][j] = INF;
        }
    }
    for (int k = 0; k < user_count; k++) {
        for (int i = 0; i < user_count; i++) {
            for (int j = 0; j < user_count; j++) {
                if (dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
    }
    printf(COLOR_GREEN "\nShortest path distances between all users (IDs):\n" COLOR_RESET);
    printf("    ");
    for (int i = 0; i < user_count && i < 20; i++) printf("%-4d", users[i].id);
    printf("\n");
    for (int i = 0; i < user_count && i < 20; i++) {
        printf("%-4d", users[i].id);
        for (int j = 0; j < user_count && j < 20; j++) {
            if (dist[i][j] == INF) printf("INF ");
            else printf("%-4d", dist[i][j]);
        }
        printf("\n");
    }
    printf("(Displaying first 20 users for brevity)\n");
}

// Shows the mutual connections between two users.
void show_mutuals(const char *name1, const char *name2) {
    int idx1 = find_user_by_name(name1);
    int idx2 = find_user_by_name(name2);
    if (idx1 == -1 || idx2 == -1) {
        printf("One or both users not found.\n");
        return;
    }

    printf(COLOR_CYAN "\nMutual connections between %s and %s (User IDs):\n" COLOR_RESET, name1, name2);
    bool found = false;
    for (int i = 0; i < users[idx1].connection_count; i++) {
        for (int j = 0; j < users[idx2].connection_count; j++) {
            if (users[idx1].connections[i] == users[idx2].connections[j]) {
                int mutual_id = users[idx1].connections[i];
                int mutual_idx = find_user_by_id(mutual_id);
                if (mutual_idx != -1) {
                    printf("- %d (%s, %d followers, %s)\n",
                           mutual_id,
                           users[mutual_idx].name,
                           users[mutual_idx].followers,
                           users[mutual_idx].country);
                    found = true;
                }
            }
        }
    }
    if (!found) {
        printf("No mutual connections found.\n");
    }
}

// Comparison function for sorting users by followers (descending)
int compareByFollowers(const void *a, const void *b) {
    const User *userA = (const User *)a;
    const User *userB = (const User *)b;
    return userB->followers - userA->followers;
}

// Comparison function for sorting users by activity score (descending)
int compareByActivity(const void *a, const void *b) {
    const User *userA = (const User *)a;
    const User *userB = (const User *)b;
    if (userB->activity_score > userA->activity_score) return 1;
    if (userB->activity_score < userA->activity_score) return -1;
    return 0;
}

// Function to print a simple text-based tree
void printTextTree(User *root, User *parent, int depth, int isLast) {
    if (root == NULL || depth > 3) return; // Limit depth

    for (int i = 0; i < depth; i++) {
        printf("%s", (i == depth - 1) ? (isLast ? "└── " : "├── ") : "│   ");
    }
    printf("%s (ID: %d, F:%d, A:%.2f, C:%s)\n", root->name, root->id, root->followers, root->activity_score, root->country);

    // Find connected users (excluding parent to avoid cycles in the tree)
    int connected_count = 0;
    int connected_indices[MAX_CONNECTIONS];
    for (int i = 0; i < root->connection_count; i++) {
        int connected_id = root->connections[i];
        int connected_index = find_user_by_id(connected_id);
        if (connected_index != -1 && (parent == NULL || users + connected_index != parent)) {
            connected_indices[connected_count++] = connected_index;
        }
    }

    // Sort connected users by followers (descending) for tree structure
    qsort(connected_indices, connected_count, sizeof(int), compareByFollowers);

    for (int i = 0; i < (connected_count > 3 ? 3 : connected_count); i++) {
        printTextTree(users + connected_indices[i], root, depth + 1, (i == connected_count - 1));
    }
}

// Shows the top N influencers based on a specified criteria (followers or activity).
void show_top_n_influencers_by(int n, const char *criteria) {
    if (n <= 0 || n > user_count) {
        printf("Invalid number of influencers.\n");
        return;
    }

    User sorted_users[MAX_USERS];
    memcpy(sorted_users, users, sizeof(User) * user_count);

    if (strcmp(criteria, "followers") == 0) {
        qsort(sorted_users, user_count, sizeof(User), compareByFollowers);
        printf(COLOR_YELLOW "\nTop %d Influencers by Followers:\n" COLOR_RESET, n);
    } else if (strcmp(criteria, "activity") == 0) {
        qsort(sorted_users, user_count, sizeof(User), compareByActivity);
        printf(COLOR_YELLOW "\nTop %d Influencers by Activity Score:\n" COLOR_RESET, n);
    } else {
        printf("Invalid sorting criteria.\n");
        return;
    }

    for (int i = 0; i < n; i++) {
        printf("%d. %s (ID: %d) - F:%d, A:%.2f, C:%s\n",
               i + 1, sorted_users[i].name, sorted_users[i].id, sorted_users[i].followers,
               sorted_users[i].activity_score, sorted_users[i].country);
    }
}

// Searches for and displays influencers from a specific country.
void search_by_country(const char *country) {
    printf(COLOR_GREEN "\nInfluencers from %s:\n" COLOR_RESET, country);
    bool found = false;
    for (int i = 0; i < user_count; i++) {
        if (strcasecmp(users[i].country, country) == 0) {
            printf("- %s (ID: %d) - F:%d, A:%.2f\n",
                   users[i].name, users[i].id, users[i].followers, users[i].activity_score);
            found = true;
        }
    }
    if (!found) {
        printf("No influencers found from this country.\n");
    }
}

// Function to get friend recommendations based on mutual friends
void recommend_friends(const char *name) {
    int user_index = find_user_by_name(name);
    if (user_index == -1) {
        printf("User not found.\n");
        return;
    }

    int recommendations[MAX_USERS];
    int recommendation_count = 0;
    int mutual_friend_counts[MAX_USERS] = {0};
    bool already_connected[MAX_USERS] = {false};

    for (int i = 0; i < users[user_index].connection_count; i++) {
        int connected_id = users[user_index].connections[i];
        int connected_index = find_user_by_id(connected_id);
        if (connected_index != -1) {
            already_connected[connected_index] = true;
        }
    }
    already_connected[user_index] = true;

    for (int i = 0; i < user_count; i++) {
        if (!already_connected[i]) {
            for (int j = 0; j < users[user_index].connection_count; j++) {
                int connected_id = users[user_index].connections[j];
                int connected_index = find_user_by_id(connected_id);
                if (connected_index != -1) {
                    for (int k = 0; k < users[connected_index].connection_count; k++) {
                        if (users[connected_index].connections[k] == users[i].id) {
                            mutual_friend_counts[i]++;
                            break;
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < user_count; i++) {
        if (mutual_friend_counts[i] > 0) {
            recommendations[recommendation_count++] = i;
        }
    }

    for (int i = 0; i < recommendation_count - 1; i++) {
        for (int j = i + 1; j < recommendation_count; j++) {
            if (mutual_friend_counts[recommendations[i]] < mutual_friend_counts[recommendations[j]]) {
                int temp = recommendations[i];
                recommendations[i] = recommendations[j];
                recommendations[j] = temp;
            }
        }
    }

    printf(COLOR_YELLOW "\nFriend recommendations for %s (ID: %d) based on mutual friends:\n" COLOR_RESET, users[user_index].name, users[user_index].id);
    if (recommendation_count > 0) {
        for (int i = 0; i < (recommendation_count > 5 ? 5 : recommendation_count); i++) {
            int recommended_index = recommendations[i];
            printf("- %s (ID: %d) - %d mutual friends\n",
                   users[recommended_index].name, users[recommended_index].id, mutual_friend_counts[recommended_index]);
        }
    } else {
        printf("No friend recommendations found based on mutual connections.\n");
    }
}

// Displays a menu for graph algorithms related to a specific user.
void graph_algorithms_menu(int start_index) {
    int choice;
    while (1) {
        printf(COLOR_CYAN "\nGraph Algorithms for User ID %d (%s):\n" COLOR_RESET, users[start_index].id, users[start_index].name);
        printf("1. Depth-First Search (DFS) Traversal (by User ID)\n");
        printf("2. Breadth-First Search (BFS) Traversal (by User ID)\n");
        printf("3. All-PairsShortest Path (Floyd-Warshall) (by User ID)\n");
        printf("4. Return to main menu\n");
        printf("Choose option: ");
        scanf("%d", &choice);
        getchar(); // Consume newline

        if (choice == 4) break;

        switch (choice) {
            case 1:
                dfs(start_index);
                break;
            case 2:
                bfs(start_index);
                break;
            case 3:
                floyd_warshall();
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }
}

// Displays the main menu of the social network analysis tool.
void display_menu() {
    printf(COLOR_BOLD COLOR_CYAN "\n===== SOCIAL MEDIA NETWORK ANALYSIS =====\n" COLOR_RESET);
    printf("1. Show user connections count\n");
    printf("2. Show user activity score\n");
    printf("3. Find mutual connections between two users (by User ID)\n");
    printf("4. Show influencers with minimum followers\n");
    printf("5. Show top N influencers\n");
    printf("6. Search influencers by country\n");
    printf("7. Graph algorithms (DFS/BFS/Shortest Path by User ID)\n");
    printf("8. Friend recommendations based on mutual friends\n");
    printf("9. Visualize network as tree\n");
    printf("10. Exit\n");
    printf(COLOR_BOLD "Enter your choice: " COLOR_RESET);
}

// Handles the visualization of the network as a text-based tree based on user choice.
void visualize_network_tree() {
    int choice;
    printf("\nVisualize Network Tree Based On:\n");
    printf("1. Country (Top 3 Influencers)\n");
    printf("2. Mutual Connections (Starting from a user, max depth/width 3)\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // Consume newline

    if (choice == 1) {
        char country[MAX_COUNTRY];
        printf("Enter country to visualize top 3 influencers: ");
        fgets(country, sizeof(country), stdin);
        trim_newline(country);
        // Find top 3 influencers from the given country
        User country_influencers[MAX_USERS];
        int count = 0;
        for (int i = 0; i < user_count; i++) {
            if (strcasecmp(users[i].country, country) == 0) {
                country_influencers[count++] = users[i];
            }
        }
        qsort(country_influencers, count, sizeof(User), compareByFollowers);
        printf(COLOR_YELLOW "\nTree visualization of top 3 influencers from %s:\n" COLOR_RESET, country);
        for (int i = 0; i < (count > 3 ? 3 : count); i++) {
            printf("└── %s (ID: %d, F:%d, A:%.2f)\n", country_influencers[i].name, country_influencers[i].id, country_influencers[i].followers, country_influencers[i].activity_score);
        }
    } else if (choice == 2) {
        char start_name[MAX_NAME];
        printf("Enter user name to start mutual connection tree: ");
        fgets(start_name, sizeof(start_name), stdin);
        trim_newline(start_name);
        int start_index = find_user_by_name(start_name);
        if (start_index != -1) {
            printf(COLOR_CYAN "\nTree visualization of mutual connections (max depth/width 3):\n" COLOR_RESET);
            printTextTree(&users[start_index], NULL, 0, 1);
        } else {
            printf("User not found.\n");
        }
    } else {
        printf("Invalid choice for tree visualization.\n");
    }
}

int main() {
    load_users("e:/C++/DSA/Project/DSA-Project/data/social_dataset_with_connections.csv");
    build_adj_matrix();

    printf(COLOR_GREEN "Social Network Analysis System Initialized\n" COLOR_RESET);
    printf("Loaded %d users from dataset\n", user_count);

    int choice;
    char name[MAX_NAME], name2[MAX_NAME], country[MAX_COUNTRY];
    int threshold, n, index;
    char continue_choice;

    do {
        display_menu();
        scanf("%d", &choice);
        getchar(); // Consume newline

        switch (choice) {
            case 1:
                printf("Enter user name: ");
                fgets(name, sizeof(name), stdin);
                trim_newline(name);
                index = find_user_by_name(name);
                if (index != -1) {
                    printf("%s (ID: %d) has %d connections.\n", users[index].name, users[index].id, users[index].connection_count);
                } else {
                    printf("User not found.\n");
                }
                break;

            case 2:
                printf("Enter user name: ");
                fgets(name, sizeof(name), stdin);
                trim_newline(name);
                index = find_user_by_name(name);
                if (index != -1) {
                    printf("%s (ID: %d) has an activity score of %.2f\n", users[index].name, users[index].id, users[index].activity_score);
                } else {
                    printf("User not found.\n");
                }
                break;

            case 3:
                printf("Enter first user name: ");
                fgets(name, sizeof(name), stdin);
                trim_newline(name);
                printf("Enter second user name: ");
                fgets(name2, sizeof(name2), stdin);
                trim_newline(name2);
                show_mutuals(name, name2);
                break;

            case 4:
                printf("Enter minimum follower threshold: ");
                scanf("%d", &threshold);
                getchar();
                printf("\nInfluencers with more than %d followers:\n", threshold);
                for (int i = 0; i < user_count; i++) {
                    if (users[i].followers > threshold) {
                        printf("- %s (ID: %d) - %d followers (%s)\n",
                               users[i].name, users[i].id, users[i].followers, users[i].country);
                    }
                }
                break;

            case 5:
                printf("Enter number of top influencers to show: ");
                scanf("%d", &n);
                getchar();
                show_top_n_influencers_by(n, "followers"); // Default to followers
                break;

            case 6:
                printf("Enter country name: ");
                fgets(country, sizeof(country), stdin);
                trim_newline(country);
                search_by_country(country);
                break;

            case 7:
                printf("Enter user name for graph analysis: ");
                fgets(name, sizeof(name), stdin);
                trim_newline(name);
                index = find_user_by_name(name);
                if (index != -1) {
                    graph_algorithms_menu(index);
                } else {
                    printf("User not found.\n");
                }
                break;

            case 8:
                printf("Enter user name to get friend recommendations for: ");
                fgets(name, sizeof(name), stdin);
                trim_newline(name);
                recommend_friends(name);
                break;

            case 9:
                visualize_network_tree();
                break;

            case 10:
                printf("Exiting program. Goodbye!\n");
                exit(0);

            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }

        printf("\nDo you want to perform another operation? (y/n): ");
        scanf("%c", &continue_choice);
        getchar(); // Consume newline
    } while (tolower(continue_choice) == 'y');

    printf("Program finished.\n");
    return 0;
}
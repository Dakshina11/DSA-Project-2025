# DSA-Project-2025
Social Media Analysis 
## Overview
The *Social Media Network Analysis System* is a C-based program designed to analyze and visualize social media networks. It provides various functionalities such as finding mutual connections, recommending friends, identifying top influencers, and performing graph-based algorithms like BFS, DFS, and Floyd-Warshall.

The program uses a dataset (social_dataset_with_connections.csv) to load user information, including their connections, activity scores, and other attributes.

---

## Features
1. *Show User Connections Count*: Displays the number of connections a user has.
2. *Show User Activity Score*: Displays the activity score of a user.
3. *Find Mutual Connections*: Finds and displays mutual connections between two users.
4. *Show Influencers with Minimum Followers*: Lists influencers with followers above a specified threshold.
5. *Show Top N Influencers*: Displays the top N influencers based on followers or activity score.
6. *Search Influencers by Country*: Lists influencers from a specific country.
7. *Graph Algorithms*:
   - Depth-First Search (DFS)
   - Breadth-First Search (BFS)
   - All-Pairs Shortest Path (Floyd-Warshall)
8. *Friend Recommendations*: Recommends friends for a user based on mutual connections.
9. *Visualise Network as Tree*: Displays a tree visualisation of the network based on country or mutual connections.
10. *Exit*: Exits the program.

---

## Dataset Format
The program uses a CSV file named social_dataset_with_connections.csv to load user data. The file should be formatted as follows:

### Example:
```csv
id, name, followers,activity_score, country, connections
1,Alice,150,4.5,USA,2 3 4
2,Bob,200,3.8,Canada,1 3
3,Charlie,180,4.2,USA,1 2
4,Diana,250,4.9,UK,1

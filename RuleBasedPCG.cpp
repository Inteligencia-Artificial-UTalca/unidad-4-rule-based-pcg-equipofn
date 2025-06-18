#include <iostream>
#include <vector>
#include <random>   // For random number generation
#include <chrono>   // For seeding the random number generator

using Map = std::vector<std::vector<int>>;

void printMap(const Map& map) {
    std::cout << "--- Current Map ---" << std::endl;
    for (const auto& row : map) {
        for (int cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "-------------------" << std::endl;
}

Map cellularAutomata(const Map& currentMap, int W, int H, int R, double U) 
{
    Map newMap = currentMap;
    for (int x = 0; x < H; ++x) {
        for (int y = 0; y < W; ++y) {
            int count = 0, total = 0;
            for (int dx = -R; dx <= R; ++dx) {
                for (int dy = -R; dy <= R; ++dy) {
                    int nx = x + dx, ny = y + dy;
                    if (nx < 0 || nx >= H || ny < 0 || ny >= W) {
                        count++;
                        total++;
                    } else {
                        count += currentMap[nx][ny];
                        total++;
                    }
                }
            }
            double ratio = (double)count / total;
            newMap[x][y] = (ratio > U) ? 1 : 0;
        }
    }
    return newMap;
}
void cellularAutomataInPlace(Map& map, int W, int H, int R, double U) {
    for (int x = 0; x < H; ++x) {
        for (int y = 0; y < W; ++y) {
            int count = 0, total = 0;
            for (int dx = -R; dx <= R; ++dx) {
                for (int dy = -R; dy <= R; ++dy) {
                    int nx = x + dx, ny = y + dy;
                    if (nx < 0 || nx >= H || ny < 0 || ny >= W) {
                        count++;
                        total++;
                    } else {
                        count += map[nx][ny];
                        total++;
                    }
                }
            }
            double ratio = (double)count / total;
            map[x][y] = (ratio > U) ? 1 : 0;
        }
    }
}

Map drunkAgent(const Map& currentMap, int W, int H, int J, int I, int roomSizeX, int roomSizeY,
               double probGenerateRoom, double probIncreaseRoom,
               double probChangeDirection, double probIncreaseChange,
               int& agentX, int& agentY) {
    Map newMap = currentMap;
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dirDist(0, 3);
    std::uniform_real_distribution<double> probDist(0.0, 1.0);

    int dx[4] = {-1, 1, 0, 0};
    int dy[4] = {0, 0, -1, 1};

    if (agentX < 0 || agentX >= H || agentY < 0 || agentY >= W) {
        agentX = rng() % H;
        agentY = rng() % W;
    }

    double roomProb = probGenerateRoom;
    double dirProb = probChangeDirection;
    int dir = dirDist(rng);

    for (int j = 0; j < J; ++j) {
        for (int i = 0; i < I; ++i) {
            if (agentX >= 0 && agentX < H && agentY >= 0 && agentY < W)
                newMap[agentX][agentY] = 1;
                
            if (probDist(rng) < roomProb) {
                int rx = std::max(0, agentX - roomSizeX / 2);
                int ry = std::max(0, agentY - roomSizeY / 2);
                int rxe = std::min(H, rx + roomSizeX);
                int rye = std::min(W, ry + roomSizeY);
                for (int x = rx; x < rxe; ++x)
                    for (int y = ry; y < rye; ++y)
                        newMap[x][y] = 1;
                roomProb = probGenerateRoom;
            } else {
                roomProb = std::min(1.0, roomProb + probIncreaseRoom);
            }

            if (probDist(rng) < dirProb) {
                dir = dirDist(rng);
                dirProb = probChangeDirection;
            } else {
                dirProb = std::min(1.0, dirProb + probIncreaseChange);
            }

            int nx = agentX + dx[dir];
            int ny = agentY + dy[dir];

            if (nx < 0 || nx >= H || ny < 0 || ny >= W) {
                dir = dirDist(rng);
                break;
            } else {
                agentX = nx;
                agentY = ny;
            }
        }
    }
    return newMap;
}

int main() {
    std::cout << "--- CELLULAR AUTOMATA AND DRUNK AGENT SIMULATION ---" << std::endl;

    int mapRows = 10;
    int mapCols = 20;
    Map myMap(mapRows, std::vector<int>(mapCols, 0));

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> binDist(0, 1);
    for (int i = 0; i < mapRows; ++i)
        for (int j = 0; j < mapCols; ++j)
            myMap[i][j] = binDist(rng);

    int drunkAgentX = mapRows / 2;
    int drunkAgentY = mapCols / 2;

    std::cout << "\nInitial map state:" << std::endl;
    printMap(myMap);

    int numIterations = 5;

    int ca_W = mapCols;
    int ca_H = mapRows;
    int ca_R = 1;
    double ca_U = 0.5;

    int da_W = mapCols;
    int da_H = mapRows;
    int da_J = 5;
    int da_I = 10;
    int da_roomSizeX = 5;
    int da_roomSizeY = 3;
    double da_probGenerateRoom = 0.1;
    double da_probIncreaseRoom = 0.05;
    double da_probChangeDirection = 0.2;
    double da_probIncreaseChange = 0.03;

    for (int iteration = 0; iteration < numIterations; ++iteration) {
        std::cout << "\n--- Iteration " << iteration + 1 << " ---" << std::endl;

        //myMap = cellularAutomata(myMap, ca_W, ca_H, ca_R, ca_U);
        cellularAutomataInPlace(myMap, ca_W, ca_H, ca_R, ca_U);
        myMap = drunkAgent(myMap, da_W, da_H, da_J, da_I, da_roomSizeX, da_roomSizeY,
                           da_probGenerateRoom, da_probIncreaseRoom,
                           da_probChangeDirection, da_probIncreaseChange,
                           drunkAgentX, drunkAgentY);

        printMap(myMap);
    }

    std::cout << "\n--- Simulation Finished ---" << std::endl;
    return 0;
}
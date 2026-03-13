#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define ROW 10
#define COL 10

// Apothikeush monopatiou
typedef struct Path {
    int x, y;               // Thesh kombou (x, y)
    int g, f;               // g: Pragmatikh apostash, f: ektimhsh apostashs
    struct Path* parent;    // Goneas monopatiou
} Path;

// Apothikeush swrou
typedef struct MinHeap {
    Path** paths;
    int size;
    int capacity;
} MinHeap;

typedef struct Path Path;
typedef struct MinHeap MinHeap;
MinHeap* createMinHeap(int capacity);                // Dhmiourgia swrou
void swapPaths(Path** a, Path** b);                  // Adallagh duo monopatiwn sto swro 
void heapifyUp(MinHeap* heap, int index);            // Metatopish pros ta panw tou monopatiou sto swro
void heapifyDown(MinHeap* heap, int index);          // Metatopish pros ta katw tou monopatiou sto swro
void HeapInsert(MinHeap* heap, Path* path);          // Eisagwgh monopatiou sto swro
Path* HeapExtract(MinHeap* heap);                    // Afairesh tou monopatiou me thn elaxisth ektimhsh apo to swro
int isValid(int x, int y);                           // Elegxos an to shmeio einai entos twn orivn tou plegmatos
int calculateManhattanDistance(int x1, int y1, int x2, int y2);                                         // Ypologismos ths apostashs Manhattan
void generateRoom(int grid[ROW][COL], int *goalX, int *goalY, int *startX, int *startY);                // Dhmiourgia tuxaiou dwmatiou me embodia kai tuxaia topothethsh thw exodou
Path* AStar(int grid[ROW][COL], int startX, int startY, int goalX, int goalY);                          // Algorithmos A* me xrhsh ouras proteraiothtas kai swrou
void expandHeap(MinHeap* heap);                                                                         // Epektash xwrhtikothtas swrou
void moveRobot(int grid[ROW][COL], int *currentX, int *currentY, int newX, int newY);                   // Kinhsh robot
void printRoom(int grid[ROW][COL], int startX, int startY, int goalX, int goalY);                       // Ektupwsh tou dwmatiou me thn diadromh
void printReversePath(Path* path, int grid[ROW][COL], int startX, int startY, int goalX, int goalY);    // Ektupwsh ths antistrofhs poreias ths diadromhs apo thn ejodo

int main(void) {
    int grid[ROW][COL];
    int goalX, goalY;
    int startX, startY;
    Path* currentPath = NULL; // Dhlwsh currentPath 

    generateRoom(grid, &goalX, &goalY, &startX, &startY);  // Dhmiourgia tyxaiou dwmatiou

    printf("Dwmatio prin thn anazhthsh:\n");
    printRoom(grid, startX, startY, goalX, goalY);

    currentPath = AStar(grid, startX, startY, goalX, goalY);  // Perasma metablhths currentPath ws orisma apo thn synarthsh AStar

    if (grid[goalX][goalY] == 2) {
        
        char answer;    // Rwtame ton xrhsth ean thelei na dei thn antistrofh diadromh
        printf("Thelete na deite thn antistrofh diadromh? (y/n): ");
        
        while (1) { // Prepei o xrhsths na eisagei 'Y'or y' or 'N' or 'n'
            scanf(" %c", &answer);  // %c eisodos xarakthra

            if (answer == 'y' || answer == 'Y') {
                printf("Ektypwsh antistrofhs diadromhs:\n");
                printReversePath(currentPath, grid, startX, startY, goalX, goalY);  // Perasma metablhths currentPath ws orisma
                break;  // Ejodos an den eisagei o xrhsths to swsto gramma
            } 
            else if (answer == 'n' || answer == 'N') {
                printf("H antistrodh diadromh de tha emfanistei.\n");
                printf("Exodos...\n");
                break;  // Ejodos an den eisagei o xrhsths to swsto gramma
            } 
            else {
                printf("Lathos eisodos. Parakalw plhktrologhste 'Y' h y' h 'N' h 'n': ");
            }
        }
    }
    return 0;
}

// Dhmiourgia swrou
MinHeap* createMinHeap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->paths = (Path**)malloc(sizeof(Path*) * capacity);
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

// Adallagh duo monopatiwn sto swro 
void swapPaths(Path** a, Path** b) {
    Path* temp = *a;
    *a = *b;
    *b = temp;
}

// Metatopish pros ta panw tou monopatiou sto swro
void heapifyUp(MinHeap* heap, int index) {
    while (index > 0 && heap->paths[(index - 1) / 2]->f > heap->paths[index]->f) {
        swapPaths(&heap->paths[(index - 1) / 2], &heap->paths[index]);
        index = (index - 1) / 2;
    }
}

// Metatopish pros ta katw tou monopatiou sto swro
void heapifyDown(MinHeap* heap, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->size && heap->paths[left]->f < heap->paths[smallest]->f) {
        smallest = left;
    }

    if (right < heap->size && heap->paths[right]->f < heap->paths[smallest]->f) {
        smallest = right;
    }

    if (smallest != index) {
        swapPaths(&heap->paths[index], &heap->paths[smallest]);
        heapifyDown(heap, smallest);
    }
}

// Eisagwgh monopatiou sto swro
void HeapInsert(MinHeap* heap, Path* path) {
    if (heap->size == heap->capacity) {
        expandHeap(heap); // Dunamikh epektash xwrhtikothtas
    }
    heap->paths[heap->size] = path;
    heapifyUp(heap, heap->size);
    heap->size++;
}

// Afairesh tou monopatiou me thn elaxisth ektimhsh apo to swro
Path* HeapExtract(MinHeap* heap) {
    if (heap->size == 0) {
        return NULL;
    } 
    Path* minPath = heap->paths[0];
    heap->paths[0] = heap->paths[heap->size - 1];
    heap->size--;
    heapifyDown(heap, 0);

    return minPath;
}

// Elegxos an to shmeio einai entos twn orivn tou plegmatos
int isValid(int x, int y) {
    return (x >= 0 && x < ROW && y >= 0 && y < COL);
}

// Ypologismos ths apostashs Manhattan
int calculateManhattanDistance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// Dhmiourgia tuxaiou dwmatiou me embodia kai tuxaia topothethsh thw exodou
void generateRoom(int grid[ROW][COL], int *goalX, int *goalY, int *startX, int *startY) {
    srand(time(0));

    int obstaclePercentage = rand() % 81 + 10;  // Dhmiourgia tyxaiou posostou empodiwn (10% - 90%)

    printf("Tyxaio pososto empodiwn: %d%%\n", obstaclePercentage);

    for (int i = 0; i < ROW; i++) { // Gemisma dwmatiou me empodia kai kena
        for (int j = 0; j < COL; j++) {
            if ((rand() % 100) < obstaclePercentage) {
                grid[i][j] = 1;  // An to pososto einai mikrotero apo to obstaclePercentage, bazoume empodio
            } 
            else {
                grid[i][j] = 0;  // Alliws, bazoume keno
            }
        }
    }

    do {    // Tyxaia topothetish ths ejodou (E) sta pleyrika
        int side = rand() % 4;  // 0: epanw, 1: katw, 2: aristera, 3: dejia
        if (side == 0) {  // Panw
            *goalX = 0;
            *goalY = rand() % COL;
        } 
        else if (side == 1) {  // Katw
            *goalX = ROW - 1;
            *goalY = rand() % COL;
        } 
        else if (side == 2) {  // Aristera
            *goalX = rand() % ROW;
            *goalY = 0;
        } 
        else {  // Dejia
            *goalX = rand() % ROW;
            *goalY = COL - 1;
        }
    } while (grid[*goalX][*goalY] != 0);  // Diasfalizoume oti h ejodos de kalyptetai apo empodio

    do {    // Tyxaia topothetish tou robot (R) sta pleyrika
        int side = rand() % 4;  // 0: panw, 1: katw, 2: aristera, 3: dejia
        if (side == 0) {  // Panw 
            *startX = 0;
            *startY = rand() % COL;
        } 
        else if (side == 1) {  // Katw
            *startX = ROW - 1;
            *startY = rand() % COL;
        } 
        else if (side == 2) {  // Aristera
            *startX = rand() % ROW;
            *startY = 0;
        } 
        else {  // Dejia
            *startX = rand() % ROW;
            *startY = COL - 1;
        }
    } while ((grid[*startX][*startY] != 0) || (*startX == *goalX && *startY == *goalY));  // Apofygh sygkroushs me empodia h thn ejodo
    // Diasfalish oti oi theseis einai eleytheres
    grid[*goalX][*goalY] = 0;  // H ejodos einai eleytherh
    grid[*startX][*startY] = 0;  // H thesh tou robot einai eleytherh
}

// Anamonh gia Enter
void waitForEnter() {
    char input;
    while (1) {
        printf("Pathste Enter gia na sunexisete...\n");
        input = getchar();      // Diabazei thn eisodo
        if (input == '\n') {    // Elegxei ean einai Enter
            break;              // Ejodos an den pathsei o xrhsths Enter
        } 
        else {
            while (getchar() != '\n');  // Katharismos buffer kai emfanish mhnymatos lathous
            printf("Lathos! Pathste mono Enter.\n");
        }
    }
}

// Algorithmos A* me xrhsh ouras proteraiothtas kai swrou
Path* AStar(int grid[ROW][COL], int startX, int startY, int goalX, int goalY) {
    MinHeap* openList = createMinHeap(100);    // Dhmiourgia swrou gia anoixth lista
    MinHeap* closedList = createMinHeap(100);  // Dhmiourgia swrou gia kleisth lista

    Path* startPath = (Path*)malloc(sizeof(Path));  // Dhmiourgia arxikou monopatiou kai h prosthikh tou sto swro (open list)
    startPath->x = startX;
    startPath->y = startY;
    startPath->g = 0;
    startPath->f = calculateManhattanDistance(startX, startY, goalX, goalY);
    startPath->parent = NULL;
    HeapInsert(openList, startPath);

    while (openList->size > 0) {
        Path* currentPath = HeapExtract(openList);  // Pairnoume kombo me thn elaxisth ektimwmenh timh f

        if (currentPath->x == goalX && currentPath->y == goalY) {   // An broume th diadromh, kanoume backtracking
            Path* temp = currentPath;
            Path* pathSteps[ROW * COL];
            int stepCount = 0;

            while (temp != NULL) {
                pathSteps[stepCount++] = temp;
                temp = temp->parent;
            }
            printf("Brethike diadromh!\n");

            for (int i = stepCount - 1; i >= 0; i--) {
                Path* p = pathSteps[i];
                grid[p->x][p->y] = 2;  // Basikh diadromh 2
                printRoom(grid, startX, startY, goalX, goalY);
                if (i > 0) {  // Mono an den einai to teleutaio bhma
                    if (i > 0) {  
                        waitForEnter();
                    }
                }
            }

            return currentPath;  // Epistrofh telikhs diadromhs
        }
        // Epektash geitonwn gia ejereunhsh
        int dx[] = {1, -1, 0, 0};  // Kateythunseis gia x
        int dy[] = {0, 0, 1, -1};  // Kateythunseis gia y

        for (int i = 0; i < 4; i++) {
            int newX = currentPath->x + dx[i];
            int newY = currentPath->y + dy[i];

            if (newX >= 0 && newX < ROW && newY >= 0 && newY < COL && grid[newX][newY] != 1) {  // Elegxos an h thesh einai egkyrh kai den einai empodio
                Path* neighbor = (Path*)malloc(sizeof(Path));
                neighbor->x = newX;
                neighbor->y = newY;
                neighbor->g = currentPath->g + 1;
                neighbor->f = neighbor->g + calculateManhattanDistance(newX, newY, goalX, goalY);
                neighbor->parent = currentPath;

                HeapInsert(openList, neighbor); // Eisagwgh geitona sto openlist

                if (grid[newX][newY] != 2 && grid[newX][newY] != 1) {   // Katagrafh ejereunhmenwn thesewn "*"
                    grid[newX][newY] = 4; // Shmeiwsh theshs ws ejereunhmenh me "*"
                }
            }
        }
    }
    
    printf("De brethike diadromh!\n");   // An de brethike diadromh
    printf("Exodos...");
    return NULL; // NULL an de brethike diadromh
}

// Synarthsh gia thn ektypwsh ths antistrofhs poreias ths diadromhs
void printReversePath(Path* path, int grid[ROW][COL], int startX, int startY, int goalX, int goalY) {
    Path* current = path;

        while (current != NULL) {
        printf("Bhma: (%d, %d)\n", current->x, current->y); // Ektypwsh trexousas theshs

        grid[current->x][current->y] = 3;  // Xrhsh tou arithmou 3 gia na deijoume th poreia ths diadromhs
        printRoom(grid, startX, startY, goalX, goalY);

        current = current->parent;

        if (current != NULL) {  // Mono an den einai to teleutaio bhma
            if (current != NULL) {  
                waitForEnter();
            }
        }
    }
}

// Epektash xwrhtikothtas swrou
void expandHeap(MinHeap* heap) { 
    heap->capacity *= 2;
    heap->paths = (Path**)realloc(heap->paths, sizeof(Path*) * heap->capacity);
    if (!heap->paths) { // Den einai dynath h epektash mnhmhs swrou
        exit(EXIT_FAILURE);
    }
}

// Kinhsh robot
void moveRobot(int grid[ROW][COL], int *currentX, int *currentY, int newX, int newY) {
    grid[*currentX][*currentY] = 3; // Shmeiwnoume thn trexousa thesh ws diadromh pou perase to robot

    // Metakinoume to robot sth nea thesh
    *currentX = newX;
    *currentY = newY;
}

// Ektupwsh tou dwmatiou me thn diadromh
void printRoom(int grid[ROW][COL], int startX, int startY, int goalX, int goalY) {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (i == startX && j == startY) {
                printf("R ");  // Robot sth trexousa thesi
            } 
            else if (i == goalX && j == goalY) {
                printf("E ");  // Ejodos
            } 
            else if (grid[i][j] == 4) {
                printf("* ");  // Ypoloipes diadromes
            } 
            else if (grid[i][j] == 2) {
                printf("R ");  // H basikh diadromh
            } 
            else if (grid[i][j] == 1) {
                printf("# ");  // Empodio
            } 
            else {
                printf(". ");  // Keno
            }
        }
        printf("\n");
    }
    printf("\n");
}
/*******************************************************************************************
*
*   raylib maze game
*
*   Procedural maze generator using Maze Grid Algorithm
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2024-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <stdlib.h>     // Required for: malloc(), free()
#include <time.h>

#define MAZE_WIDTH          64
#define MAZE_HEIGHT         64
#define MAZE_SCALE          10.0f
#define MAX_MAZE_ITEMS      16

// Declare new data type: Point
typedef struct Point {
    int x;
    int y;
} Point;

// Generate procedural maze image, using grid-based algorithm
// NOTE: Functions defined as static are internal to the module
static Image GenImageMaze(int width, int height, int spacingRows, int spacingCols, float pointChance);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;
    
    float playerSpeed = 2.0f;

    InitWindow(screenWidth, screenHeight, "Delivery04 - maze game");

    // Current application mode
    int currentMode = 1;    // 0-Game, 1-Editor

    // Random seed defines the random numbers generation,
    // always the same if using the same seed
    SetRandomSeed((unsigned int)time(NULL));

    // Generate maze image using the grid-based generator
    // TODO: [1p] Implement GenImageMaze() function with required parameters
    Image imMaze = GenImageMaze(MAZE_WIDTH, MAZE_HEIGHT, 4, 4, 0.75f);

    // Load a texture to be drawn on screen from our image data
    // WARNING: If imMaze pixel data is modified, texMaze needs to be re-loaded
    Texture texMaze = LoadTextureFromImage(imMaze);

    // Player start-position and end-position initialization
    Point startCell = { 1, 1 };
    Point endCell = { imMaze.width - 2, imMaze.height - 2 };

    // Maze drawing position (editor mode)
    Vector2 mazePosition = {
        GetScreenWidth()/2 - texMaze.width*MAZE_SCALE/2,
        GetScreenHeight()/2 - texMaze.height*MAZE_SCALE/2
    };

    // Define player position and size
    Rectangle player = { mazePosition.x + 1*MAZE_SCALE + 2, mazePosition.y + 1*MAZE_SCALE + 2, 8, 8 };


    // Camera 2D for 2d gameplay mode
    // TODO: [2p] Initialize camera parameters as required
    Camera2D camera2d = { 0 };
    camera2d.target = (Vector2){ player.x + player.width/2, player.y + player.height/2 };
    camera2d.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera2d.rotation = 0.0f;
    camera2d.zoom = 1.0f;

    // Mouse selected cell for maze editing
    Point selectedCell = { 0 };

    // Maze items position and state
    Point mazeItems[MAX_MAZE_ITEMS] = { 0 };
    bool mazeItemPicked[MAX_MAZE_ITEMS] = { 0 };
    
    // Define textures to be used as our "biomes"
    Texture texBiomes[4] = { 0 };
    texBiomes[0] = LoadTexture("resources/maze_atlas01.png");
    // TODO: Load additional textures for different biomes
    int currentBiome = 0;

    // TODO: Define all variables required for game UI elements (sprites, fonts...)

    SetTargetFPS(60);       // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // Select current mode as desired
        if (IsKeyPressed(KEY_SPACE)) currentMode = !currentMode; // Toggle mode: 0-Game, 1-Editor

        if (currentMode == 0) // Game mode
        {
            // TODO: [2p] Player 2D movement from predefined Start-point to End-point
            // Implement maze 2D player movement logic (cursors || WASD)
            // Use imMaze pixel information to check collisions
            // Detect if current playerCell == endCell to finish game
            
            // 1) Actualizar cámara para que siga al jugador
            camera2d.target.x = player.x + player.width/2;
            camera2d.target.y = player.y + player.height/2;
            
            // 2) Movimiento del jugador (WASD o flechas).
            //    - Calculamos la posición "nueva" antes de mover
            //    - Verificamos si colisionaría con pared (blanca)
            //    - Si NO colisiona, actualizamos player.x / player.y
            // Clonamos la posición actual del jugador
            float newX = player.x;
            float newY = player.y;
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    newY -= playerSpeed;
            if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  newY += playerSpeed;
            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  newX -= playerSpeed;
            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) newX += playerSpeed;
            
            // 3) Verificamos colisión con pared
            //    Para ello, obtenemos el centro del jugador y convertimos a coordenadas de celda
            //    Revisamos el color en imMaze: si es WHITE => pared => no movemos.
            // Calcular las posiciones de las 4 esquinas en coordenadas de celda
            int left   = (int)((newX - mazePosition.x) / MAZE_SCALE);
            int right  = (int)(((newX + player.width) - mazePosition.x) / MAZE_SCALE);
            int top    = (int)((newY - mazePosition.y) / MAZE_SCALE);
            int bottom = (int)(((newY + player.height) - mazePosition.y) / MAZE_SCALE);

            // Asegurarse de no salir de los límites
            if (left < 0) left = 0;
            if (right >= MAZE_WIDTH) right = MAZE_WIDTH - 1;
            if (top < 0) top = 0;
            if (bottom >= MAZE_HEIGHT) bottom = MAZE_HEIGHT - 1;

            // Comprobar las 4 esquinas del jugador
            bool collision = false;
            if (ColorIsEqual(GetImageColor(imMaze, left, top), WHITE)) collision = true;
            if (ColorIsEqual(GetImageColor(imMaze, right, top), WHITE)) collision = true;
            if (ColorIsEqual(GetImageColor(imMaze, left, bottom), WHITE)) collision = true;
            if (ColorIsEqual(GetImageColor(imMaze, right, bottom), WHITE)) collision = true;

            // Solo actualizamos la posición si no hay colisión
            if (!collision)
            {
                player.x = newX;
                player.y = newY;
            }
            
            // 4) Comprobar si hemos llegado al endCell
            //    Basta con comparar las celdas del jugador con endCell
            // Calcular la celda actual del jugador usando el centro del rectángulo
            int centerCellX = (int)((player.x + player.width/2 - mazePosition.x) / MAZE_SCALE);
            int centerCellY = (int)((player.y + player.height/2 - mazePosition.y) / MAZE_SCALE);

            if ((centerCellX == endCell.x) && (centerCellY == endCell.y))
            {
                DrawText("YOU REACHED THE END!", 200, 200, 40, GREEN);
            }

            
            // TODO: [1p] Camera 2D system following player movement around the map
            // Update Camera2D parameters as required to follow player and zoom control

            // TODO: [2p] Maze items pickup logic
        }
        else if (currentMode == 1) // Editor mode
        {
            // TODO: [2p] Maze editor mode, edit image pixels with mouse.
            // Implement logic to selecte image cell from mouse position -> TIP: GetMousePosition()
            // NOTE: Mouse position is returned in screen coordinates and it has to 
            // transformed into image coordinates
            // Once the cell is selected, if mouse button pressed add/remove image pixels
            
            // WARNING: Remember that when imMaze changes, texMaze must be also updated!
            
             // 1) Obtener posición del ratón en coordenadas de pantalla
            Vector2 mousePos = GetMousePosition();
            float mouseXRelative = (mousePos.x - mazePosition.x)/MAZE_SCALE;
            float mouseYRelative = (mousePos.y - mazePosition.y)/MAZE_SCALE;
            int cellX = (int)mouseXRelative;
            int cellY = (int)mouseYRelative;
            
            
            if ((cellX >= 0) && (cellX < MAZE_WIDTH) &&
                (cellY >= 0) && (cellY < MAZE_HEIGHT))
            {

                // BOTÓN IZQUIERDO: BLACK (camino)
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                {
                    ImageDrawPixel(&imMaze, cellX, cellY, BLACK);
                    UpdateTexture(texMaze, imMaze.data);  // Refrescar textura
                }
                // BOTÓN CENTRAL: RED (ítem)
                else if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
                {
                    ImageDrawPixel(&imMaze, cellX, cellY, RED);
                    UpdateTexture(texMaze, imMaze.data);
                }
                // BOTÓN DERECHO: WHITE (pared)
                else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
                {
                    // Si además mantenemos CTRL, lo ponemos en GREEN (punto final)
                    if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
                    {
                        ImageDrawPixel(&imMaze, cellX, cellY, GREEN);
                        UpdateTexture(texMaze, imMaze.data);

                        // (Opcional) Actualizar endCell si queremos que sea la meta
                        endCell.x = cellX;
                        endCell.y = cellY;
                    }
                    else
                    {
                        ImageDrawPixel(&imMaze, cellX, cellY, WHITE);
                        UpdateTexture(texMaze, imMaze.data);
                    }
                }
            }

            // TODO: [2p] Collectible map items: player score
            // Using same mechanism than maze editor, implement an items editor, registering
            // points in the map where items should be added for player pickup -> TIP: Use mazeItems[]
        }

        // TODO: [1p] Multiple maze biomes supported
        // Implement changing between the different textures to be used as biomes
        // NOTE: For the 3d model, the current selected texture must be applied to the model material  

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            if (currentMode == 0) // Game mode
            {
                // Draw maze using camera2d (for automatic positioning and scale)
                BeginMode2D(camera2d);

                    // TODO: Draw maze walls and floor using current texture biome 
                    DrawTextureEx(texMaze, mazePosition, 0.0f, MAZE_SCALE, WHITE);
             
                    // TODO: Draw player rectangle or sprite at player position
                    DrawRectangleRec(player, BLUE);

                    // TODO: Draw maze items 2d (using sprite texture?)

                EndMode2D();

                // TODO: Draw game UI (score, time...) using custom sprites/fonts
                // NOTE: Game UI does not receive the camera2d transformations,
                // it is drawn in screen space coordinates directly
                DrawText("GAME MODE", 10, 40, 20, DARKGRAY);
            }
            else if (currentMode == 1) // Editor mode
            {
                // Draw generated maze texture, scaled and centered on screen 
                DrawTextureEx(texMaze, mazePosition, 0.0f, MAZE_SCALE, WHITE);

                // Draw lines rectangle over texture, scaled and centered on screen 
                DrawRectangleLines(mazePosition.x, mazePosition.y, MAZE_WIDTH*MAZE_SCALE, MAZE_HEIGHT*MAZE_SCALE, RED);

                // TODO: Draw player using a rectangle, consider maze screen coordinates!
                DrawRectangleRec(player, BLUE);

                // TODO: Draw editor UI required elements
                DrawText("EDITOR MODE", 10, 40, 20, DARKGRAY);
                DrawText("Left = BLACK", 10, 60, 20, DARKGRAY);
                DrawText("Middle = RED", 10, 80, 20, DARKGRAY);
                DrawText("Right = WHITE", 10, 100, 20, DARKGRAY);
                DrawText("Right+Ctrl = GREEN", 10, 120, 20, DARKGRAY);
            }

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texMaze);     // Unload maze texture from VRAM (GPU)
    UnloadImage(imMaze);        // Unload maze image from RAM (CPU)

    // TODO: Unload all loaded resources
    
    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Generate procedural maze image, using grid-based algorithm
// NOTE: Color scheme used: WHITE = Wall, BLACK = Walkable, RED = Item
static Image GenImageMaze(int width, int height, int spacingRows, int spacingCols, float pointChance)
{
    // 1) Crear un array temporal para guardar la información de cada celda
    //    0 = Caminable, 1 = Pared
    unsigned char *mapData = (unsigned char *)malloc(width * height * sizeof(unsigned char));
    
    // Inicializar todo el mapa como 0 (caminable)
    for (int i = 0; i < width * height; i++) mapData[i] = 0;
    
    // 2) Poner los bordes del mapa como paredes (1)
    //    Esto evita que el algoritmo dibuje fuera de los límites
    for (int x = 0; x < width; x++)
    {
        mapData[0 * width + x] = 1;           // Borde superior
        mapData[(height - 1) * width + x] = 1; // Borde inferior
    }
    for (int y = 0; y < height; y++)
    {
        mapData[y * width + 0] = 1;           // Borde izquierdo
        mapData[y * width + (width - 1)] = 1; // Borde derecho
    }
    
    // 3) Generar y almacenar los puntos aleatorios (con spacing y probabilidad)
    //    Recorremos la cuadrícula saltando cada spacingRows / spacingCols
    //    y con pointChance decidimos si ponemos un punto en esa celda
    int maxPoints = ((height - 2) / spacingRows) * ((width - 2) / spacingCols);
    Point *points = (Point *)malloc(maxPoints * sizeof(Point));
    int count = 0;

    for (int y = spacingRows; y < height - 1; y += spacingRows)
    {
        for (int x = spacingCols; x < width - 1; x += spacingCols)
        {
            float rnd = (float)GetRandomValue(0, 10000)/10000.0f; // Valor entre 0.0 y 1.0
            if (rnd < pointChance)
            {
                points[count].x = x;
                points[count].y = y;
                count++;
            }
        }
    }
    
    // 4) Barajar (shuffle) el array de puntos para que el orden de trazar paredes sea aleatorio
    for (int i = 0; i < count - 1; i++)
    {
        // Elegimos un índice aleatorio desde i hasta el final
        int r = GetRandomValue(i, count - 1);
        // Intercambiamos points[i] con points[r]
        Point temp = points[i];
        points[i] = points[r];
        points[r] = temp;
    }
    
    // 5) Para cada punto, elegir una dirección aleatoria y "avanzar" pintando paredes
    //    hasta toparse con otra pared o con el borde
    for (int i = 0; i < count; i++)
    {
        int px = points[i].x;
        int py = points[i].y;
        
        // Elegir dirección aleatoria (0=right, 1=left, 2=down, 3=up)
        int dir = GetRandomValue(0, 3);
        int dx = 0, dy = 0;
        
        switch (dir)
        {
            case 0: dx = 1;  dy = 0;  break; // Derecha
            case 1: dx = -1; dy = 0;  break; // Izquierda
            case 2: dx = 0;  dy = 1;  break; // Abajo
            case 3: dx = 0;  dy = -1; break; // Arriba
        }
        
        // Avanzar en la dirección elegida, pintando paredes
        while (1)
        {
            // Si ya es pared o está en el borde, detenemos
            if (mapData[py * width + px] == 1) break;
            
            // Marcamos la celda como pared
            mapData[py * width + px] = 1;
            
            // Avanzamos
            px += dx;
            py += dy;
            
            // Si salimos del mapa o topamos un borde, terminamos
            if ((px < 0) || (px >= width) || (py < 0) || (py >= height)) break;
        }
    }
    
    // Ya no necesitamos el array de puntos
    free(points);
    
    // 6) Crear la Image final y asignar cada pixel según mapData
    //    Formato: RGBA de 32 bits (PIXELFORMAT_UNCOMPRESSED_R8G8B8A8)
    Color *pixels = (Color *)malloc(width * height * sizeof(Color));
    
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Si la celda es 1 => Pared (blanco)
            if (mapData[y * width + x] == 1)
            {
                pixels[y * width + x] = WHITE;
            }
            else
            {
                // 0 => Caminable (negro)
                pixels[y * width + x] = BLACK;
            }
        }
    }
    
    // Liberar el array de celdas (ya tenemos los píxeles listos)
    free(mapData);
    
    // 7) Construir la estructura Image de raylib
    Image imMaze = { 0 };
    imMaze.data = pixels;
    imMaze.width = width;
    imMaze.height = height;
    imMaze.mipmaps = 1;
    imMaze.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    
    // Retornar la imagen resultante
    return imMaze;
}
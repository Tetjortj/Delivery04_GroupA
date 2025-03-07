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

    InitWindow(screenWidth, screenHeight, "Delivery04 - maze game");

    // Current application mode
    int currentMode = 1;    // 0-Game, 1-Editor

    // Random seed defines the random numbers generation,
    // always the same if using the same seed
    SetRandomSeed((unsigned int)time(NULL));

    // Generate maze image using the grid-based generator
    // TODO: [1p] Implement GenImageMaze() function with required parameters
    Image imMaze = GenImageMaze(MAZE_WIDTH, MAZE_HEIGHT, 4, 4, 0.5f);

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
    Rectangle player = { mazePosition.x + 1*MAZE_SCALE + 2, mazePosition.y + 1*MAZE_SCALE + 2, 4, 4 };

    // Camera 2D for 2d gameplay mode
    // TODO: [2p] Initialize camera parameters as required
    Camera2D camera2d = { 0 };

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
             
                    // TODO: Draw player rectangle or sprite at player position

                    // TODO: Draw maze items 2d (using sprite texture?)

                EndMode2D();

                // TODO: Draw game UI (score, time...) using custom sprites/fonts
                // NOTE: Game UI does not receive the camera2d transformations,
                // it is drawn in screen space coordinates directly
            }
            else if (currentMode == 1) // Editor mode
            {
                // Draw generated maze texture, scaled and centered on screen 
                DrawTextureEx(texMaze, mazePosition, 0.0f, MAZE_SCALE, WHITE);

                // Draw lines rectangle over texture, scaled and centered on screen 
                DrawRectangleLines(mazePosition.x, mazePosition.y, MAZE_WIDTH*MAZE_SCALE, MAZE_HEIGHT*MAZE_SCALE, RED);

                // TODO: Draw player using a rectangle, consider maze screen coordinates!

                // TODO: Draw editor UI required elements
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
Image GenImageMaze(int width, int height, int spacingRows, int spacingCols, float pointChance)
{
    // 1) Creamos la imagen rellena de blanco
    Image imMaze = GenImageColor(width, height, WHITE);

    // 2) Pintamos los bordes en negro:
    // Fila superior e inferior
    for (int x = 0; x < width; x++)
    {
        ImageDrawPixel(&imMaze, x, 0, BLACK);              // Borde superior
        ImageDrawPixel(&imMaze, x, height - 1, BLACK);       // Borde inferior
    }
    // Columna izquierda y derecha
    for (int y = 0; y < height; y++)
    {
        ImageDrawPixel(&imMaze, 0, y, BLACK);              // Borde izquierdo
        ImageDrawPixel(&imMaze, width - 1, y, BLACK);      // Borde derecho
    }

    // 3) Para cada píxel del borde (excepto esquinas), evaluamos de forma dinámica:
    Point sprouts[width * height];
    int sproutsCount = 0;
    
    // Borde superior (fila 0 => interno es fila 1)
    for (int x = 1; x < width - 1; ) {
        if (GetRandomValue(0, 99) < (int)(pointChance * 100)) {
            ImageDrawPixel(&imMaze, x, 1, BLACK);
            sprouts[sproutsCount++] = (Point){ x, 1 };
            x += 2;  // si se pinta, saltamos el siguiente para evitar adyacencia fija
        } else {
            x++;
        }
    }
    // Borde inferior (fila height-1 => interno es height-2)
    for (int x = 1; x < width - 1; ) {
        if (GetRandomValue(0, 99) < (int)(pointChance * 100)) {
            ImageDrawPixel(&imMaze, x, height - 2, BLACK);
            sprouts[sproutsCount++] = (Point){ x, height - 2 };
            x += 2;
        } else {
            x++;
        }
    }
    // Borde izquierdo (columna 0 => interno es columna 1)
    for (int y = 1; y < height - 1; ) {
        if (GetRandomValue(0, 99) < (int)(pointChance * 100)) {
            ImageDrawPixel(&imMaze, 1, y, BLACK);
            sprouts[sproutsCount++] = (Point){ 1, y };
            y += 2;
        } else {
            y++;
        }
    }
    // Borde derecho (columna width-1 => interno es width-2)
    for (int y = 1; y < height - 1; ) {
        if (GetRandomValue(0, 99) < (int)(pointChance * 100)) {
            ImageDrawPixel(&imMaze, width - 2, y, BLACK);
            sprouts[sproutsCount++] = (Point){ width - 2, y };
            y += 2;
        } else {
            y++;
        }
    }
    
        // 4) Extender cada sprout: a partir de cada sprout, se añade otro píxel negro
    //    en la misma dirección en la que "nació" el sprout.
    int maxSpaces = (((width - 2) < (height - 2)) ? (width - 2) : (height - 2)) - 1;
    int minLength = (maxSpaces / 4) < 1 ? 1 : (maxSpaces / 4);
    
// Extender paredes a partir de cada sprout
for (int i = 0; i < sproutsCount; i++) {
    // Para cada sprout, definimos la longitud de la pared de forma aleatoria entre minLength y maxSpaces
    int wallLength = GetRandomValue(minLength, maxSpaces);
    
    // Establecemos la posición inicial a partir del sprout
    int curX = sprouts[i].x;
    int curY = sprouts[i].y;
    
    // Definir la dirección base según el borde de origen:
    // Si el sprout está en la fila 1 (borde superior), la dirección base es hacia abajo (índice 1)
    // Si está en la fila height-2 (borde inferior), la dirección base es hacia arriba (índice 0)
    // Si está en la columna 1 (borde izquierdo), la dirección base es hacia la derecha (índice 3)
    // Si está en la columna width-2 (borde derecho), la dirección base es hacia la izquierda (índice 2)
    int baseDir = -1;
    if (sprouts[i].y == 1)
        baseDir = 1;
    else if (sprouts[i].y == height - 2)
        baseDir = 0;
    else if (sprouts[i].x == 1)
        baseDir = 3;
    else if (sprouts[i].x == width - 2)
        baseDir = 2;
    
    // Ahora extendemos la pared para wallLength pasos,
    // usando siempre la dirección base (no se actualiza en cada paso)
    for (int step = 0; step < wallLength; step++) {
        int r = GetRandomValue(0, 99);
        int chosen;
        // Según la dirección base, establecemos la distribución:
        // Por ejemplo, si baseDir == 1 (hacia abajo):
        // 50% se continúa hacia abajo, 25% se gira a la izquierda y 25% a la derecha.
        if (baseDir == 1) { // Desde el borde superior, base = abajo
            if (r < 50)
                chosen = 1;        // Abajo
            else if (r < 75)
                chosen = 2;        // Izquierda
            else
                chosen = 3;        // Derecha
        } else if (baseDir == 0) { // Desde el borde inferior, base = arriba
            if (r < 50)
                chosen = 0;        // Arriba
            else if (r < 75)
                chosen = 2;        // Izquierda
            else
                chosen = 3;        // Derecha
        } else if (baseDir == 3) { // Desde el borde izquierdo, base = derecha
            if (r < 50)
                chosen = 3;        // Derecha
            else if (r < 75)
                chosen = 0;        // Arriba
            else
                chosen = 1;        // Abajo
        } else if (baseDir == 2) { // Desde el borde derecho, base = izquierda
            if (r < 50)
                chosen = 2;        // Izquierda
            else if (r < 75)
                chosen = 0;        // Arriba
            else
                chosen = 1;        // Abajo
        } else {
            chosen = baseDir; // Fallback (no debería ocurrir)
        }
        
        // Convertir la dirección elegida en desplazamientos
        int dx = 0, dy = 0;
        if (chosen == 0)      dy = -1;   // Arriba
        else if (chosen == 1) dy = 1;    // Abajo
        else if (chosen == 2) dx = -1;   // Izquierda
        else if (chosen == 3) dx = 1;    // Derecha
        
        // Calculamos el píxel candidato (inmediato adyacente)
        int candX = curX + dx;
        int candY = curY + dy;
        // Y el píxel de comprobación, que es el siguiente en la misma dirección:
        int checkX = candX + dx;
        int checkY = candY + dy;
        
        // Verificamos que el píxel de comprobación esté dentro de los límites internos (dejando borde)
        if (checkX < 1 || checkX >= width - 1 || checkY < 1 || checkY >= height - 1)
            break;
        
        // Comprobamos el píxel de comprobación para evitar fusionar paredes
        Color colCheck = GetImageColor(imMaze, checkX, checkY);
        if (colCheck.r == 0 && colCheck.g == 0 && colCheck.b == 0)
            break;
        
        // Si pasa la comprobación, dibujamos el píxel candidato en negro
        ImageDrawPixel(&imMaze, candX, candY, BLACK);
        // Actualizamos la posición actual
        curX = candX;
        curY = candY;
        // NOTA: No actualizamos la dirección base, se mantiene fija
    }
}

    return imMaze;
}
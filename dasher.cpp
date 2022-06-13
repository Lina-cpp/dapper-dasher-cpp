#include "raylib.h"

struct AnimData{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

//czy jest na ziemi
bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    // update running time
    data.runningTime += deltaTime;
    if(data.runningTime >= data.updateTime)
    {
        data.runningTime = 0; 
        //update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
            if(data.frame > maxFrame) data.frame = 0;

    }
    return data;
}



int main()
{
    //rozmiary okna - array
    int windowDimensions[2];
        windowDimensions[0] = 512;
        windowDimensions[1] = 380;

    //tworze okno uwu
    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper-Dasher");

    //przyspieszenie grawitacji (pixels/s)/s | 1s 10m posY 10 | 2s 20m posY 30 | 3s 30m posY 60
    const int gravity = 1'000;
    //przys. skoku (pixels/s)
    const int jumpVel = -600;
    bool isInAir;



    //deklaracja postaci
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData;
    //rectangle
        scarfyData.rec.width = scarfy.width/6;
        scarfyData.rec.height = scarfy.height;
        scarfyData.rec.x = 0;
        scarfyData.rec.y = 0;
    //Vector2
        scarfyData.pos.x = windowDimensions[0]/2 - scarfyData.rec.width/2;
        scarfyData.pos.y = windowDimensions[1] - scarfyData.rec.height;
    //animation frame scarfy
        scarfyData.frame = 0;
        scarfyData.updateTime = 1.0 / 12.0;
        scarfyData.runningTime = 0.0;


        int velocity = 0;

//ladowanie nebuli
Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

int nebulaVel = -200;   //nebula x velocity (pixels/second)


    const int sizeOfNebArray = 5;
    //AnimData for nebula
    AnimData nebArray[sizeOfNebArray]{};

    for (int i=0; i< sizeOfNebArray; i++)
    {
        nebArray[i].rec.x = 0.0;
        nebArray[i].rec.y = 0.0;
        nebArray[i].rec.width = nebula.width/8;
        nebArray[i].rec.height = nebula.height/8;
        nebArray[i].pos.y = windowDimensions[1] - nebula.height/8;
        nebArray[i].frame = 0;
        nebArray[i].runningTime = 0.0;
        nebArray[i].updateTime = 0.0;
        nebArray[i].pos.x = windowDimensions[0] + i * 300;
    }

    float finishLine{ nebArray[sizeOfNebArray -1].pos.x};
   
    Texture2D background = LoadTexture("textures/far-buildings.png");
        float bgX = 0;
    Texture2D midground = LoadTexture("textures/back-buildings.png");
        float mgX = 0;
    Texture2D foreground = LoadTexture("textures/foreground.png");
        float fgX = 0;


    bool collision{}; // kolizje
    
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        const float dT = GetFrameTime(); //delta time (czas od ostatniego frameu)

        //Start gry 
        BeginDrawing();
        ClearBackground(WHITE); //anty epilepsja

        //background
        bgX -= 20 * dT;
        if(bgX <= -background.width * 2) bgX = 0.0; 

        //draw the background
        Vector2 bg1Pos {bgX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        Vector2 bg2Pos{bgX + background.width * 2, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);


        //midground
        mgX -= 40 * dT;
        if(mgX <= -midground.width * 2) mgX = 0.0;

        Vector2 mg1Pos{mgX,0};
        DrawTextureEx(midground, mg1Pos, 0, 2, WHITE);
        Vector2 mg2Pos{mgX + midground.width*2, 0};
        DrawTextureEx(midground, mg2Pos, 0, 2, WHITE);

        //foreground
        fgX -= 80 * dT;
        if(fgX <= -foreground.width * 2) fgX = 0.0;

        Vector2 fg1Pos{fgX, 0};
        DrawTextureEx(foreground, fg1Pos, 0, 2, WHITE);
        Vector2 fg2Pos{fgX + foreground.width * 2, 0};
        DrawTextureEx(foreground, fg2Pos, 0, 2, WHITE);





        // czy jestemy na ziemi
        if (isOnGround(scarfyData, windowDimensions[1])) 
        {
            velocity = 0;  //jestesmy na ziemi!
            isInAir = false;
        }
        else 
        {
            velocity += gravity * dT; //Lecymy duuuur
            isInAir = true;
        }



        //check dla jumpa
        if (IsKeyPressed(KEY_SPACE) && isInAir==false) velocity += jumpVel;



        //update pozycji nebuli
        for(int i=0; i<sizeOfNebArray; i++)
        {
            nebArray[i].pos.x += nebulaVel * dT;
        }



        //finish line
        finishLine += nebulaVel * dT;


        //update pozycji scarfy
        scarfyData.pos.y += velocity * dT;

        //animacja scarfy, oraz freez w powietrzu
        if(isInAir == false) scarfyData = updateAnimData(scarfyData, dT, 5);

        for (int i=0; i < sizeOfNebArray; i++)
        {
            nebArray[i]= updateAnimData(nebArray[i], dT, 7);
        }


        //kolizje
            for(AnimData nebula : nebArray)
                {

                    float pad = 50; //zmniejszenie hitboxa nebuli
                    Rectangle nebRec
                    {
                        nebula.pos.x + pad,
                        nebula.pos.y + pad,
                        nebula.rec.width - 2*pad,
                        nebula.rec.height - 2*pad
                    };
                    Rectangle scarfyRec
                    {
                        scarfyData.pos.x,
                        scarfyData.pos.y,
                        scarfyData.rec.width,
                        scarfyData.rec.height
                    };
                    if(CheckCollisionRecs(nebRec, scarfyRec)) collision = true;
                }

        if(collision)
        {
            //lose game
            DrawText("Game Over!", windowDimensions[0] / 4, windowDimensions[1] / 2, 40, RED);
        }

        else if (scarfyData.pos.x >= finishLine)
        {
            DrawText("You win!", windowDimensions[0] / 4, windowDimensions[1] / 2, 30, BLUE);
        }

        else
        {
        //draw nebule
            for(int i=0; i < sizeOfNebArray; i++)
            {
                DrawTextureRec(nebula, nebArray[i].rec, nebArray[i].pos, WHITE);    //draw nebula
            }

            //draw scarfy
            DrawTextureRec(scarfy,scarfyData.rec, scarfyData.pos, WHITE);
        }


        //Koniec gry
        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);  
    CloseWindow();
    
}

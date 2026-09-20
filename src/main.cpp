#include "raylib.h"
#include "raymath.h"

#if defined(PLATFORM_WEB)
    #include<emscripten/emscripten.h>
#endif

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

int squareSize = 100;
int offsetX = 100;
int offsetY = 100;
int fontSize = squareSize / 2;
int outline = squareSize / 20;

const int FPS = 60;
const int SPRITE_ANIMATION_FRAME = 36;
const int CHESS_PIECE_SIZE = 256;

const int ORIGINAL_GAME_WIDTH = 1000;
const int ORIGINAL_GAME_HEIGHT = 1000;

struct pieceID{
    uint8_t x;
    uint8_t y;
    char name;
    bool moved;
    bool passo;
};

void initPieces(std::vector<std::vector<pieceID>>& arr){
    arr.clear();
    arr.resize(1);
    arr[0].push_back({4, 7, 'k', false, false});
    arr[0].push_back({4, 0, 'K', false, false});


    arr[0].push_back({0, 0, 'R', false, false});
    arr[0].push_back({1, 0, 'N', false, false});
    arr[0].push_back({2, 0, 'B', false, false});
    arr[0].push_back({3, 0, 'Q', false, false});
    arr[0].push_back({5, 0, 'B', false, false});
    arr[0].push_back({6, 0, 'N', false, false});
    arr[0].push_back({7, 0, 'R', false, false});

    for(int i = 0; i < 8; i++){
        arr[0].push_back({(uint8_t)i, 1, 'P', false, false});
    }

    arr[0].push_back({0, 7, 'r', false, false});
    arr[0].push_back({1, 7, 'n', false, false});
    arr[0].push_back({2, 7, 'b', false, false});
    arr[0].push_back({3, 7, 'q', false, false});
    arr[0].push_back({5, 7, 'b', false, false});
    arr[0].push_back({6, 7, 'n', false, false});
    arr[0].push_back({7, 7, 'r', false, false});

    for(int i = 0; i < 8; i++){
        arr[0].push_back({(uint8_t)i, 6, 'p', false, false});
    }
}

void removeFromVector(std::vector<pieceID>& arr, float x, float y){
    for(int i = 0; i < (int)arr.size(); i++){
        if(arr[i].x == x && arr[i].y == y){
            arr.erase(arr.begin() + i);
            return;
        }
    }
}

int getPieceIndex(std::vector<pieceID>& arr, float x, float y){
    for(int i = 0; i < (int)arr.size(); i++){
        if(arr[i].x == x && arr[i].y == y){
            return i;
        }
    }
    return -1;
}

bool sameColor(char pieceName1, char pieceName2){
    if((pieceName1 - 'a' > 0 && pieceName2 - 'a' > 0) || (pieceName1 - 'a' < 0 && pieceName2 - 'a' < 0)){
        return true;
    }
    return false;
}

bool isMove(char piece, int move){
    if((piece - 'a' > 0 && move % 2 == 0) || (piece - 'a' < 0 && move % 2 == 1)) return true;
    return false;
}

bool inRange(std::vector<Vector2>& attack, float x, float y){
    for(int i = 0; i < (int)attack.size(); i++){
        if(attack[i].x == x && attack[i].y == y) return true;
    }
    return false;
}

bool checkCollisionRecRounded(float x, float y, Rectangle rec, float roundness, float lineThick){
    lineThick /= 2;
    rec.x -= lineThick;
    rec.y -= lineThick;
    rec.height += lineThick * 2;
    rec.width += lineThick * 2;

    if(x < rec.x || x > rec.x + rec.width || y < rec.y || y > rec.y + rec.height) return false;

    x -= rec.x;
    y -= rec.y;
    
    float r = rec.height / 2 * roundness;
    if(y > rec.height / 2) y = rec.height - y;
    if(x > rec.width / 2) x = rec.width - x;

    if(x >= r || y >= r) return true;

    x -= r;
    y -= r;

    if(x * x + y * y <= r * r) return true;

    return false;
}

class chessPiece{
public:

    bool flipBlack = false;
    Texture2D spriteSheet;

    chessPiece(){
        spriteSheet = LoadTexture("sprites/ChessSpriteSheet.png");
    }

    ~chessPiece(){
        UnloadTexture(spriteSheet);
    }

    void Draw(const std::vector<pieceID>& pieces){
        for(pieceID piece : pieces){
            Rectangle source = {0.F, 0.F, (float)CHESS_PIECE_SIZE, (float)CHESS_PIECE_SIZE};
            switch(piece.name){
                case 'p':
                    source.x = 0.F;
                    source.y = (float)CHESS_PIECE_SIZE;
                    break;
                case 'r':
                    source.x = (float)CHESS_PIECE_SIZE;
                    source.y = (float)CHESS_PIECE_SIZE;
                    break;
                case 'n':
                    source.x = (float)CHESS_PIECE_SIZE * 2;
                    source.y = (float)CHESS_PIECE_SIZE;
                    break;
                case 'b':
                    source.x = (float)CHESS_PIECE_SIZE * 3;
                    source.y = (float)CHESS_PIECE_SIZE;
                    break;
                case 'q':
                    source.x = (float)CHESS_PIECE_SIZE * 4;
                    source.y = (float)CHESS_PIECE_SIZE;
                    break;
                case 'k':
                    source.x = (float)CHESS_PIECE_SIZE * 5;
                    source.y = (float)CHESS_PIECE_SIZE;
                    break;
                case 'P':
                    source.x = 0.F;
                    source.y = 0.F;
                    break;
                case 'R':
                    source.x = (float)CHESS_PIECE_SIZE;
                    source.y = 0.F;
                    break;
                case 'N':
                    source.x = (float)CHESS_PIECE_SIZE * 2;
                    source.y = 0.F;
                    break;
                case 'B':
                    source.x = (float)CHESS_PIECE_SIZE * 3;
                    source.y = 0.F;
                    break;
                case 'Q':
                    source.x = (float)CHESS_PIECE_SIZE * 4;
                    source.y = 0.F;
                    break;
                case 'K':
                    source.x = (float)CHESS_PIECE_SIZE * 5;
                    source.y = 0.F;
                    break;
            }
            int rotation = ((flipBlack && piece.name < 'a') ? 180 : 0);
            Rectangle destination = {
                (float)offsetX + piece.x * squareSize + squareSize / 2,
                (float)offsetY + piece.y * squareSize + squareSize / 2,
                (float)squareSize,
                (float)squareSize
            };
            DrawTexturePro(spriteSheet, source, destination, {(float)squareSize / 2, (float)squareSize / 2}, rotation, WHITE);
        }
    }
};

class Button{
public:
    std::string name;
    Vector2 coords;
    Vector2 offsetPercentages;
    Vector2 squarePercentages;
    Rectangle rec;
    float roundness = 0.4;
    float lineThick = (float)squareSize / 33;

    Button(const std::string& val, const Vector2& offsets, const Vector2& squares)
        : name(val), offsetPercentages(offsets), squarePercentages(squares)
    {
        coords.x = offsetX * offsetPercentages.x + squareSize * squarePercentages.x;
        coords.y = offsetY * offsetPercentages.y + squareSize * squarePercentages.y;
        rec = {coords.x - outline, coords.y - outline, (float)MeasureText(name.c_str(), fontSize) + outline * 2, (float)fontSize + outline * 2};
    }

    void Draw(){
        DrawRectangleRoundedLinesEx(rec, roundness, 1, lineThick, BLACK);
        DrawText(name.c_str(), (int)coords.x, (int)coords.y, fontSize, BLACK);
    }

    void onGoofyWindowResize(){
        coords.x = offsetX * offsetPercentages.x + squareSize * squarePercentages.x;
        coords.y = offsetY * offsetPercentages.y + squareSize * squarePercentages.y;
        lineThick = (float)squareSize / 33;
        rec = {coords.x - outline, coords.y - outline, (float)MeasureText(name.c_str(), fontSize) + outline * 2, (float)fontSize + outline * 2};
    }
};

class spriteAnimation{
public:
    Texture2D texture;
    std::string name;
    Vector2 coords;
    bool animation = false;
    const int fps = FPS / 3;
    int counter = 0;
    int frames;
    double animationDuration;
    double animationStartTime;

    spriteAnimation(const std::string& val, const int& length)
    : name(val), frames(length)
    {
        texture = LoadTexture(name.c_str());
        animationDuration = (double)frames / fps;
    }

    ~spriteAnimation(){
        UnloadTexture(texture);
    }

    void StartAnimation(){
        animation = true;
        animationStartTime = GetTime();
    }

    void Draw(){
        if(!animation) return;

        double animationTimeElapsed = GetTime() - animationStartTime;

        if(animationTimeElapsed >= animationDuration){
            animation = false;
            return;
        }

        int frame = (int)(animationTimeElapsed * frames / animationDuration);

        Rectangle src = {(float)SPRITE_ANIMATION_FRAME * frame, 0.F, (float)SPRITE_ANIMATION_FRAME, (float)SPRITE_ANIMATION_FRAME};
        Rectangle dest = {offsetX + coords.x * squareSize - squareSize, offsetY + coords.y * squareSize - squareSize, (float)squareSize * 3, (float)squareSize * 3};
        DrawTexturePro(texture, src, dest, {0, 0}, 0, WHITE);
    }

};

class RandomSounds{
public:
    std::vector<Sound> sounds;
    int elements = 0;

    void Load(const std::vector<std::string>& names){
        for(std::string name : names){
            sounds.push_back(LoadSound(name.c_str()));
        }
        elements = sounds.size();
    }

    void Unload(){
        for(Sound& sound : sounds){
            UnloadSound(sound);
        }
        sounds.clear();
        elements = 0;
    }

    void Explode(){
        if(elements == 0) return;

        int rand = GetRandomValue(0, elements - 1);
        PlaySound(sounds[rand]);
    }

};

class Game{
public:
    int move = 0;
    int currMove = 0;

    chessPiece chesspiece;

    float x_coord_for_reset_button_pls_dont_crash_my_code = (float)(squareSize * 8 - MeasureText("Reset", fontSize) - outline * 2) / squareSize;
    Button resetButton = Button("Reset", {1.F, 0.3}, {x_coord_for_reset_button_pls_dont_crash_my_code, 0.F});

    Button flipBlackButton = Button("Flip", {1.F, 0.3}, {(float)outline * 2 / squareSize, 0.F});
    
    float backButtonX = 4.F - ((float)(MeasureText(" <- ", fontSize) + outline * 2) / squareSize);
    Button moveBackButton = Button(" <- ", {1.F, 1.2}, {backButtonX, 8.F});
    Button moveForwardButton = Button(" -> ", {1.F, 1.2}, {(4.F + ((float)outline * 2 / squareSize)), 8.F});

    spriteAnimation boom = spriteAnimation("sprites/explosion.png", 9);
    RandomSounds boomSounds;
    RandomSounds moveSounds;
    RandomSounds pipe;

    std::vector<std::vector<pieceID>> pieces;
    std::vector<pieceID> piecesCopy;
    bool selected = false;
    Vector2 selectedCoords;
    std::vector<bool> whiteCheck;
    std::vector<bool> blackCheck;
    bool checkmate = false;
    bool stalemate = false;

    bool promotion = false;
    Vector2 proPawn;
    std::vector<pieceID> proPieces;

    std::vector<Vector2> checkAttack;
    std::vector<Vector2> attackRange;
    std::vector<std::vector<bool>> checkBoard;

    Camera2D camera;
    bool shake = false;
    int countShake = 0;
    double shakeStartTime;
    double shakeTimeElapsed = 0;
    const double shakeDuration = 0.08;

    Game(){
        selectedCoords = {-1, -1};
        whiteCheck.push_back(false);
        blackCheck.push_back(false);

        camera.offset = {GetScreenWidth() / 2.F, GetScreenHeight() / 2.F};
        camera.target = {(float)offsetX + squareSize * 4, (float)offsetY + squareSize * 4};
        camera.rotation = (float)0;
        camera.zoom = (float)1;

        InitAudioDevice();

        boomSounds.Load({
            "audio/explosion1.mp3",
            "audio/explosion2.mp3",
            "audio/explosion3.mp3",
            "audio/explosion4.mp3",
            "audio/explosion5.mp3",
        });
        moveSounds.Load({
            "audio/thud1.mp3",
            "audio/thud2.mp3",
            "audio/thud3.mp3",
            "audio/thud4.mp3",
            "audio/thud5.mp3",
        });
        pipe.Load({"audio/LePipe.mp3"});
    }

    ~Game(){
        boomSounds.Unload();
        moveSounds.Unload();
        pipe.Unload();

        CloseAudioDevice();
    }

    void Draw(){
        if(whiteCheck[currMove]){
            DrawRectangle(offsetX + pieces[currMove][0].x * squareSize, offsetY + pieces[currMove][0].y * squareSize, squareSize, squareSize, RED);
        }
        if(blackCheck[currMove]){
            DrawRectangle(offsetX + pieces[currMove][1].x * squareSize, offsetY + pieces[currMove][1].y * squareSize, squareSize, squareSize, RED);
        }

        chesspiece.Draw(pieces[currMove]);
        resetButton.Draw();
        flipBlackButton.Draw();
        moveBackButton.Draw();
        moveForwardButton.Draw();

        if(promotion){
            float posY = proPawn.y == 0 ? 0 : 4;
            DrawRectangle(offsetX + proPawn.x * squareSize, offsetY + posY * squareSize, squareSize, squareSize * 4, GRAY);
            chesspiece.Draw(proPieces);
        }

        if(checkmate && currMove == move){
            DrawText("Checkmate", offsetX + squareSize * 2.5, offsetY * 0.3, fontSize, BLACK);
        }
        else if(stalemate && currMove == move){
            DrawText("Stalemate", offsetX + squareSize * 2.5, offsetY * 0.3, fontSize, BLACK);
        }

        boom.Draw();
    }

    void SelectPiece(){
        int index = getPieceIndex(pieces[currMove], selectedCoords.x, selectedCoords.y);
        if(index == -1) return;
        Color color;
        if(((int)selectedCoords.y % 2 == 0 && (int)selectedCoords.x % 2 == 0) || ((int)selectedCoords.y % 2 == 1 && (int)selectedCoords.x % 2 == 1)){
            color = {0, 255, 0, 255};
        }
        else{
            color = GREEN;
        }
        DrawRectangle(offsetX + selectedCoords.x * squareSize, offsetY + selectedCoords.y * squareSize, squareSize, squareSize, color);
        for(Vector2 coord : attackRange){
            if(((int)coord.y % 2 == 0 && (int)coord.x % 2 == 0) || ((int)coord.y % 2 == 1 && (int)coord.x % 2 == 1)){
                color = {0, 255, 0, 255};
            }
            else{
                color = GREEN;
            }
            DrawRectangle(offsetX + coord.x * squareSize, offsetY + coord.y * squareSize, squareSize, squareSize, color);
        }
    }

    //i'm sorry
    void findAttackRange(int index, std::vector<Vector2>& attackVec, bool isCheckinCheck = false){
        attackVec.resize(0);
        if(index == -1) return;

        if(pieces[currMove][index].name != 'k' && pieces[currMove][index].name != 'K' && !isCheckinCheck){
            pieceID temp = pieces[currMove][index];
            removeFromVector(pieces[currMove], pieces[currMove][index].x, pieces[currMove][index].y);
            pieces[currMove].push_back(temp);
            index = pieces[currMove].size() - 1;
        }

        if(pieces[currMove][index].name == 'p'){
            float x = pieces[currMove][index].x;
            float y = pieces[currMove][index].y;
            bool tempCheck = whiteCheck[currMove];

            int tempID = getPieceIndex(pieces[currMove], x + 1, y - 1);
            if(x + 1 <= 7 && y - 1 >= 0 && tempID != -1 && !sameColor(pieces[currMove][index].name, pieces[currMove][tempID].name)){
                if(!isCheckinCheck){
                    pieceID tempCaptured = pieces[currMove][tempID];
                    removeFromVector(pieces[currMove], x + 1, y - 1);
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].x = x + 1;
                    pieces[currMove][index].y = y - 1;
                    helpCheck();
                    if(!whiteCheck[currMove]){
                        attackVec.push_back({x + 1, y - 1});
                    }
                    whiteCheck[currMove] = tempCheck;
                    pieces[currMove][index].x = x;
                    pieces[currMove][index].y = y;
                    pieceID tempCurr = pieces[currMove].back();
                    pieces[currMove].pop_back();
                    pieces[currMove].push_back(tempCaptured);
                    pieces[currMove].push_back(tempCurr);
                    index = pieces[currMove].size() - 1;
                }
                else{
                    attackVec.push_back({x + 1, y - 1});
                }
            }

            tempID = getPieceIndex(pieces[currMove], x - 1, y - 1);
            if(x - 1 >= 0 && y - 1 >= 0 && tempID != -1 && !sameColor(pieces[currMove][index].name, pieces[currMove][tempID].name)){
                if(!isCheckinCheck){
                    pieceID tempCaptured = pieces[currMove][tempID];
                    removeFromVector(pieces[currMove], x - 1, y - 1);
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].x = x - 1;
                    pieces[currMove][index].y = y - 1;
                    helpCheck();
                    if(!whiteCheck[currMove]){
                        attackVec.push_back({x - 1, y - 1});
                    }
                    whiteCheck[currMove] = tempCheck;
                    pieces[currMove][index].x = x;
                    pieces[currMove][index].y = y;
                    pieceID tempCurr = pieces[currMove].back();
                    pieces[currMove].pop_back();
                    pieces[currMove].push_back(tempCaptured);
                    pieces[currMove].push_back(tempCurr);
                    index = pieces[currMove].size() - 1;
                }
                else{
                    attackVec.push_back({x - 1, y - 1});
                }
            }

            tempID = getPieceIndex(pieces[currMove], x + 1, y);
            if(x + 1 <= 7 && y == 3 && tempID != -1 && pieces[currMove][tempID].name == 'P' && pieces[currMove][tempID].passo){
                if(!isCheckinCheck){
                    pieceID tempCaptured = pieces[currMove][tempID];
                    removeFromVector(pieces[currMove], x + 1, y);
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].x = x + 1;
                    pieces[currMove][index].y = y - 1;
                    helpCheck();
                    if(!whiteCheck[currMove]){
                        attackVec.push_back({x + 1, y - 1});
                    }
                    whiteCheck[currMove] = tempCheck;
                    pieces[currMove][index].x = x;
                    pieces[currMove][index].y = y;
                    pieceID tempCurr = pieces[currMove].back();
                    pieces[currMove].pop_back();
                    pieces[currMove].push_back(tempCaptured);
                    pieces[currMove].push_back(tempCurr);
                    index = pieces[currMove].size() - 1;
                }
            }

            tempID = getPieceIndex(pieces[currMove], x - 1, y);
            if(x - 1 >= 0 && y == 3 && tempID != -1 && pieces[currMove][tempID].name == 'P' && pieces[currMove][tempID].passo){
                if(!isCheckinCheck){
                    pieceID tempCaptured = pieces[currMove][tempID];
                    removeFromVector(pieces[currMove], x - 1, y);
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].x = x - 1;
                    pieces[currMove][index].y = y - 1;
                    helpCheck();
                    if(!whiteCheck[currMove]){
                        attackVec.push_back({x - 1, y - 1});
                    }
                    whiteCheck[currMove] = tempCheck;
                    pieces[currMove][index].x = x;
                    pieces[currMove][index].y = y;
                    pieceID tempCurr = pieces[currMove].back();
                    pieces[currMove].pop_back();
                    pieces[currMove].push_back(tempCaptured);
                    pieces[currMove].push_back(tempCurr);
                    index = pieces[currMove].size() - 1;
                }
            }

            if(getPieceIndex(pieces[currMove], x, y - 1) == -1 && y - 1 >= 0){
                if(!isCheckinCheck){
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].y = y - 1;
                    helpCheck();
                    if(!whiteCheck[currMove]){
                        attackVec.push_back({x, y - 1});
                    }
                    whiteCheck[currMove] = tempCheck;
                    pieces[currMove][index].y = y;
                }
                else{
                    attackVec.push_back({x, y - 1});
                }
            }
            else{
                return;
            }
            if(getPieceIndex(pieces[currMove], x, y - 2) == -1 && !pieces[currMove][index].moved && y - 2 >= 0){
                if(!isCheckinCheck){
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].y = y - 2;
                    helpCheck();
                    if(!whiteCheck[currMove]){
                        attackVec.push_back({x, y - 2});
                    }
                    whiteCheck[currMove] = tempCheck;
                    pieces[currMove][index].y = y;
                }
                else{
                    attackVec.push_back({x, y - 2});
                }
            }
        }
        else if(pieces[currMove][index].name == 'P'){
            float x = pieces[currMove][index].x;
            float y = pieces[currMove][index].y;
            bool tempCheck = blackCheck[currMove];

            int tempID = getPieceIndex(pieces[currMove], x + 1, y + 1);
            if(x + 1 <= 7 && y + 1 <= 7 && tempID != -1 && !sameColor(pieces[currMove][index].name, pieces[currMove][tempID].name)){
                if(!isCheckinCheck){
                    pieceID tempCaptured = pieces[currMove][tempID];
                    removeFromVector(pieces[currMove], x + 1, y + 1);
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].x = x + 1;
                    pieces[currMove][index].y = y + 1;
                    helpCheck();
                    if(!blackCheck[currMove]){
                        attackVec.push_back({x + 1, y + 1});
                    }
                    blackCheck[currMove] = tempCheck;
                    pieces[currMove][index].x = x;
                    pieces[currMove][index].y = y;
                    pieceID tempCurr = pieces[currMove].back();
                    pieces[currMove].pop_back();
                    pieces[currMove].push_back(tempCaptured);
                    pieces[currMove].push_back(tempCurr);
                    index = pieces[currMove].size() - 1;
                }
                else{
                    attackVec.push_back({x + 1, y + 1});
                }
            }

            tempID = getPieceIndex(pieces[currMove], x - 1, y + 1);
            if(x - 1 >= 0 && y + 1 <= 7 && tempID != -1 && !sameColor(pieces[currMove][index].name, pieces[currMove][tempID].name)){
                if(!isCheckinCheck){
                    pieceID tempCaptured = pieces[currMove][tempID];
                    removeFromVector(pieces[currMove], x - 1, y + 1);
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].x = x - 1;
                    pieces[currMove][index].y = y + 1;
                    helpCheck();
                    if(!blackCheck[currMove]){
                        attackVec.push_back({x - 1, y + 1});
                    }
                    blackCheck[currMove] = tempCheck;
                    pieces[currMove][index].x = x;
                    pieces[currMove][index].y = y;
                    pieceID tempCurr = pieces[currMove].back();
                    pieces[currMove].pop_back();
                    pieces[currMove].push_back(tempCaptured);
                    pieces[currMove].push_back(tempCurr);
                    index = pieces[currMove].size() - 1;
                }
                else{
                    attackVec.push_back({x - 1, y + 1});
                }
            }

            tempID = getPieceIndex(pieces[currMove], x + 1, y);
            if(x + 1 <= 7 && y == 4 && tempID != -1 && pieces[currMove][tempID].name == 'p' && pieces[currMove][tempID].passo){
                if(!isCheckinCheck){
                    pieceID tempCaptured = pieces[currMove][tempID];
                    removeFromVector(pieces[currMove], x + 1, y);
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].x = x + 1;
                    pieces[currMove][index].y = y + 1;
                    helpCheck();
                    if(!blackCheck[currMove]){
                        attackVec.push_back({x + 1, y + 1});
                    }
                    blackCheck[currMove] = tempCheck;
                    pieces[currMove][index].x = x;
                    pieces[currMove][index].y = y;
                    pieceID tempCurr = pieces[currMove].back();
                    pieces[currMove].pop_back();
                    pieces[currMove].push_back(tempCaptured);
                    pieces[currMove].push_back(tempCurr);
                    index = pieces[currMove].size() - 1;
                }
            }

            tempID = getPieceIndex(pieces[currMove], x - 1, y);
            if(x - 1 >= 0 && y == 4 && tempID != -1 && pieces[currMove][tempID].name == 'p' && pieces[currMove][tempID].passo){
                if(!isCheckinCheck){
                    pieceID tempCaptured = pieces[currMove][tempID];
                    removeFromVector(pieces[currMove], x - 1, y);
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].x = x - 1;
                    pieces[currMove][index].y = y + 1;
                    helpCheck();
                    if(!blackCheck[currMove]){
                        attackVec.push_back({x - 1, y + 1});
                    }
                    blackCheck[currMove] = tempCheck;
                    pieces[currMove][index].x = x;
                    pieces[currMove][index].y = y;
                    pieceID tempCurr = pieces[currMove].back();
                    pieces[currMove].pop_back();
                    pieces[currMove].push_back(tempCaptured);
                    pieces[currMove].push_back(tempCurr);
                    index = pieces[currMove].size() - 1;
                }
            }

            if(getPieceIndex(pieces[currMove], x, y + 1) == -1 && y + 1 < 8){
                if(!isCheckinCheck){
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].y = y + 1;
                    helpCheck();
                    if(!blackCheck[currMove]){
                        attackVec.push_back({x, y + 1});
                    }
                    blackCheck[currMove] = tempCheck;
                    pieces[currMove][index].y = y;
                }
                else{
                    attackVec.push_back({x, y + 1});
                }
            }
            else{
                return;
            }
            if(getPieceIndex(pieces[currMove], x, y + 2) == -1 && !pieces[currMove][index].moved && y + 2 < 8){
                if(!isCheckinCheck){
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].y = y + 2;
                    helpCheck();
                    if(!blackCheck[currMove]){
                        attackVec.push_back({x, y + 2});
                    }
                    blackCheck[currMove] = tempCheck;
                    pieces[currMove][index].y = y;
                }
                else{
                    attackVec.push_back({x, y + 2});
                }
            }
        }
        else if(pieces[currMove][index].name == 'R' || pieces[currMove][index].name == 'r'
             || pieces[currMove][index].name == 'Q' || pieces[currMove][index].name == 'q'){
            float x = pieces[currMove][index].x;
            float y = pieces[currMove][index].y;
            bool tempCheckWhite = whiteCheck[currMove];
            bool tempCheckBlack = blackCheck[currMove];

            int dx = 1;

            while(x - dx >= 0){
                int tempID = getPieceIndex(pieces[currMove], x - dx, y);
                if(tempID != -1 && sameColor(pieces[currMove][index].name, pieces[currMove][tempID].name)){
                    if(isCheckinCheck){
                        attackVec.push_back({x - dx, y});
                    }
                    break;
                }
                bool take = tempID != -1;
                if(!isCheckinCheck){
                    pieceID tempCaptured;
                    if(take){
                        tempCaptured = pieces[currMove][tempID];
                        removeFromVector(pieces[currMove], x - dx, y);
                    }
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].x = x - dx;
                    helpCheck();
                    if(!whiteCheck[currMove] && !blackCheck[currMove]){
                        attackVec.push_back({x - dx, y});
                    }
                    whiteCheck[currMove] = tempCheckWhite;
                    blackCheck[currMove] = tempCheckBlack;
                    pieces[currMove][index].x = x;
                    if(take){
                        pieceID tempCurr = pieces[currMove].back();
                        pieces[currMove].pop_back();
                        pieces[currMove].push_back(tempCaptured);
                        pieces[currMove].push_back(tempCurr);
                    }
                    index = pieces[currMove].size() - 1;
                }
                else{
                    attackVec.push_back({x - dx, y});
                }

                if(take){
                    break;
                }
                dx++;
            }

            dx = 1;

            while(x + dx < 8){
                int tempID = getPieceIndex(pieces[currMove], x + dx, y);
                if(tempID != -1 && sameColor(pieces[currMove][index].name, pieces[currMove][tempID].name)){
                    if(isCheckinCheck){
                        attackVec.push_back({x + dx, y});
                    }
                    break;
                }
                bool take = tempID != -1;
                if(!isCheckinCheck){
                    pieceID tempCaptured;
                    if(take){
                        tempCaptured = pieces[currMove][tempID];
                        removeFromVector(pieces[currMove], x + dx, y);
                    }
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].x = x + dx;
                    helpCheck();
                    if(!whiteCheck[currMove] && !blackCheck[currMove]){
                        attackVec.push_back({x + dx, y});
                    }
                    whiteCheck[currMove] = tempCheckWhite;
                    blackCheck[currMove] = tempCheckBlack;
                    pieces[currMove][index].x = x;
                    if(take){
                        pieceID tempCurr = pieces[currMove].back();
                        pieces[currMove].pop_back();
                        pieces[currMove].push_back(tempCaptured);
                        pieces[currMove].push_back(tempCurr);
                    }
                    index = pieces[currMove].size() - 1;
                }
                else{
                    attackVec.push_back({x + dx, y});
                }

                if(take){
                    break;
                }
                dx++;
            }

            int dy = 1;

            while(y - dy >= 0){
                int tempID = getPieceIndex(pieces[currMove], x, y - dy);
                if(tempID != -1 && sameColor(pieces[currMove][index].name, pieces[currMove][tempID].name)){
                    if(isCheckinCheck){
                        attackVec.push_back({x, y - dy});
                    }
                    break;
                }
                bool take = tempID != -1;
                if(!isCheckinCheck){
                    pieceID tempCaptured;
                    if(take){
                        tempCaptured = pieces[currMove][tempID];
                        removeFromVector(pieces[currMove], x, y - dy);
                    }
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].y = y - dy;
                    helpCheck();
                    if(!whiteCheck[currMove] && !blackCheck[currMove]){
                        attackVec.push_back({x, y - dy});
                    }
                    whiteCheck[currMove] = tempCheckWhite;
                    blackCheck[currMove] = tempCheckBlack;
                    pieces[currMove][index].y = y;
                    if(take){
                        pieceID tempCurr = pieces[currMove].back();
                        pieces[currMove].pop_back();
                        pieces[currMove].push_back(tempCaptured);
                        pieces[currMove].push_back(tempCurr);
                    }
                    index = pieces[currMove].size() - 1;
                }
                else{
                    attackVec.push_back({x, y - dy});
                }

                if(take){
                    break;
                }
                dy++;
            }

            dy = 1;
            
            while(y + dy < 8){
                int tempID = getPieceIndex(pieces[currMove], x, y + dy);
                if(tempID != -1 && sameColor(pieces[currMove][index].name, pieces[currMove][tempID].name)){
                    if(isCheckinCheck){
                        attackVec.push_back({x, y + dy});
                    }
                    break;
                }
                bool take = tempID != -1;
                if(!isCheckinCheck){
                    pieceID tempCaptured;
                    if(take){
                        tempCaptured = pieces[currMove][tempID];
                        removeFromVector(pieces[currMove], x, y + dy);
                    }
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].y = y + dy;
                    helpCheck();
                    if(!whiteCheck[currMove] && !blackCheck[currMove]){
                        attackVec.push_back({x, y + dy});
                    }
                    whiteCheck[currMove] = tempCheckWhite;
                    blackCheck[currMove] = tempCheckBlack;
                    pieces[currMove][index].y = y;
                    if(take){
                        pieceID tempCurr = pieces[currMove].back();
                        pieces[currMove].pop_back();
                        pieces[currMove].push_back(tempCaptured);
                        pieces[currMove].push_back(tempCurr);
                    }
                    index = pieces[currMove].size() - 1;
                }
                else{
                    attackVec.push_back({x, y + dy});
                }

                if(take){
                    break;
                }
                dy++;
            }
        }
        if(pieces[currMove][index].name == 'B' || pieces[currMove][index].name == 'b'
             || pieces[currMove][index].name == 'Q' || pieces[currMove][index].name == 'q'){
            float x = pieces[currMove][index].x;
            float y = pieces[currMove][index].y;
            bool tempCheckWhite = whiteCheck[currMove];
            bool tempCheckBlack = blackCheck[currMove];

            int dx = 1;
            int dy = 1;

            while(x + dx < 8 && y + dy < 8){
                int tempID = getPieceIndex(pieces[currMove], x + dx, y + dy);
                if(tempID != -1 && sameColor(pieces[currMove][index].name, pieces[currMove][tempID].name)){
                    if(isCheckinCheck){
                        attackVec.push_back({x + dx, y + dy});
                    }
                    break;
                }
                bool take = tempID != -1;
                if(!isCheckinCheck){
                    pieceID tempCaptured;
                    if(take){
                        tempCaptured = pieces[currMove][tempID];
                        removeFromVector(pieces[currMove], x + dx, y + dy);
                    }
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].x = x + dx;
                    pieces[currMove][index].y = y + dy;
                    helpCheck();
                    if(!whiteCheck[currMove] && !blackCheck[currMove]){
                        attackVec.push_back({x + dx, y + dy});
                    }
                    whiteCheck[currMove] = tempCheckWhite;
                    blackCheck[currMove] = tempCheckBlack;
                    pieces[currMove][index].x = x;
                    pieces[currMove][index].y = y;
                    if(take){
                        pieceID tempCurr = pieces[currMove].back();
                        pieces[currMove].pop_back();
                        pieces[currMove].push_back(tempCaptured);
                        pieces[currMove].push_back(tempCurr);
                    }
                    index = pieces[currMove].size() - 1;
                }
                else{
                    attackVec.push_back({x + dx, y + dy});
                }

                if(take){
                    break;
                }
                dx++;
                dy++;
            }
            dx = 1;
            dy = 1;
            while(x - dx >= 0 && y - dy >= 0){
                int tempID = getPieceIndex(pieces[currMove], x - dx, y - dy);
                if(tempID != -1 && sameColor(pieces[currMove][index].name, pieces[currMove][tempID].name)){
                    if(isCheckinCheck){
                        attackVec.push_back({x - dx, y - dy});
                    }
                    break;
                }
                bool take = tempID != -1;
                if(!isCheckinCheck){
                    pieceID tempCaptured;
                    if(take){
                        tempCaptured = pieces[currMove][tempID];
                        removeFromVector(pieces[currMove], x - dx, y - dy);
                    }
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].x = x - dx;
                    pieces[currMove][index].y = y - dy;
                    helpCheck();
                    if(!whiteCheck[currMove] && !blackCheck[currMove]){
                        attackVec.push_back({x - dx, y - dy});
                    }
                    whiteCheck[currMove] = tempCheckWhite;
                    blackCheck[currMove] = tempCheckBlack;
                    pieces[currMove][index].x = x;
                    pieces[currMove][index].y = y;
                    if(take){
                        pieceID tempCurr = pieces[currMove].back();
                        pieces[currMove].pop_back();
                        pieces[currMove].push_back(tempCaptured);
                        pieces[currMove].push_back(tempCurr);
                    }
                    index = pieces[currMove].size() - 1;
                }
                else{
                    attackVec.push_back({x - dx, y - dy});
                }

                if(take){
                    break;
                }
                dx++;
                dy++;
            }

            dx = 1;
            dy = 1;

            while(x + dx < 8 && y - dy >= 0){
                int tempID = getPieceIndex(pieces[currMove], x + dx, y - dy);
                if(tempID != -1 && sameColor(pieces[currMove][index].name, pieces[currMove][tempID].name)){
                    if(isCheckinCheck){
                        attackVec.push_back({x + dx, y - dy});
                    }
                    break;
                }
                bool take = tempID != -1;
                if(!isCheckinCheck){
                    pieceID tempCaptured;
                    if(take){
                        tempCaptured = pieces[currMove][tempID];
                        removeFromVector(pieces[currMove], x + dx, y - dy);
                    }
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].x = x + dx;
                    pieces[currMove][index].y = y - dy;
                    helpCheck();
                    if(!whiteCheck[currMove] && !blackCheck[currMove]){
                        attackVec.push_back({x + dx, y - dy});
                    }
                    whiteCheck[currMove] = tempCheckWhite;
                    blackCheck[currMove] = tempCheckBlack;
                    pieces[currMove][index].x = x;
                    pieces[currMove][index].y = y;
                    if(take){
                        pieceID tempCurr = pieces[currMove].back();
                        pieces[currMove].pop_back();
                        pieces[currMove].push_back(tempCaptured);
                        pieces[currMove].push_back(tempCurr);
                    }
                    index = pieces[currMove].size() - 1;
                }
                else{
                    attackVec.push_back({x + dx, y - dy});
                }

                if(take){
                    break;
                }
                dx++;
                dy++;
            }
            dx = 1;
            dy = 1;
            while(x - dx >= 0 && y + dy < 8){
                int tempID = getPieceIndex(pieces[currMove], x - dx, y + dy);
                if(tempID != -1 && sameColor(pieces[currMove][index].name, pieces[currMove][tempID].name)){
                    if(isCheckinCheck){
                        attackVec.push_back({x - dx, y + dy});
                    }
                    break;
                }
                bool take = tempID != -1;
                if(!isCheckinCheck){
                    pieceID tempCaptured;
                    if(take){
                        tempCaptured = pieces[currMove][tempID];
                        removeFromVector(pieces[currMove], x - dx, y + dy);
                    }
                    index = pieces[currMove].size() - 1;
                    pieces[currMove][index].x = x - dx;
                    pieces[currMove][index].y = y + dy;
                    helpCheck();
                    if(!whiteCheck[currMove] && !blackCheck[currMove]){
                        attackVec.push_back({x - dx, y + dy});
                    }
                    whiteCheck[currMove] = tempCheckWhite;
                    blackCheck[currMove] = tempCheckBlack;
                    pieces[currMove][index].x = x;
                    pieces[currMove][index].y = y;
                    if(take){
                        pieceID tempCurr = pieces[currMove].back();
                        pieces[currMove].pop_back();
                        pieces[currMove].push_back(tempCaptured);
                        pieces[currMove].push_back(tempCurr);
                    }
                    index = pieces[currMove].size() - 1;
                }
                else{
                    attackVec.push_back({x - dx, y + dy});
                }

                if(take){
                    break;
                }
                dx++;
                dy++;
            }
        }
        else if(pieces[currMove][index].name == 'N' || pieces[currMove][index].name == 'n'){
            float x = pieces[currMove][index].x;
            float y = pieces[currMove][index].y;
            bool tempCheckWhite = whiteCheck[currMove];
            bool tempCheckBlack = blackCheck[currMove];

            int dx[] = {1, 2, -1, -2, 1, 2, -1, -2};
            int dy[] = {2, 1, -2, -1, -2, -1, 2, 1};

            for(int i = 0; i < 8; i++){
                if(x + dx[i] < 0 || x + dx[i] >= 8 || y + dy[i] < 0 || y + dy[i] >= 8) continue;
                if(isCheckinCheck){
                    attackVec.push_back({x + dx[i], y + dy[i]});
                    continue;
                }
                int tempID = getPieceIndex(pieces[currMove], x + dx[i], y + dy[i]);
                if(tempID != -1 && sameColor(pieces[currMove][index].name, pieces[currMove][tempID].name)) continue;
                bool take = tempID != -1;

                pieceID tempCaptured;
                if(take){
                    tempCaptured = pieces[currMove][tempID];
                    removeFromVector(pieces[currMove], x + dx[i], y + dy[i]);
                }
                index = pieces[currMove].size() - 1;
                pieces[currMove][index].x = x + dx[i];
                pieces[currMove][index].y = y + dy[i];
                helpCheck();
                if(!whiteCheck[currMove] && !blackCheck[currMove]){
                    attackVec.push_back({x + dx[i], y + dy[i]});
                }
                whiteCheck[currMove] = tempCheckWhite;
                blackCheck[currMove] = tempCheckBlack;
                pieces[currMove][index].x = x;
                pieces[currMove][index].y = y;
                if(take){
                    pieceID tempCurr = pieces[currMove].back();
                    pieces[currMove].pop_back();
                    pieces[currMove].push_back(tempCaptured);
                    pieces[currMove].push_back(tempCurr);
                }
                index = pieces[currMove].size() - 1;
            }

        }
        else if(pieces[currMove][index].name == 'K' || pieces[currMove][index].name == 'k'){
            float x = pieces[currMove][index].x;
            float y = pieces[currMove][index].y;
            bool tempCheckWhite = whiteCheck[currMove];
            bool tempCheckBlack = blackCheck[currMove];

            int dx[] = {1, 1, 1, 0, -1, -1, -1, 0};
            int dy[] = {1, 0, -1, -1, -1, 0, 1, 1};

            for(int i = 0; i < 8; i++){
                if(x + dx[i] < 0 || x + dx[i] >= 8 || y + dy[i] < 0 || y + dy[i] >= 8) continue;
                if(isCheckinCheck){
                    attackVec.push_back({x + dx[i], y + dy[i]});
                    continue;
                }
                int tempID = getPieceIndex(pieces[currMove], x + dx[i], y + dy[i]);
                if(tempID != -1 && sameColor(pieces[currMove][index].name, pieces[currMove][tempID].name)) continue;
                bool take = tempID != -1;

                pieceID tempCaptured;
                if(take){
                    tempCaptured = pieces[currMove][tempID];
                    removeFromVector(pieces[currMove], x + dx[i], y + dy[i]);
                }
                pieces[currMove][index].x = x + dx[i];
                pieces[currMove][index].y = y + dy[i];
                helpCheck();
                if(!whiteCheck[currMove] && !blackCheck[currMove]){
                    attackVec.push_back({x + dx[i], y + dy[i]});
                }
                whiteCheck[currMove] = tempCheckWhite;
                blackCheck[currMove] = tempCheckBlack;
                pieces[currMove][index].x = x;
                pieces[currMove][index].y = y;
                if(take){
                    pieceID tempCurr = pieces[currMove].back();
                    pieces[currMove].pop_back();
                    pieces[currMove].push_back(tempCaptured);
                    pieces[currMove].push_back(tempCurr);
                }
            }

            if(!isCheckinCheck){
                helpCheck();
                int tempID = getPieceIndex(pieces[currMove], x + 3, y);
                if(!pieces[currMove][index].moved && tempID != -1 && !pieces[currMove][tempID].moved && !checkBoard[y][x]
                && !checkBoard[y][x + 1] && !checkBoard[y][x + 2] && getPieceIndex(pieces[currMove], x + 1, y) == -1
                && getPieceIndex(pieces[currMove], x + 2, y) == -1){
                    attackVec.push_back({x + 2, y});
                }

                tempID = getPieceIndex(pieces[currMove], x - 4, y);
                if(!pieces[currMove][index].moved && tempID != -1 && !pieces[currMove][tempID].moved && !checkBoard[y][x]
                && !checkBoard[y][x - 1] && !checkBoard[y][x - 2] && getPieceIndex(pieces[currMove], x - 1, y) == -1
                && getPieceIndex(pieces[currMove], x - 2, y) == -1 && getPieceIndex(pieces[currMove], x - 3, y) == -1){
                    attackVec.push_back({x - 2, y});
                }
            }
            
        }
    }

    void setCheck(){
        if(checkBoard[pieces[currMove][0].y][pieces[currMove][0].x] && currMove % 2 == 0){
            whiteCheck[currMove] = true;
        }
        else{
            whiteCheck[currMove] = false;
        }
        if(checkBoard[pieces[currMove][1].y][pieces[currMove][1].x] && currMove % 2 == 1){
            blackCheck[currMove] = true;
        }
        else{
            blackCheck[currMove] = false;
        }
    }

    void initCheck(){
        checkBoard.assign(8, std::vector<bool>(8, 0));
    }

    void helpCheck(){
        initCheck();
        if(currMove % 2 == 0){
            for(int i = 0; i < (int)pieces[currMove].size(); i++){
                if(pieces[currMove][i].name > 'a') continue;
                if(pieces[currMove][i].name == 'P'){
                    int x = pieces[currMove][i].x;
                    int y = pieces[currMove][i].y;
                    if(x + 1 < 8 && y + 1 < 8){
                        checkBoard[y + 1][x + 1] = true;
                    }
                    if(x - 1 >= 0 && y + 1 < 8){
                        checkBoard[y + 1][x - 1] = true;
                    }
                    continue;
                }
                findAttackRange(i, checkAttack, true);
                for(int i = 0; i < (int)checkAttack.size(); i++){
                    int x = checkAttack[i].x;
                    int y = checkAttack[i].y;
                    if(x < 0 || x >= 8 || y < 0 || y >= 8) continue;
                    checkBoard[y][x] = true;
                }
            }
        }
        else{
            for(int i = 0; i < (int)pieces[currMove].size(); i++){
                if(pieces[currMove][i].name < 'a') continue;
                if(pieces[currMove][i].name == 'p'){
                    int x = pieces[currMove][i].x;
                    int y = pieces[currMove][i].y;
                    if(x + 1 < 8 && y - 1 >= 0){
                        checkBoard[y - 1][x + 1] = true;
                    }
                    if(x - 1 >= 0 && y - 1 >= 0){
                        checkBoard[y - 1][x - 1] = true;
                    }
                    continue;
                }
                findAttackRange(i, checkAttack, true);
                for(int i = 0; i < (int)checkAttack.size(); i++){
                    int x = checkAttack[i].x;
                    int y = checkAttack[i].y;
                    if(x < 0 || x >= 8 || y < 0 || y >= 8) continue;
                    checkBoard[y][x] = true;
                }
            }
        }
        setCheck();
        
    }

    void castle(int index, int dx){
        float x = pieces[currMove][index].x;
        float y = pieces[currMove][index].y;

        if(x < dx){
            int tempID = getPieceIndex(pieces[currMove], x + 3, y);
            pieces[currMove][tempID].x = x + 1;
            pieces[currMove][tempID].moved = true;
        }
        else{
            int tempID = getPieceIndex(pieces[currMove], x - 4, y);
            pieces[currMove][tempID].x = x - 1;
            pieces[currMove][tempID].moved = true;
        }
    }

    void initPromotion(){
        proPieces.resize(0);
        int dir = proPawn.y == 0 ? 1 : -1;
        int pieceColor = dir == 1 ? 0 : 'A' - 'a';
        char pieceNames[] = {'q', 'r', 'n', 'b'};

        for(int i = 0; i < 4 && i > -4; i += dir){
            char pieceName = pieceNames[abs(i)] + pieceColor;
            proPieces.push_back({(uint8_t)proPawn.x, (uint8_t)(proPawn.y + i), pieceName, false});
        }
    }

    void checkCheckmate(){
        initPiecesCopy();
        if(currMove % 2 == 0){
            for(int i = 0; i < (int)piecesCopy.size(); i++){
                if(piecesCopy[i].name < 'a') continue;
                int index = getPieceIndex(pieces[currMove], piecesCopy[i].x, piecesCopy[i].y);
                findAttackRange(index, attackRange);
                if((int)attackRange.size() > 0) return;
            }
            if(whiteCheck[currMove]){
                checkmate = true;
                pipe.Explode();
            }
            else{
                stalemate = true;
                pipe.Explode();
            }
        }
        else{
            for(int i = 0; i < (int)piecesCopy.size(); i++){
                if(piecesCopy[i].name > 'a') continue;
                int index = getPieceIndex(pieces[currMove], piecesCopy[i].x, piecesCopy[i].y);
                findAttackRange(index, attackRange);
                if((int)attackRange.size() > 0) return;
            }
            if(blackCheck[currMove]){
                checkmate = true;
                pipe.Explode();
            }
            else{
                stalemate = true;
                pipe.Explode();
            }
        }
    }

    void initPiecesCopy(){
        piecesCopy.resize(0);
        for(pieceID piece : pieces[currMove]){
            piecesCopy.push_back(piece);
        }
    }

    void unPasso(){
        if(currMove % 2 == 0){
            for(int i = 0; i < (int)pieces[currMove].size(); i++){
                if(pieces[currMove][i].name < 'a') continue;
                pieces[currMove][i].passo = false;
            }
        }
        else{
            for(int i = 0; i < (int)pieces[currMove].size(); i++){
                if(pieces[currMove][i].name > 'a') continue;
                pieces[currMove][i].passo = false;
            }
        }
    }

    void resetGame(){
        currMove = 0;
        move = 0;
        selectedCoords = {-1, -1};
        initPieces(pieces);
        whiteCheck.resize(1);
        blackCheck.resize(1);
        selected = false;
        checkmate = false;
        stalemate = false;
        promotion = false;
        checkBoard.resize(0);
        attackRange.resize(0);
        proPieces.resize(0);
        piecesCopy.resize(0);
        boom.counter = 0;
        boom.animation = false;
    }

    void screenShake(){
        Vector2 d_shake[] = {{10.F, 10.F}, {-10.F, -10.F}, {10.F, -10.F}, {-10.F, 10.F}};
        
        shakeTimeElapsed = GetTime() - shakeStartTime;

        if(shakeTimeElapsed >= shakeDuration){
            shake = false;
            camera.target = {(float)offsetX + squareSize * 4, (float)offsetY + squareSize * 4};
            return;
        }

        int shakeFrames = (int)sizeof(d_shake) / (int)sizeof(Vector2);
        int index = (int)(shakeTimeElapsed * shakeFrames / shakeDuration);

        camera.target = {offsetX + squareSize * 4 + d_shake[index].x, offsetY + squareSize * 4 + d_shake[index].y};
    }

    void StartShake(){
        shake = true;
        shakeStartTime = GetTime();
    }

};

void UpdateFrame(Game& game){
    if(IsWindowResized()){
        int width = GetScreenWidth();
        int height = GetScreenHeight();
        
        int boardSize;
        if(height - 200 > width){
            boardSize = width - (width % 8);
        }
        else{
            boardSize = (int)(std::min(width, height) * 0.8);
        }
        squareSize = boardSize / 8;

        offsetX = (width - boardSize)  / 2;
        offsetY = (height - boardSize) / 2;

        game.camera.target = {(float)offsetX + squareSize * 4, (float)offsetY + squareSize * 4};
        game.camera.offset = {(float)offsetX + squareSize * 4, (float)offsetY + squareSize * 4};
        fontSize = squareSize / 2;
        outline = squareSize / 20;
        game.resetButton.onGoofyWindowResize();
        game.flipBlackButton.onGoofyWindowResize();
        game.moveBackButton.onGoofyWindowResize();
        game.moveForwardButton.onGoofyWindowResize();
    }
    
    if(IsKeyPressed(KEY_LEFT)){
        game.currMove = std::max(0, game.currMove - 1);
        game.attackRange.clear();
        game.selected = false;
        game.promotion = false;
    }
    else if(IsKeyPressed(KEY_RIGHT)){
        game.currMove = std::min(game.move, game.currMove + 1);
        game.attackRange.clear();
        game.selected = false;
        game.promotion = false;
    }
    else if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), game.camera);
        float x = mouse.x;
        float y = mouse.y;

        if(checkCollisionRecRounded(x, y, game.resetButton.rec, game.resetButton.roundness, game.resetButton.lineThick)){
            game.resetGame();
        }
        else if(checkCollisionRecRounded(x, y, game.flipBlackButton.rec, game.flipBlackButton.roundness, game.flipBlackButton.lineThick)){
            game.chesspiece.flipBlack = !game.chesspiece.flipBlack;
        }
        else if(checkCollisionRecRounded(x, y, game.moveBackButton.rec, game.moveBackButton.roundness, game.moveBackButton.lineThick)){
            game.currMove = std::max(0, game.currMove - 1);
            game.attackRange.clear();
            game.selected = false;
            game.promotion = false;
        }
        else if(checkCollisionRecRounded(x, y, game.moveForwardButton.rec, game.moveForwardButton.roundness, game.moveForwardButton.lineThick)){
            game.currMove = std::min(game.move, game.currMove + 1);
            game.attackRange.clear();
            game.selected = false;
            game.promotion = false;
        }
        else if(game.promotion){
            int boardX = (int)((x - offsetX) / squareSize);
            int boardY = (int)((y - offsetY) / squareSize);

            int index = getPieceIndex(game.proPieces, boardX, boardY);
            if(index != -1){
                int pawnIndex = getPieceIndex(game.pieces[game.currMove], game.proPawn.x, game.proPawn.y);
                game.pieces[game.currMove][pawnIndex].name = game.proPieces[index].name;
                game.promotion = false;
                game.move++;
                game.unPasso();
                game.helpCheck();
                game.checkCheckmate();
                game.attackRange.resize(0);
            } 
        }
        else if(!((game.stalemate || game.checkmate) && game.currMove == game.move)){
            int j = (int)((x - offsetX) / squareSize);
            int i = (int)((y - offsetY) / squareSize);

            if(x >= offsetX &&
            x < offsetX + 8 * squareSize &&
            y >= offsetY &&
            y < offsetY + 8 * squareSize){
                if(inRange(game.attackRange, (float)j, (float)i) && game.selected){

                    if(game.currMove < game.move){
                        game.checkmate = false;
                        game.stalemate = false;
                    }

                    game.pieces.resize(game.currMove + 1);
                    game.pieces.push_back(game.pieces[game.currMove]);
                    game.move = game.currMove;
                    game.currMove++;

                    if(getPieceIndex(game.pieces[game.currMove], (float)j, (float)i) != -1){
                        removeFromVector(game.pieces[game.currMove], (float)j, (float)i);
                        game.boom.coords = {(float)j, (float)i};
                        game.boom.StartAnimation();
                        game.boomSounds.Explode();
                        game.StartShake();
                    }
                    
                    int index = getPieceIndex(game.pieces[game.currMove], game.selectedCoords.x, game.selectedCoords.y);

                    if((index == 0 || index == 1) && abs(game.pieces[game.currMove][index].x - (float)j) == 2){
                        game.castle(index, j);
                    }

                    if((game.pieces[game.currMove][index].name == 'p' || game.pieces[game.currMove][index].name == 'P')
                    && abs(i - (int)game.pieces[game.currMove][index].y) == 2){
                        game.pieces[game.currMove][index].passo = true;
                    }
                    
                    game.pieces[game.currMove][index].x = (float)j;
                    game.pieces[game.currMove][index].y = (float)i;
                    game.pieces[game.currMove][index].moved = true;
                    
                    if(game.pieces[game.currMove][index].name == 'p' && getPieceIndex(game.pieces[game.currMove], (float)j, (float)i + 1) != -1
                    && game.pieces[game.currMove][getPieceIndex(game.pieces[game.currMove], (float)j, (float)i + 1)].passo){
                        removeFromVector(game.pieces[game.currMove], (float)j, (float)i + 1);
                        game.boom.coords = {(float)j, (float)i + 1};
                        game.boom.StartAnimation();
                        game.boomSounds.Explode();
                        game.StartShake();
                    }
                    else if(game.pieces[game.currMove][index].name == 'P' && getPieceIndex(game.pieces[game.currMove], (float)j, (float)i - 1) != -1
                    && game.pieces[game.currMove][getPieceIndex(game.pieces[game.currMove], (float)j, (float)i - 1)].passo){
                        removeFromVector(game.pieces[game.currMove], (float)j, (float)i - 1);
                        game.boom.coords = {(float)j, (float)i - 1};
                        game.boom.StartAnimation();
                        game.boomSounds.Explode();
                        game.StartShake();
                    }
                    
                    if(!game.boom.animation) game.moveSounds.Explode();

                    index = getPieceIndex(game.pieces[game.currMove], (float)j, (float)i);
                    
                    game.selected = false;
                    game.attackRange.resize(0);
                    
                    if((game.pieces[game.currMove][index].name == 'p' || game.pieces[game.currMove][index].name == 'P')
                    && (game.pieces[game.currMove][index].y == 7 || game.pieces[game.currMove][index].y == 0)){
                        game.promotion = true;
                        game.proPawn.x = game.pieces[game.currMove][index].x;
                        game.proPawn.y = game.pieces[game.currMove][index].y;
                        game.initPromotion();
                    }
                    else{
                        game.move++;
                        game.unPasso();
                        game.helpCheck();
                        game.checkCheckmate();
                        game.attackRange.resize(0);
                    }
                }
                else{
                    game.attackRange.resize(0);
                    int index = getPieceIndex(game.pieces[game.currMove], (float)j, (float)i);
                    if(!(index != -1 && isMove(game.pieces[game.currMove][index].name, game.currMove))){
                        game.selected = false;
                    }
                    else if(game.selectedCoords.x == (float)j && game.selectedCoords.y == (float)i){
                        game.selected = !game.selected;
                    }
                    else if(index != -1){
                        game.selected = true;
                    }
                    if(index != -1 && isMove(game.pieces[game.currMove][index].name, game.currMove)){
                        game.selectedCoords = {(float)j, (float)i};
                        game.findAttackRange(index, game.attackRange);
                    }
                }
            }
        }
    }

    BeginDrawing();

    BeginMode2D(game.camera);

    ClearBackground(LIGHTGRAY);
    
    bool _black;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if((i % 2 == 0 && j % 2 == 0) || (i % 2 == 1 && j % 2 == 1)){
                _black = false;
            }
            else{
                _black = true;
            }
            if(_black){
                DrawRectangle(offsetX + squareSize * j, offsetY + squareSize * i, squareSize, squareSize, BLACK);
            }
            else{
                DrawRectangle(offsetX + squareSize * j, offsetY + squareSize * i, squareSize, squareSize, WHITE);
            }
        }
    }

    if(game.selected){
        game.SelectPiece();
    }
    game.Draw();

    if(game.shake){
        game.screenShake();
    }
    
    EndMode2D();

    EndDrawing();
}

void goofyFuncWrap(void* ptr){
    Game* game = (Game*)(ptr);
    UpdateFrame(*game);
}

int main(){
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(ORIGINAL_GAME_WIDTH, ORIGINAL_GAME_HEIGHT, "Chess(?)");

    SetWindowMinSize(200, 200);
    
    Game game;
    initPieces(game.pieces);

    void* pGame = &game;

    #if defined(PLATFORM_WEB)

        emscripten_set_main_loop_arg(goofyFuncWrap, pGame, 0, 1);

    #else

        SetTargetFPS(FPS);
        while(!WindowShouldClose()){
            goofyFuncWrap(pGame);
        }
    
    #endif

    CloseWindow();
}

// em++ src/main.cpp -o web.html -I"C:/raylib/raylib/src" "C:/raylib/raylib/src/libraylib.web.a" -sUSE_GLFW=3 -sASYNCIFY -sSTACK_SIZE=262144 -sEXPORTED_RUNTIME_METHODS=HEAPF32 --shell-file "C:/raylib/raylib/src/minshell.html" -DPLATFORM_WEB --preload-file sprites --preload-file audio
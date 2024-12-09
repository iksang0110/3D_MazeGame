#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <windows.h>

// 화면과 맵의 크기 정의
// 처음에는 화면 크기를 더 크게 잡았었는데, 렌더링 속도가 너무 느려서 120x40으로 조정함
// 맵 크기는 32x32로 설정했지만 현재는 더 작은 크기만 사용 중. 나중에 더 큰 맵을 만들 때를 대비해 여유있게 잡음
#define screenWidth 120
#define screenHeight 40
#define mapWidth 32
#define mapHeight 32

// 콘솔 설정 함수
// 처음에는 단순히 콘솔 크기만 조정했었는데, 확장 ASCII 문자가 제대로 표시되지 않는 문제가 있었음
// SetConsoleOutputCP(437)을 추가하고 나서야 제대로 표시됨 - 이건 찾는데 꽤 오래 걸렸다!
void setupConsole() {
    // CP437 코드페이지 설정 (확장 ASCII 블록 문자용)
    SetConsoleOutputCP(437);

    // 콘솔 폰트 설정
    // Consolas 폰트가 가장 깔끔하게 보여서 선택함
    // 폰트 크기는 8x16이 가장 적절한 비율이었음 - 다른 크기는 화면이 찌그러져 보였다
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 8;
    cfi.dwFontSize.Y = 16;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

    // ANSI 이스케이프 시퀀스 활성화
    // 이걸 설정 안 하면 컬러 출력이 안 됨... 하..............왜 색상이 안 먹히나 한참 고민했다
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

// 미로 맵 정의
// 1: 벽, 0: 길, 2: 출구
// 처음에는 더 복잡한 맵을 만들었다가 테스트하기 어려워서 간단한 버전으로 수정함
// 나중에 다양한 맵을 쉽게 추가할 수 있도록 배열 크기를 넉넉히 잡아둠
// 매직아이로 보면 길이 보임 ㅋㅋ,,, 사실 시작하고 가장 왼쪽길로 쭉 따라가면 바로 출구
int worldMap[mapWidth][mapHeight] = {
     {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,1,1,0,1,0,0,0,1,0,1,1,1,1,0,1,0,1},
    {1,0,1,0,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,0,1,0,1},
    {1,0,1,0,1,1,1,0,1,0,0,0,0,0,0,0,1,0,0,1,0,1,0,1},
    {1,0,1,0,1,0,0,0,1,1,1,1,1,1,1,1,1,0,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1}
    //{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    //{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,3,0,3,0,3,0,0,0,1},
    //{1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,3,0,0,0,3,0,0,0,1},
    //{1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,0,0,0,0,0,1,1,0,1,1,0,0,0,0,3,0,3,0,3,0,0,0,1},
    //{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    //{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}

        /*{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}*/
};


// 플레이어 상태 변수들
// 위치(posX, posY)와 방향(dirX, dirY), 카메라 평면(planeX, planeY) 정의
// 이 값들은 레이캐스팅의 핵심이 되는 변수들
double posX = 1.5, posY = 1.5;      // 시작 위치 - 정수가 아닌 1.5로 한 이유는 벽과 겹치지 않게 하기 위함
double dirX = -1, dirY = 0;         // 시작 방향 - (-1,0)은 서쪽을 바라보는 상태
double planeX = 0, planeY = 0.66;   // 카메라 평면 - FOV를 약 66도로 설정
// planeY가 0.66인 이유: tan(FOV/2) = 0.66이므로 FOV가 약 66도가 됨
// 처음에는 0.66이 아니라 0.5로 설정했었는데, 너무 좁아서 FOV를 늘려야겠다고 판단함 구글링 하니까 fps게임 가장 이상적인 시야각이 66도정도라고 함
// 화면 렌더링 함수
// 레이캐스팅의 핵심 알고리즘이 구현된 부분
void render(char screen[screenHeight][screenWidth]) {
    // 화면 버퍼 초기화
    // 매 프레임마다 화면을 지우고 다시 그림
    for (int y = 0; y < screenHeight; y++)
        for (int x = 0; x < screenWidth; x++)
            screen[y][x] = ' ';

    // 각 수직선마다 레이캐스팅 수행
    for (int x = 0; x < screenWidth; x++) {
        // 카메라 평면상의 x 좌표 계산
        // -1에서 1 사이의 값으로 정규화됨
        // 이 값이 레이의 방향을 결정하는 데 사용됨
        double cameraX = 2 * x / (double)screenWidth - 1;

        // 레이의 방향 벡터 계산
        // 플레이어의 방향 벡터와 카메라 평면 벡터의 선형 결합
        // 이렇게 하면 화면의 왼쪽부터 오른쪽까지 부채꼴 모양으로 레이가 발사됨
        double rayDirX = dirX + planeX * cameraX;
        double rayDirY = dirY + planeY * cameraX;

        // 현재 맵상의 위치
        int mapX = (int)posX;
        int mapY = (int)posY;

        // DDA 알고리즘을 위한 변수들
        // sideDistX/Y: 레이가 다음 x/y측 격자선과 만나는 지점까지의 거리
        // deltaDistX/Y: 레이가 x/y방향으로 한 칸을 이동하는 데 필요한 거리
        // 이 부분 계산이 이해가 안 됐는데, 삼각형의 닮음비를 이용한 것임을 깨달음
        double sideDistX, sideDistY;
        double deltaDistX = fabs(1 / rayDirX);  // |1/rayDirX|
        double deltaDistY = fabs(1 / rayDirY);  // |1/rayDirY|
        double perpWallDist;  // 수직 벽까지의 거리 (어안 렌즈 효과를 피하기 위해 필요)

        // 레이의 이동 방향 결정
        // stepX/Y: 레이가 x/y방향으로 이동할 때 1 또는 -1
        int stepX, stepY;
        int hit = 0;  // 벽과 충돌 여부
        int side;     // 어느 면(x=0 또는 y=1)과 충돌했는지

        // 초기 사이드 거리와 이동 방향 계산
        // 레이의 방향에 따라 다음 격자선까지의 거리가 다름
        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (posX - mapX) * deltaDistX;
        }
        else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - posX) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (posY - mapY) * deltaDistY;
        }
        else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - posY) * deltaDistY;
        }

        // DDA(Digital Differential Analysis) 알고리즘
        // 레이가 벽에 부딪힐 때까지 맵을 순회
        // 이 알고리즘이 생각보다 빠르다! 처음에는 더 복잡한 방법을 생각했었는데,
        // 이 단순한 방법이 오히려 더 효율적이었음
        while (hit == 0) {
            // 다음 맵 사각형으로 이동
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;  // x 면과 충돌
            }
            else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;  // y 면과 충돌
            }
            if (worldMap[mapX][mapY] > 0) hit = 1;  // 벽을 만남
        }

        // 벽까지의 수직거리 계산
        // 어안 렌즈 효과를 피하기 위해 카메라 평면까지의 수직 거리를 사용
        // 이해하는데 꽤 시간이 걸렸다... 기하학적으로 많이 고민했음
        if (side == 0)
            perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
        else
            perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

        // 화면에 그릴 선의 높이 계산
        // 거리가 멀수록 선이 짧아짐 (원근감 표현)
        int lineHeight = (int)(screenHeight * 1.6 / perpWallDist);

        // 선의 시작점과 끝점 계산
        int drawStart = -lineHeight / 2 + screenHeight / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + screenHeight / 2;
        if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;

        // 벽 문자 선택
        // 거리와 충돌한 면에 따라 다른 문자를 사용해 깊이감을 표현
        // ASCII 블록 문자를 사용한 것이 3d 처럼 보이게 되어서 꽤 효과적이었다
        char wallChar;
        if (worldMap[mapX][mapY] == 2) {
            wallChar = 'X';  // 출구
        }
        else {
            // 거리와 측면에 따른 벽 표현
            // 측면은 더 어둡게 표현해서 입체감을 줌
            if (side == 0) {
                if (perpWallDist < 3) wallChar = '\xDB';        // 완전 블록
                else if (perpWallDist < 5) wallChar = '\xB2';   // 진한 음영
                else if (perpWallDist < 7) wallChar = '\xB1';   // 중간 음영
                else wallChar = '\xB0';                        // 연한 음영
            }
            else {
                // 측면은 한 단계 어둡게
                if (perpWallDist < 3) wallChar = '\xB2';
                else if (perpWallDist < 5) wallChar = '\xB1';
                else wallChar = '\xB0';
            }
        }

        // 벽 그리기
        for (int y = drawStart; y < drawEnd; y++) {
            if (y >= 0 && y < screenHeight && x >= 0 && x < screenWidth) {
                screen[y][x] = wallChar;
            }
        }

        // 바닥 그리기
        //  바닥을 안 그렸더니 공중에 떠 있는 것 같은 느낌이었음
        // 바닥을 추가하니 훨씬 더 현실감 있어졌다 신기하구만
        for (int y = drawEnd; y < screenHeight; y++) {
            if (y >= 0 && y < screenHeight) {
                screen[y][x] = 'F';  // 바닥 표시용 마커
            }
        }
    }
}

// 화면 표시 함수
// ANSI 이스케이프 시퀀스를 사용해 컬러 출력
// 처음에는 Windows API를 사용했는데, ANSI 시퀀스가 더 간단하고 빨랐다
void display(char screen[screenHeight][screenWidth]) {
    printf("\033[H");  // 커서를 화면 왼쪽 상단으로 이동
    for (int y = 0; y < screenHeight; y++) {
        for (int x = 0; x < screenWidth; x++) {
            if (screen[y][x] == 'F') {
                printf("\033[44m \033[0m");  // 바닥 - 파란색
            }
            else if (screen[y][x] == ' ') {
                printf("\033[40m \033[0m");  // 하늘 - 검은색
            }
            else if (screen[y][x] == 'X') {
                printf("\033[31;1mX\033[0m");  // 출구 - 빨간색
            }
            else {
                putchar(screen[y][x]);  // 벽
            }
        }
        printf("\n");
    }
}

// 플레이어 이동 함수
// 충돌 검사를 포함해서 플레이어의 위치를 업데이트
void movePlayer(int forward) {
    // 이동 속도 - 처음에는 0.05로 설정했었는데 너무 느렸음
    // 0.15가 적당한 속도라고 판단됨
    double moveSpeed = 0.15;

    // 새로운 위치 계산
    // forward가 1이면 앞으로, -1이면 뒤로 이동
    double newPosX = posX + dirX * moveSpeed * forward;
    double newPosY = posY + dirY * moveSpeed * forward;

    // X 방향 충돌 검사
    // 벽이 아닌 경우에만 이동
    // 출구(2)인 경우 게임 종료
    if (worldMap[(int)newPosX][(int)posY] == 0) {
        posX = newPosX;
    }
    else if (worldMap[(int)newPosX][(int)posY] == 2) {
        system("cls");
        printf("\033[32m탈출 성공!\033[0m\n");  // 초록색으로 성공 메시지 표시
        printf("아무 키나 누르면 종료됩니다...");// 글자깨짐 ㅠㅠ
        _getch();
        exit(0);
    }

    // Y 방향 충돌 검사
    // X 방향과 동일한 로직
    if (worldMap[(int)posX][(int)newPosY] == 0) {
        posY = newPosY;
    }
    else if (worldMap[(int)posX][(int)newPosY] == 2) {
        system("cls");
        printf("\033[32m탈출 성공!\033[0m\n");
        printf("아무 키나 누르면 종료됩니다..."); // 글자 다 깨짐....
        _getch();
        exit(0);
    }
}

// 플레이어 회전 함수
// 방향 벡터와 카메라 평면을 회전 행렬로 변환
void rotatePlayer(int direction) {
    // 회전 속도 - 이것도 여러 번 조정해봤는데 0.1이 가장 자연스러웠음
    double rotSpeed = 0.1;

    // 방향 벡터 회전
    // 2D 회전 행렬 적용:
    // | cos 세타  -sin 세타 |
    // | sin 세타   cos 세타 |
    double oldDirX = dirX;
    dirX = dirX * cos(rotSpeed * direction) - dirY * sin(rotSpeed * direction);
    dirY = oldDirX * sin(rotSpeed * direction) + dirY * cos(rotSpeed * direction);

    // 카메라 평면도 같은 각도로 회전
    // 이걸 빼먹었을 때는 회전하면서 화면이 이상하게 찌그러졌었다 
    double oldPlaneX = planeX;
    planeX = planeX * cos(rotSpeed * direction) - planeY * sin(rotSpeed * direction);
    planeY = oldPlaneX * sin(rotSpeed * direction) + planeY * cos(rotSpeed * direction);
}

int main() {
    setupConsole();  // 콘솔 설정
    char screen[screenHeight][screenWidth];

    // 화면 초기화 및 커서 숨기기
    // ANSI 이스케이프 시퀀스 사용
    printf("\033[2J\033[?25l");

    // 프레임 타이밍을 위한 변수들
    // QueryPerformanceCounter는 Sleep보다 정확한 타이밍 제공
    LARGE_INTEGER frequency, lastTime, currentTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);

    while (1) {
        // 키 입력 처리
        // _kbhit으로 키 입력 체크
        if (_kbhit()) {
            char input = _getch();
            switch (input) {
            case 'q': goto cleanup;  // 종료
            case 'a': rotatePlayer(1); break;   // 왼쪽 회전
            case 'd': rotatePlayer(-1); break;  // 오른쪽 회전
            }
        }

        // W/S 키 상태 확인
        // GetAsyncKeyState를 사용해서 키를 누르고 있는 동안 계속 이동
        // _getch()만 사용했을 때는 움직임이 너무 끊겼는데, 이렇게 하니 부드러워졌다! 하지만 여전히 매끄럽지는 않다
        if (GetAsyncKeyState('W') & 0x8000) {
            movePlayer(1);  // 전진
        }
        if (GetAsyncKeyState('S') & 0x8000) {
            movePlayer(-1);  // 후진
        }

        render(screen);   // 화면 렌더링
        display(screen);  // 화면 표시

        // 60 FPS 유지
        // Sleep(16)을 사용했는데 프레임이 불안정했음
        // QueryPerformanceCounter를 사용하니 훨씬 안정적이 되었다
        QueryPerformanceCounter(&currentTime);
        double elapsedTime = (double)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
        if (elapsedTime < 1.0 / 60.0) {
            Sleep((DWORD)((1.0 / 60.0 - elapsedTime) * 1000));
        }
        QueryPerformanceCounter(&lastTime);
    }

cleanup:
    printf("\033[?25h");  // 커서 다시 보이기
    return 0;
}
#include <ncurses.h>
#include <csignal>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <thread>
#include <semaphore.h>
#include <mutex>
#include <fstream>


bool stopFlag;
sem_t sem;
std::mutex lock;

struct MassageJoin // first messge
{
    long mtype;
    pid_t pid;
};

struct Massge{ // puzniejsze wiadomosci
    long mtype;
    int move;
    pid_t pid_;
    char map[5][5];

    pid_t pidServer;
    unsigned int numberRound;
    unsigned int numberPlayer;
    std::string type;
    unsigned int deaths;
    unsigned int coinsSecured;
    unsigned int coinsUnSecured;
    unsigned int positionX;
    unsigned int positionY;
};


void getMsg(Massge &msg, int msgId);
void sentMsg(Massge *msg, int msgId);

int main()
{
    system("resize -s 12 50");
    initscr(); //1
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_WHITE);
    init_pair(3, COLOR_RED, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_YELLOW);
    init_pair(5, COLOR_BLACK, COLOR_RED);
    noecho();
    attron(COLOR_PAIR(2));
    curs_set(0);
    keypad(stdscr, TRUE);
    stopFlag = true;


//    sem_init(&sem, 0 ,0);
    stopFlag = true;
    //kolejka do przystapienia do gry
    MassageJoin msgJoin;
    key_t keyJoin = ftok("tmp", 150);
    int msgIdJoin = msgget(keyJoin, 0666 | IPC_CREAT);
    msgJoin.mtype = 5;
    msgJoin.pid = getpid();
    msgsnd(msgIdJoin, &msgJoin, sizeof(msgJoin), 0);

    mvprintw(0, 0,"Czekam na serwer !!!!!!");

//    //kolejka odbieraca
    Massge msg;
    key_t keyToQueue = ftok("tmp1", getpid());
    int msgId = msgget(keyToQueue, 0666 | IPC_CREAT);
    std::thread input(sentMsg, &msg, msgId);
    getMsg(msg, msgId);

//    lock.lock();
//    getch(); //3
//    pthread_join(test1);
    endwin(); //4

    return 0;
}
#define SCHIFT_Y 2
#define SCHIFT_X 1
void getMsg(Massge &msg, int msgId) {
    while (stopFlag){
        int temp1 = msgrcv(msgId, &msg, sizeof(msg), getpid(), 0);
        attron(COLOR_PAIR(1));
        mvprintw(0, 0,"                       ");
        lock.lock();
        attron(COLOR_PAIR(5));
        for ( unsigned int i = 0; i <12; i++ )
            for (unsigned int j = 0; j < 50; j++ )
                mvprintw(i, j+10, " ");
        attron(COLOR_PAIR(2));
        for (auto i = 0; i < 7; i++){
            mvprintw(0+SCHIFT_Y, i+SCHIFT_X, "#");
            mvprintw(i+SCHIFT_Y, 0+SCHIFT_X, "#");
            mvprintw(i+SCHIFT_Y, 6+SCHIFT_X, "#");
            mvprintw(6+SCHIFT_Y, i+SCHIFT_X, "#");
        }
        for ( int i = 0; i<5; i++ ){
            for ( int j = 0; j < 5; j++ ){
                auto ch = msg.map[i][j];
                if ( ch == '#' ){
                    attron(COLOR_PAIR(1));
                    mvprintw(i+1+SCHIFT_Y, j+1+SCHIFT_X, " ");
                }
                else if ( ch == ' '){
                    attron(COLOR_PAIR(2));
                    mvprintw(i+1+SCHIFT_Y, j+1+SCHIFT_X, " ");
                }
                else if ( ch == 'c' ){
                    attron(COLOR_PAIR(4));
                    mvprintw(i+1+SCHIFT_Y, j+1+SCHIFT_X, "C");
                }
                else if ( ch == 'T' ){
                    attron(COLOR_PAIR(4));
                    mvprintw(i+1+SCHIFT_Y, j+1+SCHIFT_X, "T");
                }
                else if ( ch == 't' ){
                    attron(COLOR_PAIR(4));
                    mvprintw(i+1+SCHIFT_Y, j+1+SCHIFT_X, "t");
                }
                else if ( ch == '*' ){
                    attron(COLOR_PAIR(5));
                    mvprintw(i+1+SCHIFT_Y, j+1+SCHIFT_X, "*");
                }
                else {
                    attron(COLOR_PAIR(5));
                    mvprintw(i+1+SCHIFT_Y, j+1+SCHIFT_X, "%c", ch);
                }
//                mvprintw(i, j, "%c", msg.map[i][j]);
            }
        }
        attron(COLOR_PAIR(5));
        mvprintw(3+SCHIFT_Y, 3+SCHIFT_X, "G");
        mvprintw(0, 10, "Server's PID:%d ", msg.pidServer);
        mvprintw(1, 10, "Campsite " );
        mvprintw(2, 10, "Round number %d ", msg.numberRound);
        mvprintw(5, 10, "Player:");
            mvprintw(6, 12, "%d", msg.numberPlayer);
            mvprintw(7, 12, "Curr X/Y %d/%d", msg.positionX, msg.positionY);
            mvprintw(8, 12, "Deths %d", msg.deaths);

            mvprintw(10, 12, "Coins secured: %d", msg.coinsSecured );
            mvprintw(11, 12, "Coins unsecured: %d", msg.coinsUnSecured );


        refresh();
        lock.unlock();
        sem_post(&sem);
        usleep(100);
    }
}

void sentMsg(Massge *msg, int msgId)
{
    int input = 0;
    while ( stopFlag ){
        input = getch();
        refresh();
        if ( input == KEY_LEFT )
            msg->move = KEY_LEFT;
        else if ( input == KEY_DOWN )
            msg->move = KEY_DOWN;
        else if ( input == KEY_RIGHT )
            msg->move = KEY_RIGHT;
        else if ( input == KEY_UP )
            msg->move = KEY_UP;
        else if ( input == 'e' ){
            stopFlag = false;
            break;
        }
//        else
//            continue;
        msg->mtype = getpid() * 2;
        msgsnd(msgId, msg, sizeof(Massge), 0);
        lock.unlock();
        sleep(1);
    }
}


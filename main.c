#include<stdio.h>
#include<windows.h>
#include<stdlib.h>
#include<conio.h>
#include<time.h>
#define clear() system("cls")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))
#define CURSOR_RESET gotoxy(0,58);
#define plyr_pos(plyr_id) players[plyr_id].pos
#define print_name_wclr(plyr_id) printf("%s%s\033[0m",clr_codes[players[plyr_id].colour],players[plyr_id].name);
#define BOARD_RESET draw_board();for(int p=0;p<player_count;p++) {place_plyr(p,players[p].pos);}
#define TAX 250
#define JAIL_FEE 100
#define START_MONEY 700
#define SPEEDING_FINE 80
#define _Red "\033[0;31m"
#define _Green "\033[0;32m"
#define _Blue "\033[0;34m"
#define _Purple "\033[0;35m"
#define _Cyan "\033[0;36m"
#define _Yellow "\033[0;33m"
#define _White "\033[0;37m"
#define _NORMAL "\033[0m"
//CONSOLE ATTRIBUTES
#define Terminal_Red 79
#define Terminal_Green 175
#define Terminal_Blue 159
#define Terminal_DBlue 31
#define Terminal_Purple 95
#define Terminal_Cyan 63
#define Terminal_NORMAL 7
//NAMES OF PROPERTIES
#define Balt_avan  "Baltic Avenue"
#define Read_rail  "Bond Street"
#define Verm_avan  "Vermonto Avenue"
#define NewY_avan  "New York Avenue"
#define Bow_street "Bow Street"
#define ol_ken_rod "Old Kent Road"
#define Marv_avan  "Marvin Avenue"
#define Whitehall  "WhiteHall"
#define Park_Lane  "Park Lane"
#define Mayf_PP    "Mayfair Park Palace"

void over_print(int,int);
void end_screen_fun();
void game_start();
void print_name_block_wclr(int,int);
void draw_board();
int get_player_info();
int startup();
int sell_prop(int,int,int);
const char* clr_codes[5]={_Red,_Green,_Blue,_Purple,_Cyan};
const char* clr_names[5]={"Red","Green","Blue","Purple","Cyan"};
const int clr_terminal[5]={Terminal_Red,Terminal_Green,Terminal_Blue,Terminal_Purple,Terminal_Cyan};
const char* prop_names[15]={"",Balt_avan ,Read_rail ,Verm_avan ,"",NewY_avan ,Bow_street,ol_ken_rod,"",Marv_avan ,Whitehall ,"","",Park_Lane ,Mayf_PP};
char clr_choice[]={0,1,2,3,4,-1}; //Will be empty after colour selection
const int jail_coord[2] = {12,42};
int player_count=0;
int current_player = 0;
int num_gameover = 0;
int gameIsRunning = 1;
//Board Coordinate {x,y}
int board_coord[16][2] = {
    {108,47},{84,47},{60,47},{36,47},
    {6,43},{12,37},{12,27},{12,17},
    {12, 2},{36, 7},{60, 7},{84, 7},
    {99, 3},{108,17},{108,27},{108,37}
    };
//1 2 3 5 6 9 10 13 
//0-> Property Code, 1-> Owned = 1, Not Owned = 0, 2-> Player ID, 3-> Houses, 4-> Hotels
int property_properies[16][5]={
    {0,0,-1,0,0},{1,0,-1,0,0},{2,0,-1,0,0},{3,0,-1,0,0},
    {4,0,-1,0,0},{5,0,-1,0,0},{6,0,-1,0,0},{7,0,-1,0,0},
    {8,0,-1,0,0},{9,0,-1,0,0},{10,0,-1,0,0},{11,0,-1,0,0},
    {12,0,-1,0,0},{13,0,-1,0,0},{14,0,-1,0,0},{15,0,-1,0,0}
};
//0-> Buy Money, 1-> Sell Money, 2-> Rent Normal, 3-> Rent House, 4-> Rent Hotel, 5-> Buy House1, 6-> Buy House2, 7-> Buy Hotel
int buy_sell_pay[16][8]={
    {0,0,0,0,0,0,0,0},{120,180,50,50,100,110,180,350},{180,140,60,40,80,120,180,200},{240,200,80,50,100,160,180,220},
    {0,0,0,0,0,0,0,0},{320,160,100,60,100,220,240,280},{380,190,140,100,120,240,250,270},{550,250,180,100,120,240,250,270},
    {0,0,0,0,0,0,0,0},{600,350,220,120,150,425,430,520},{670,380,240,140,160,440,450,550},{0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},{1000,600,330,180,200,580,720,1200},{1200,720,400,200,220,650,890,1500},{0,0,0,0,0,0,0,0}
};

//File pointer declaration
FILE* start_screen_fp;
FILE* board_draw_fp;
FILE* plyr_stats_fp;
FILE* jail_draw_fp;
FILE* chance_fp;

//Structure for each player
struct playerprop{
    char name[10];// Name of user {Default= Empty String}
    int name_len; //Length of the name of the user {Default=0}
    int colour; //Colour of choice of user (Colour id) {Default=-1}->[as colour starts from 0]
    int money; //How much money the user has {Default=1000}
    int pos; //Current position of User {Default=0}->[Position at GO]
    int in_jail; //Flag value weather if user is in Jail or not {Default=0}-> 0 implies not in JAIL
    int in_park; //Flag value weather if user is in Parking or not {Default=0}-> 0 implies not in PARK
    int num_owned_prop; // Number of owned properties the user has {Default=0}
    int owned_prop[9]; // Property ID of owned properties {Default=Empty array}
    int game_over; //Flag value if the player is removed from the game {0 if still in play, 1 if not}
    int total_asset; // Net value of Player
    int reset_flag; // Flag value to check if property has been reset or not
}players[4]={{"",0,-1,START_MONEY,0,0,0,0,{},0,0,0},{"",0,-1,START_MONEY,0,0,0,0,{},0,0,0},{"",0,-1,START_MONEY,0,0,0,0,{},0,0,0},{"",0,-1,START_MONEY,0,0,0,0,{},0,0,0}};

//End screen after quiting or game over
void end_screen_fun(){
    FILE* end_screen;
    int cursor_offset=0;
    char end_lines[250];
    clear();
    end_screen = fopen("Files\\Leaving_Message.txt","r");
    while(!feof(end_screen)) {
        fgets(end_lines,250,end_screen);
        gotoxy(80,20+(cursor_offset++));
        printf(end_lines);
    }
    fclose(end_screen);
    gotoxy(0,100);
    exit(1);
}

//Rolls DICE
int roll_dice() {
    srand(time(NULL));
    return ((rand()%6)+1);
}
//Gives Random number
int rand_number(int max) {
    srand(time(NULL));
    return ((rand()%max)+1);
}

//Finds number of digits
int find_digits(int n)  
{
    int counter=0;while(n!=0){n=n/10;counter++;}return counter;  
} 

//Prints colour pallete on the right to select from
void print_colors () {
    FILE* colour_pal;
    int cursor_offset=0;
    char colorlines[40];
    colour_pal = fopen("Files\\Colour_Selector.txt","r");
    while(!feof(colour_pal)) {
        fgets(colorlines,40,colour_pal);
        gotoxy(150-4,20+(cursor_offset++));
        printf(colorlines);
    }
    fclose(colour_pal);
    for(int i=0;i<5;i++) {
        gotoxy(150,23+i);
        for(int j=0;j<5;j++)
            if(clr_choice[j]==i) printf("%c - %s%s\033[0m",clr_names[i][0],clr_codes[i],clr_names[i]);
    }
}

void print_househotel() {
    for(int pos=0;pos<16;pos++){
        int y_offset=0;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),240);
        for(int i=0;i<property_properies[pos][3];i++) {
            gotoxy(board_coord[pos][0]+6,board_coord[pos][1]-3+(y_offset++));
            printf(" House ");
        }
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),112);
        for(int i=0;i<property_properies[pos][4];i++) {
            gotoxy(board_coord[pos][0]+6,board_coord[pos][1]-3+(y_offset++));
            printf(" Hotel ");
        }
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);
    }
}

//Print name block with colour (Flag=0 means normal Block, Flag=1 means Long Block)
void print_name_block_wclr(int plyr_id,int flag) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),clr_terminal[players[plyr_id].colour]);
    if(flag==0) {printf(" %c ",*players[plyr_id].name);}
    else {printf("           %c           ",*players[plyr_id].name);}
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),Terminal_NORMAL);
}

//Places Player's block/piece on the board
void place_plyr(int plyr_id,int board_pos){
    if(players[plyr_id].in_jail && !players[plyr_id].game_over) {
        gotoxy(jail_coord[0],jail_coord[1]+(plyr_id+1)%4);
        print_name_block_wclr(plyr_id,0);
    } else if(!players[plyr_id].game_over){
        gotoxy(board_coord[board_pos][0],board_coord[board_pos][1]+(plyr_id+1)%4);
        print_name_block_wclr(plyr_id,0);
    }
}

//Finds the length of the name of player
void find_len(int plyr_id) {
    int i=0,len=0;
    while(players[plyr_id].name[i++]!='\0') {len++;}
    players[plyr_id].name_len = len;
} 

//Prints player stats on the top right
void print_plyr_stats () {
    int cursor_offset=0;
    int x_offset=0;
    int i=0;
    char plyr_statlines[40];
    for(int plyr_id=0;plyr_id<player_count;plyr_id++){
        if(!players[plyr_id].game_over){
            int center_money=find_digits(players[plyr_id].money);
            cursor_offset=0;
            int y_prop_offset=0;
            plyr_stats_fp = fopen("Files\\player_stats.txt","r");
            while(!feof(plyr_stats_fp)) {
                fgets(plyr_statlines,40,plyr_stats_fp);
                gotoxy(126+x_offset,1+(cursor_offset++));
                for(int i=0;i<23;i++) {
                    if(plyr_statlines[i]=='#' ) {
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),clr_terminal[players[plyr_id].colour]);
                        printf(" ");
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),Terminal_NORMAL);
                    } else {
                        printf("%c",plyr_statlines[i]);
                    }
                }
                if(plyr_id==current_player){
                    gotoxy(133+x_offset-(int)(players[plyr_id].name_len/2),2);
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),6);
                    printf("===>");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),Terminal_NORMAL);
                }
            }
            fclose(plyr_stats_fp);
            gotoxy(137+x_offset-(int)(players[plyr_id].name_len/2),2); // Centering Player Money
            print_name_wclr(plyr_id); // Printing Player Money
            gotoxy(137+x_offset-(int)(center_money/2),4);
            printf("%d",players[plyr_id].money); //Printing Player Money
            for(int plr_prop=0;plr_prop<players[plyr_id].num_owned_prop;plr_prop++) { 
                gotoxy(129+x_offset,6+y_prop_offset++); // Centering Text
                printf("%s",prop_names[players[plyr_id].owned_prop[plr_prop]]); // Printing Owned Properties
            }
            x_offset+=28;
        }
    }
}

//Moves Player on Board {GO flag = 1 implies collect on go}
void move_plyr(int plyr_id,int pos_offset,int go_flag) {
    CURSOR_RESET;
    for(int coord=(plyr_pos(plyr_id)+1);coord<=plyr_pos(plyr_id)+pos_offset;coord++) {
        draw_board();
        for(int tmp_plr=0;tmp_plr<player_count;tmp_plr++){
            if(tmp_plr==plyr_id){
                place_plyr(plyr_id,coord%16);
                if(coord%16==0 && go_flag==1) {
                    players[plyr_id].money += 200;
                }
            } else {
                place_plyr(tmp_plr,players[tmp_plr].pos);
            }
        }
        CURSOR_RESET;
        Sleep(100); //TODO UNDO
    }
    Sleep(100); //TODO UNDO
    plyr_pos(plyr_id) = (plyr_pos(plyr_id)+pos_offset)%16;
    CURSOR_RESET;
}

//MOVES PLAYER BACKWARDS
void move_plyr_back(int plyr_id,int pos_offset) {
    CURSOR_RESET;
    for(int coord=(plyr_pos(plyr_id)-1);coord>=plyr_pos(plyr_id)-pos_offset;coord--) {
        draw_board();
        for(int tmp_plr=0;tmp_plr<player_count;tmp_plr++){
            if(tmp_plr==plyr_id){
                place_plyr(plyr_id,coord%16);
            } else {
                place_plyr(tmp_plr,players[tmp_plr].pos);
            }
        }
        CURSOR_RESET;
        Sleep(100); //TODO UNDO
    }
    Sleep(100); //TODO UNDO
    plyr_pos(plyr_id) = (plyr_pos(plyr_id)-pos_offset)%16;
    CURSOR_RESET;
}

//Draws the Board
void draw_board() {
    char board_lines[200];
    int cursor_offset=0;
    clear();
    board_draw_fp = fopen("Files\\Board.txt","r");
    while(!feof(board_draw_fp)) {
        fgets(board_lines,200,board_draw_fp);
        gotoxy(0,1+(cursor_offset++));
        printf(board_lines);        
    }printf("\n");
    fclose(board_draw_fp);
    for(int plyr_id=0;plyr_id<player_count;plyr_id++) {
        for(int i=0;i<players[plyr_id].num_owned_prop;i++) {
            if(!players[plyr_id].game_over) {
                gotoxy(board_coord[players[plyr_id].owned_prop[i]][0]-10,board_coord[players[plyr_id].owned_prop[i]][1]-5);
                print_name_block_wclr(plyr_id,1);
            }
        }
    }
    print_househotel();
    print_plyr_stats();
}

//Prints the current card on which player is on
void print_prop_card(int crd_no) {
    int temp_prop_id=crd_no;
    if(crd_no==5)crd_no=4;if(crd_no==6)crd_no=5;
    if(crd_no==7)crd_no=6;if(crd_no==9)crd_no=7;
    if(crd_no==10)crd_no=8;if(crd_no==13)crd_no=9;
    if(crd_no==14)crd_no=10;
    char card_lines[100];
    int line=1,cursor_offset=0;
    FILE* card_disp;
    card_disp=fopen("Files\\Cards.txt","r");
    while(!feof(card_disp)) {
            fgets(card_lines,100,card_disp);
        if(line >(crd_no-1)*17 && line<=((crd_no)*17)+1){
            gotoxy(126,35+(cursor_offset++));
            printf(card_lines);
        } if(line == crd_no*17) break;
        line++;
    }printf("\n");
    fclose(card_disp);
    if(!players[current_player].in_jail && property_properies[temp_prop_id][1]){
        int temp_len=(players[property_properies[temp_prop_id][2]].name_len + 11)/2;
        gotoxy(139-temp_len,39);printf("Owned by - ");print_name_wclr(property_properies[temp_prop_id][2]);
    }
}


//Updates Player Assets
void update_assets(int plyr_id) {
    players[plyr_id].total_asset=0;
    for(int i=0;i<players[plyr_id].num_owned_prop;i++) {
        players[plyr_id].total_asset += buy_sell_pay[players[plyr_id].owned_prop[i]][0] + //Property Buy Cost
        (property_properies[players[plyr_id].owned_prop[i]][3] * buy_sell_pay[players[plyr_id].owned_prop[i]][3]) + //House payment
        (property_properies[players[plyr_id].owned_prop[i]][4] * buy_sell_pay[players[plyr_id].owned_prop[i]][4]); // Hotel Payment
    }
    players[plyr_id].total_asset += players[plyr_id].money;
}

//Prints Victory Screen
void print_victory_screen(int plyr_id, int how) {
    FILE* win_fp;
    int cursor_offset=0;
    char win_file_lines[250];
    clear();
    win_fp = fopen("Files\\Victory_Screen.txt","r");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),clr_terminal[players[plyr_id].colour]);
    for(int i=0;i<62;i++){for(int j=0;j<236;j++) {printf(" ");}printf("\n");}
    while(!feof(win_fp)) {
        fgets(win_file_lines,250,win_fp);
        gotoxy(36,5+(cursor_offset++));
        printf(win_file_lines);
    }
    fclose(win_fp);
    if(how){
        gotoxy(117-(50+players[plyr_id].name_len)/2,48);printf("%s has Attained MONOPOLY by Being the only person on the board",players[plyr_id].name);
    } else {
        gotoxy(117-(50+players[plyr_id].name_len)/2,48);printf("%s has Attained MONOPOLY by Acquiring all properties",players[plyr_id].name);
    }
       
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),Terminal_NORMAL);
    gotoxy(0,62);
    exit(1);
}

//Checks if Winner has been Declared
void check_winner() {
    for(int plyr_id=0;plyr_id<player_count;plyr_id++) {
        if(players[plyr_id].num_owned_prop==10) { // Winning by Owning all the properties in the game 
            print_victory_screen(plyr_id,0);
        }
        if(players[plyr_id].game_over==0 && player_count-num_gameover == 1) { // Winning by being the only person in the game
            print_victory_screen(plyr_id,1);
        } 
    }
}

//Checks player assets {Returns 1 if has assets Returns 0 if does not}
int check_assets(int plyr_id) {
    int offset=0;
    if(players[plyr_id].money<=0) {
        BOARD_RESET;
        if(players[plyr_id].num_owned_prop==0) {
            gotoxy(126,20+(offset++));printf("You have no money and no property to sell, you cannot attain Monopoly");
            players[plyr_id].game_over=1;num_gameover+=1;
            Sleep(2400);
            return 0;
        } else {
            gotoxy(126,20+(offset++));printf("You have no money,");
            gotoxy(126,20+(offset++));printf("you are forced to sell your owned properties");
            Sleep(2400);
            BOARD_RESET;
            sell_prop(plyr_id,0,-1);
            Sleep(1600);
            gotoxy(126,30);printf("Press any key to continue...");getch();
        } 
    }
    return 1;
}

//RESETS PROPERTY PROPERTIES
void prop_reset(int plyr_id,int prop_id){
    property_properies[prop_id][1]=0;
    property_properies[prop_id][2]=-1;
    property_properies[prop_id][3]=0;
    property_properies[prop_id][4]=0;
    for(int i=0;i<players[plyr_id].num_owned_prop;i++) {
        if(players[plyr_id].owned_prop[i]==prop_id){
            for(int j=i;j<players[plyr_id].num_owned_prop;j++) { // testing
                players[plyr_id].owned_prop[j]=players[plyr_id].owned_prop[j+1];
            }
        }
    }
    players[plyr_id].num_owned_prop -=1;
}

//Resets Player Properties
void player_reset(int plyr_id) {
    players[plyr_id].colour = -1;
    players[plyr_id].money = -1;
    players[plyr_id].pos = -1;
    players[plyr_id].num_owned_prop = 0;
    players[plyr_id].game_over = 1;
    players[plyr_id].total_asset = 0;
}

//Sells an owned propert {PlayerID, Flag value for debt sell, Payment for Debt Sell} {Returns cursor_offset}
int sell_prop(int plyr_id, int force_pay_flag, int payment) {
    int sell_inp;
    int temp_ids[9][3]={}; //0-> Number Assigned , 1-> PropCode, 2-> Sell Cost
    int sell_off=1;
    BOARD_RESET;
    if(force_pay_flag==1) {
        gotoxy(126,20+(sell_off++));printf("You have to Pay %dR", payment);
    }
    for(int i=0;i<players[plyr_id].num_owned_prop;i++) {
        int sell_payment = buy_sell_pay[players[plyr_id].owned_prop[i]][1] + //Property Sell Cost
            (property_properies[players[plyr_id].owned_prop[i]][3] * buy_sell_pay[players[plyr_id].owned_prop[i]][3]) + //House payment
            (property_properies[players[plyr_id].owned_prop[i]][4] * buy_sell_pay[players[plyr_id].owned_prop[i]][4]); // Hotel Payment
        gotoxy(126,21+(sell_off++));printf("Press %d to Sell %s (+ %dR)",i+1,prop_names[players[plyr_id].owned_prop[i]],sell_payment);
        temp_ids[i][0]=i+1;
        temp_ids[i][1]=players[plyr_id].owned_prop[i];
        temp_ids[i][2]=sell_payment;
    }
    gotoxy(126,21+sell_off);
    while((sell_inp=(getch()-48))!=EOF){
        if(sell_inp>0 && sell_inp<=players[plyr_id].num_owned_prop) {
            for(int i=0;i<players[plyr_id].num_owned_prop;i++) {
                if(temp_ids[i][0]==sell_inp) {
                    prop_reset(plyr_id,temp_ids[i][1]);
                    players[current_player].money += temp_ids[i][2];
                    gotoxy(126,22+(sell_off++));print_name_wclr(current_player);printf(" + %dR",temp_ids[i][2]);
                    gotoxy(126,23+(sell_off++));printf("You have \033[0;32mSuccessfully\033[0m SOLD %s",prop_names[temp_ids[i][1]]);
                    Sleep(1000);
                    return sell_off;
                }
            }
        }
    }
}

//Assigns colour to each player, cannot take colour which is already picked
int assign_clr(int plyr_id,int clr_id) {
    for(int i=0;i<5;i++) {
        if(clr_choice[i]==clr_id) {
            for(int j=0;j<5-i;j++) {
                clr_choice[j+i] = clr_choice[j+i+1];
            }
            players[plyr_id].colour=clr_id;
            return 0;
        }
    }
    return 1;
}

//Enter Jail Screen
int enter_jail() {
    char jail_lines[200];
    int cursor_offset=0;
    clear();
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),Terminal_Red);
    for(int i=0;i<28;i++){for(int j=0;j<235;j++) {printf(" ");}printf("\n");}
    jail_draw_fp = fopen("Files\\jail_flash_red.txt","r");
    while(!feof(jail_draw_fp)) {
        fgets(jail_lines,200,jail_draw_fp);
        gotoxy(70,0+(cursor_offset++));
        printf(jail_lines);        
    }printf("\n");
    fclose(jail_draw_fp);
    printf("\n\n\n\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),Terminal_DBlue);
    for(int i=0;i<30;i++){for(int j=0;j<235;j++) {printf(" ");}printf("\n");}
    cursor_offset=0;
    jail_draw_fp = fopen("Files\\jail_flash_blue.txt","r");
    while(!feof(jail_draw_fp)) {
        fgets(jail_lines,200,jail_draw_fp);
        gotoxy(39,35+(cursor_offset++));
        printf(jail_lines);        
    }printf("\n");
    fclose(jail_draw_fp);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),Terminal_NORMAL);
    Sleep(4000);
    CURSOR_RESET;
}


int attempt_exitjail(int plyr_id) {
    char choice;
    char confirm_pay;
    int die;
    int outer_flag=0;
    int inner_flag=0;
    int offset=0;
    gotoxy(126,20);printf("You are currently in Jail....");
    gotoxy(126,23);printf("Press P to Pay Police 500R and get out");
    gotoxy(126,24);printf("Press R to Roll die, if you get a 6 you are set free");
    gotoxy(126,25);
    while(outer_flag==0 && (choice=getch())!=EOF){
        switch(choice) {
            case 'p':
                if(players[plyr_id].money>=JAIL_FEE){
                    players[plyr_id].money -= JAIL_FEE;
                    gotoxy(126,25);print_name_wclr(plyr_id);printf(" - %dR ",JAIL_FEE);
                    Sleep(1300);
                    gotoxy(126,30);printf("Press any key to continue...");getch();
                    players[plyr_id].pos=4;
                    outer_flag=1;
                    BOARD_RESET;
                    break;
                } else if(players[plyr_id].total_asset >= JAIL_FEE){
                    if(players[plyr_id].money<JAIL_FEE){
                        BOARD_RESET;
                        gotoxy(126,21);printf("You dont have enough money to pay for Police Fee, you have to sell owned properties to pay");
                        gotoxy(126,22);printf("OR");
                        gotoxy(126,23);printf("Roll a 6 to get out");
                        gotoxy(126,24);printf("Press S to sell owned property");
                        gotoxy(126,25);printf("Press R to roll dice");
                        while((confirm_pay=getch())!=EOF && inner_flag==0) {
                            switch(confirm_pay) {
                                case 's':
                                    if(players[plyr_id].total_asset>=JAIL_FEE) {
                                        while(players[plyr_id].money<JAIL_FEE) {
                                            offset = sell_prop(plyr_id,0,-1);
                                            Sleep(1800);
                                        }
                                        players[plyr_id].money -= JAIL_FEE;
                                        gotoxy(126,20+offset);print_name_wclr(plyr_id);printf(" - %dR",JAIL_FEE);
                                        players[plyr_id].pos=4;
                                        Sleep(1300);
                                        gotoxy(126,30);printf("Press any key to continue...");getch();
                                        inner_flag = 1;
                                        break;
                                    }
                                case 'r':
                                    goto roll;
                            }
                        }
                    }
                    outer_flag=1;
                    break;
                } else {
                    gotoxy(126,21);printf("You dont have enough assets to pay for JAIL FEE, you are forced to roll the dice");
                    Sleep(1000);
                    goto roll;
                }
            case 'r':
                roll:
                die = roll_dice();
                gotoxy(126,26);printf("ROLLING");
                for(int i=0;i<3;i++) {
                    Sleep(170);
                    printf(".");
                }
                gotoxy(126,28);printf("Dice: %d",die);
                Sleep(600);
                if(die==6) {
                    gotoxy(126,29);
                    printf("You got a 6!!, you are now set free...");
                    players[plyr_id].pos=4;
                    Sleep(800);
                    BOARD_RESET;
                    return 1;
                } else {
                    gotoxy(126,30);
                    printf("Better luck next time... :)");
                    Sleep(800);
                    return 0;
                }
        }
    }
}

int is_on_property(int plyr_id) {
    if(plyr_pos(plyr_id)!=0 && plyr_pos(plyr_id)!=4 && plyr_pos(plyr_id)!=8 && plyr_pos(plyr_id)!=11 && plyr_pos(plyr_id)!=15) 
    return 1;
    else return 0;
}

//Main function to get player info
int get_player_info() {
    get_again:
    clear();
    int cursor_offset=0;
    char startlines[50],colour_inp;
    start_screen_fp=fopen("Files\\Monopoly.txt","r");
    while(!feof(start_screen_fp)) {
        fgets(startlines,150,start_screen_fp);
        gotoxy(95,20+(cursor_offset++));
        printf(startlines);
    }printf("\n");
    fclose(start_screen_fp);
    gotoxy(90,22+cursor_offset);printf("Enter Number of Players(Max 4 / Min 2): ");
    scanf("%d",&player_count);
    // player_count-=48;
    if( player_count>4 || player_count<0) {
        gotoxy(90,23+cursor_offset);printf("\033[0;31mINVALID!\033[0m MAX players is 4 and MIN is 2, Please enter 4 or less");
        player_count=0;
        Sleep(1100); //TODO UNDO
        goto get_again;
    } else if(player_count)
    for(int i=0;i<player_count;i++) {
        enter_again:
        clear();
        cursor_offset=0;
        start_screen_fp=fopen("Files\\Monopoly.txt","r");
        while(!feof(start_screen_fp)) {
            fgets(startlines,150,start_screen_fp);
            gotoxy(95,20+(cursor_offset++));
            printf(startlines);
        }printf("\n");
        fclose(start_screen_fp);
        print_colors();
        gotoxy(90,22+cursor_offset);printf("Enter Player %d's Name: ",i+1);scanf("%s",&players[i].name);find_len(i);
        // Checks if name has any special characters
        for(int n=0;n<players[i].name_len;n++) {
            if( players[i].name[n]<97 || players[i].name[n]>122) {
                gotoxy(90,23+cursor_offset);printf("-%c-",players[i].name[n]);
                gotoxy(90,24+cursor_offset);printf("Name has \033[0;31mINVALID\033[0m Characters");
                Sleep(500); //TODO UNDO
                goto enter_again;
            }
        }
        gotoxy(90,23+cursor_offset);int _lenn=printf("Enter %s's Colour: ",players[i].name);
        int flag = 0;
        while((colour_inp=getch())!=EOF){
            switch(colour_inp) {
                case 'r': flag = assign_clr(i,colour_inp=0);break;
                case 'g': flag = assign_clr(i,colour_inp=1);break;
                case 'b': flag = assign_clr(i,colour_inp=2);break;
                case 'p': flag = assign_clr(i,colour_inp=3);break;
                case 'c': flag = assign_clr(i,colour_inp=4);break;
                default:flag = 2;break;
            }
            if (!flag) break;
            else if(flag!=2) {gotoxy(90,24+cursor_offset);printf("%s%s\033[0m is already designated..",clr_codes[colour_inp],clr_names[colour_inp]);gotoxy(90+_lenn,23+cursor_offset);}
            else {gotoxy(90,24+cursor_offset);printf("\033[0;31mINVALID CHARACTER\033[0m                          ");gotoxy(90+_lenn,23+cursor_offset);}
        }
        gotoxy(90,25+cursor_offset);printf("\033[0;32mSuccessfully\033[0m Assigned %s to ",players[i].name);print_name_wclr(i);
        Sleep(1100); //TODO UNDO
    }
    clear();
    cursor_offset=0;
    start_screen_fp=fopen("Files\\Monopoly.txt","r");
    while(!feof(start_screen_fp)) {
        fgets(startlines,150,start_screen_fp);
        gotoxy(95,20+(cursor_offset++));
        printf(startlines);
    }printf("\n");
    fclose(start_screen_fp);
    // PRINT PLAYERS {CHECKING}
    for(int i=0;i<player_count;i++){
        gotoxy(90,22+cursor_offset++);printf("Player %d: ",i+1);print_name_wclr(i);
    }
    //First letter of name to Capital to each player
    for(int i=0;i<player_count;i++) {
        if(players[i].name[0]>=97 && players[i].name[0]<=122) {players[i].name[0] -= 32;} 
    }
    Sleep(2000); //TODO UNDO
}


//Startup Screen/Menu
int startup() {
    clear();
    char st_inp;
    int cursor_offset=0;
    char start_lines[50];
    start_screen_fp=fopen("Files\\Monopoly.txt","r");
    while(!feof(start_screen_fp)) {
        fgets(start_lines,150,start_screen_fp);
        gotoxy(95,20+(cursor_offset++));
        printf(start_lines);
    }printf("\n");
    fclose(start_screen_fp);
    gotoxy(98,22+cursor_offset);printf("Press P to Play");
    gotoxy(124,22+cursor_offset);printf("Press Q to Quit\n");                              
    gotoxy(0,0);
    while((st_inp=getch())!=EOF){
        switch(st_inp) {
            case 'q': end_screen_fun();
            case 'p': get_player_info();goto end;
        }
    }
    end:
        return 1;
}

void handle_quit() {
    char response;

    printf("Are you sure you want to quit? (Y/N): ");
    scanf(" %c", &response); // Note the space before %c to consume the newline character

    if (response == 'Y' || response == 'y') {
        printf("Quitting the game. Goodbye!\n");
        gameIsRunning = 0; // Set the game state to quit
    } else {
        printf("Resuming the game.\n");
    }
}

//Game start
void game_start(){
    clear();
    char st_inp;
    int dice_flag=0;
    int dice_num=0;
    int offset=0;
    int sell_offset=0;
    int rent_payment=0;
    int chan_offset=0;
    int c_line=0;
    char chance_lines[150]; // _++_+_+_+_+__+_+_+_=-+-+_=_+_+-+-
    char plyr_statlines[40];
    char opt;
    char prop_confirm;
    while(1) {
        offset = 0;
        check_winner();
        update_assets(current_player);
        if(players[current_player].game_over) {
            if(!players[current_player].reset_flag) {
                for(int i=0;i<players[current_player].num_owned_prop;i++) {
                    prop_reset(current_player,players[current_player].owned_prop[i]);
                    player_reset(current_player);
                }
                players[current_player].reset_flag=1;
            }
            goto next;
        }
        BOARD_RESET;
        //JAIL LOGIC
        if(players[current_player].in_jail==1 || players[current_player].in_park==1) {
            players[current_player].in_park=0;
            if(players[current_player].in_jail==1) {
                if(attempt_exitjail(current_player)) {
                    players[current_player].in_jail=0;
                    goto jail_bypass;
                }
            }
            goto next;
        }
        jail_bypass:
        gotoxy(126,19);print_name_wclr(current_player);printf(" Press R to Roll the dice...");
        while((st_inp=getch())!=EOF && dice_flag==0){
            if(st_inp=='r') {
                dice_num=roll_dice();
                gotoxy(126,21);printf("Dice: %d",dice_num);dice_flag=1;gotoxy(126,21);
                if(dice_num==6) {
                    gotoxy(126,23);print_name_wclr(current_player);printf(" + 50");
                    players[current_player].money+=50;
                }
                gotoxy(126,24);
                Sleep(550);
                break;
            }
        }
        Sleep(1000);
        move_plyr(current_player,dice_num,1);
        dice_flag = 0;

        //CHANCE LOGIC
        chan_offset=0;
        c_line=0;
        if(plyr_pos(current_player)==11) {
            int rand_num = rand_number(14);
            chance_fp = fopen("Files\\Chance_cards.txt","r");
            while(!feof(chance_fp)) {
                fgets(chance_lines,150,chance_fp);
                c_line++;
                if(c_line==rand_num){
                    gotoxy(126,21+(chan_offset++));printf(chance_lines);
                    Sleep(2000);
                    break;
                }
            }
            fclose(chance_fp);
            switch(c_line) {
                case 1:
                    move_plyr(current_player,2,1);
                    break;
                case 2:
                    move_plyr(current_player,11,0);
                    break;
                case 3:
                    players[current_player].money+=150;
                    gotoxy(126,22+(chan_offset++));print_name_wclr(current_player);printf(" + 150");
                    break;
                case 4:
                    break;
                case 5:
                    move_plyr_back(current_player,1);
                    break;
                case 6:
                    move_plyr(current_player,10,1);
                    break;
                case 7:
                    move_plyr(current_player,6,1);
                    break;
                case 8:
                    for(int plyr_id=0;plyr_id<player_count;plyr_id++) {
                        if(!players[plyr_id].game_over && plyr_id!=current_player && players[plyr_id].money>=25) {
                            players[plyr_id].money-=25;
                            gotoxy(126,22+(chan_offset++));print_name_wclr(plyr_id);printf(" - 25");
                            players[current_player].money +=25;
                            gotoxy(126,22+(chan_offset++));print_name_wclr(current_player);printf(" + 25");
                            chan_offset++;
                        }
                    }
                    break;
                case 9:
                    move_plyr(current_player,5,1);
                    break;
                case 10:
                    move_plyr(current_player,1,1);
                    goto bypass;
                    break;
                case 11:
                    for(int i=0;i<players[current_player].num_owned_prop;i++) {
                        int payment = ((property_properies[players[current_player].owned_prop[i]][3]) * 20) + 
                                      ((property_properies[players[current_player].owned_prop[i]][4]) * 40);
                        gotoxy(126,22+(chan_offset++));print_name_wclr(current_player);printf(" - %d for %s",payment,prop_names[players[current_player].owned_prop[i]]);
                    }
                    break;
                case 12:
                    move_plyr(current_player,14,1);
                    break;
                case 13:
                    gotoxy(126,22+(chan_offset++));printf("You have to pay %d Speeding Fine :(",SPEEDING_FINE);
                    Sleep(1000);
                    if(players[current_player].money>=SPEEDING_FINE) {
                        players[current_player].money -= SPEEDING_FINE;
                    }
                    else if(players[current_player].total_asset >= SPEEDING_FINE){
                        while(players[current_player].money<SPEEDING_FINE) {
                            BOARD_RESET;
                            gotoxy(126,22+(chan_offset++));printf("You dont have enough money to pay for Fine, you have to sell owned properties");
                            Sleep(3000);
                            offset=sell_prop(current_player,1,SPEEDING_FINE);
                            Sleep(3000);
                        }
                        players[current_player].money -= SPEEDING_FINE;
                        gotoxy(126,22+(offset++));print_name_wclr(current_player);printf(" - %d",SPEEDING_FINE);
                        Sleep(2000);
                    } else {
                        gotoxy(126,22+(offset++));printf("You dont have enough assets to pay for Speeding Fine, you cannot attain MONOPOLY");
                        players[current_player].game_over=1;num_gameover+=1;
                        Sleep(4000);
                    }
                    gotoxy(126,25+(offset++));print_name_wclr(current_player);printf(" - %d",SPEEDING_FINE);
                    break;
                case 14:
                    for(int plyr_id=0;plyr_id<player_count;plyr_id++) {
                        if(players[current_player].money>=30 && plyr_id!=current_player) {
                            players[current_player].money-=30;
                            gotoxy(126,22+(offset++));print_name_wclr(current_player);printf(" - 30");
                            players[plyr_id].money +=30;
                            gotoxy(126,22+(offset++));print_name_wclr(plyr_id);printf(" + 30");
                            offset++;
                        }
                    }
                    break;
                case 15:
                    move_plyr(current_player,2,1);
                    break;
                case 16:
                    for(int plyr_id=0;plyr_id<player_count;plyr_id++) {
                        if(!players[plyr_id].game_over && plyr_id!=current_player && players[plyr_id].money>=50) {
                            players[plyr_id].money-=50;
                            gotoxy(126,22+(chan_offset++));print_name_wclr(plyr_id);printf(" - 50");
                            players[current_player].money +=50;
                            gotoxy(126,22+(chan_offset++));print_name_wclr(current_player);printf(" + 50");
                            chan_offset++;
                        }
                    }
                    break;
            }
            bypass:
            gotoxy(126,30);printf("Press any key to continue...");
            getch();
        }

        //TAX SYSTEM
        
        if(plyr_pos(current_player)==15) {
            gotoxy(126,20+(offset++));printf("You have to pay %dR TAX :(",TAX);
            Sleep(1000);
            if(players[current_player].money>=TAX) {
                players[current_player].money -= TAX;
            }
            else if(players[current_player].total_asset >= TAX){
                while(players[current_player].money<TAX) {
                    gotoxy(126,22+(offset++));printf("You dont have enough money to pay for Tax, you have to sell owned properties to pay for Tax");
                    Sleep(1300);
                    offset = sell_prop(current_player,1,TAX);
                    Sleep(3000);
                }
                players[current_player].money -= TAX;
            } else {
                gotoxy(126,22+(++offset));printf("You dont have enough assets to pay for Tax, you cannot attain MONOPOLY");
                players[current_player].game_over=1;num_gameover+=1;
                Sleep(3000);
                goto next;
            }
            gotoxy(126,22+(++offset));print_name_wclr(current_player);printf(" - %dR",TAX);
            Sleep(2000);
            gotoxy(126,30);printf("Press any key to continue...");getch();
        }

        else if(plyr_pos(current_player)==8) {
            players[current_player].in_park=1;
        }
        else if(plyr_pos(current_player)==12) {
            Sleep(500);
            enter_jail();
            plyr_pos(current_player) = -1;
            place_plyr(current_player,plyr_pos(current_player));
            players[current_player].in_jail=1;
        }

        offset = 0;
        sell_offset=0;

        if(is_on_property(current_player)) {
            BOARD_RESET; 
            if(property_properies[plyr_pos(current_player)][2]!=current_player && property_properies[plyr_pos(current_player)][1]==1) {
                print_prop_card(plyr_pos(current_player));
                rent_payment = buy_sell_pay[plyr_pos(current_player)][2] + //Property Rent
                (property_properies[plyr_pos(current_player)][3] * buy_sell_pay[plyr_pos(current_player)][3]) + //House payment
                (property_properies[plyr_pos(current_player)][4] * buy_sell_pay[plyr_pos(current_player)][4]); // Hotel Payment
                gotoxy(126,19);
                printf("This property has already been Acquired, you must pay %dR to ",rent_payment);print_name_wclr(property_properies[plyr_pos(current_player)][2]);
                if(players[current_player].money>=rent_payment) {
                    players[current_player].money -= rent_payment;
                } else if(players[current_player].total_asset>=rent_payment) {
                    while(players[current_player].money<rent_payment) { //Keeps Selling Property until player has money to pay
                        gotoxy(126,22+offset);printf("You dont have enough money to pay the Player, you have to sell Owned property to pay");
                        Sleep(2000);
                        offset=sell_prop(current_player,1,rent_payment);
                        Sleep(3000);
                    }
                    players[current_player].money-=rent_payment;
                } else {
                    gotoxy(126,20+(offset++));printf("You dont have enough assets to pay for rent, you cannot attain MONOPOLY");
                    players[current_player].game_over=1;num_gameover+=1;
                    Sleep(3000);
                    goto next;
                }
                gotoxy(126,21+(offset++));print_name_wclr(current_player);printf(" - %dR",rent_payment);
                players[property_properies[plyr_pos(current_player)][2]].money += rent_payment;
                gotoxy(126,22+(offset++));print_name_wclr(property_properies[plyr_pos(current_player)][2]);printf(" + %dR",rent_payment);
                Sleep(1000);
                gotoxy(126,23+(offset++));printf("Press any key to continue...");getch();
            }


            offset = 0;
            if(current_player == property_properies[plyr_pos(current_player)][2]) {
                gotoxy(126,21);printf("You have landed on your own Property, you get 25R");
                players[current_player].money += 25;
                gotoxy(126,22);print_name_wclr(current_player);printf(" + %dR",25);
                Sleep(1200);
            }
            BOARD_RESET;
            print_prop_card(plyr_pos(current_player));
            if(players[current_player].num_owned_prop!=0){
                gotoxy(126,20);printf("Press S to Sell any owned properties");
            }
            if(property_properies[plyr_pos(current_player)][1]==0 && players[current_player].money>=buy_sell_pay[plyr_pos(current_player)][0]) {
                gotoxy(126,21+(offset++));printf("Press B to Buy current property");
            }
            if(!players[current_player].in_jail && property_properies[plyr_pos(current_player)][2]==current_player) {
                gotoxy(126,21+(offset++));printf("Press H to build a House/Hotel on this property");
            }
            gotoxy(126,21+(offset++));printf("Press C to Continue to next player");
            gotoxy(126,21+(offset++));
            offset = 0;
            while((opt=getch())!=EOF){
                switch(opt) {
                    case 's':
                        if(players[current_player].num_owned_prop!=0){
                            offset=sell_prop(current_player,0,-1);
                            Sleep(2000);
                            gotoxy(126,30);printf("Press any key to continue...");getch();
                            goto next;
                        }
                    case 'b':
                        if(property_properies[plyr_pos(current_player)][1]==0 && players[current_player].money>=buy_sell_pay[plyr_pos(current_player)][0]) {
                            BOARD_RESET;
                            print_prop_card(plyr_pos(current_player));
                            if(players[current_player].money>=buy_sell_pay[plyr_pos(current_player)][0]) {
                                property_properies[plyr_pos(current_player)][1] = 1 ; // Sets Ownership Flag to 1
                                players[current_player].money -= buy_sell_pay[plyr_pos(current_player)][0]; // Subtracts Money
                                gotoxy(126,19+(offset++));print_name_wclr(current_player);printf(" - %dR ",buy_sell_pay[plyr_pos(current_player)][0]); // Shows Transaction
                                property_properies[plyr_pos(current_player)][2] = current_player; //Setting Property owner to PlayerID
                                players[current_player].num_owned_prop+=1; // Increments number of owned properties for player
                                players[current_player].owned_prop[players[current_player].num_owned_prop-1] = plyr_pos(current_player);//Gives property code to owner
                                gotoxy(126,20+(offset++));printf("You have \033[0;32mSuccessfully\033[0m Acquired %s",prop_names[plyr_pos(current_player)]);
                            }
                            else {
                                gotoxy(126,21+(offset++));printf("You dont have enough money to buy %s :(",prop_names[plyr_pos(current_player)]);
                            }
                            Sleep(2000);
                            goto next;
                        }  
                    case 'h':
                        if(!players[current_player].in_jail && property_properies[plyr_pos(current_player)][2]==current_player) {
                            BOARD_RESET;
                            gotoxy(126,21+(offset++));
                            if(property_properies[plyr_pos(current_player)][3]==0 || property_properies[plyr_pos(current_player)][3]==1) {
                                if(property_properies[plyr_pos(current_player)][3]==0 && players[current_player].money>=buy_sell_pay[plyr_pos(current_player)][5]) {
                                    players[current_player].money -= buy_sell_pay[plyr_pos(current_player)][5];// Subtracts Money
                                    gotoxy(126,22+(offset++));print_name_wclr(current_player);printf(" - %dR",buy_sell_pay[plyr_pos(current_player)][5]);// Shows Transaction 
                                    property_properies[plyr_pos(current_player)][3] +=1;// Builds House
                                }
                                else if(property_properies[plyr_pos(current_player)][3]==1 && players[current_player].money>=buy_sell_pay[plyr_pos(current_player)][6]) {
                                    players[current_player].money -= buy_sell_pay[plyr_pos(current_player)][6];// Subtracts Money
                                    gotoxy(126,22+(offset++));print_name_wclr(current_player);printf(" - %dR",buy_sell_pay[plyr_pos(current_player)][6]);// Shows Transaction 
                                    property_properies[plyr_pos(current_player)][3] +=1;// Builds House
                                }
                                else if(players[current_player].money<buy_sell_pay[plyr_pos(current_player)][5] || players[current_player].money<buy_sell_pay[plyr_pos(current_player)][6]){
                                    gotoxy(126,22+(offset++));printf("You dont have enough money to buy a House on this property :(");
                                    Sleep(3000);
                                    goto next;
                                }
                                gotoxy(126,23+(offset++));printf("You have \033[0;32mSuccessfully\033[0m built a House on %s",prop_names[plyr_pos(current_player)]);
                                Sleep(2500);
                                gotoxy(126,30);printf("Press any key to continue...");getch();
                                goto next;
                            } else if(property_properies[plyr_pos(current_player)][4]==0) {
                                if(players[current_player].money>=buy_sell_pay[plyr_pos(current_player)][7]){
                                    players[current_player].money -= buy_sell_pay[plyr_pos(current_player)][7]; // Subtracts money
                                    property_properies[plyr_pos(current_player)][4] += 1; // Sets number of hotels to 1
                                    gotoxy(126,21+(offset++));print_name_wclr(current_player);printf(" - %dR",buy_sell_pay[plyr_pos(current_player)][7]); // Shows Transaction
                                }
                                else {
                                    gotoxy(126,21+(offset++));printf("You dont have enough money to buy a Hotel on this property :(");
                                    Sleep(2000);
                                    goto next;
                                }
                                gotoxy(126,23+(offset++));printf("You have \033[0;32mSuccessfully\033[0m built a Hotel on %s",prop_names[plyr_pos(current_player)]);
                                gotoxy(126,30);printf("Press any key to continue...");getch();
                            }
                        }
                    case 'c':
                        goto next;
                    }   
            }
        }
        next:
        current_player=(current_player+1)%player_count;
    }
    CURSOR_RESET;
}
//Main fucntion
int main() {
    startup();
    game_start();
    char key;

    // Game loop
    while (gameIsRunning) {
        key = getchar(); // Get user input
        
        // Check for quit shortcut (for example, pressing 'Q' key)
        if (key == 'Q' || key == 'q') {
            handle_quit();
        }
    }
    exit(1);
}

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>

using namespace std;

//Embora tenha usado linux para desenvolver o projeto deparei-me com um problema no Windows.

//O uso da copia dos arrays north e south servem para puder alterar a cópia sem ter que alterar os originais e assim
//conseguir testar as jogadas de forma segura, ou seja, sem perder dado nenhum

#ifdef _WIN32
#ifndef UNICODE
#define UNICODE
#endif
#include <windows.h>
#include <wincon.h>

// Fix compilatin on MinGW
#ifndef DISABLE_NEWLINE_AUTO_RETURN
#define DISABLE_NEWLINE_AUTO_RETURN 0x0008
#endif

#ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200
#endif

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif





bool SetupConsole() {
	//configura a consola no Windows de modo a que os ASCII codes sejam aceites. Código retirado do Microsoft Docs:
	//https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#example-of-enabling-virtual-terminal-processing
	// Set output mode to handle virtual terminal sequences
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hIn == INVALID_HANDLE_VALUE || hOut == INVALID_HANDLE_VALUE) {
		return false;
	}
	DWORD dwOriginalInMode = 0;
	DWORD dwOriginalOutMode = 0;
	if (!GetConsoleMode(hIn, &dwOriginalInMode)) {
		return false;
	}
	if (!GetConsoleMode(hOut, &dwOriginalOutMode)) {
		return false;
	}
	DWORD dwInMode = dwOriginalInMode | ENABLE_VIRTUAL_TERMINAL_INPUT;
	if (!SetConsoleMode(hIn, dwInMode))
	{
		// Failed to set VT input mode, can't do anything here.
		return false;
	}
	DWORD dwOutMode = dwOriginalOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
	if (!SetConsoleMode(hOut, dwOutMode)) {
		// we failed to set both modes, try to step down mode gracefully.
		dwOutMode = dwOriginalOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if (!SetConsoleMode(hOut, dwOutMode))
		{
			// Failed to set any VT mode, can't do anything here.
			return false;
		}
	}
	return true;
}
#endif //_WIN32

#define YELLOW "\033[1;33m"
#define NO_COLOR "\033[0m"

void cleanInput(){
	//Limpa o output e caso seja feito Ctrl-D sai automáticamente do jogo
	if (cin.eof())
		exit(0);
	if (cin.peek()!=EOF)
		while (cin.get()!='\n');
}

void atualizaTabuleiro(int a[6], int b[6], int& c, int& d, string name_player1, string name_player2) {
	//mostra a situação atual do tabuleiro
	cout << "        ______________________________________________________" << endl;
	cout << "       /                                                      \\" << endl;
	cout << "      /      " << std::left << setfill(' ') << setw(20) << name_player2 << "                              \\" << endl;
	cout << "     /       ____    ____    ____    ____    ____    ____       \\" << endl;
	cout << "    /       |f   |  |e   |  |d   |  |c   |  |b   |  |a   |       \\" << endl;
	cout << "   / ____   | " << std::right << setw(2) << setfill('0') << a[5] << " |  | " << setw(2) << setfill('0') << a[4] << " |  | " << setw(2) << setfill('0') << a[3] << " |  | " << setw(2) << setfill('0') << a[2] << " |  | " << setw(2) << setfill('0') << a[1] << " |  | " << setw(2) << setfill('0') << a[0] << " |        \\" << endl;
	cout << "  / |T2  |  |____|  |____|  |____|  |____|  |____|  |____|   ____  \\" << endl;
	cout << " /  |    |                                                  |T1  |  \\" << endl;
	cout << "/   | " << setw(2) << setfill('0') << c << " |                                                  |    |   \\" << endl;
	cout << "\\   |    |                                                  | " << setw(2) << setfill('0') << d << " |   /" << endl;
	cout << " \\  |____|   ____    ____    ____    ____    ____    ____   |    |  /" << endl;
	cout << "  \\         |A   |  |B   |  |C   |  |D   |  |E   |  |F   |  |____| /" << endl;
	cout << "   \\        | " << setw(2) << setfill('0') << b[0] << " |  | " << setw(2) << setfill('0') << b[1] << " |  | " << setw(2) << setfill('0') << b[2] << " |  | " << setw(2) << setfill('0') << b[3] << " |  | " << setw(2) << setfill('0') << b[4] << " |  | " << setw(2) << setfill('0') << b[5] << " |        /" << endl;
	cout << "    \\       |____|  |____|  |____|  |____|  |____|  |____|       /" << endl;
	cout << "     \\                                                          /" << endl;
	cout << "      \\                              " << setw(20) << setfill(' ') << name_player1 << "       /" << endl;
	cout << "       \\______________________________________________________/" << endl << endl;
	cout << "===========================================================================" << endl << endl;
}



void initTabuleiro(int a[6], int b[6], int& c, int& d, string name_player1, string name_player2) {
	//inicializa os arrays correspondentes a cada jogador e mostra pela primeira vez o tabuleiro
	int i;
	for (i = 0; i < 6; i++) {
		a[i] = 4;
		b[i] = 4;
	}

	c = 0;
	d = 0;
	atualizaTabuleiro(a,b,c,d,name_player1,name_player2);
}



void updateArrays_copy(int north[6], int south[6], int north_copy[6], int south_copy[6]) {
	//cópia dos arrays é atualizada para ficarem iguais aos originais
	int i;
	for (i = 0; i < 6; i++) {
		north_copy[i] = north[i];
		south_copy[i] = south[i];
	}
}



void updateArrays(int north[6], int south[6], int north_copy[6], int south_copy[6]) {
	//arrays originais é atualizada para ficarem iguais à sua cópia
	int i;
	for (i = 0; i < 6; i++) {
		north[i] = north_copy[i];
		south[i] = south_copy[i];
	}
}



void getNames(string& name_player1, string& name_player2) {
	//obtem os nomes dos 2 jogadores
	do{
		cout << "Nickname for player 1: (max. of 20 characters) ";
		getline(cin, name_player1);
		if (name_player1.length() > 20 || name_player1.length() == 0)
			cout << YELLOW << "Input invalid because have more than 20 characters. Please try again: " << NO_COLOR;			
	}while (name_player1.length() > 20 || name_player1.length() == 0);
	do{
		cout << "Nickname for player 2: (max. of 20 characters) ";
		getline(cin, name_player2);
		if (name_player2.length() > 20 || name_player2.length() == 0)
			cout << YELLOW << "Input invalid because have more than 20 characters. Please try again: "<<NO_COLOR;			
	}while (name_player2.length() > 20 || name_player2.length() == 0);
}



void getSingleName(string& name_player1) {
	//obtém o nome de um jogador no caso de estar a jogar contra o PC
	do{
		cout << "Nickname for player 1: (max. of 20 characters) ";
		getline(cin, name_player1);
		if (name_player1.length() > 20 || name_player1.length() == 0)
			cout << YELLOW << "Input invalid because have more than 20 characters. Please try again: " << NO_COLOR;			
	}while (name_player1.length() > 20 || name_player1.length() == 0);
}



void wait_on_enter() {
	//espera por um '\n'
	string enter;
    cout << "Enter to continue..." << endl;
    getline(cin, enter);
}



int numberOfhole(char ch) {
	//devolve o número do array associado há letra da sua posição
	if (ch<'a' && ch>='A')
		return int(ch)-int('A');
	else
		return int(ch)-int('a');
}



void lastSum(int north[6], int south[6], int& n_captured_north, int& n_captured_south) {
	//após acabar o jogo por impossibilidade de jogada então soma os elementos restantes no tabuleiro
	int i;
	for (i = 0; i < 6; i++) {
		n_captured_north += north[i];
		n_captured_south += south[i];
		north[i] = 0;
		south[i] = 0;
	}
}



void checkLetterHole(int north[6], int south[6], char& letter_of_hole, int n_of_plays) {
	//verifica se o buraco escolhido não está vazia para começar a jogada
	if (n_of_plays % 2 == 1) {
		while (true) {
			if (int(letter_of_hole) < 65 || int(letter_of_hole) > 70) {
				cout << YELLOW << "The input is invalid. Make sure you choose one of the followings elements: A; B; C; D; E; F" << NO_COLOR << endl;
			}
			else if (south[numberOfhole(letter_of_hole)] == 0) {
				cout << YELLOW << "The input is invalid because the hole is empty. Please, choose another hole." << NO_COLOR << endl;;
			}
			else 
				break;
			cin >> letter_of_hole;
			if (int(cin.peek())!=10 && int(cin.peek())!=13)
				letter_of_hole='X';
			cleanInput();
		}
	}
	else if (n_of_plays % 2 == 0) {
		while (true) {
			if (int(letter_of_hole) < 97 || int(letter_of_hole) > 102) {
				cout << YELLOW << "The input is invalid. Make sure you choose one of the followings elements: a; b; c; d; e; f" << NO_COLOR << endl;
			}
			else if (north[numberOfhole(letter_of_hole)] == 0) {
				cout << YELLOW << "The input is invalid because the hole is empty. Please, choose another hole." << NO_COLOR << endl;;
			}
			else 
				break;
			cin >> letter_of_hole;
			if (int(cin.peek())!=10 && int(cin.peek())!=13)
				letter_of_hole='X';
			cleanInput();
		}
	}
}



bool checkSow(int north_copy[6], int south_copy[6], int n_of_plays) {
	//verifica se a distribuição da jogada pode ser feita, ou seja, se após da distribuição o oponente tem alguma peça
	int i;
	bool check = false;
	if (n_of_plays % 2 == 1) {
		for (i = 0; i < 6; i++) {
			if (north_copy[i] != 0) {
				check = true;
				break;
			}
		}
	}
	else if (n_of_plays % 2 == 0) {
		for (i = 0; i < 6; i++) {
			if (south_copy[i] != 0) {
				check = true;
				break;
			}
		}
	}
	return check;
}



void sowing(int north[6], int south[6], int number_of_hole, int n_of_plays, vector<int> &v_allpositions, int& last_hole) {
	//faz a distribuição das peças contidas no buraco escolhido
	int i, n_sowing, number_of_hole_copy;
	number_of_hole_copy = number_of_hole;
	for (i = 0; i < 12; i++) {
		if (i < 6) {
			v_allpositions.push_back(south[i]);
		}
		else if (i > 5) {
			v_allpositions.push_back(north[i - 6]);
		}
	}
	if (n_of_plays % 2 == 1) {		
		n_sowing = v_allpositions[number_of_hole];
		while (n_sowing > 0) {
			number_of_hole_copy++;
			if (number_of_hole_copy < 12) {
				if (number_of_hole_copy != number_of_hole) {
					v_allpositions[number_of_hole_copy]++;
					n_sowing -= 1;
				}
			}
			else if (number_of_hole_copy == 12) {
				number_of_hole_copy =0;
				if (number_of_hole_copy != number_of_hole) {
					v_allpositions[number_of_hole_copy]++;
					n_sowing -= 1;
				}
			}
		}
	}
	else if (n_of_plays % 2 == 0) {		
		number_of_hole += 6;
		number_of_hole_copy = number_of_hole;
		n_sowing = v_allpositions[number_of_hole];
		while (n_sowing > 0) {
			number_of_hole_copy++;
			if (number_of_hole_copy < 12) {
				if (number_of_hole_copy != number_of_hole) {
					v_allpositions[number_of_hole_copy]++;
					n_sowing -= 1;
				}
			}
			else if (number_of_hole_copy == 12) {
				number_of_hole_copy =0;
				if (number_of_hole_copy != number_of_hole) {
					v_allpositions[number_of_hole_copy]++;
					n_sowing -= 1;
				}
			}
		}
	}
	last_hole = number_of_hole_copy;
	v_allpositions[number_of_hole] = 0;
	for (i = 0; i < 12; i++) {
		if (i < 6) {
			south[i] = v_allpositions[i];
		}
		else if (i > 5) {
			north[i - 6] = v_allpositions[i];
		}
	}
}



bool checkCapture(int north_copy[6], int south_copy[6], int& n_captured_north, int& n_captured_south, int n_of_plays, vector<int> v_allpositions, int last_hole) {
	//faz a captura das peças caso haja e verifica se essa captura se pode realizar
	int sum_north = 0, sum_south = 0, i;
	if (n_of_plays % 2 == 1) {
		for (i = last_hole; i > 5; i -= 1) {
			if (v_allpositions[i] == 2 || v_allpositions[i] == 3) {
				sum_south += v_allpositions[i];
				v_allpositions[i] = 0;
			}
			else break;
		}
		for (i = 0; i < 12; i++) {
			if (i < 6) {
				south_copy[i] = v_allpositions[i];
			}
			else if (i > 5) {
				north_copy[i - 6] = v_allpositions[i];
			}
		}
		for (i = 0; i < 6; i++) {
			if (north_copy[i] != 0) {
				n_captured_south += sum_south;
				return true;
			}
		}
	}
	else if (n_of_plays % 2 == 0) {
		for (i = last_hole; i >= 0; i -= 1) {
			if (v_allpositions[i] == 2 || v_allpositions[i] == 3) {
				sum_north += v_allpositions[i];
				v_allpositions[i] = 0;
			}
			else break;
		}
		for (i = 0; i < 12; i++) {
			if (i < 6) {
				south_copy[i] = v_allpositions[i];
			}
			else if (i > 5) {
				north_copy[i - 6] = v_allpositions[i];
			}
		}
		for (i = 0; i < 6; i++) {
			if (south_copy[i] != 0) {
				n_captured_north += sum_north;
				return true;
			}
		}
	}
	return false;
}



void capture(int north[6], int south[6], int north_copy[6], int south_copy[6], int& n_captured_north, int& n_captured_south, int n_of_plays, vector<int> v_allpositions, int last_hole) {
	//caso se possa realizar a captura, atualiza arrays originais
	if (n_of_plays % 2 == 1) {
		if (last_hole > 5) {
			if (checkCapture(north_copy, south_copy, n_captured_north, n_captured_south, n_of_plays, v_allpositions, last_hole)) {
				updateArrays(north, south, north_copy, south_copy);
			}
		}
	}
	else if (n_of_plays % 2 == 0) {
		if (last_hole < 6) {
			if (checkCapture(north_copy, south_copy, n_captured_north, n_captured_south, n_of_plays, v_allpositions, last_hole)) {
				updateArrays(north, south, north_copy, south_copy);
			}
		}
	}
}



void checkRandNumber(int &number_of_hole, int north[6]) {
	//verifica se o número random pode ser usado, se não, escolhe o número a seguir até o computadorpuder jogar 
	while (true) {
			if (north[number_of_hole] == 0) {
				if (number_of_hole < 5) {
					number_of_hole++;
				}
				else {
					number_of_hole -= 5;
				}
			}
			else break;
	}
}



bool checkNumber(int number_of_hole,int north_copy[6]) {
	//verifica que o buraco escolhido tem alguma peça
	if (north_copy[number_of_hole] == 0) {
		return false;
	}
	return true;
}



int diffCapture(int north_copy2[6], int south_copy2[6], vector<int> v_allpositions, int &last_hole) {
	//devolve a quantidade de peças que uma determinada jogada faz
	int sum_north = 0, i;
	bool check = false;
	if (last_hole < 6) {
		for (i = last_hole; i >= 0; i -= 1) {
			if (v_allpositions[i] == 2 || v_allpositions[i] == 3) {
				sum_north += v_allpositions[i];
				v_allpositions[i] = 0;
			}
			else break;
		}
		for (i = 0; i < 12; i++) {
			if (i < 6) {
				south_copy2[i] = v_allpositions[i];
			}
			else if (i > 5) {
				north_copy2[i - 6] = v_allpositions[i];
			}
		}
		for (i = 0; i < 6; i++) {
			if (south_copy2[i] != 0) {
				check = true;
			}
		}
		if (check == true) {
			return sum_north;
		}
	}
	return 0;
}



int bestPossibility(int north_copy[6], int south_copy[6],int n_of_plays, vector<int> v_allpositions) {
	//escolhe, favorecendo o maior numero de peças capturadas, a melhor jogada que o computador pode realizar.
	//Caso nenhuma capture, a jogada é feita de forma aleatória
	int test, index, biggest = 0, i, number_of_hole, north_copy1[6], south_copy1[6], north_copy2[6], south_copy2[6], last_hole;
	bool check;
	for (i = 0; i < 6; i++) {
		v_allpositions.resize(0);
		updateArrays_copy(north_copy, south_copy, north_copy1, south_copy1);
		updateArrays_copy(north_copy, south_copy, north_copy2, south_copy2);
		number_of_hole = i;
		if (checkNumber(number_of_hole, north_copy) == false) {
			continue;
		}
		sowing(north_copy2, south_copy2, number_of_hole, n_of_plays, v_allpositions, last_hole);
		check = checkSow(north_copy2, south_copy2, n_of_plays);
		if (check == true) {
			updateArrays(north_copy1, south_copy1, north_copy2, south_copy2);
			test = diffCapture(north_copy2, south_copy2, v_allpositions, last_hole);
			if (test >= biggest) {
				biggest = test;
				index = number_of_hole;
			}
		}
	}
	if (biggest == 0) {
		index = rand() % 6;
		checkRandNumber(index, north_copy);
	}
	return index;
}



void turn_player(int north[6], int south[6], int& n_captured_north, int& n_captured_south, int n_of_plays, int north_copy[6], int south_copy[6]) {
	//turno para uma pessoa jogar (junção das funções anteriores)
	bool check;
	char letter_of_hole;
	int number_of_hole, last_hole = 0;
	vector<int> v_allpositions;
	if (n_of_plays % 2 == 1) {
		cout << "What's the hole you choose to this move? (The holes are between 'A' and 'F' as the board shows)" << endl;
	}
	else if (n_of_plays % 2 == 0) {
		cout << "What's the hole you choose to this move? (The holes are between 'a' and 'f' as the board shows)" << endl;
	}
		do {
			cin >> letter_of_hole;
			if (int(cin.peek())!=10 && int(cin.peek())!=13)
				letter_of_hole='X';
			cleanInput();
			checkLetterHole(north, south, letter_of_hole, n_of_plays);
			number_of_hole = numberOfhole(letter_of_hole);
			v_allpositions.resize(0);
			sowing(north_copy, south_copy, number_of_hole, n_of_plays, v_allpositions, last_hole);
			check = checkSow(north_copy, south_copy, n_of_plays);
			if (check == false) {
				cout << YELLOW << "Invalid move: opponent without seeds." << NO_COLOR << endl;
				updateArrays_copy(north, south, north_copy, south_copy);				
			}
		} while (check == false);
		updateArrays(north, south, north_copy, south_copy);
		capture(north, south, north_copy, south_copy, n_captured_north, n_captured_south, n_of_plays, v_allpositions, last_hole);
}



void turn_bot(int north[6], int south[6], int& n_captured_north, int& n_captured_south, int n_of_plays, int north_copy[6], int south_copy[6], char bot_difficulty) {
	//turno para o bot do computador escolhido pelo utilizador jogar
	bool check;
	int number_of_hole, last_hole = 0;
	vector<int> v_allpositions;
	if (bot_difficulty == 'E') {
		do {
			number_of_hole = rand() % 6;
			checkRandNumber(number_of_hole, north);
			sowing(north_copy, south_copy, number_of_hole, n_of_plays, v_allpositions, last_hole);
			check = checkSow(north_copy, south_copy, n_of_plays);
		} while (check == false);
	}
	else if (bot_difficulty == 'H') {
		number_of_hole = bestPossibility(north_copy,south_copy, n_of_plays, v_allpositions);
		updateArrays_copy(north, south, north_copy, south_copy);
		sowing(north_copy, south_copy, number_of_hole, n_of_plays, v_allpositions, last_hole);
	}
	updateArrays(north, south, north_copy, south_copy);
	capture(north, south, north_copy, south_copy, n_captured_north, n_captured_south, n_of_plays, v_allpositions, last_hole);
}	



bool checkPossibilitys(int north[6], int south[6], int north_copy[6], int south_copy[6], int n_of_plays) {
	//verifica se existe alguma possibilidade de movimento.
	int var, i;
	vector<int> v;
	if (n_of_plays % 2 == 1) {
		for (i = 0; i < 6; i++) {
			if (south_copy[i] != 0) {
				sowing(north_copy, south_copy, i, n_of_plays, v, var);
				if (checkSow(north_copy, south_copy, n_of_plays)) {
					return true;
				}
				updateArrays_copy(north, south, north_copy, south_copy);
			}
		}
	}
	else if (n_of_plays % 2 == 0) {
		for (i = 0; i < 6; i++) {
			if (north_copy[i] != 0) {
				sowing(north_copy, south_copy, i, n_of_plays, v, var);
				if (checkSow(north_copy, south_copy, n_of_plays)) {
					return true;
				}
				updateArrays_copy(north, south, north_copy, south_copy);
			}
		}
	}
	return false;
}



void playGame(int north[6], int south[6], int& n_captured_north, int& n_captured_south, string name_player1, string name_player2, bool check_bot, char bot_difficulty) {
	//ocorre durante um jogo inteiro, termina conforme as regras explicitas no ReadMe
	int n_of_plays = 1, north_copy[6], south_copy[6]; 
	char finish_game;
	bool game_over = false;
	string name_player;
	while (game_over == false) {
		updateArrays_copy(north, south, north_copy, south_copy);
		if (n_of_plays % 2 == 1)
			name_player = name_player1;
		else
			name_player = name_player2;

		if (checkPossibilitys(north, south, north_copy, south_copy, n_of_plays) == false) {
				game_over = true;
				lastSum(north, south, n_captured_north, n_captured_south);
				continue;
		}
		updateArrays_copy(north, south, north_copy, south_copy);

		if (n_of_plays % 2 == 1 || (n_of_plays % 2 == 0 && check_bot == false)) {
			cout << "It's your turn, " << name_player << "!" << endl;
			turn_player(north, south, n_captured_north, n_captured_south, n_of_plays, north_copy, south_copy);
	
		}
		else if (n_of_plays % 2 == 0 && check_bot == true) {
			cout << "It's " << name_player2 << " turn!" << endl;
			turn_bot(north, south, n_captured_north, n_captured_south, n_of_plays, north_copy, south_copy, bot_difficulty);
			wait_on_enter();
		}
		cout << "\033[2J\033[H";
		atualizaTabuleiro(north, south, n_captured_north, n_captured_south, name_player1, name_player2);
		if (n_captured_north > 24 || n_captured_south > 24) {
			game_over = true;
		}
		else if (n_captured_north == 24 && n_captured_south == 24) {
			game_over = true;
		}	
		if (n_of_plays >= 60 and n_of_plays % 5 == 0) {
			cout << "Want to end the game? answer with 'y' = yes or 'n' = no " << endl;
			cin >> finish_game;
			if (int(cin.peek())!=10 && int(cin.peek())!=13)
				finish_game='X';
			cleanInput();
			while (int(finish_game) != 121 && int(finish_game) != 110) {
				cout << YELLOW << "The input is invalid. Make sure you choose one of 'y' or 'n' " << NO_COLOR << endl;
				cin >> finish_game;
				if (int(cin.peek())!=10 && int(cin.peek())!=13)
					finish_game='X';
				cleanInput();
			}
			if (finish_game == 'y') {
				game_over = true;
				lastSum(north, south, n_captured_north, n_captured_south);
				continue;
			}
		}
		n_of_plays++;
	}
	cout << "\033[2J\033[H";
	atualizaTabuleiro(north, south, n_captured_north, n_captured_south, name_player1, name_player2);
	if (n_captured_north == n_captured_south) {
		cout << YELLOW << "It's a draw. Well played to you both!" << NO_COLOR << endl << endl;
	}
	else if (n_captured_north > n_captured_south) {
		cout << YELLOW << "The winner is " << name_player2 << " with " << n_captured_north << " points. Well played to you both!" << NO_COLOR << endl << endl;
	}
	else if (n_captured_south > n_captured_north) {
		cout << YELLOW << "The winner is " << name_player1 << " with " << n_captured_south << " points. Well played to you both!" << NO_COLOR << endl << endl;
	}
}



int main() {
	//menu e possibilidades de escolhes para o utilizador escolher o que deseja fazer.
	#ifdef _WIN32
		SetupConsole();
	#endif
	int north[6], south[6], n_captured_north, n_captured_south;
	string name_player1, name_player2;
	char another_game, game_mode = 'x', bot_difficulty = 'x';
	bool check_bot;

	while (game_mode != 'E') {

		cout << "\033[2J\033[H";
		check_bot = false;
		cout << endl << "WELCOME TO OWARE!!" << endl << endl;
		cout << "Choose the game mode you would like play: (answer with A or B)" << endl << endl << "A - Multiplayer (two players)" << endl << "B - Against the computer" << endl << endl << endl;
		cout << "If you want to exit the game answer with 'E'" << endl;
		cin >> game_mode;
		if (int(cin.peek())!=10 && int(cin.peek())!=13)
			game_mode='X';
		cleanInput();
		while  (int(game_mode) != 65 && int(game_mode) != 66 && int(game_mode != 69)) {
				cout << YELLOW << "The input is invalid. Make sure you choose one of 'A' or 'B' to play or 'E' to exit" << NO_COLOR << endl;
				cin >> game_mode;
				if (int(cin.peek())!=10 && int(cin.peek())!=13)
					game_mode='X';
				cleanInput();
			}
			cout << endl;
		if (int(game_mode) == 65) {
			getNames(name_player1, name_player2);
			cout << "\033[2J\033[H";
			do {
				initTabuleiro(north, south, n_captured_north, n_captured_south, name_player1, name_player2);
				cout << "Let the game begin! Good luck!" << endl << endl;
				playGame(north, south, n_captured_north, n_captured_south, name_player1, name_player2, check_bot, bot_difficulty);
				cout << "You want to play again? (answer with 'y' = yes or 'n' = no) " << endl;
				cin >> another_game;
				if (int(cin.peek())!=10 && int(cin.peek())!=13)
					another_game='X';
				cleanInput();
				while (int(another_game) != 121 && int(another_game) != 110) {
					cout << YELLOW << "The input is invalid. Make sure you choose one of 'y', 'n'" << NO_COLOR << endl;
					cin >> another_game;
					if (int(cin.peek())!=10 && int(cin.peek())!=13)
						another_game='X';
					cleanInput();
					
				}
			} while (another_game == 'y');
		}
		if (int(game_mode) == 66) {
			check_bot = true;
			cout << "\033[2J\033[H";
			cout << "Choose the dificulty of the game" << endl << endl << "E - easy" << endl << "H - hard" << endl;
			cin >> bot_difficulty;
			if (int(cin.peek())!=10 && int(cin.peek())!=13)
				bot_difficulty='X';
			cleanInput();
			while (int(bot_difficulty) != 69 && int(bot_difficulty) != 72) {
				cout << YELLOW << "The input is invalid. Make sure you choose one of 'E', 'H'" << NO_COLOR << endl;
				cin >> bot_difficulty;
				if (int(cin.peek())!=10 && int(cin.peek())!=13)
					bot_difficulty='X';
				cleanInput();
			}
			name_player2 = "Computer";
			cout << endl;
			getSingleName(name_player1);
			cout << "\033[2J\033[H";
			do {
				initTabuleiro(north, south, n_captured_north, n_captured_south, name_player1, name_player2);
				cout << "Let the game begin! Good luck!" << endl << endl;
				playGame(north, south, n_captured_north, n_captured_south, name_player1, name_player2, check_bot, bot_difficulty);
				cout << "You want to play again? (answer with 'y' = yes or 'n' = no) " << endl;
				cin >> another_game;
				if (int(cin.peek())!=10 && int(cin.peek())!=13)
					another_game='X';
				cleanInput();
				while (int(another_game) != 121 && int(another_game) != 110) {
					cout << YELLOW << "The input is invalid. Make sure you choose one of 'y', 'n'" << NO_COLOR << endl;
					cin >> another_game;
					if (int(cin.peek())!=10 && int(cin.peek())!=13)
						another_game='X';
					cleanInput();

				}
			} while (another_game == 'y');
		}
	}
}
// Saper.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <Windows.h>
#include <time.h>
#include <vector>
#include "ScreenCapturer.h"

//#define WIDTH 30
//#define HEIGHT 16
#define UNKNOWN '#'
#define FLAG '.'
#define KNOWN ' '
#define PLAYING 'P'
#define GAME_OVER 'O'
#define VICTORY 'V'
//#define START_X 232
//#define START_Y 144
//#define END_X 1134
//#define END_Y 624
//#define CELL_SIZE_X ((double(END_X) - START_X) / WIDTH)
//#define CELL_SIZE_Y ((double(END_Y) - START_Y) / HEIGHT)
#define COLOR_COUNT 12

using namespace std;


//bool /*mines[WIDTH][HEIGHT], */;
bool playfield_invalid = true, help_needed;
char field[99][99], decision[99][99];//, game_state;
float mine_probabilities[99][99], delta_probabilities[99][99];
int digits[99][99], mine_count, width, height;
double start_x, start_y, end_x, end_y, cell_size_x, cell_size_y;
ScreenCapturer screen_capturer;
double colors[COLOR_COUNT][3] = {
	{118, 202, 255}, // unknown
	{159, 220, 62}, // ?
	{227, 163, 60}, // flag
	{253, 253, 253}, // zero
	{210, 235, 241}, // one
	{213, 221, 196}, // two
	{235, 196, 211}, // three
	{192, 206, 231}, // four
	{227, 196, 196}, // five
	{183, 213, 191}, // six
	{222, 204, 222}, // seven
	{241, 200, 179}, // eight
};


struct Coord
{
	int x, y;
};


void for_every_cell(void (*function)(int x, int y, void* data), void* data = NULL)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			function(x, y, data);
		}
	}
}

void for_every_neighbor(int x, int y, void (*function)(int sx, int sy, int nx, int ny, void* data), void* data = NULL)
{
	for (int nx = x - 1; nx <= x + 1; nx++)
	{
		if (nx < 0) continue;
		if (nx >= width) break;
		for (int ny = y - 1; ny <= y + 1; ny++)
		{
			if (ny < 0) continue;
			if (ny >= height) break;
			if (nx == x && ny == y) continue;
			function(x, y, nx, ny, data);
		}
	}
}

void for_every_neighbor2(int x, int y, void (*function)(int sx, int sy, int nx, int ny, void* data), void* data = NULL)
{
	for (int nx = x - 2; nx <= x + 2; nx++)
	{
		if (nx < 0) continue;
		if (nx >= width) break;
		for (int ny = y - 2; ny <= y + 2; ny++)
		{
			if (ny < 0) continue;
			if (ny >= height) break;
			if (nx == x && ny == y) continue;
			function(x, y, nx, ny, data);
		}
	}
}


void goto_xy(int x, int y)
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (SHORT)x, (SHORT)y });
}

/*void _increment_if_mine(int, int, int x, int y, void* data)
{
	if (mines[x][y]) (*(int*)data)++;
}*/

int field_digit(int x, int y)
{
	//int digit = 0;
	//for_every_neighbor(x, y, _increment_if_mine, &digit);
	//return digit;
	return digits[x][y];
}

void draw(int x, int y)
{
	/*if (game_state != PLAYING && mines[x][y])
	{
		if (field[x][y] == KNOWN) cout << 'X';
		else if (field[x][y] == FLAG) cout << '$';
		else cout << '@';
	}
	else*/
	{
		int digit = field_digit(x, y);
		if (field[x][y] == KNOWN && digit) cout << digit;
		else cout << field[x][y];
	}
}

/*void put_mines(int init_x, int init_y)
{
	for (int i = 0; i < MINE_COUNT; i++)
	{
		int x, y;
		do
		{
			x = rand() % WIDTH;
			y = rand() % HEIGHT;
		} while ((x >= init_x - 1 && x <= init_x + 1 && y >= init_y - 1 && y <= init_y + 1) || mines[x][y]);
		mines[x][y] = true;
	}
}*/

/*void click(int, int);

void _click_if_unknown(int, int, int x, int y, void*)
{
	if (field[x][y] == UNKNOWN) click(x, y);
}

/*void _increment_if_unknown_and_empty(int x, int y, void* data)
{
	if (!mines[x][y] && field[x][y] == UNKNOWN) (*(int*)data)++;
}*/

void click(int x, int y)
{
	/*if (first_click)
	{
		//put_mines(x, y);
		first_click = false;
	}
	/*if (field[x][y] == UNKNOWN)
	{
		field[x][y] = KNOWN;
		if (mines[x][y])
		{
			game_state = GAME_OVER;
		}
		else if (field_digit(x, y) == 0)
		{
			for_every_neighbor(x, y, _click_if_unknown);
		}
	}
	int unknown_empty_count = 0;
	for_every_cell(_increment_if_unknown_and_empty, &unknown_empty_count);
	if (unknown_empty_count == 0) game_state = VICTORY;*/


	INPUT Inputs[3] = { 0 };

	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dx = (LONG)((start_x + (x + 0.5) * cell_size_x) * 65536.0 / 1366.0); // desired X coordinate
	Inputs[0].mi.dy = (LONG)((start_y + (y + 0.5) * cell_size_y) * 65536.0 / 768.0); // desired Y coordinate
	Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

	Inputs[1].type = INPUT_MOUSE;
	Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

	Inputs[2].type = INPUT_MOUSE;
	Inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	SendInput(3, Inputs, sizeof(INPUT));
	//screenshot_needed = true;
	//Sleep(100);
}

void put_flag(int x, int y)
{
	//if (click_flags)
	{
		INPUT Inputs[3] = { 0 };

		Inputs[0].type = INPUT_MOUSE;
		Inputs[0].mi.dx = (LONG)((start_x + (x + 0.5) * cell_size_x) * 65536.0 / 1366.0); // desired X coordinate
		Inputs[0].mi.dy = (LONG)((start_y + (y + 0.5) * cell_size_y) * 65536.0 / 768.0); // desired Y coordinate
		Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

		Inputs[1].type = INPUT_MOUSE;
		Inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

		Inputs[2].type = INPUT_MOUSE;
		Inputs[2].mi.dwFlags = MOUSEEVENTF_RIGHTUP;

		SendInput(3, Inputs, sizeof(INPUT));
	}
	//field[x][y] = FLAG;
}

void draw_field()
{
	//goto_xy(0, 0);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			draw(x, y);
		}
		cout << endl;
	}
	cout << endl;
}

void _increment_if_flag(int, int, int x, int y, void* data)
{
	if (field[x][y] == FLAG) (*(int*)data)++;
}

void _increment_if_flag(int x, int y, void* data)
{
	if (field[x][y] == FLAG) (*(int*)data)++;
}

int flags_around(int x, int y)
{
	int flag_count = 0;
	for_every_neighbor(x, y, _increment_if_flag, &flag_count);
	return flag_count;
}

void _increment_if_unknown(int x, int y, void* data)
{
	if (field[x][y] == UNKNOWN) (*(int*)data)++;
}

void _increment_if_unknown(int, int, int x, int y, void* data)
{
	if (field[x][y] == UNKNOWN) (*(int*)data)++;
}

int unknown_around(int x, int y)
{
	int count = 0;
	for_every_neighbor(x, y, _increment_if_unknown, &count);
	return count;
}

void _reset_probability(int x, int y, void* default_probability)
{
	if (field[x][y] == UNKNOWN)
	{
		mine_probabilities[x][y] = *(float*)default_probability;
		delta_probabilities[x][y] = 0.0f;
	}
}

void _sum_probabilities(int, int, int x, int y, void* data)
{
	if (field[x][y] == UNKNOWN)
	{
		*(float*)data += mine_probabilities[x][y];
	}
}

void _sum_probabilities(int x, int y, void* data)
{
	if (field[x][y] == UNKNOWN)
	{
		*(float*)data += mine_probabilities[x][y];
	}
}

void _add_to_probabilities(int, int, int x, int y, void* data)
{
	if (field[x][y] == UNKNOWN)
	{
		delta_probabilities[x][y] += *(float*)data;
	}
}

void _add_to_probabilities(int x, int y, void* data)
{
	if (field[x][y] == UNKNOWN)
	{
		delta_probabilities[x][y] += *(float*)data;
	}
}

void _add_gradient(int x, int y, void*)
{
	if (field[x][y] == KNOWN)
	{
		float probability_sum = 0.f;
		for_every_neighbor(x, y, _sum_probabilities, &probability_sum);
		float delta = ((float)field_digit(x, y) - (probability_sum + (float)flags_around(x, y))) * 0.015625f;
		for_every_neighbor(x, y, _add_to_probabilities, &delta);
	}
}

void _apply_gradient(int x, int y, void*)
{
	if (field[x][y] == UNKNOWN)
	{
		mine_probabilities[x][y] += delta_probabilities[x][y];

		if (mine_probabilities[x][y] > 1.0f) mine_probabilities[x][y] = 1.0f;
		else if (mine_probabilities[x][y] < 0.0f) mine_probabilities[x][y] = 0.0f;

		delta_probabilities[x][y] = 0.0f;
	}
}

void emergency_click()
{
	int flag_count = 0, unknown_cells = 0;
	for_every_cell(_increment_if_flag, &flag_count);
	for_every_cell(_increment_if_unknown, &unknown_cells);
	float average_probability = float(mine_count - flag_count) / (float)unknown_cells;
	for_every_cell(_reset_probability, &average_probability);
	for (int i = 0; i < 10; i++)
	{
		for_every_cell(_add_gradient);
		for_every_cell(_apply_gradient);
		float probability_sum = 0.f;
		for_every_cell(_sum_probabilities, &probability_sum);
		float delta = average_probability - probability_sum / float(unknown_cells);
		for_every_cell(_add_to_probabilities, &delta);
	}
	float best_probability = 9999.0f;
	int choosen_x = -1, choosen_y = -1;
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (field[x][y] == UNKNOWN)
			{
				if (mine_probabilities[x][y] < best_probability)
				{
					best_probability = mine_probabilities[x][y];
					choosen_x = x;
					choosen_y = y;
				}
				else if (mine_probabilities[x][y] == best_probability)
				{
					int distance = min(min(x, width - 1 - x), min(y, height - 1 - y));
					int best_distance = min(min(choosen_x, width - 1 - choosen_x), min(choosen_y, height - 1 - choosen_y));
					if (distance > best_distance)
					{
						best_probability = mine_probabilities[x][y];
						choosen_x = x;
						choosen_y = y;
					}
					else if (distance == best_distance)
					{
						int distance2 = max(min(x, width - 1 - x), min(y, height - 1 - y));
						int best_distance2 = max(min(choosen_x, width - 1 - choosen_x), min(choosen_y, height - 1 - choosen_y));
						if (distance2 > best_distance2)
						{
							best_probability = mine_probabilities[x][y];
							choosen_x = x;
							choosen_y = y;
						}
					}
				}
			}
		}
	}
	if (choosen_x != -1 && choosen_y != -1)
	{
		cout << int(best_probability * 100) << "% RISKY: click at (" << choosen_x << ", " << choosen_y << ")\n";
		decision[choosen_x][choosen_y] = KNOWN;
	}
}

void _click_if_unknown(int, int, int x, int y, void*)
{
	if (field[x][y] == UNKNOWN)
	{
		cout << "basic: click at (" << x << ", " << y << ")\n";
		decision[x][y] = KNOWN;
		help_needed = false;
	}
}

void _flag_if_unknown(int, int, int x, int y, void*)
{
	if (field[x][y] == UNKNOWN)
	{
		cout << "basic: put flag at (" << x << ", " << y << ")\n";
		decision[x][y] = FLAG;
		field[x][y] = FLAG;
	}
}

void _add_if_unknown_and_not_neighbor_front(int, int, int x, int y, void* container)
{
	Coord* front = &((vector<Coord>*)container)->front();
	if (field[x][y] == UNKNOWN && (x < front->x - 1 || x > front->x + 1 || y < front->y - 1 || y > front->y + 1))
	{
		((vector<Coord>*)container)->push_back({ x, y });
	}
}

void _find_actions_on_neightbors(int sx, int sy, int nx, int ny, void*)
{
	if (field[nx][ny] == KNOWN)
	{
		vector<Coord> unknown_neighbors_n_and_not_s;
		unknown_neighbors_n_and_not_s.push_back({ sx, sy });
		for_every_neighbor(nx, ny, _add_if_unknown_and_not_neighbor_front, &unknown_neighbors_n_and_not_s);
		unknown_neighbors_n_and_not_s.erase(unknown_neighbors_n_and_not_s.begin());
		int unknown_mines_delta = field_digit(nx, ny) - flags_around(nx, ny) - (field_digit(sx, sy) - flags_around(sx, sy));
		if (unknown_neighbors_n_and_not_s.size() == unknown_mines_delta)
		{
			for (auto& n : unknown_neighbors_n_and_not_s)
			{
				cout << "advanced: put flag at (" << n.x << ", " << n.y << ")\n";
				decision[n.x][n.y] = FLAG;
				field[n.x][n.y] = FLAG;
			}
			vector<Coord> unknown_neighbors_s_and_not_n;
			unknown_neighbors_s_and_not_n.push_back({ nx, ny });
			for_every_neighbor(sx, sy, _add_if_unknown_and_not_neighbor_front, &unknown_neighbors_s_and_not_n);
			unknown_neighbors_s_and_not_n.erase(unknown_neighbors_s_and_not_n.begin());
			for (auto& n : unknown_neighbors_s_and_not_n)
			{
				cout << "advanced: click at (" << n.x << ", " << n.y << ")\n";
				decision[n.x][n.y] = KNOWN;
				help_needed = false;
			}
		}
	}
}

void _find_actions_on_cell(int x, int y, void*)
{
	if (field[x][y] == KNOWN)
	{
		int digit = field_digit(x, y);
		int flags = flags_around(x, y);
		int unknown = unknown_around(x, y);
		if (digit == flags)
		{
			for_every_neighbor(x, y, _click_if_unknown);
		}
		if (digit == flags + unknown)
		{
			for_every_neighbor(x, y, _flag_if_unknown);
		}
		for_every_neighbor(x, y, _find_actions_on_neightbors);
	}
}

void find_actions()
{
	int unknown_count = 0;
	for_every_cell(_increment_if_unknown, &unknown_count);
	if (unknown_count == width * height)
	{
		cout << "first click at the middle: (" << ((width - 1) / 2) << ", " << ((height - 1) / 2) << ")\n";
		decision[(width - 1) / 2][(height - 1) / 2] = KNOWN;
		return;
	}
	help_needed = true;
	for_every_cell(_find_actions_on_cell);
	if (help_needed) emergency_click();
}

void _read_cell_from_screen(int x, int y, void*)
{
	if (playfield_invalid == false)
	{
		int px_start_x = int(start_x + (x + 0.25) * cell_size_x);
		int px_start_y = int(start_y + (y + 0.25) * cell_size_y);
		int px_end_x = int(start_x + (x + 0.75) * cell_size_x);
		int px_end_y = int(start_y + (y + 0.75) * cell_size_y);
		int count = (px_end_x - px_start_x) * (px_end_y - px_start_y);
		double r = 0.0, g = 0.0, b = 0.0;
		for (int ty = px_start_y; ty < px_end_y; ty++)
		{
			for (int tx = px_start_x; tx < px_end_x; tx++)
			{
				RGBQUAD pixel = screen_capturer.pixel(tx, ty);
				r += pixel.rgbRed;
				g += pixel.rgbGreen;
				b += pixel.rgbBlue;
			}
		}
		r /= count;
		g /= count;
		b /= count;
		int best_index = -1;
		double difference;
		for (int i = 0; i < COLOR_COUNT; i++)
		{
			double new_difference = pow(r - colors[i][0], 2.0) + pow(g - colors[i][1], 2.0) + pow(b - colors[i][2], 2.0);
			if (best_index == -1 || new_difference < difference)
			{
				best_index = i;
				difference = new_difference;
			}
		}
		if (difference > 256)
		{
			playfield_invalid = true;
		}
		else
		{
			if (best_index <= 1) field[x][y] = UNKNOWN;
			else if (best_index == 2) field[x][y] = FLAG;
			else
			{
				field[x][y] = KNOWN;
				digits[x][y] = best_index - 3;
			}
		}
	}
}

void try_get_playfield_coords()
{
	/*static double row_sum_b[768], column_sum_b[1366], total_b = 0.0;
	memset(row_sum_b, 0, sizeof(double) * 768);
	memset(column_sum_b, 0, sizeof(double) * 1366);
	for (int y = 0; y < 768; y++)
	{
		for (int x = 0; x < 1366; x++)
		{
			double b = screen_capturer.pixel(x, y).rgbBlue;
			row_sum_b[y] += b;
			column_sum_b[x] += b;
			total_b += b;
		}
	}
	for (int x = 0; x < 1366; x++)
	{
		if (column_sum_b[x] > total_b / 1366)
		{
			start_x = x - 2.0;
			break;
		}
	}
	for (int x = 1365; x >= 0; x--)
	{
		if (column_sum_b[x] > total_b / 1366)
		{
			end_x = x + 2.0;
			break;
		}
	}
	for (int y = 50; y < 768; y++)
	{
		if (row_sum_b[y] > total_b / 768)
		{
			start_y = y - 2.0;
			break;
		}
	}
	for (int y = 737; y >= 0; y--)
	{
		if (row_sum_b[y] > total_b / 768)
		{
			end_y = y + 2.0;
			break;
		}
	}*/
	width = 30;
	height = 16;
	start_x = 232.0;
	start_y = 144.0;
	end_x = 1134.0;
	end_y = 624.0;
	cell_size_x = (end_x - start_x) / width;
	cell_size_y = (end_y - start_y) / height;
	playfield_invalid = false;
	//cout << "Playfield detected at (" << (int)start_x << ", " << (int)start_y << "), (" << (int)end_x << ", " << (int)end_y << ")\n";
}

void read_from_screen()
{
	screen_capturer.takeScreenshot();
	for_every_cell(_read_cell_from_screen);
	bool detected_info = false;
	while (playfield_invalid)
	{
		if (!detected_info) cout << "\nWaiting for playfield...\n";
		detected_info = true;
		screen_capturer.takeScreenshot();
		try_get_playfield_coords();
		for_every_cell(_read_cell_from_screen);
	}
	if (detected_info)
	{
		cout << "\nPlayfield detected at (" << (int)start_x << ", " << (int)start_y << "), (" << (int)end_x << ", " << (int)end_y << ")\n";
		cout << "Size: (" << width << ", " << height << "), mines: " << mine_count << "\n\n";
	}
}

void _decide_to_perform_click(int x, int y, void*)
{
	if (decision[x][y] == KNOWN) click(x, y);
}

void _decide_to_put_flag(int x, int y, void*)
{
	if (decision[x][y] == FLAG) put_flag(x, y);
}

void perform_actions()
{
	for_every_cell(_decide_to_perform_click);
	for_every_cell(_decide_to_put_flag);
	Sleep(200);
}

int main()
{
	/*HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = false; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);

	srand((unsigned)time(0));*/

	memset(field, UNKNOWN, 99 * 99);
	//memset(mines, false, WIDTH * HEIGHT);
	//game_state = PLAYING;
	//system("PAUSE");
	cout << "Enter mine count\n";
	cin >> mine_count;
	while (true)
	{
		read_from_screen();
		draw_field();
		memset(decision, UNKNOWN, 99 * 99);
		find_actions();
		perform_actions();
		cout << "\n\n";
		/*draw_field();
		//Sleep(3000);
		perform_action();
		if (game_state == GAME_OVER)
		{
			draw_field();
			cout << "game over!\n";
			break;
		}
		else if (game_state == VICTORY)
		{
			draw_field();
			cout << "victory!\n";
			break;
		}*/
	}
}
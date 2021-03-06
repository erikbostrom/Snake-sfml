#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <sstream>
#include <vector>
#include <string>
#include <iterator>

#define MAX_LEVEL 5
#define M 25
#define N 25
#define M_bg 200
#define N_bg 128
#define WIN_WIDTH 1600
#define WIN_HEIGHT 1024

using namespace sf;
using namespace std;


// Global variables
int block_size_x = 32;
int block_size_y = 32;
int bg_block_size_x = 8;
int bg_block_size_y = 8;


string name="                    ";
string input;

int dir;


struct Parameter
{
  int level, points;
  float level_time[MAX_LEVEL], level_speed[MAX_LEVEL];
  float timer, timer2, tot_time, new_level_delay, start_time;

  void init()
  {
    level    = 0;
    points   = 0;
    timer    = 0;
    timer2   = 0;
    tot_time = 0;
    new_level_delay = 2;

    for(int i=0; i<MAX_LEVEL; i++)
      level_time[i] = 20*(1+0.8*i);
   

    for(int i=0; i<MAX_LEVEL; i++)
      level_speed[i] = 0.15*pow(0.8,i);
  }  
};


struct Flag
{
  bool game_over;
  bool processing_writing;
  bool writing;
  bool f;
  bool new_level;
  bool start;
  bool show_highscore;
  bool startup_screen;

  void init()
  {
    game_over=false; 
    processing_writing=false;
    writing=false;
    new_level=false;
    start=false;
    show_highscore=false;
    startup_screen=true;
    f=false;
  }
  
  void reset()
  {
    game_over=false; 
    processing_writing=false;
    writing=false;
    new_level=false;
    start=false;
    show_highscore=false;
    startup_screen=false;
    f=false;
  }
};


struct Fruit
{
  int x;
  int y;
  int value;
  float time;

  void init()
  {
    x=10;
    y=10;
    value = 5;
  }

  void new_location()
  {
    x = rand() % N;
    y = rand() % M;
  }

  void reset_value()
  {
    value=5;
  }
  
  void reset_timer()
  {
    time = 0;
  }
  void point_value(float time_interval)
  {
    time += time_interval;
    for (int i=0; i<5; i++)
      {
	if(time > 2*i && time < 2*(i+1) )
	  value = 5-i;
      }
  }

};


int center_rect_x(int tot_w, int rect_w)
{
  return (tot_w-rect_w)/2;
}


// Put the two below in a struct!!

Vector2f center_point_rect(RectangleShape rectangle)
{
  FloatRect rect_bounds = rectangle.getLocalBounds();
  return Vector2f(rect_bounds.left + rect_bounds.width/2.0f, rect_bounds.top + rect_bounds.height/2.0f);
}

template<typename T>
Vector2f center(T rect_shape)
{
  FloatRect rect_bounds = rect_shape.getLocalBounds();
  return Vector2f(rect_bounds.left + rect_bounds.width/2.0f, rect_bounds.top + rect_bounds.height/2.0f);
}


class Snake
{
private:
  
public:

  int x[100];
  int y[100];

  int length;
  int head_dir[2];

  Snake(){};
  
  void init()
  {
    head_dir[0]=2;
    head_dir[1]=2;
    length=4;
    x[0]=2;
    x[1]=2;
    x[2]=2;
    x[3]=2;
    y[0]=5;
    y[1]=4;
    y[2]=3;
    y[3]=2;    
  }
  
  void move_one_box()
  {

    // Update body location
    for (int i=length; i>0; --i)
      {
	x[i] = x[i-1];
	y[i] = y[i-1];
      }

    if(abs(head_dir[0]-head_dir[1])==2) head_dir[0]=head_dir[1];    
    
    // Update head location
    if (head_dir[0]==0) y[0] -= 1;  
    if (head_dir[0]==1) x[0] -= 1;  
    if (head_dir[0]==2) y[0] += 1;         
    if (head_dir[0]==3) x[0] += 1;

    head_dir[1]=head_dir[0];

    // The domain is a torus.
    if (x[0]>N-1) x[0]=0;
    if (x[0]<0  ) x[0]=N-1;
    if (y[0]>M-1) y[0]=0;
    if (y[0]<0  ) y[0]=M-1;
    
  }

  bool on_snake(int fx, int fy)
  {
    bool temp;
    for (int i=0; i<length; i++)
      {
	if(x[i]==fx && y[i]==fy)
	  {
	    return true;
	  }
	else
	  temp = false;
      }
    return temp;
  }
  
  bool on_snake_body(int fx, int fy)
  {
    bool temp;
    for (int i=1; i<length; i++)
      {
	if( x[i]==fx && y[i]==fy )
	  {
	    return true;
	  }
	else
	  temp = false;
      }
    return temp;
  }

  bool eat(int fx, int fy, int value)
  {
    if (x[0]==fx && y[0]==fy)
      {
	for (int i=0; i<value; i++)
	  {
	    x[length+i]=x[length+i-1];
	    y[length+i]=y[length+i-1];
	  }
	length = length + value;
	return true;
      }
    else
      return false;
  }

  bool crash()
  {
    if(on_snake_body(x[0],y[0]))
      {
	return true;
      }
    else
      return false;
  }
};
  

bool point_in_rect(Vector2i point2d, RectangleShape rectangle)
{
  Text text;
  bool result;
  Vector2f rectangle_position = rectangle.getPosition();
  FloatRect rectangle_bounds  = rectangle.getLocalBounds();
  
  int xmin = rectangle_position.x - rectangle_bounds.width/2.0;
  int xmax = rectangle_position.x + rectangle_bounds.width/2.0;
  int ymin = rectangle_position.y - rectangle_bounds.height/2.0;
  int ymax = rectangle_position.y + rectangle_bounds.height/2.0;
  
  if (point2d.x > xmin && point2d.x < xmax && point2d.y > ymin && point2d.y < ymax)
    {
      result = true;
    }
  else
    {
      result = false;
    }

  return result;
}



class highscore
{
private:
  std::fstream& file;
  std::string line;
  
public:

  highscore(std::fstream& f) : file{f} {}
  ~highscore(){}


  std::string read_line(int nr)
  {
    for(int i=0; i<nr; i++)
      {
	std::getline(file,line);
      }
    file.clear();
    file.seekg(0,std::ios::beg);
    return line;
  }

  int get_score_from_line(std::string line)
  {
    std::string spart;
    int score;
    spart = line.substr(20,25);
    score = std::stoi( spart );
    return score;
  }

  double get_time_from_line(std::string line)
  {
    std::string spart;
    double time;
    spart = line.substr(25,30);
    time = std::stod( spart );
    return time;
  }
  
  void add(std::string name, int score, float time)
  {
    std::string s, spart, line;
    int points_line;
    double time_line;
    std::vector<std::string> vecOfStrs;

    int length = name.size();
    std::string new_str(40, ' ');
    new_str.insert(0,name);
    new_str.insert(20,to_string(score));
    new_str.insert(30,to_string(time));

    file.clear();
    while (std::getline(file, line))
      {
	if (line.size() > 0)
	  {
	    points_line = get_score_from_line(line);
	    time_line   = get_time_from_line(line);
	    if (score >= points_line)
	      {
		
		if( score > points_line ||
		    (score == points_line && time < time_line))
		  {
		    vecOfStrs.push_back(new_str);
		    vecOfStrs.push_back(line);
		    score = 0;
		  }
		else
		  {
		    vecOfStrs.push_back(line);
		    cout << line << endl;
		  }
	      }
	    else{
	      vecOfStrs.push_back(line);
	    }
	  }
      }
    file.clear();
    file.seekg(0,std::ios::beg);
    
    std::ostream_iterator<std::string> output_iterator(file, "\n");
    std::copy(vecOfStrs.begin(), vecOfStrs.end(), output_iterator);
  }
  
  std::string last_string()
  {
    for(int i=0; i<10; i++) std::getline (file,line);
    file.clear();
    file.seekg(0,std::ios::beg);
    return line;
  }

  int last_points()
  {
    std::string str = last_string();
    return get_score_from_line(str);
  }

};


int main()
{

  /////////////////////////////////////////////////////////
  // INITIALIZATION                                      //
  /////////////////////////////////////////////////////////

  Clock clock;
  Event event;
  Text text;  
  string time_str, score_str;
  fstream file;
  Snake snake;
  Fruit fruit;
  Flag flag;
  Parameter par;
  
  snake.init();
  fruit.init();
  flag.init();
  par.init();
  
  // Open a window
  RenderWindow window(VideoMode(WIN_WIDTH, WIN_HEIGHT), "Snake");

  // Open highscore file
  file.open ("highscore.txt");		  		      
  highscore hs(file);
  
  
  //  start=true;
  //startup_screen=false;
  //game_over = true;
  //points = 9;
  //time_str = "Time: 500.1";
  //tot_time = 500.1;
  

  // Textures
  Texture t1_64, t1_128, snake_sprite, snake_body, snake_body_64, snake_body_128,
    snake_head_up, snake_head_left, snake_head_right, snake_head_right_64,
    snake_head_right_128, snake_head_down, snake_head_down_64, t1, t2, t3,
    points1,points2,points3,points4,points5,
    bg_texture_0, bg_texture_1, bg_texture_2, bg_texture_3, bg_frame,
    bg_texture_64_1,bg_texture_64_2,bg_texture_64_3,bg_texture_64_4,bg_texture_64_5,
    bg_frame_lu_corner, bg_frame_ld_corner, bg_frame_ru_corner,
    bg_frame_rd_corner, bg_frame_left, bg_frame_right, bg_frame_up,
    bg_frame_down, bg_frame_inner_lu_corner, bg_frame_inner_ru_corner,
    bg_frame_inner_ld_corner, bg_frame_inner_rd_corner, bg_frame_inner,
    bg_frame_rarrow, bg_frame_larrow, bg_frame_larrow2, bg_frame_uarrow,
    bg_frame_darrow, one_128, one_64, button_292x72;


  // Open sprites
  snake_sprite.loadFromFile("images/snake_BW_600.png");
  t1.loadFromFile("images/gray_bg_32x32.png");
  t1_64.loadFromFile("images/gray_bg_64x64.png");
  t1_128.loadFromFile("images/gray_bg_128x128.png");
  snake_body.loadFromFile("images/Snake_body_32x32.png");
  snake_body_64.loadFromFile("images/Snake_body_64x64.png");
  snake_body_128.loadFromFile("images/Snake_body_128x128.png");
  snake_head_up.loadFromFile("images/Snake_head_up_32x32.png");
  snake_head_left.loadFromFile("images/Snake_head_left_32x32.png");
  snake_head_right.loadFromFile("images/Snake_head_right_32x32.png");
  snake_head_right_64.loadFromFile("images/Snake_head_right_64x64.png");
  snake_head_right_128.loadFromFile("images/Snake_head_right_128x128.png");
  snake_head_down.loadFromFile("images/Snake_head_down_32x32.png");
  snake_head_down_64.loadFromFile("images/Snake_head_down_64x64.png");  
  points1.loadFromFile("images/one_point_32x32.png");
  points2.loadFromFile("images/two_points_32x32.png");
  points3.loadFromFile("images/three_points_32x32.png");
  points4.loadFromFile("images/four_points_32x32.png");
  points5.loadFromFile("images/five_points_32x32.png");
  one_64.loadFromFile("images/one_point_64x64.png");
  one_128.loadFromFile("images/one_point_128x128.png");
  bg_frame_lu_corner.loadFromFile("images/frame_lu-corner_8x8.png");
  bg_frame_inner_lu_corner.loadFromFile("images/frame_inner_lu-corner_8x8.png");
  bg_frame_ld_corner.loadFromFile("images/frame_ld-corner_8x8.png");
  bg_frame_inner_ld_corner.loadFromFile("images/frame_inner_ld-corner_8x8.png");
  bg_frame_ru_corner.loadFromFile("images/frame_ru-corner_8x8.png");
  bg_frame_inner_ru_corner.loadFromFile("images/frame_inner_ru-corner_8x8.png");
  bg_frame_rd_corner.loadFromFile("images/frame_rd-corner_8x8.png");
  bg_frame_inner_rd_corner.loadFromFile("images/frame_inner_rd-corner_8x8.png");
  bg_frame_left.loadFromFile("images/frame_left_8x8.png");
  bg_frame_right.loadFromFile("images/frame_right_8x8.png");
  bg_frame_up.loadFromFile("images/frame_up_8x8.png");
  bg_frame_down.loadFromFile("images/frame_down_8x8.png");
  bg_frame_inner.loadFromFile("images/frame_inner_8x8.png");
  bg_frame_rarrow.loadFromFile("images/frame_rarrow_16x8.png");
  bg_frame_larrow.loadFromFile("images/frame_larrow_16x8.png");
  bg_frame_uarrow.loadFromFile("images/frame_uarrow_8x16.png");
  bg_frame_darrow.loadFromFile("images/frame_darrow_8x16.png");
  bg_texture_0.loadFromFile("images/bg_0_8x8.png");
  bg_texture_1.loadFromFile("images/bg_1_8x8.png");
  bg_texture_2.loadFromFile("images/bg_2_8x8.png");
  bg_texture_3.loadFromFile("images/bg_3_8x8.png");
  bg_texture_64_1.loadFromFile("images/bg_64x64_1.png");
  bg_texture_64_2.loadFromFile("images/bg_64x64_2.png");
  bg_texture_64_3.loadFromFile("images/bg_64x64_3.png");
  bg_texture_64_4.loadFromFile("images/bg_64x64_4.png");
  bg_texture_64_5.loadFromFile("images/bg_64x64_5.png");
  button_292x72.loadFromFile("images/gray_button_292x72.png");

  // Declare sprites
  Sprite background_img(t1);
  Sprite background_64_img(t1_64);   
  Sprite background_128_img(t1_128);
  Sprite snake_body_img(snake_body);
  Sprite snake_body_64_img(snake_body_64);  
  Sprite snake_body_128_img(snake_body_128);
  Sprite snake_head_up_img(snake_head_up);
  Sprite snake_head_left_img(snake_head_left);
  Sprite snake_head_right_img(snake_head_right);
  Sprite snake_head_right_64_img(snake_head_right_64);
  Sprite snake_head_right_128_img(snake_head_right_128);
  Sprite snake_head_down_img(snake_head_down);
  Sprite snake_head_down_64_img(snake_head_down_64);
  Sprite fruit1_img(points1);
  Sprite fruit2_img(points2);
  Sprite fruit3_img(points3);
  Sprite fruit4_img(points4);
  Sprite fruit5_img(points5);
  Sprite one_64_img(one_64);
  Sprite one_128_img(one_128);
  Sprite bg_frame_left_img(bg_frame_left);
  Sprite bg_frame_right_img(bg_frame_right);
  Sprite bg_frame_up_img(bg_frame_up);
  Sprite bg_frame_down_img(bg_frame_down);
  Sprite bg_frame_lu_corner_img(bg_frame_lu_corner);
  Sprite bg_frame_inner_lu_corner_img(bg_frame_inner_lu_corner);
  Sprite bg_frame_ld_corner_img(bg_frame_ld_corner);
  Sprite bg_frame_inner_ld_corner_img(bg_frame_inner_ld_corner);
  Sprite bg_frame_ru_corner_img(bg_frame_ru_corner);
  Sprite bg_frame_inner_ru_corner_img(bg_frame_inner_ru_corner);
  Sprite bg_frame_rd_corner_img(bg_frame_rd_corner);
  Sprite bg_frame_inner_rd_corner_img(bg_frame_inner_rd_corner);
  Sprite bg_frame_inner_img(bg_frame_inner);
  Sprite bg_frame_rarrow_img(bg_frame_rarrow);
  Sprite bg_frame_larrow_img(bg_frame_larrow);
  Sprite bg_frame_uarrow_img(bg_frame_uarrow);
  Sprite bg_frame_darrow_img(bg_frame_darrow);
  Sprite bg_img_0(bg_texture_0);
  Sprite bg_img_1(bg_texture_1);
  Sprite bg_img_2(bg_texture_2);
  Sprite bg_img_3(bg_texture_3);
  Sprite bg_64_1(bg_texture_64_1);
  Sprite bg_64_2(bg_texture_64_2);
  Sprite bg_64_3(bg_texture_64_3);
  Sprite bg_64_4(bg_texture_64_4);
  Sprite bg_64_5(bg_texture_64_5);
  Sprite bg_frame_img(bg_frame);
  Sprite snake_frontpage(snake_sprite);
  Sprite button_292x72_img(button_292x72);
  
  
  // Rectangles

  int button_width  = 292, button_height = 72; 
  RectangleShape rect1(Vector2f(button_width, button_height));
  RectangleShape rect2(Vector2f(button_width, button_height));
  RectangleShape rect3(Vector2f(button_width, button_height));
  RectangleShape rect4(Vector2f(button_width, button_height));
  RectangleShape line(Vector2f(WIN_WIDTH*0.8, 5));
  RectangleShape cursor(Vector2f(16, 48));
  RectangleShape hline(Vector2f(WIN_WIDTH*0.6, 4));
  
  rect1.setOrigin(center<RectangleShape>(rect1));
  rect1.setPosition(Vector2f(WIN_WIDTH/2.0, WIN_HEIGHT*(2.5/6.0)));
  rect1.setFillColor(sf::Color(192, 192, 192));
  rect1.setOutlineThickness(1);
  rect1.setOutlineColor(Color::Black);
  
  rect2.setOrigin(center<RectangleShape>(rect2));
  rect2.setPosition(Vector2f(WIN_WIDTH/2.0, WIN_HEIGHT*(3.5/6.0)));
  rect2.setFillColor(sf::Color(192, 192, 192));
  rect2.setOutlineThickness(1);
  rect2.setOutlineColor(Color::Black);
  
  rect3.setOrigin(center<RectangleShape>(rect3));
  rect3.setPosition(Vector2f(WIN_WIDTH/2.0, WIN_HEIGHT*(4.5/6.0)));
  rect3.setFillColor(sf::Color(192, 192, 192));
  rect3.setOutlineThickness(1);
  rect3.setOutlineColor(Color::Black);

  rect4.setOrigin(center<RectangleShape>(rect3));
  rect4.setPosition(Vector2f(WIN_WIDTH*0.1, WIN_HEIGHT*0.05));
  rect4.setFillColor(sf::Color(192, 192, 192));
  rect4.setOutlineThickness(1);
  rect4.setOutlineColor(Color::Black);

  line.setOrigin(center<RectangleShape>(line));
  line.setPosition(Vector2f(WIN_WIDTH/2.0, WIN_HEIGHT*(1.5/6.0)));  
  line.setFillColor(sf::Color(255, 255, 255));

  cursor.setFillColor(sf::Color(102, 255, 51));

  snake_frontpage.setPosition(WIN_WIDTH*0.2, WIN_HEIGHT*0.6);


  // Buttons
  button_292x72_img.setOrigin(center<RectangleShape>(rect1));
  
  // Fonts  
  Font font_menu, font_buttons, font_level_text, score_font, Game_over_font,
    Game_over_font2, frontp_font, frontp_font_2, title_font, body_font;
  font_menu.loadFromFile("fonts/SuperMarioBros.ttf");
  font_buttons.loadFromFile("fonts/DejaVuSans.ttf");
  font_level_text.loadFromFile("fonts/DejaVuSans-Bold.ttf");
  frontp_font.loadFromFile("fonts/SuperMarioBros.ttf");
  frontp_font_2.loadFromFile("fonts/DejaVuSans.ttf");
  score_font.loadFromFile("fonts/DejaVuSans-Bold.ttf");
  Game_over_font.loadFromFile("fonts/DejaVuSerif-Bold.ttf");
  title_font.loadFromFile("fonts/DejaVuSerif-Bold.ttf");
  body_font.loadFromFile("fonts/DejaVuSerif-Bold.ttf");
  Game_over_font2.loadFromFile("fonts/DejaVuSerif.ttf");
  
  // Text initialization

  Text menu_text, text_rect1, text_rect2, text_rect3, text_rect4, level_text, score_text,
    time_text, Game_over_text, final_result_text, text_input, text_input_info,
    frontp_text_1, frontp_text_2, title_text, body_text;

  frontp_text_1.setFont(frontp_font);
  frontp_text_1.setString("SNAKE-sfml");
  frontp_text_1.setCharacterSize(180);
  frontp_text_1.setOrigin(center<Text>(frontp_text_1));
  frontp_text_1.setPosition(Vector2f(WIN_WIDTH/2.0, 272));
  frontp_text_1.setColor(Color::White);

  frontp_text_2.setFont(frontp_font_2);
  frontp_text_2.setOrigin(Vector2f(0,0));
  frontp_text_2.setString("Erik Bostrom, 2018");
  frontp_text_2.setCharacterSize(28);
  frontp_text_2.setPosition(Vector2f(128*2-96+32, 128*3+64));
  frontp_text_2.setColor(Color::White);	   
  
  menu_text.setFont(font_menu);
  menu_text.setString("SNAKE-SFML");
  menu_text.setCharacterSize(80);
  menu_text.setOrigin(center<Text>(menu_text));
  menu_text.setPosition(Vector2f(WIN_WIDTH/2.0f, 128+72));
  menu_text.setColor(Color::White);

  text_rect1.setFont(font_buttons);
  text_rect1.setString("Start");
  text_rect1.setCharacterSize(36);
  text_rect1.setOrigin(center<Text>(text_rect1));
  text_rect1.setColor(Color::Black);
  text_rect1.setPosition(Vector2f(WIN_WIDTH/2.0f, WIN_HEIGHT*(2.5/6.0)));

  text_rect2.setFont(font_buttons);
  text_rect2.setString("High-Score List");
  text_rect2.setCharacterSize(36);
  text_rect2.setOrigin(center<Text>(text_rect2));
  text_rect2.setPosition(Vector2f(WIN_WIDTH/2.0f, WIN_HEIGHT*(3.5/6.0)));
  text_rect2.setColor(Color::Black);

  text_rect3.setFont(font_buttons);
  text_rect3.setString("Help");
  text_rect3.setCharacterSize(36);
  text_rect3.setOrigin(center<Text>(text_rect3));
  text_rect3.setPosition(Vector2f(WIN_WIDTH/2.0f, WIN_HEIGHT*(4.5/6.0)));
  text_rect3.setColor(Color::Black);

  text_rect4.setFont(font_buttons);
  text_rect4.setString("To Menu");
  text_rect4.setCharacterSize(36);
  text_rect4.setOrigin(center<Text>(text_rect4));
  text_rect4.setPosition(Vector2f(WIN_WIDTH*0.1, WIN_HEIGHT*0.05));
  text_rect4.setColor(Color::Black);

  score_text.setFont(score_font);  
  score_text.setCharacterSize(28);
  score_text.setColor(Color::Black);

  time_text.setFont(score_font);
  time_text.setCharacterSize(28);
  time_text.setColor(Color::Black);

  Game_over_text.setFont(Game_over_font);
  Game_over_text.setString("GAME OVER!");
  Game_over_text.setCharacterSize(70);
  Game_over_text.setOrigin(center<Text>(Game_over_text));
  Game_over_text.setColor(Color::White);

  final_result_text.setFont(Game_over_font2);
  final_result_text.setColor(Color::White);
  final_result_text.setCharacterSize(48);

  text_input_info.setFont(Game_over_font2);
  text_input_info.setCharacterSize(48);
  text_input_info.setColor(Color::White); 
  
  text_input.setFont(Game_over_font2);
  text_input.setCharacterSize(48);
  text_input.setColor(Color::Black);

  level_text.setFont(font_level_text);
  level_text.setCharacterSize(60);    

  body_text.setFont(body_font);
  body_text.setCharacterSize(36);
  body_text.setColor(Color::Black);
  
  title_text.setFont(body_font);
  title_text.setCharacterSize(60);
  title_text.setString("Top 10");
  title_text.setOrigin(center<Text>(title_text));
  title_text.setPosition(Vector2f(WIN_WIDTH/2.0, WIN_HEIGHT*0.2));
  title_text.setColor(Color::Black);



  
  /////////////////////////////////////////////////////////
  // MAIN LOOP                                           //
  /////////////////////////////////////////////////////////
  

  srand(time(0));
  while (window.isOpen())
    {

      while (window.pollEvent(event))
	{
	  if (event.type == sf::Event::Closed) window.close();

	  if (flag.processing_writing)
	    {
	      if (event.type == sf::Event::TextEntered)
		{
		  if(event.text.unicode == '\b')
		    {
		      input.erase(input.size()-1,1);
		    }
		  else if (event.text.unicode == 13)
		    {
		      flag.f = true;
		    }
		  else
			
		    {
		      input += event.text.unicode;
		    }
		  text_input.setString(input);
		  text_input.setPosition(Vector2f(WIN_WIDTH*0.15+16, WIN_HEIGHT*0.8+8));

		}
	    }
	}      
      
      if (flag.startup_screen)
	{
	  
	  window.clear(Color::Black);
	  for(int i=0; i<WIN_WIDTH/128+2; i++)
	    for(int j=0; j<WIN_HEIGHT/128; j++)
	      {
		if(!((i>1 && i<12) && (j>0 && j<4)))
		  {
		    background_128_img.setPosition(128*i-96, 128*j);
		    window.draw(background_128_img);
		  }
	      }
	  snake_body_128_img.setPosition(128*3-96, 128*4);
	  window.draw(snake_body_128_img);	  	  	  
	  snake_body_128_img.setPosition(128*3-96, 128*5);
	  window.draw(snake_body_128_img);	  	  
	  snake_body_128_img.setPosition(128*4-96, 128*5);
	  window.draw(snake_body_128_img);	  
	  snake_body_128_img.setPosition(128*4-96, 128*6);
	  window.draw(snake_body_128_img);	  
	  snake_body_128_img.setPosition(128*5-96, 128*6);
	  window.draw(snake_body_128_img);
	  snake_body_128_img.setPosition(128*6-96, 128*6);
	  window.draw(snake_body_128_img);
	  snake_body_128_img.setPosition(128*7-96, 128*6);
	  window.draw(snake_body_128_img);
	  snake_head_right_128_img.setPosition(128*8-96, 128*6);
	  window.draw(snake_head_right_128_img);
	  one_128_img.setPosition(128*10-96, 128*6);
	  window.draw(one_128_img);
	  
	  window.draw(frontp_text_1);
	  window.draw(frontp_text_2);
	  
	  float time1 = clock.getElapsedTime().asSeconds();	  
	  window.display();	  
	  if (time1>5.0) flag.startup_screen=false;
	}
      if (!flag.startup_screen && !flag.new_level && !flag.start && !flag.show_highscore)
	{
	  window.clear(Color::Black);	  
  
	  for(int i=0; i<WIN_WIDTH/64; i++)
	    for(int j=0; j<WIN_HEIGHT/64; j++)
	      {
		if(!((i>5 && i<19) && (j>1 && j<14)))
		  {
		    background_64_img.setPosition(64*i, 64*j);
		    window.draw(background_64_img);
		  }
	      }
  
	  snake_body_64_img.setPosition(64*1, 64*2);
	  window.draw(snake_body_64_img);	  	  	  
	  snake_body_64_img.setPosition(64*1, 64*3);
	  window.draw(snake_body_64_img);	  	  
	  snake_body_64_img.setPosition(64*1, 64*4);
	  window.draw(snake_body_64_img);	  
	  snake_body_64_img.setPosition(64*1, 64*5);
	  window.draw(snake_body_64_img);	  
	  snake_body_64_img.setPosition(64*1, 64*5);
	  window.draw(snake_body_64_img);
	  snake_body_64_img.setPosition(64*1, 64*6);
	  window.draw(snake_body_64_img);
	  snake_body_64_img.setPosition(64*1, 64*6);
	  window.draw(snake_body_64_img);
	  snake_body_64_img.setPosition(64*1, 64*7);
	  window.draw(snake_body_64_img);
	  snake_body_64_img.setPosition(64*1, 64*8);
	  window.draw(snake_body_64_img);
	  snake_body_64_img.setPosition(64*2, 64*8);
	  window.draw(snake_body_64_img);
	  snake_body_64_img.setPosition(64*2, 64*9);
	  window.draw(snake_body_64_img);
	  snake_body_64_img.setPosition(64*3, 64*9);
	  window.draw(snake_body_64_img);
	  snake_head_down_64_img.setPosition(64*3, 64*10);
	  window.draw(snake_head_down_64_img);
	  one_64_img.setPosition(64*3, 64*13);
	  window.draw(one_64_img);

	  button_292x72_img.setPosition(Vector2f(WIN_WIDTH/2.0, WIN_HEIGHT*(2.5/6.0)));
	  window.draw(button_292x72_img);
	  button_292x72_img.setPosition(Vector2f(WIN_WIDTH/2.0, WIN_HEIGHT*(3.5/6.0)));
	  window.draw(button_292x72_img);
	  button_292x72_img.setPosition(Vector2f(WIN_WIDTH/2.0, WIN_HEIGHT*(4.5/6.0)));
	  window.draw(button_292x72_img);
	  window.draw(menu_text);
	  window.draw(text_rect1);
	  window.draw(text_rect2);
	  window.draw(text_rect3);

	  window.display();	  
	  	  
	  if (Mouse::isButtonPressed(Mouse::Left))
	    {
	      Vector2i localPosition = Mouse::getPosition(window);
	      if(point_in_rect(localPosition,rect1)) flag.start=true;
	      if(point_in_rect(localPosition,rect2)) flag.show_highscore=true;

	    }
	  if(flag.start)
	    {
	      flag.new_level=true;
	      clock.restart();
	    }	    
	}
      if(!flag.startup_screen && !flag.new_level && !flag.start && flag.show_highscore)
	{
    
	  window.clear(Color::Black);


	  int MM = 96;
	  int NN = 144;
	  int x_lu_corner = (WIN_WIDTH-NN*8)/2.0;
	  int y_lu_corner = (WIN_HEIGHT-MM*8)/2.0+16;
	  
	  for(int i=0; i<NN; i++)
		for(int j=0; j<MM; j++)
		  {
		    bg_frame_inner_img.setPosition(x_lu_corner+i*8, y_lu_corner+j*8);
		    window.draw(bg_frame_inner_img);
		  }	      
	      
	  // Frame
	  for (int i=0; i<MM; i++)
	    {
	      bg_frame_left_img.setPosition(x_lu_corner, y_lu_corner + i*8);
	      window.draw(bg_frame_left_img);
	    }
	  
	  for (int i=0; i<MM; i++)
	    {
	      bg_frame_right_img.setPosition(x_lu_corner+NN*8, y_lu_corner + i*8);
	      window.draw(bg_frame_right_img);
	    }	      
	  for (int i=0; i<NN; i++)
	    {
	      bg_frame_up_img.setPosition(x_lu_corner+i*8, y_lu_corner);
	      window.draw(bg_frame_up_img);
	    }
	  
	  for (int i=0; i<NN; i++)
	    {
	      bg_frame_down_img.setPosition(x_lu_corner + i*8, y_lu_corner + MM*8);
	      window.draw(bg_frame_down_img);
	    }
	  bg_frame_lu_corner_img.setPosition(x_lu_corner,y_lu_corner);
	  bg_frame_ru_corner_img.setPosition(x_lu_corner+NN*8,y_lu_corner);
	  bg_frame_ld_corner_img.setPosition(x_lu_corner,y_lu_corner+MM*8);
	  bg_frame_rd_corner_img.setPosition(x_lu_corner+NN*8,y_lu_corner+MM*8);
	  window.draw(bg_frame_lu_corner_img);
	  window.draw(bg_frame_ru_corner_img);
	  window.draw(bg_frame_ld_corner_img);
	  window.draw(bg_frame_rd_corner_img);
	  
	  
	  window.draw(title_text);

	  body_text.setString("Pos.");
	  body_text.setPosition(Vector2f(WIN_WIDTH*0.2, 0.3*WIN_HEIGHT));
	  window.draw(body_text);
	  body_text.setString("Name");
	  body_text.setPosition(Vector2f(WIN_WIDTH*0.2+104, 0.3*WIN_HEIGHT));
	  window.draw(body_text);
	  body_text.setString("Points");
	  body_text.setPosition(Vector2f(WIN_WIDTH*0.55, 0.3*WIN_HEIGHT));
	  window.draw(body_text);
	  body_text.setString("Time");	
	  body_text.setPosition(Vector2f(WIN_WIDTH*0.65, 0.3*WIN_HEIGHT));
	  window.draw(body_text);
	  
	  hline.setPosition(Vector2f(WIN_WIDTH*0.2, WIN_HEIGHT*0.35));
	  hline.setFillColor(sf::Color(0, 0, 0));
	  window.draw(hline);
    
	  file.clear();
	  file.seekg(0,std::ios::beg);      
	  std::string line;
	  for(int i=0; i<10; i++)
	    {
	      std::getline(file,line);
	      body_text.setString(to_string(i+1) + ".");	
	      body_text.setPosition(Vector2f(WIN_WIDTH*0.2, (0.32+0.05*(i+1))*WIN_HEIGHT));
	      window.draw(body_text);
	      body_text.setString(line.substr (0,19));
	      body_text.setPosition(Vector2f(WIN_WIDTH*0.2+104, (0.32+0.05*(i+1))*WIN_HEIGHT));
	      window.draw(body_text);
	      body_text.setString(line.substr (20,5));
	      body_text.setPosition(Vector2f(WIN_WIDTH*0.55, (0.32+0.05*(i+1))*WIN_HEIGHT));
	      window.draw(body_text);
	      body_text.setString(line.substr (30,5));
	      body_text.setPosition(Vector2f(WIN_WIDTH*0.65, (0.32+0.05*(i+1))*WIN_HEIGHT));
	      window.draw(body_text);
	      
	    }
	  //	  window.draw(rect4);
	  button_292x72_img.setPosition(Vector2f(WIN_WIDTH*0.1, WIN_HEIGHT*0.05));
	  window.draw(button_292x72_img);
	  window.draw(text_rect4);	  
	  window.display();
	  
	  if (Mouse::isButtonPressed(Mouse::Left))
	    {
	      Vector2i localPosition = Mouse::getPosition(window);
	      if(point_in_rect(localPosition,rect4)) flag.show_highscore=false;	      	     
	    }	  
	}
      if (flag.start && !flag.game_over)
	{

	  float time = clock.getElapsedTime().asSeconds();
	  clock.restart();
	  par.timer += time;
	  window.clear(Color::Black);

	  if (!(par.level==0 && flag.new_level))
	    {
	      
	      if (!flag.new_level) par.tot_time += time;

	      // Keyboard arrow directions
	      if (Keyboard::isKeyPressed(Keyboard::Up   )) snake.head_dir[0]=0;
	      if (Keyboard::isKeyPressed(Keyboard::Left )) snake.head_dir[0]=1; 
	      if (Keyboard::isKeyPressed(Keyboard::Down )) snake.head_dir[0]=2;
	      if (Keyboard::isKeyPressed(Keyboard::Right)) snake.head_dir[0]=3;

	      // Level up
	      if (par.tot_time > par.level_time[par.level] && par.level+1<MAX_LEVEL){
		flag.new_level = true;
		par.level++;
	      }

	      // Move the snake
	      if (!flag.new_level && par.timer>par.level_speed[par.level])
		{		  
		  snake.move_one_box();

		  if (snake.crash())
		    flag.game_over=true;

		  if(snake.eat(fruit.x,fruit.y,fruit.value))
		    {
		      par.points = par.points + fruit.value;
		      
		      // New fruit, not on top of the snake
		      fruit.new_location();
		      while(snake.on_snake(fruit.x,fruit.y))
			{
			  fruit.new_location();
			}
		      
		      fruit.reset_timer();
		      fruit.reset_value();
		    }
		  fruit.point_value(par.timer);		  
		  par.timer=0;
		}

	      // Draw background
	      if (par.level==0){
		for (int i=0; i<WIN_WIDTH/64; i++) 
		  for (int j=0; j<WIN_HEIGHT/64; j++) 
		    {
		      bg_64_1.setPosition(i*64, j*64);
		      window.draw(bg_64_1);
		    }  
	      }
	      if (par.level==1){
		for (int i=0; i<WIN_WIDTH/64; i++) 
		  for (int j=0; j<WIN_HEIGHT/64; j++) 
		    {
		      bg_64_2.setPosition(i*64, j*64);
		      window.draw(bg_64_2);
		    }  
	      }	     
	      if (par.level==2){
		for (int i=0; i<WIN_WIDTH/64; i++) 
		  for (int j=0; j<WIN_HEIGHT/64; j++) 
		    {
		      bg_64_3.setPosition(i*64, j*64);
		      window.draw(bg_64_3);
		    }  
	      }	      
	      if (par.level==3){
		for (int i=0; i<WIN_WIDTH/64; i++) 
		  for (int j=0; j<WIN_HEIGHT/64; j++) 
		    {
		      bg_64_4.setPosition(i*64, j*64);
		      window.draw(bg_64_4);
		    }  
	      }	      
	      if (par.level>4){
		for (int i=0; i<WIN_WIDTH/64; i++) 
		  for (int j=0; j<WIN_HEIGHT/64; j++) 
		    {
		      bg_64_5.setPosition(i*64, j*64);
		      window.draw(bg_64_5);
		    }  
	      }	      

	      // PLAY AREA
	      int offset_x = (WIN_WIDTH-M*block_size_x)/2.0;
	      int offset_y = (WIN_HEIGHT-N*block_size_y)/2.0;


	      // Frame
	      for (int i=-2; i<N*4+2; i++)
		{
		  bg_frame_left_img.setPosition(offset_x-24, offset_y+i*8);
		  bg_frame_inner_img.setPosition(offset_x-16, offset_y+i*8);
		  if(i>-1 && i< N*4) bg_frame_right_img.setPosition(offset_x-8 , offset_y+i*8);
		  
		  window.draw(bg_frame_left_img);
		  window.draw(bg_frame_inner_img);
		  if(i>-1 && i<N*4) window.draw(bg_frame_right_img);		  
		}
	      for (int i=0; i<4; i++)
		{
		  bg_frame_rarrow_img.setPosition(offset_x-16, offset_y+i*N*8+N*4);
		  window.draw(bg_frame_rarrow_img);
		}
		      	      
	      for (int i=-2; i<N*4+2; i++)
		{
		  bg_frame_right_img.setPosition(offset_x+M*block_size_x+16,offset_y+i*8);
		  bg_frame_inner_img.setPosition(offset_x+M*block_size_x+8, offset_y+i*8);
		  if(i>-1 && i< N*4) bg_frame_left_img.setPosition(offset_x+M*block_size_x,offset_y+i*8);

		  window.draw(bg_frame_right_img);
		  window.draw(bg_frame_inner_img);
		  if(i>-1 && i< N*4) window.draw(bg_frame_left_img);
		}

	      for (int i=0; i<4; i++)
		{
		  bg_frame_larrow_img.setPosition(offset_x+M*block_size_x+8, offset_y+i*N*8+N*4);
		  window.draw(bg_frame_larrow_img);
		}
	      
	      for (int i=-2; i<M*4+2; i++)
		{
		  bg_frame_up_img.setPosition(offset_x+8*i,offset_y-24);
		  bg_frame_inner_img.setPosition(offset_x+8*i,offset_y-16);
		  if(i>-1 && i< N*4) bg_frame_down_img.setPosition(offset_x+8*i,offset_y-8);
		  window.draw(bg_frame_up_img);
		  window.draw(bg_frame_inner_img);
		  if(i>-1 && i< M*4) window.draw(bg_frame_down_img);
		}
	      for (int i=0; i<4; i++)
		{
		  bg_frame_darrow_img.setPosition(offset_x+i*N*8+N*4, offset_y-16);
		  window.draw(bg_frame_darrow_img);
		}
	      
	      for (int i=-2; i<M*4+2; i++)
		{
		  bg_frame_down_img.setPosition(offset_x+8*i,offset_y+M*block_size_y+16);
		  bg_frame_inner_img.setPosition(offset_x+8*i,offset_y+M*block_size_y+8);
		  if(i>-1 && i< N*4) bg_frame_up_img.setPosition(offset_x+8*i,offset_y+M*block_size_y);
		  window.draw(bg_frame_down_img);
		  window.draw(bg_frame_inner_img);
		  if(i>-1 && i< M*4) window.draw(bg_frame_up_img);
		}
	      for (int i=0; i<4; i++)
		{
		  bg_frame_uarrow_img.setPosition(offset_x+i*N*8+N*4, offset_y+M*block_size_y+8);
		  window.draw(bg_frame_uarrow_img);
		} 
	     
	      bg_frame_lu_corner_img.setPosition(offset_x-24,offset_y-24);
	      bg_frame_inner_lu_corner_img.setPosition(offset_x-8,offset_y-8);
	      window.draw(bg_frame_lu_corner_img);
	      window.draw(bg_frame_inner_lu_corner_img);
	      
	      bg_frame_ld_corner_img.setPosition(offset_x-24,offset_y+N*block_size_y+16);
	      bg_frame_inner_ld_corner_img.setPosition(offset_x-8,offset_y+N*block_size_y);
	      window.draw(bg_frame_ld_corner_img);
	      window.draw(bg_frame_inner_ld_corner_img);
	      
	      bg_frame_ru_corner_img.setPosition(offset_x+M*block_size_x+16,offset_y-24);
	      bg_frame_inner_ru_corner_img.setPosition(offset_x+M*block_size_x,offset_y-8);
	      window.draw(bg_frame_ru_corner_img);
	      window.draw(bg_frame_inner_ru_corner_img);

	      bg_frame_rd_corner_img.setPosition(offset_x+M*block_size_x+16,offset_y+N*block_size_y+16);
	      bg_frame_inner_rd_corner_img.setPosition(offset_x+M*block_size_x,offset_y+N*block_size_y);
	      window.draw(bg_frame_rd_corner_img);
	      window.draw(bg_frame_inner_rd_corner_img);

	     
	      for (int i=0; i<N; i++) 
		for (int j=0; j<M; j++) 
		  {
		    background_img.setPosition(offset_x + i*block_size_x, offset_y + j*block_size_y);
		    window.draw(background_img);
		  }
	      
	      if(snake.head_dir[0]==0){
		snake_head_up_img.setPosition(offset_x+snake.x[0]*block_size_x, offset_y+snake.y[0]*block_size_y);
		window.draw(snake_head_up_img);
	      }
	      if(snake.head_dir[0]==1){
		snake_head_left_img.setPosition(offset_x +snake.x[0]*block_size_x, offset_y+snake.y[0]*block_size_y);
		window.draw(snake_head_left_img);
	      }
	      if(snake.head_dir[0]==2){
		snake_head_down_img.setPosition(offset_x+snake.x[0]*block_size_x, offset_y+snake.y[0]*block_size_y);
		window.draw(snake_head_down_img);
	      }
	      if(snake.head_dir[0]==3){
		snake_head_right_img.setPosition(offset_x+snake.x[0]*block_size_x, offset_y+snake.y[0]*block_size_y);
		window.draw(snake_head_right_img);
	      }

	      for (int i=1;i<snake.length;i++)
		{
		  snake_body_img.setPosition(offset_x+snake.x[i]*block_size_x, offset_y+snake.y[i]*block_size_y);
		  window.draw(snake_body_img);
		}
	      if(fruit.value==1)
		{
		  fruit1_img.setPosition(offset_x+fruit.x*block_size_x,  offset_y+fruit.y*block_size_y);
	      window.draw(fruit1_img);
		}
	      if(fruit.value==2)
		{
	      fruit2_img.setPosition(offset_x+fruit.x*block_size_x,  offset_y+fruit.y*block_size_y);
	      window.draw(fruit2_img);
		}
	      if(fruit.value==3)
		{
	      fruit3_img.setPosition(offset_x+fruit.x*block_size_x,  offset_y+fruit.y*block_size_y);
	      window.draw(fruit3_img);
		}
	      if(fruit.value==4)
		{
	      fruit4_img.setPosition(offset_x+fruit.x*block_size_x,  offset_y+fruit.y*block_size_y);
	      window.draw(fruit4_img);
		}
	      if(fruit.value==5)
		{
	      fruit5_img.setPosition(offset_x+fruit.x*block_size_x,  offset_y+fruit.y*block_size_y);
	      window.draw(fruit5_img);
		}

	      
	      // >>>> SCORE-BOARD <<<<

	      int x_lu_corner = offset_x/5;
	      int y_lu_corner = offset_y;
	      int MM = 40;
	      int NN = 30;

	      for(int i=0; i<NN; i++)
		for(int j=0; j<MM; j++)
		  {
		    bg_frame_inner_img.setPosition(x_lu_corner+i*8, y_lu_corner+j*8);
		    window.draw(bg_frame_inner_img);
		  }	      
	      
	      // Frame
	      for (int i=0; i<MM; i++)
	       	{
	       	  bg_frame_left_img.setPosition(x_lu_corner, y_lu_corner + i*8);
	       	  window.draw(bg_frame_left_img);
		}
		      	      
	      for (int i=0; i<MM; i++)
	       	{
	       	  bg_frame_right_img.setPosition(x_lu_corner+NN*8, y_lu_corner + i*8);
	       	  window.draw(bg_frame_right_img);
	       	}	      
	      for (int i=0; i<NN; i++)
	       	{
	       	  bg_frame_up_img.setPosition(x_lu_corner+i*8, y_lu_corner);
	       	  window.draw(bg_frame_up_img);
		}
		      	      
	      for (int i=0; i<NN; i++)
	       	{
	       	  bg_frame_down_img.setPosition(x_lu_corner + i*8, y_lu_corner + MM*8);
	       	  window.draw(bg_frame_down_img);
	       	}
	      bg_frame_lu_corner_img.setPosition(x_lu_corner,y_lu_corner);
	      bg_frame_ru_corner_img.setPosition(x_lu_corner+NN*8,y_lu_corner);
	      bg_frame_ld_corner_img.setPosition(x_lu_corner,y_lu_corner+MM*8);
	      bg_frame_rd_corner_img.setPosition(x_lu_corner+NN*8,y_lu_corner+MM*8);
	      window.draw(bg_frame_lu_corner_img);
	      window.draw(bg_frame_ru_corner_img);
	      window.draw(bg_frame_ld_corner_img);
	      window.draw(bg_frame_rd_corner_img);	      	      
	      
		
	      string tot_time_str;
	      stringstream ss;
	      ss.precision(3);
	      ss << par.tot_time;
	      ss >> tot_time_str;
	      time_str = "Time: " + tot_time_str;
	      
	      time_text.setString(time_str);
	      time_text.setPosition(Vector2f(offset_x/5 + 20, offset_y + 20));  
	      window.draw(time_text);
	      
	      score_str = "Score: " + to_string(par.points);
	      score_text.setString(score_str);
	      score_text.setPosition(Vector2f(offset_x/5 + 20, offset_y + 60));  
	      window.draw(score_text);	      

	      
	    }

	  if (flag.new_level && par.level<MAX_LEVEL)
	    {
	      string s = "Level " + to_string(par.level+1);
	      level_text.setString(s);
	      level_text.setOrigin(center<Text>(level_text));
	      level_text.setPosition(Vector2f(WIN_WIDTH/2.0f, WIN_HEIGHT/2.0));  
	      
	      if (par.level==0)
		{
		  level_text.setColor(Color::White);
		}
	      else
		{
		  level_text.setColor(Color::Black);
		}
	      window.draw(level_text);
	      if (par.timer > par.new_level_delay)
		{
		  par.timer = 0;
		  flag.new_level = false;
		}	          
	    }
	  window.display();
	  
	}
      if(flag.game_over)
	{
	  
	  Game_over_text.setPosition(Vector2f(WIN_WIDTH/2.0f, WIN_HEIGHT*0.2));
	  window.draw(Game_over_text);
	  final_result_text.setString("FINAL RESULTS:");
	  final_result_text.setPosition(Vector2f(WIN_WIDTH*0.15, WIN_HEIGHT*0.35));
	  window.draw(final_result_text);
	  string points_str = "Points: " + to_string(par.points);
	  final_result_text.setString(points_str);
	  final_result_text.setPosition(Vector2f(WIN_WIDTH*0.15, WIN_HEIGHT*0.425));
	  window.draw(final_result_text);
	  final_result_text.setString(time_str);
	  final_result_text.setPosition(Vector2f(WIN_WIDTH*0.15, WIN_HEIGHT*0.485));
	  window.draw(final_result_text);

	  
	  if (par.points >= hs.last_points()) flag.processing_writing=true;
	  
	  if(flag.processing_writing)
	    {

	      text_input_info.setString("You made it into the top 10 highscore!");
	      text_input_info.setPosition(Vector2f(WIN_WIDTH*0.15, WIN_HEIGHT*0.65));
	      window.draw(text_input_info);
	      text_input_info.setString("Type your name:");
	      text_input_info.setPosition(Vector2f(WIN_WIDTH*0.15, WIN_HEIGHT*0.725));
	      window.draw(text_input_info);
	      
	      
	      int x_lu_corner = WIN_WIDTH*0.15;
	      int y_lu_corner = WIN_HEIGHT*0.8;
	      int NN = 0.7*WIN_WIDTH/8;
	      int MM = 8;

	      for(int i=0; i<NN; i++)
		for(int j=0; j<MM; j++)
		  {
		    bg_frame_inner_img.setPosition(x_lu_corner+i*8, y_lu_corner+j*8);
		    window.draw(bg_frame_inner_img);
		  }
	      for (int i=0; i<MM; i++)
		{
		  bg_frame_right_img.setPosition(x_lu_corner, y_lu_corner + i*8);
		  window.draw(bg_frame_right_img);
	       	}
		      	      
	      for (int i=0; i<MM; i++)
		{
		  bg_frame_left_img.setPosition(x_lu_corner+NN*8, y_lu_corner + i*8);
		  window.draw(bg_frame_left_img);
		}	      
	      for (int i=0; i<NN; i++)
		{
		  bg_frame_down_img.setPosition(x_lu_corner+i*8, y_lu_corner);
		  window.draw(bg_frame_down_img);
	       	}
		      	      
	      for (int i=0; i<NN; i++)
		{
		  bg_frame_down_img.setPosition(x_lu_corner + i*8, y_lu_corner + MM*8);
		  window.draw(bg_frame_down_img);
		}
	      bg_frame_inner_lu_corner_img.setPosition(x_lu_corner,y_lu_corner);
	      bg_frame_inner_ru_corner_img.setPosition(x_lu_corner+NN*8,y_lu_corner);
	      bg_frame_inner_ld_corner_img.setPosition(x_lu_corner,y_lu_corner+MM*8);
	      bg_frame_inner_rd_corner_img.setPosition(x_lu_corner+NN*8,y_lu_corner+MM*8);
	      window.draw(bg_frame_inner_lu_corner_img);
	      window.draw(bg_frame_inner_ru_corner_img);
	      window.draw(bg_frame_inner_ld_corner_img);
	      window.draw(bg_frame_inner_rd_corner_img);	      
	     
	      FloatRect text_bounds = text_input.getLocalBounds();
	      cursor.setPosition(Vector2f(WIN_WIDTH*0.15+16+text_bounds.left+text_bounds.width+8, WIN_HEIGHT*0.8+14));
	      if(int(par.timer2) % 2 == 0) window.draw(cursor);
	      window.draw(text_input);

	      if(flag.f)
		{
		  flag.game_over = false;
		  flag.show_highscore = true;
		}
	      
	    }
	  if(!flag.game_over && flag.show_highscore)
	    {
	      name.replace(0, input.size(), input);
	      hs.add(name,par.points,par.tot_time);
	      flag.f=false;
	    }
	  
	  window.display();
		  
	  float time2 = clock.getElapsedTime().asSeconds();
	  clock.restart();
	  par.timer2 += time2;
	  
	  if (!flag.game_over || (!flag.processing_writing && par.timer2>5.0))
	    {
	      snake.init();
	      fruit.init();
	      flag.reset();
	      par.init();
	      flag.show_highscore=true;
	    }	  
	}
    }
  file.close();
  return 0;
}

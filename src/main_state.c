#include <main_state.h>
#include <glad/glad.h>
#include <math.h>


#include <rafgl.h>

#include <game_constants.h>

static rafgl_raster_t doge;
static rafgl_raster_t house;
static rafgl_raster_t upscaled_doge;
static rafgl_raster_t raster, raster2;
static rafgl_raster_t checker;

static rafgl_texture_t texture;

static rafgl_spritesheet_t hero;

float time = 0;

// struktura za pravljenje snega
typedef struct snowpoint
{
    float x, y, dx, dy;
    float life;


} snowpoint;

#define MAX_SNOW 1000

snowpoint snows[MAX_SNOW];



void draw_snow(rafgl_raster_t *raster)
{
    int i;
    snowpoint s;
    for(i = 0; i < MAX_SNOW; i++)
    {
        s = snows[i];
        if(s.life <= 0) continue;
        rafgl_raster_draw_line(raster, s.x - s.dx, s.y - s.dy, s.x, s.y, rafgl_RGB(255, 255,  255));
    }
}

// bacanje snega po tileu
void snowMagic()
{



    int i;
    for(i = 0; i < MAX_SNOW; i++)
    {
        if(snows[i].life <= 0) continue;

        snows[i].life-0.001;

        snows[i].x += snows[i].dx;
        snows[i].y += snows[i].dy;
        snows[i].dx *= 0.995f;
        snows[i].dy *= 0.995f;
        snows[i].dy += 0.05;

        /*if(snows[i].x < 0)
        {
            snows[i].x = 0;
            snows[i].dx = (rafgl_abs_m(snows[i].dx)) * randf() ;
        }

        if(snows[i].y < 0)
        {
            snows[i].y = 0;
            snows[i].dy = (rafgl_abs_m(snows[i].dy)) * randf() ;
        }

        if(snows[i].x >= RASTER_WIDTH)
        {
            snows[i].x = RASTER_WIDTH ;
            snows[i].dx = (rafgl_abs_m(snows[i].dx)) * randf();
        }

        if(snows[i].y >= RASTER_HEIGHT)
        {
            snows[i].y = RASTER_HEIGHT - 1;
            snows[i].dy = (rafgl_abs_m(snows[i].dy)) * randf();
        }
*/


    }
}



#define NUMBER_OF_TILES 22
rafgl_raster_t tiles[NUMBER_OF_TILES];
rafgl_raster_t tilesSnow[NUMBER_OF_TILES];



#define WORLD_SIZE 60
int tile_world[WORLD_SIZE][WORLD_SIZE];

#define TILE_SIZE 64

static int raster_width = RASTER_WIDTH, raster_height = RASTER_HEIGHT;

static char save_file[256];
int save_file_no = 0;

int camx = 300, camy = 300;

int selected_x, selected_y;


static int hero_health=100;



void init_tilemap(void)
{
    int x, y;

    for(y = 0; y < WORLD_SIZE; y++)
    {
        for(x = 0; x < WORLD_SIZE; x++)
        {
            if(randf() > 0.8f)
            {
                tile_world[y][x] = 3 + rand() % 3;
            }
            else
            {
                tile_world[y][x] = rand() % 3;
            }
            if(x==7 || x==8 || x==9){
                tile_world[y][x] = 17;
            }
            if(x==2 && y==2){
                tile_world[y][x] = 18;
            }
            if(x==4 && y==4){
                tile_world[y][x] = 19;
            }
             if(x==0 && y==0){
                tile_world[y][x] = 21;
            }
        }
    }
}

//renderovanje obicne mape
void render_tilemap(rafgl_raster_t *raster, int render)
{

    int x, y;
    int x0 = camx / TILE_SIZE;
    int x1 = x0 + (raster_width / TILE_SIZE) + 1;
    int y0 = camy / TILE_SIZE;
    int y1 = y0 + (raster_height / TILE_SIZE) + 2;

    if(x0 < 0) x0 = 0;
    if(y0 < 0) y0 = 0;
    if(x1 < 0) x1 = 0;
    if(y1 < 0) y1 = 0;

    if(x0 >= WORLD_SIZE) x0 = WORLD_SIZE - 1;
    if(y0 >= WORLD_SIZE) y0 = WORLD_SIZE - 1;
    if(x1 >= WORLD_SIZE) x1 = WORLD_SIZE - 1;
    if(y1 >= WORLD_SIZE) y1 = WORLD_SIZE - 1;

    rafgl_raster_t *draw_tile;
    if(render == 0){
        for(y = y0; y <= y1; y++)
        {
            for(x = x0; x <= x1; x++)
            {


                if(x == 7 || x == 8 || x ==9){

                    draw_tile = tiles + (tile_world[y][x] % NUMBER_OF_TILES);
                        int i, j;

                    rafgl_pixel_rgb_t sampled, resulting;
                    int xnew, ynew;
                    for(j = 0; j < 64; j++)
                    {
                        for(i = 0; i < 64; i++)
                        {

                            ynew = j + 1;

                            xnew = rafgl_clampi(xnew, 0, raster_width - 1);
                            ynew = rafgl_clampi(ynew, 0, raster_height - 1);

                            sampled = pixel_at_pm(draw_tile, i,ynew);

                            resulting = sampled;


                            /* "obrcemo" svaku komponentu zasebno */


                            pixel_at_pm(draw_tile, i, j) = resulting;



                            }
                        }






                }else{
                    draw_tile = tiles + (tile_world[y][x] % NUMBER_OF_TILES);
                }

                rafgl_raster_draw_raster(raster, draw_tile, x * TILE_SIZE - camx, y * TILE_SIZE - camy - draw_tile->height + TILE_SIZE);
            }
        }
    }
    else if(render==1){
        for(y = y0; y <= y1; y++)
        {
            for(x = x0; x <= x1; x++)
            {

                if(tile_world[y][x]==18){

                    draw_tile = tiles + (tile_world[y][x] % NUMBER_OF_TILES);
                    int i, j;

                    rafgl_pixel_rgb_t sampled, resulting;

                    for(j = 26; j < 56; j++)
                    {
                        for(i = 7; i < 40; i++)
                        {


                            sampled = pixel_at_pm(draw_tile, i,j);


                                resulting.r = 255 - sampled.r;
                                resulting.g = 255 - sampled.g;
                                resulting.b = 255 - sampled.b;

                            /* "obrcemo" svaku komponentu zasebno */


                            pixel_at_pm(draw_tile, i, j) = resulting;



                            }
                        }





                }else if(x == 7 || x == 8 || x ==9){
                    draw_tile = tiles + (tile_world[y][x] % NUMBER_OF_TILES);
                        int i, j;

                    rafgl_pixel_rgb_t sampled, resulting;
                    int xnew, ynew;
                    for(j = 0; j < 64; j++)
                    {
                        for(i = 0; i < 64; i++)
                        {

                            ynew = j + 1;

                            xnew = rafgl_clampi(xnew, 0, raster_width - 1);
                            ynew = rafgl_clampi(ynew, 0, raster_height - 1);

                            sampled = pixel_at_pm(draw_tile, i,ynew);

                            resulting = sampled;


                            /* "obrcemo" svaku komponentu zasebno */


                            pixel_at_pm(draw_tile, i, j) = resulting;



                            }
                        }






                }else{
                     draw_tile = tiles + (tile_world[y][x] % NUMBER_OF_TILES);

                }
                rafgl_raster_draw_raster(raster, draw_tile, x * TILE_SIZE - camx, y * TILE_SIZE - camy - draw_tile->height + TILE_SIZE);
            }


            }
        }



    rafgl_raster_draw_rectangle(raster, selected_x * TILE_SIZE - camx, selected_y * TILE_SIZE - camy, TILE_SIZE, TILE_SIZE, rafgl_RGB(255, 255, 0));




}


//renderovanje snezne mape
void render_tilemapSnow(rafgl_raster_t *raster, int render )
{

    int x, y;
    int x0 = camx / TILE_SIZE;
    int x1 = x0 + (raster_width / TILE_SIZE) + 1;
    int y0 = camy / TILE_SIZE;
    int y1 = y0 + (raster_height / TILE_SIZE) + 2;

    if(x0 < 0) x0 = 0;
    if(y0 < 0) y0 = 0;
    if(x1 < 0) x1 = 0;
    if(y1 < 0) y1 = 0;

    if(x0 >= WORLD_SIZE) x0 = WORLD_SIZE - 1;
    if(y0 >= WORLD_SIZE) y0 = WORLD_SIZE - 1;
    if(x1 >= WORLD_SIZE) x1 = WORLD_SIZE - 1;
    if(y1 >= WORLD_SIZE) y1 = WORLD_SIZE - 1;

    rafgl_raster_t *draw_tile;
    if(render == 0){
        for(y = y0; y <= y1; y++)
        {
            for(x = x0; x <= x1; x++)
            {


                draw_tile = tilesSnow + (tile_world[y][x] % NUMBER_OF_TILES);


                rafgl_raster_draw_raster(raster, draw_tile, x * TILE_SIZE - camx, y * TILE_SIZE - camy - draw_tile->height + TILE_SIZE);





            }
        }
    }
    else if(render==1){
        for(y = y0; y <= y1; y++)
        {
            for(x = x0; x <= x1; x++)
            {

                                 if(tile_world[y][x]==18){

                        draw_tile = tilesSnow + (tile_world[y][x] % NUMBER_OF_TILES);
                       int i, j;

                    rafgl_pixel_rgb_t sampled, resulting;

                    for(j = 26; j < 56; j++)
                    {
                        for(i = 7; i < 40; i++)
                        {


                            sampled = pixel_at_pm(draw_tile, i,j);


                                resulting.r = 255 - sampled.r;
                                resulting.g = 255 - sampled.g;
                                resulting.b = 255 - sampled.b;

                            /* "obrcemo" svaku komponentu zasebno */


                            pixel_at_pm(draw_tile, i, j) = resulting;



                            }
                        }





                }else{
                     draw_tile = tilesSnow + (tile_world[y][x] % NUMBER_OF_TILES);

                }
                rafgl_raster_draw_raster(raster, draw_tile, x * TILE_SIZE - camx, y * TILE_SIZE - camy - draw_tile->height + TILE_SIZE);
            }


            }
        }



    rafgl_raster_draw_rectangle(raster, selected_x * TILE_SIZE - camx, selected_y * TILE_SIZE - camy, TILE_SIZE, TILE_SIZE, rafgl_RGB(255, 255, 0));




}


void main_state_init(GLFWwindow *window, void *args)
{
    /* inicijalizacija */
    /* raster init nam nije potreban ako radimo load from image */
    rafgl_raster_load_from_image(&doge, "res/images/doge.png");
    //rafgl_raster_load_from_image(&checker, "res/images/checker32.png");
    rafgl_raster_load_from_image(&house, "res/tiles/svgset18.png");

    rafgl_raster_init(&upscaled_doge, raster_width, raster_height);
    rafgl_raster_bilinear_upsample(&upscaled_doge, &doge);



    rafgl_raster_init(&raster, raster_width, raster_height);
    rafgl_raster_init(&raster2, raster_width, raster_height);

    rafgl_spritesheet_init(&hero, "res/images/character.png", 10, 4);

    int i;

    char tile_path[256];
        // snezna
    for(i = 0; i < NUMBER_OF_TILES; i++)
    {
        sprintf(tile_path, "res/tiles2/svgset%d.png", i);
        rafgl_raster_load_from_image(&tilesSnow[i], tile_path);
    }
    // obicna mapa
    for(i = 0; i < NUMBER_OF_TILES; i++)
    {
        sprintf(tile_path, "res/tiles/svgset%d.png", i);
        rafgl_raster_load_from_image(&tiles[i], tile_path);
    }


    init_tilemap();

    rafgl_texture_init(&texture);
}


int pressed;
float location = 0;
float selector = 0;

int animation_running = 0;
int animation_frame = 0;
int direction = 0;

// respawn heroja smanjio za 10 samo  da ne bih upadao u lavu odmah po respawn-u
int hero_pos_x = RASTER_WIDTH / 2 -10;
int hero_pos_y = RASTER_HEIGHT / 2 -10;

int hero_speed = 300;

int hover_frames = 0;

// da li da kuca luduje (menja boje)
int crazyOrNo = 0; //default

// da li da teren postane snezan i da se desi snowMagic funkcija
int snow = 0; //default

void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args)
{

    // heroj da bude uvek u centru kamere
    camx = hero_pos_x - RASTER_WIDTH/2;
    camy = hero_pos_y - RASTER_HEIGHT/2;



    //ako heroj izgubi sve helte respqwn
    if(hero_health <= 0){
        hero_pos_x = RASTER_WIDTH/2-10;
         hero_pos_y = RASTER_HEIGHT/2-10;
         camx = hero_pos_x - RASTER_WIDTH/2;
        camy = hero_pos_y - RASTER_HEIGHT/2;

        hero_health=100;

    }


    //ako se nalazi u lavi gubi helte i lava ga nosi ka dole, ali samo ako nije zima
    if((selected_x == 7 || selected_x == 8 || selected_x ==9) && snow==0){

            hero_pos_y = hero_pos_y+0.1 /delta_time;
            hero_health = hero_health - 0.3;
            printf("health %d                   \n", hero_health);
            printf("selected x=%d \n" ,selected_x);
        printf("selected y=%f \n",  selected_y);
        printf("hero x=%f \n",  hero_pos_x);
        printf("hero y=%f \n",  hero_pos_y);

            printf("-------------- \n");

    }

    animation_running = 1;
    if(game_data->keys_down[RAFGL_KEY_W])
    {
        hero_pos_y = hero_pos_y - hero_speed * delta_time;

        direction = 2;
    }
    else if(game_data->keys_down[RAFGL_KEY_S])
    {
        hero_pos_y = hero_pos_y + hero_speed * delta_time;
        direction = 0;
    }
    else if(game_data->keys_down[RAFGL_KEY_A])
    {
        hero_pos_x = hero_pos_x - hero_speed * delta_time;
        direction = 1;
    }
    else if(game_data->keys_down[RAFGL_KEY_D])
    {
        hero_pos_x = hero_pos_x + hero_speed * delta_time;
        direction = 3;
    }
    else
    {
        animation_running = 0;
    }

    if(animation_running)
    {
        if(hover_frames == 0)
        {
            animation_frame = (animation_frame + 1) % 10;
            hover_frames = 5;
        }
        else
        {
            hover_frames--;
        }

    }

    // selektovani je koliko je heroj daleko od pocetka/ sa tile size da bih znao na kom je tajlu
    // +50 samo zbog dimenzije heroja
    selected_x = rafgl_clampi((hero_pos_x + 50)  / (TILE_SIZE), 0, WORLD_SIZE - 1);
    selected_y = rafgl_clampi((hero_pos_y+50) / (TILE_SIZE), 0, WORLD_SIZE - 1);

    /* izmeni raster */

    int x, y;

    float xn, yn;

    rafgl_pixel_rgb_t sampled, sampled2, resulting, resulting2;


    for(y = 0; y < raster_height; y++)
    {
        yn = 1.0f * y / raster_height;
        for(x = 0; x < raster_width; x++)
        {
            xn = 1.0f * x / raster_width;

            sampled = pixel_at_m(upscaled_doge, x, y);
            sampled2 = rafgl_point_sample(&doge, xn, yn);

            resulting = sampled;
            resulting2 = sampled2;

            resulting.rgba = rafgl_RGB(0, 0, 0);
            resulting = sampled;

            pixel_at_m(raster, x, y) = resulting;
            pixel_at_m(raster2, x, y) = resulting2;


            if(pressed && rafgl_distance1D(location, y) < 3 && x > raster_width - 15)
            {
                pixel_at_m(raster, x, y).rgba = rafgl_RGB(255, 0, 0);
            }

        }
    }


    // ako sam na go vegan polju stvore se dve jabuke
   if(selected_x ==0 && selected_y==0){


            if(tile_world[1][1] < 17){
                tile_world[1][1] = 20;
            }
            if(tile_world[1][11] < 17){
                tile_world[1][11] = 20;
            }
   }


    // u koliko se nalazim na jabuci i pritisnem e jede jabuku i dodaje se heroju 15 healta i 10 speeda
   if(tile_world[selected_y][selected_x] == 20 && game_data->keys_pressed[RAFGL_KEY_E]){

        hero_health+=15;
        hero_speed+=10;
        printf("health = %d \n", hero_health);


   }




    //sve dok stojim na 19 kuca luduje
    if(tile_world[selected_y][selected_x]==19){

        crazyOrNo=1;
        //printf("house color= %d \n" , crazyOrNo);
    }else{
        crazyOrNo=0;
    }



    //pritiskom na enter krece snezno doba sa sneznom magijom
    if(game_data->keys_pressed[RAFGL_KEY_ENTER]){
            if(snow==0){snow =1;}
            else if(snow==1){ snow=0;}
    }

    if(snow==0){

        render_tilemap(&raster,crazyOrNo);
    }
    if(snow==1){

        //ako je sneg snezna magija i renderujem druge tajlove
             float angle, speed;
            time= time+0.05;
            //printf("SNOW == 1");
        int i=0, radius = 10, gen=20;
        for(i = 0; (i < MAX_SNOW) && gen ; i++)
        {
            if(snows[i].life <= 0)
            {
                snows[i].life = 100 * randf() + 100;
                snows[i].x = hero_pos_x-300;
                snows[i].y = hero_pos_y-500;

                angle = randf()  *  2.0f;
                speed = ( 0.3f + 0.4 * randf())*radius;
                snows[i].dx = cosf(angle) * speed;
                snows[i].dy = sinf(angle) * speed;


            }
        }
        //renderujem snezni tajl
        render_tilemapSnow(&raster,crazyOrNo);
        snowMagic();
        draw_snow(&raster);
    }

    //render karaktera (heroja)
    rafgl_raster_draw_spritesheet(&raster, &hero, animation_frame, direction, hero_pos_x-camx, hero_pos_y - camy);


    /* shift + s snima raster */
    if(game_data->keys_pressed[RAFGL_KEY_S] && game_data->keys_down[RAFGL_KEY_LEFT_SHIFT])
    {
        sprintf(save_file, "save%d.png", save_file_no++);
        rafgl_raster_save_to_png(&raster, save_file);
    }



    /* update-uj teksturu*/
    if(!game_data->keys_down[RAFGL_KEY_SPACE])
        rafgl_texture_load_from_raster(&texture, &raster);
    else
        rafgl_texture_load_from_raster(&texture, &raster2);
}


void main_state_render(GLFWwindow *window, void *args)
{
    /* prikazi teksturu */
    rafgl_texture_show(&texture);
}


void main_state_cleanup(GLFWwindow *window, void *args)
{
    rafgl_raster_cleanup(&raster);
    rafgl_raster_cleanup(&raster2);
    rafgl_texture_cleanup(&texture);

}

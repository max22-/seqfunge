#include <stdio.h>
#include <locale.h>
#include <unistd.h>
#include <notcurses/notcurses.h>

typedef struct {
    int w, h;
    int cx, cy;
    char *dat;
} matrix_t;

void matrix_clear(matrix_t *m)
{
    if(m == NULL)
        return;
    if(m->dat == NULL)
        return;
    for(int y = 0; y < m->h; y++)
        for(int x = 0; x < m->w; x++)
            m->dat[y * m->w + x] = ' ';
}

matrix_t *new_matrix(int w, int h)
{
    matrix_t *m = (matrix_t*)malloc(sizeof(matrix_t));
    if(m == NULL) {
        printf("1st malloc error\n");
        return NULL;
    }
    m->dat = (char*)malloc(sizeof(char) * w * h);
    if(m->dat == NULL) {
        printf("second malloc error\n");
        free(m);
        return NULL;
    }
    m->w = w;
    m->h = h;
    m->cx = 0;
    m->cy = 0;
    matrix_clear(m);
    return m;
}

void free_matrix(matrix_t *m)
{
    if(m != NULL) {
        if(m->dat != NULL)
            free(m->dat);
        free(m);
    }
}

int clamp_x(matrix_t *m, int x)
{
    if(x < 0)
        return 0;
    else if(x > m->w - 1)
        return m->w - 1;
    else
        return x;
}

int clamp_y(matrix_t *m, int y)
{
    if(y < 0)
        return 0;
    else if(y > m->h - 1)
        return m->h - 1;
    else
        return y;
}

void matrix_set(matrix_t *m, int x, int y, char c)
{
    if(m == NULL)
        return;
    if(m->dat == NULL)
        return;
    if(x < 0 || y < 0 || x > m->w - 1 || y > m-> h - 1)
        return;
    m->dat[y * m->w + x] = c;
}

char matrix_get(matrix_t *m, int x, int y)
{
    if(m == NULL)
        return ' ';
    if(m->dat == NULL)
        return ' ';
    if(x < 0 || y < 0 || x > m->w - 1 || y > m-> h - 1)
        return ' ';
    return m->dat[y * m->w + x];
}

void matrix_set_cursor(matrix_t *m, int x, int y)
{
    if(m == NULL)
        return;
    m->cx = clamp_x(m, x);
    m->cy = clamp_y(m, y);
}

void matrix_render(struct ncplane *ncp, matrix_t *m)
{
    ncplane_erase(ncp);
    for(int y=0; y < m->h; y++) {
        for(int x = 0; x < m->w; x++) {
            if(x == m->cx && y == m->cy) {
                ncplane_set_fg_rgb(ncp, 0x000000);
                ncplane_set_bg_rgb(ncp, 0xFFA500);
            }
            else {
                ncplane_set_fg_rgb(ncp, 0x00FF00);
                ncplane_set_bg_rgb(ncp, 0x000000);
            }
            ncplane_putchar_yx(ncp, y, x, matrix_get(m, x, y));
        }
    }
    
}

typedef struct {
    unsigned int x, y;
} process_t;

int is_direction(char c)
{
    return c == '<' || c == '>' || c == '^' || c == 'v';
}

int is_action(char c)
{
    return is_direction(c);
}


int main(int argc, char *argv[])
{
    matrix_t *m = new_matrix(80, 25);
    if(m == NULL) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    process_t process = {0, 0};
    
    if(!setlocale(LC_ALL, ""))
        return EXIT_FAILURE;
    notcurses_options nopts;
    memset(&nopts, 0, sizeof(nopts));
    struct notcurses* nc;
    if((nc = notcurses_init(&nopts, stdout)) == NULL)
        return EXIT_FAILURE;
    notcurses_mouse_enable(nc);

    int dimy, dimx;
    struct ncplane* nstd = notcurses_stddim_yx(nc, &dimy, &dimx);

    ncinput ni;
    char32_t key;
    while(key != NCKEY_ESC) {
        while((key = notcurses_getc_nblock(nc, &ni)) == 0)
            usleep(100);
        if(key == NCKEY_RIGHT)
           matrix_set_cursor(m, m->cx + 1, m->cy);
        else if(key == NCKEY_DOWN)
            matrix_set_cursor(m, m->cx, m->cy + 1);
        else if(key == NCKEY_LEFT)
            matrix_set_cursor(m, m->cx - 1, m->cy);
        else if(key == NCKEY_UP)
            matrix_set_cursor(m, m->cx, m->cy - 1);
        else if(nckey_mouse_p(key))
            matrix_set_cursor(m, ni.x, ni.y);
        else if(key == ' ')
            matrix_set(m, m->cx, m->cy, ' ');
        else if(key == NCKEY_BACKSPACE)
            matrix_clear(m);
        else if(is_action(key))
            matrix_set(m, m->cx, m->cy, key);
        

        matrix_render(nstd, m);
        notcurses_render(nc);

    }
    
    return notcurses_stop(nc) ? EXIT_FAILURE : EXIT_SUCCESS; 
}

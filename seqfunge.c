#include <stdio.h>
#include <locale.h>
#include <notcurses/notcurses.h>

#define MATRIX_WIDTH 80
#define MATRIX_HEIGHT 25


int main(int argc, char *argv[])
{
    char matrix[MATRIX_WIDTH * MATRIX_HEIGHT];
    
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
    ncplane_set_fg_rgb(nstd, 0x0000FF);
    ncplane_putchar_yx(nstd, 10, 10, '@');
    notcurses_render(nc);
    
    int x = 0, y = 0;

    ncinput ni;
    char32_t key;
    while(key != 'q') {
        key = notcurses_getc_blocking(nc, &ni);
        if(key == NCKEY_RIGHT)
            x++;
        if(key == NCKEY_DOWN)
            y++;
        if(key == NCKEY_LEFT)
            x--;
        if(key == NCKEY_UP)
            y--;
        if(nckey_mouse_p(key)) {
            x = ni.x;
            y = ni.y;
        }
        ncplane_erase(nstd);
        ncplane_putchar_yx(nstd, y, x, '@');
        notcurses_render(nc);
    }
    
    return notcurses_stop(nc) ? EXIT_FAILURE : EXIT_SUCCESS; 
}

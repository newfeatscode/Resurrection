/*
 * menu.c - Resurrection window manager menu routines.
 * Copyright (C) 2006 Tuomo Venäläinen
 *
 * See the file COPYING for information about using this software.
 */

#include <Resurrection/Resurrection.h>

void
Rwm_menu_label_buttonpress_handler(void *arg,
                                   XEvent *event);
void
Rwm_menu_item_enternotify_handler(void *arg,
                                  XEvent *event);
void
Rwm_menu_item_leavenotify_handler(void *arg,
                                  XEvent *event);

void Rwm_menu_exec_func(void *arg,
                        XEvent *event,
                        void *bindarg);

static char *Rwm_menu_strings[RWM_MENU_ITEMS] = {
    "apps",
    "term",
    "edit",
    "web",
    "audio",
    "video",
    "msn",
};

static char *Rwm_menu_image_names[RWM_MENU_ITEMS] = {
    RESURRECTION_IMAGE_SEARCH_PATH "widget/winh.png",
    RESURRECTION_IMAGE_SEARCH_PATH "widget/winselh.png",
    RESURRECTION_IMAGE_SEARCH_PATH "widget/winclickh.png",
    NULL,
    NULL
};

#if (USE_IMLIB2)
#define RWM_MENU_HORIZONTAL_RULER 0
#define RWM_MENU_VERTICAL_RULER   1
struct R_image Rwm_menu_images[RWM_MENU_ITEMS][R_WINDOW_STATES];
struct R_image Rwm_menu_ruler_images[RWM_MENU_ITEMS][2];
#endif

struct R_binding Rwm_menu_bindings[] = {
    {
        R_MAKE_BINDING(0,
                       R_BINDING_BUTTON1_FLAG),
        Rwm_menu_exec_func,
        &Rwm_exec_args[0],
        NULL
    },
    {
        R_MAKE_BINDING(0,
                       R_BINDING_BUTTON1_FLAG),
        Rwm_menu_exec_func,
        &Rwm_exec_args[1],
        NULL
    },
    {
        R_MAKE_BINDING(0,
                       R_BINDING_BUTTON1_FLAG),
        Rwm_menu_exec_func,
        &Rwm_exec_args[2],
        NULL
    },
    {
        R_MAKE_BINDING(0,
                       R_BINDING_BUTTON1_FLAG),
        Rwm_menu_exec_func,
        &Rwm_exec_args[3],
        NULL
    },
    {
        R_MAKE_BINDING(0,
                       R_BINDING_BUTTON1_FLAG),
        Rwm_menu_exec_func,
        &Rwm_exec_args[4],
        NULL
    }
};

#if 0
const char *Rwm_menu_command_strings[RWM_MENU_ITEMS] = {
    NULL,
    "rxvt",
    "emacs",
    "firefox"
};
#endif

int
Rwm_init_menu(struct R_app *app)
{
    struct R_window *item;
    int i;

    if (!Rwm_create_menu_window(app)) {

        return FALSE;
    }
    i = 0;
    while (i < RWM_MENU_ITEMS) {
        item = Rwm_create_menu_item(app,
                                    i);
        if (!item) {

            return FALSE;
        }
        Rwm_init_menu_item(item);
        i++;
    }
    if (!Rwm_init_menu_bindings(app)) {

        return FALSE;
    }
    Rwm_add_menu(app);
    Rwm_map_menu(app);

    return TRUE;
}

int
Rwm_create_menu_window(struct R_app *app)
{
    struct R_wm *wm;
    struct R_window *root;
    struct R_window *menu;
#if (USE_NEW_MENU)
    struct R_window *child;
    struct R_window *tmp;
    struct R_image *image;
    int flags;
    int x;
    int y;
    int w;
    int h;
    int i;
#endif

    wm = app->client;
    root = app->window;
    menu = R_create_window(app,
                           root,
                           R_WINDOW_OVERRIDE_REDIRECT_FLAG);
    if (menu == NULL) {

        return FALSE;
    }
#if (USE_NEW_MENU)
    Rwm_move_resize_window(menu,
                           0,
                           0,
                           2 * RWM_MENU_RULER_WIDTH + RWM_MENU_ITEM_WIDTH,
                           RWM_MENU_ITEM_HEIGHT);
#else
    Rwm_move_resize_window(menu,
                           0,
                           0,
                           RWM_MENU_ITEM_WIDTH,
                           RWM_MENU_ITEM_HEIGHT);
#endif
    menu->typeflags = R_WINDOW_MENU_FLAG;
    wm->menu = menu;

#if (USE_NEW_MENU)
    flags = R_WINDOW_STATIC_IMAGES_FLAG;
    /* left */
    w = RWM_MENU_RULER_WIDTH;
    h = RWM_MENU_ITEMS * (RWM_MENU_ITEM_HEIGHT + RWM_MENU_RULER_HEIGHT)
        + RWM_MENU_RULER_HEIGHT;

    child = R_create_window(app,
                            menu,
                            0);
    Rwm_move_resize_window(child,
                           0,
                           RWM_MENU_ITEM_HEIGHT,
                           w,
                           h);
    image = R_load_image_imlib2(app,
                                RESURRECTION_IMAGE_SEARCH_PATH "widget/winselv.png",
                                NULL);
    image->w = RWM_MENU_RULER_WIDTH;
    image->h = R_TILE_FIT_IMAGE;
    child->images[R_WINDOW_NORMAL_STATE] = image;
    RWM_SET_WINDOW_IMAGE(child, R_WINDOW_NORMAL_STATE);
    R_set_background_imlib2(image,
                            child,
                            w,
                            h,
                            0);
    menu->child = child;
    tmp = child;
    /* right */
    child = R_create_window(app,
                            menu,
                            0);
    Rwm_move_resize_window(child,
                           RWM_MENU_RULER_WIDTH + RWM_MENU_ITEM_WIDTH,
                           RWM_MENU_ITEM_HEIGHT,
                           w,
                           h);
    image = R_load_image_imlib2(app,
                                RESURRECTION_IMAGE_SEARCH_PATH "widget/winselv.png",
                                NULL);
    image->w = RWM_MENU_RULER_WIDTH;
    image->h = R_TILE_FIT_IMAGE;
    child->images[R_WINDOW_NORMAL_STATE] = image;
    RWM_SET_WINDOW_IMAGE(child, R_WINDOW_NORMAL_STATE);
    R_set_background_imlib2(image,
                            child,
                            w,
                            h,
                            0);
    tmp->chain = child;
    tmp = child;
    /* horizontal rulers. */
    x = 0;
    y = RWM_MENU_ITEM_HEIGHT;
    w = RWM_MENU_ITEM_WIDTH + 2 * RWM_MENU_RULER_WIDTH;
    h = RWM_MENU_RULER_HEIGHT;
    i = 0;
    while (i < RWM_MENU_ITEMS) {
        child = R_create_window(app,
                                menu,
                                0);
        Rwm_move_resize_window(child,
                               x,
                               y,
                               w,
                               h);
        image = R_load_image_imlib2(app,
                                    RESURRECTION_IMAGE_SEARCH_PATH "widget/winselh.png",
                                    NULL);
        image->w = R_TILE_FIT_IMAGE;
        image->h = RWM_MENU_RULER_HEIGHT;
        child->images[R_WINDOW_NORMAL_STATE] = image;
        RWM_SET_WINDOW_IMAGE(child, R_WINDOW_NORMAL_STATE);
        R_set_background_imlib2(image,
                                child,
                                w,
                                h,
                                0);
        y += RWM_MENU_RULER_HEIGHT + RWM_MENU_ITEM_HEIGHT;
        tmp->chain = child;
        tmp = child;
        i++;
    }
#endif

    return TRUE;
}

struct R_window *
Rwm_create_menu_item(struct R_app *app,
                     int id)
{
    struct R_wm *wm;
    struct R_window *menu;
    struct R_window *item;
#if (USE_IMLIB2)
    struct R_image *image;
#endif

    wm = app->client;
    menu = wm->menu;
    item = R_create_window(app,
                           menu,
                           R_WINDOW_OVERRIDE_REDIRECT_FLAG);
    if (item == NULL) {

        return NULL;
    }
#if (USE_IMLIB2)
    item->font = _menufont;
    image = &Rwm_menu_images[id];
#endif
    item->str = Rwm_menu_strings[id];
#if (USE_IMLIB2)
    item->image = image;
#endif
#if 0
    RWM_SET_WINDOW_SIZE(item,
                        RWM_MENU_ITEM_WIDTH,
                        RWM_MENU_ITEM_HEIGHT);
#endif
#if (USE_NEW_MENU)
    if (id == 0) {
        Rwm_move_resize_window(item,
                               0,
                               0,
                               RWM_MENU_ITEM_WIDTH + 2 * RWM_MENU_RULER_WIDTH,
                               RWM_MENU_ITEM_HEIGHT);
    } else {
        Rwm_move_resize_window(item,
                               RWM_MENU_RULER_WIDTH,
                               id * (RWM_MENU_ITEM_HEIGHT + RWM_MENU_RULER_HEIGHT),
                               RWM_MENU_ITEM_WIDTH,
                               RWM_MENU_ITEM_HEIGHT);
    }
#else
    Rwm_move_resize_window(item,
                           0,
                           id * RWM_MENU_ITEM_HEIGHT,
                           RWM_MENU_ITEM_WIDTH,
                           RWM_MENU_ITEM_HEIGHT);
#endif
    if (menu->last) {
        menu->last->chain = item;
    } else {
        menu->chain = item;
    }
    menu->last = item;

    return item;
}

#if (USE_IMLIB2)
int
Rwm_init_menu_item(struct R_window *item)
{
    struct R_image *image;
    struct R_app   *app;
    int i;

    app = item->app;
    image = R_load_image_imlib2(app,
                                RESURRECTION_IMAGE_SEARCH_PATH "widget/winh.png",
                                NULL);
    image->w = R_TILE_FIT_IMAGE;
    image->h = RWM_MENU_ITEM_HEIGHT;
    item->images[R_WINDOW_NORMAL_STATE] = image;
    image = R_load_image_imlib2(app,
                                RESURRECTION_IMAGE_SEARCH_PATH "widget/winselh.png",
                                NULL);
    image->w = R_TILE_FIT_IMAGE;
    image->h = RWM_MENU_ITEM_HEIGHT;
    item->images[R_WINDOW_ACTIVE_STATE] = image;
    RWM_SET_WINDOW_IMAGE(item, R_WINDOW_NORMAL_STATE);
    if (item->str) {
        Rwm_draw_text(item);
    }

    return TRUE;
}
#endif

int
Rwm_init_menu_bindings(struct R_app *app)
{
    struct R_wm *wm;
    struct R_window *menu;
    struct R_window *item;
    struct R_binding *binding;
    struct R_binding *menubinding;

    wm = app->client;
    menu = wm->menu;
    item = menu->chain->chain;
    menubinding = &Rwm_menu_bindings[0];
    while (item) {
        binding = R_create_binding(item,
                                   menubinding->mask,
                                   menubinding->func,
                                   menubinding->arg);
        if (binding == NULL) {

            return FALSE;
        }
        if (!R_add_binding(binding,
                           item)) {

            return FALSE;
        }
        item = item->chain;
        menubinding++;
    }

    return TRUE;
}

void
Rwm_init_menu_events(struct R_app *app)
{
    struct R_window *menu;
    struct R_window *item;
    struct R_wm     *wm;
    long             mask;

    wm = app->client;
    menu = wm->menu;
    
    R_set_window_event_handler(menu,
                               EnterNotify,
                               Rwm_menu_enternotify_handler);
    R_set_window_event_handler(menu,
                               LeaveNotify,
                               Rwm_menu_leavenotify_handler);
    item = menu->chain;
    if (wm->optflags & RWM_CLICK_MENU_FLAG) {
        R_set_window_event_handler(item,
                                   ButtonPress,
                                   Rwm_menu_label_buttonpress_handler);
        mask = R_add_window_events(item,
                                   ButtonPressMask);
    }
    item = item->chain;
    while (item) {
        R_set_window_event_handler(item,
                                   EnterNotify,
                                   Rwm_menu_item_enternotify_handler);
        R_set_window_event_handler(item,
                                   LeaveNotify,
                                   Rwm_menu_item_leavenotify_handler);
        mask = R_add_window_events(item,
                                   ButtonPressMask
                                   | EnterWindowMask
                                   | LeaveWindowMask);
        item = item->chain;
    }
    mask = R_add_window_events(menu,
                               ButtonReleaseMask
                               | EnterWindowMask
                               | LeaveWindowMask);

    return;
}

void
Rwm_add_menu(struct R_app *app)
{
    struct R_wm *wm;
    struct R_window *window;

    wm = app->client;
    window = wm->menu;
    R_add_window(window);
    window = window->chain;
    while (window) {
        R_add_window(window);
        window = window->chain;
    }

    return;
}

void
Rwm_map_menu(struct R_app *app)
{
    struct R_wm *wm;
    struct R_window *menu;

    wm = app->client;
    menu = wm->menu;
#if 0
    Rwm_reparent_window(menu,
                        app->desktop,
                        0,
                        0);
#endif
    R_map_subwindows(menu);
    R_map_window(menu);

    return;
}

void
Rwm_show_menu(struct R_window *menu)
{
    R_raise_window(menu);
#if (USE_NEW_MENU)
    Rwm_resize_window(menu,
                      RWM_MENU_ITEM_WIDTH + 2 * RWM_MENU_RULER_WIDTH,
                      RWM_MENU_ITEMS * (RWM_MENU_ITEM_HEIGHT + RWM_MENU_RULER_HEIGHT));
#else
    Rwm_resize_window(menu,
                      RWM_MENU_ITEM_WIDTH,
                      RWM_MENU_ITEMS * RWM_MENU_ITEM_HEIGHT);
#endif

    return;
}

void
Rwm_hide_menu(struct R_window *menu)
{
#if (USE_NEW_MENU)
    Rwm_resize_window(menu,
                      RWM_MENU_ITEM_WIDTH + 2 * RWM_MENU_RULER_WIDTH,
                      RWM_MENU_ITEM_HEIGHT);
#else
    Rwm_resize_window(menu,
                      RWM_MENU_ITEM_WIDTH,
                      RWM_MENU_ITEM_HEIGHT);
#endif

    return;
}

void
Rwm_menu_enternotify_handler(void *arg,
                             XEvent *event)
{
    struct R_wm *wm;
    struct R_window *window;
    struct R_window *menu;
    struct R_window *label;
    struct R_app    *app;

    window = arg;
    app = window->app;
    wm = app->client;
    menu = wm->menu;
    label = menu->chain;
    RWM_SET_WINDOW_IMAGE(label, R_WINDOW_ACTIVE_STATE);
    Rwm_draw_text(label);
    if (!(wm->optflags & RWM_CLICK_MENU_FLAG)) {
        Rwm_show_menu(wm->menu);
    }

    return;
}

void
Rwm_menu_leavenotify_handler(void *arg,
                             XEvent *event)
{
    struct R_app    *app;
    struct R_wm     *wm;
    struct R_window *window;
    struct R_window *label;

    window = arg;
    app = window->app;
    wm = app->client;
    label = wm->menu->chain;
    RWM_SET_WINDOW_IMAGE(label, R_WINDOW_NORMAL_STATE);
    Rwm_draw_text(label);
    Rwm_hide_menu(window);

    return;
}

void
Rwm_menu_label_buttonpress_handler(void *arg,
                                   XEvent *event)
{
    struct R_window *window;
    struct R_wm     *wm;

    window = arg;
    wm = window->app->client;
    if (event->xbutton.button == Button1
        && (wm->optflags & RWM_CLICK_MENU_FLAG)) {
        Rwm_show_menu(wm->menu);
    }
}

void
Rwm_menu_item_enternotify_handler(void *arg,
                                  XEvent *event)
{
    struct R_window *item;

    item = arg;
    RWM_SET_WINDOW_IMAGE(item, R_WINDOW_ACTIVE_STATE);
    Rwm_draw_text(item);
}

void
Rwm_menu_item_leavenotify_handler(void *arg,
                                  XEvent *event)
{
    struct R_window *item;

    item = arg;
    RWM_SET_WINDOW_IMAGE(item, R_WINDOW_NORMAL_STATE);
    Rwm_draw_text(item);

    return;
}

#if 0
void
Rwm_menu_item_buttonpress_handler(void *arg,
                                  XEvent *event)
{
    struct R_window *item;
    pid_t pid;
    char *cmd;
    char *argv[2];

    item = arg;
    cmd = item->cmd;
    argv[0] = cmd;
    argv[1] = NULL;
    pid = fork();
    if (!pid) {
        execvp(cmd,
               argv);
    }

    return;
}
#endif

void
Rwm_menu_exec_func(void *arg,
                   XEvent *event,
                   void *bindarg)
{
    struct R_wm *wm;
    struct R_window *window;

    window = arg;
    wm = window->app->client;
    Rwm_hide_menu(wm->menu);
    Rwm_exec_func(arg,
                  event,
                  bindarg);

    return;
}


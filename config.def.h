/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int gappx     = 8;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = {
	"Iosevka:style=Regular:size=12",
	"Font Awesome 5 Free,Font Awesome 5 Free Solid:style=Solid:pixelsize=15",
	"Font Awesome 5 Free,Font Awesome 5 Free Regular:style=Regular:pixelsize=15",
	"Font Awesome 5 Brands,Font Awesome 5 Brands Regular:style=Regular:pixelsize=15"
};
static const char dmenufont[]       = "monospace:size=14";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_burgundy[]    = "#76092a";
static const char *colors[][3]      = {
	/*               fg         bg            border   */
	[SchemeNorm] = { col_gray3, col_gray1,    col_gray2 },
	[SchemeSel]  = { col_gray4, col_burgundy, col_burgundy },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Steam",   NULL,     NULL,           1 << 4,    1,          0,          1,         -1 },
	{ NULL,      NULL,     "Steam",        1 << 4,    1,          0,          1,         -1 },
	{ "copyq",   NULL,     NULL,           0,         1,          0,          0,         -1 },
	{ "konsole", NULL,     NULL,           0,         0,          1,          0,         -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_burgundy, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "konsole", NULL };
static const char *lockcmd[]    = { "slock", NULL };
//static const char *sleepcmd[]   = { "slock", "sudo", "/usr/sbin/s2ram", NULL };
static const char *browsercmd[] = { "firefox", NULL };
static const char *prtsccmd[] = { "spectacle", NULL };

// Refresh status
#define REFRESH_STATUS "/usr/bin/kill -SIGUSR1 $(/usr/bin/ps -C slstatus -o pid=)"

// Volume control
#define UPVOL   "/usr/bin/amixer -q -D pulse sset Master 5%+ unmute; "REFRESH_STATUS
#define DOWNVOL "/usr/bin/amixer -q -D pulse sset Master 5%-; "REFRESH_STATUS
#define MUTEVOL "/usr/bin/amixer -q -D pulse sset Master toggle; "REFRESH_STATUS

// Display switching
#define DISPLAY_LAPTOP "/usr/local/bin/display-select laptop"
#define DISPLAY_EXTERN "/usr/local/bin/display-select extern"
#define DISPLAY_MIRROR "/usr/local/bin/display-select mirror"
#define DISPLAY_EXTEND "/usr/local/bin/display-select extend"

// Keyboard layout switching
#define NEXT_KB_LAYOUT "/usr/local/bin/keyboard-layout -n; "REFRESH_STATUS
#define PREV_KB_LAYOUT "/usr/local/bin/keyboard-layout -p; "REFRESH_STATUS

// Compositor
#define ENABLE_COMPOSITOR "/usr/bin/picom -b"
#define DISABLE_COMPOSITOR "/usr/bin/kill -9 picom"

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_F12,    spawn,          {.v = lockcmd } },
//	{ MODKEY|ShiftMask,             XK_F12,    spawn,          {.v = sleepcmd } },
	{ MODKEY|ShiftMask,             XK_F10,    spawn,          {.v = browsercmd } },
	{ MODKEY|ShiftMask,             XK_Print,  spawn,          {.v = prtsccmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_u,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -1 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0  } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },

	// Volume control
	{ 0,                            XF86XK_AudioRaiseVolume, spawn, SHCMD(UPVOL)   },
	{ 0,                            XF86XK_AudioLowerVolume, spawn, SHCMD(DOWNVOL) },
	{ 0,                            XF86XK_AudioMute,        spawn, SHCMD(MUTEVOL) },
	{ MODKEY,                       XK_F3,                   spawn, SHCMD(UPVOL)   },
	{ MODKEY,                       XK_F2,                   spawn, SHCMD(DOWNVOL) },
	{ MODKEY,                       XK_F1,                   spawn, SHCMD(MUTEVOL) },

	// Display switching
	{ MODKEY|ShiftMask,             XK_F1,                   spawn, SHCMD(DISPLAY_LAPTOP) },
	{ MODKEY|ShiftMask,             XK_F2,                   spawn, SHCMD(DISPLAY_EXTERN) },
	{ MODKEY|ShiftMask,             XK_F3,                   spawn, SHCMD(DISPLAY_MIRROR) },
	{ MODKEY|ShiftMask,             XK_F4,                   spawn, SHCMD(DISPLAY_EXTEND) },

	// Keyboard layout
	{ MODKEY,                       XK_Up,                   spawn, SHCMD(NEXT_KB_LAYOUT) },
	{ MODKEY,                       XK_Down,                 spawn, SHCMD(PREV_KB_LAYOUT) },

	// Compositor
	{ MODKEY|ShiftMask,             XK_p,                    spawn, SHCMD(ENABLE_COMPOSITOR) },
	{ MODKEY,                       XK_p,                    spawn, SHCMD(DISABLE_COMPOSITOR) },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

